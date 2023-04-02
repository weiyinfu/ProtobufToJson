#include <iostream>
#include "gen/haha.pb.h"
#include <google/protobuf/message.h>
#include <google/protobuf/util/json_util.h>
#include <google/protobuf/text_format.h>

using namespace std;
using namespace google::protobuf::util;

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

int main() {
    cout << "天下大势为我所控" << endl;
    User u;
    u.set_displayname("weiyinfu");
    u.set_age(13);
    auto extra = u.mutable_extra();
    (*extra)["职业"] = "programmar";
    auto s = proto2json(u);
    cout << "Proto官方：" << s << endl;
    User uu;
    auto status = json2proto(s, uu);
    cout << status << endl;
    cout << "Proto官方第二次：" << proto2json(uu) << endl;
    cout << "this is debug string" << uu.Utf8DebugString() << endl;
    s = "{\"Id\":\"\",\"DisplayName\":\"weiyinfu\",\"Age\":\"144443\",\"Address\":\"\",\"BirthYear\":0}";
    status = json2proto(s, uu);
    cout << status << endl;
    cout << uu.age() << endl;
    cout << "===========" << endl;
    cout << uu.SerializeAsString() << endl;
    return 0;
}
