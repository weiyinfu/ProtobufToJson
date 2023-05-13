#include <iostream>
#include "gen3/testMany.pb.h"
#include <google/protobuf/message.h>
#include <google/protobuf/util/json_util.h>
#include <google/protobuf/text_format.h>
#include<sstream>
#include<random>
#include "proto2json/proto2json.h"

using namespace std;
using namespace google::protobuf::util;

string randomString(int n = 10) {
    stringstream o;
    for (auto i = 0; i < n; i++) {
        auto x = abs(random()) % 52;
        if (x < 26) {
            o << (char) ('a' + x);
        } else if (x < 52) {
            o << (char) ('A' + x - 26);
        } else if (x < 62) {
            o << (char) ('0' + x - 52);
        } else {
            o << '_';
        }
    }
    return o.str();
}

int randRange(int begInclusive, int endExclusive) {
    return (int) (abs(random()) % (endExclusive - begInclusive) + begInclusive);
}

int64_t randInt64() {
    int64_t x = random();
    x <<= 32;
    x |= random();
    return x;
}

User randUser() {
    User user;
    user.set_id(randomString());
    user.set_address(randomString(20));
    user.set_displayname(randomString(15));
    user.set_birthyear(randRange(1990, 2023));
    user.set_birthdatetime(randInt64());
    user.set_age(randRange(18, 80));
    user.set_gender((Gender) randRange(Gender_MIN, Gender_MAX));
    if (random() & 1) {
        user.set_age(0);//故意把年龄设置成0
    }
    auto extraCount = randRange(0, 2);
    auto extra = user.mutable_extra();
    for (auto i = 0; i < extraCount; i++) {
        (*extra)[randomString()] = randomString();
    }
    return user;
}

UserList randUserList() {
    auto userCount = randRange(1, 10);
    UserList x;
    for (auto i = 0; i < userCount; i++) {
        auto u = randUser();
        auto target = x.add_users();
        *target = u;
        auto ma = x.mutable_usermap();
        (*ma)[u.id()] = u;
    }
    return x;
}

UserAndUserList randUserAndUserList() {
    UserAndUserList x;
    *x.mutable_currentuser() = randUser();
    *x.mutable_friendlist() = randUserList();
    return x;
}

google::protobuf::util::Status json2proto(const std::string &body, google::protobuf::Message &target) {
    JsonParseOptions options;
    options.ignore_unknown_fields = true;
    return JsonStringToMessage(body, &target, options);
}

string proto2json(const google::protobuf::Message &body) {
    JsonPrintOptions options;
    std::string bodyString;
    options.always_print_enums_as_ints = true;
    options.always_print_primitive_fields = true;
    options.preserve_proto_field_names = true;
    auto status = MessageToJsonString(body, &bodyString, options);
    if (!status.ok()) {
        printf("serialize data failed:%s\n", status.ToString().c_str());
    }
    return bodyString;
}

void case1() {
    User user = randUser();
    weiyinfu::proto2json::PrintOptions options;
    auto s = pb2jsonString(user, options);
    cout << "第一次序列化：" << endl << s << endl;
    cout << "正确结果：" << endl << proto2json(user) << endl;
    User user2;
    auto status = json2proto(s, user2);
    cout << "json2proto done:" << status << endl;
    auto ss = pb2jsonString(user2, options);
    cout << "第二次序列化：" << endl << ss << endl;
    cout << (s == ss) << endl;
    cout << "=================第一部分（User结构体）结束================" << endl;
}

void case2() {
    weiyinfu::proto2json::PrintOptions options;
    UserList ul = randUserList();
    auto s = pb2jsonString(ul, options);
    UserList ul2;
    cout << "第一次序列化" << endl << s << endl;
    auto status = json2proto(s, ul2);
    cout << "json2proto done：" << status << endl;
    cout << "第二次序列化 " << endl << proto2json(ul2) << endl;
    auto ss = pb2jsonString(ul2, options);
    cout << (s == ss) << endl;
    cout << "=============第二部分(UserList)结束=============" << endl;
}

void case3() {
    weiyinfu::proto2json::PrintOptions options;
    UserAndUserList uul = randUserAndUserList();
    auto s = pb2jsonString(uul, options);
    cout << "第一次序列化" << endl << s << endl;
    cout << "正确结果" << endl << proto2json(uul) << endl;
    UserAndUserList uul2;
    auto status = json2proto(s, uul2);
    cout << "json2proto done：" << status << endl;
    auto ss = pb2jsonString(uul2, options);
    cout << (s == ss) << endl;
    cout << "============第三部分（User&UserList）结束==========" << endl;
}

void case4_efficiency_test() {
    weiyinfu::proto2json::PrintOptions options;
    UserAndUserList uul = randUserAndUserList();
    string s;
    int times = 10000;
    auto begin_time = std::clock();
    for (int i = 0; i < times; i++) {
        s = pb2jsonString(uul, options);
    }
    auto end_time = std::clock();
    cout << "我的时间" << end_time - begin_time << endl;
    cout << "第一次序列化" << endl << s << endl;
    begin_time = std::clock();
    for (int i = 0; i < times; i++) {
        proto2json(uul);
    }
    end_time = std::clock();
    cout << "正确的 时间" << end_time - begin_time << endl;
    cout << "正确结果" << endl << proto2json(uul) << endl;
    UserAndUserList uul2;
    auto status = json2proto(s, uul2);
    cout << "json2proto done：" << status << endl;
    auto ss = pb2jsonString(uul2, options);
    cout << (s == ss) << endl;
    cout << "============第三部分（User&UserList）结束==========" << endl;
}

void caseTestEmpty() {
    weiyinfu::proto2json::PrintOptions options;
    auto u = randUserList();
    u.mutable_users()->Clear();
    u.mutable_usermap()->clear();
    cout << weiyinfu::proto2json::pb2jsonString(u, options) << endl;
    cout << proto2json(u) << endl;
}

void caseTestEmpty2() {
    weiyinfu::proto2json::PrintOptions options;
    UserAndUserList u;
    cout << weiyinfu::proto2json::pb2jsonString(u, options) << endl;
    cout << proto2json(u) << endl;
}

void case5_emptyStruct() {
    weiyinfu::proto2json::PrintOptions options;
    EmptyStruct x;
    cout << weiyinfu::proto2json::pb2jsonString(x, options) << endl;
    cout << proto2json(x) << endl;
}

int main() {
//    case4_efficiency_test();
//    case5_emptyStruct();
    caseTestEmpty2();
    return 0;
}