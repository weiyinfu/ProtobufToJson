#include <google/protobuf/message.h>
#include <google/protobuf/util/json_util.h>
#include <google/protobuf/text_format.h>
#include <nlohmann/json.hpp>
#include "google/protobuf/reflection.h"
#include "google/protobuf/descriptor.h"
#include "proto2json.h"

namespace weiyinfu {
    namespace proto2json {
        using namespace std;
        using namespace google::protobuf::util;
        using namespace google::protobuf;
        using namespace nlohmann;

        string hexEncode(string binInput) {
            string hexOutput;
            char hByte, lByte;

            for (size_t i = 0; i < binInput.length(); i++) {
                hByte = binInput[i];
                lByte = hByte & 0x0f;

                hByte += 0x30;
                lByte += 0x30;

                if (hByte > 0x39)
                    hByte += 0x07;
                if (lByte > 0x39)
                    lByte += 0x07;

                hexOutput.push_back(hByte);
                hexOutput.push_back(lByte);
            }

            return hexOutput;
        }

        json pb2json(const Message &msg, const PrintOptions &options);

        json pbRepeated2json(const Message &msg, const FieldDescriptor &repeatedField, const PrintOptions &options);

        json pbRepeated2json(const Message &msg, const FieldDescriptor &repeatedField, const PrintOptions &options) {
            const Reflection *ref = msg.GetReflection();
            json json_array(json::array());
            int size = ref->FieldSize(msg, &repeatedField);
            //repeated类型一定是基本数据类型
            switch (repeatedField.cpp_type()) {

#define GET_REPEAT_VALUE(cpptype, ctype)        \
    case FieldDescriptor::CPPTYPE_##cpptype:                      \
    {                                                             \
        for (int i = 0; i < size; i++)                            \
        {                                                         \
            auto value = ref->GetRepeated##ctype(msg, &repeatedField, i); \
            json_array.push_back(value);               \
        }                                                         \
        break;                                                    \
    }

                GET_REPEAT_VALUE(DOUBLE, Double);
                GET_REPEAT_VALUE(FLOAT, Float);
                GET_REPEAT_VALUE(INT32, Int32);
                GET_REPEAT_VALUE(INT64, Int64);
                GET_REPEAT_VALUE(UINT32, UInt32);
                GET_REPEAT_VALUE(UINT64, UInt64);
                GET_REPEAT_VALUE(BOOL, Bool);
#undef GET_REPEAT_VALUE

                case FieldDescriptor::CPPTYPE_ENUM: {
                    for (int i = 0; i < size; i++) {
                        const EnumValueDescriptor *enumValue = ref->GetRepeatedEnum(msg, &repeatedField, i);
                        if (options.always_print_enums_as_ints) {
                            json_array.push_back(enumValue->number());
                        } else {
                            json_array.push_back(enumValue->name());
                        }
                    }
                    break;
                }
                case FieldDescriptor::CPPTYPE_MESSAGE: {
                    //map是特殊类型的list，它的每一个元素都是
                    if (repeatedField.is_map()) {
                        json json_map(json::object());
                        for (int i = 0; i < size; i++) {
                            const Message *subMsg = &ref->GetRepeatedMessage(msg, &repeatedField, i);
                            if (subMsg->ByteSizeLong()) {
                                //如果真的有值
                                auto value = pb2json(*subMsg, options);
                                json_map[value["key"]] = value["value"];
                            }
                        }
                        return json_map;
                    }
                    for (int i = 0; i < size; i++) {
                        const Message *subMsg = &ref->GetRepeatedMessage(msg, &repeatedField, i);
                        if (subMsg->ByteSizeLong()) {
                            //如果真的有值
                            auto value = pb2json(*subMsg, options);
                            json_array.push_back(value);
                        }
                    }
                    break;
                }
                case FieldDescriptor::CPPTYPE_STRING: {
                    //string需要进一步区分是真正的string还是byte数组
                    for (int i = 0; i < size; i++) {
                        string stringValue = ref->GetRepeatedString(msg, &repeatedField, i);
                        json value;
                        switch (repeatedField.type()) {
                            case FieldDescriptor::TYPE_STRING: {
                                value = stringValue;
                                break;
                            }

                            case FieldDescriptor::TYPE_BYTES: {
                                value = hexEncode(stringValue);
                                break;
                            }
                            default: {
                                break;
                            }
                        }
                        json_array.push_back(value);
                    }
                    break;
                }
                default: {
                    break;
                }
            }

            return json_array;
        }

        json pb2json(const Message &msg, const PrintOptions &options) {
            const Descriptor *descriptor = msg.GetDescriptor();
            const Reflection *ref = msg.GetReflection();
            json json_obj(json::object());

            if (descriptor == nullptr || ref == nullptr) {
                return nullptr;
            }

            for (int i = 0; i < descriptor->field_count(); i++) {
                const FieldDescriptor *field = descriptor->field(i);
                const char *field_name = field->name().c_str();
                //repeated包括list和map两种类型
                if (field->is_repeated()) {
                    json repeat_field = pbRepeated2json(msg, *field, options);
                    json_obj[field_name] = repeat_field;
                    continue;
                }
                //以下解析基本类型
                json value;
                //基本数据类型
                switch (field->cpp_type()) {

#define CASE_VALUE(protoType, targetType)        \
    case FieldDescriptor::CPPTYPE_##protoType:       \
    {                                              \
        value = ref->Get##targetType(msg, field); \
        break;                                     \
    }

                    CASE_VALUE(DOUBLE, Double);
                    CASE_VALUE(FLOAT, Float);
                    CASE_VALUE(INT32, Int32);
                    CASE_VALUE(INT64, Int64);
                    CASE_VALUE(UINT32, UInt32);
                    CASE_VALUE(UINT64, UInt64);
                    CASE_VALUE(BOOL, Bool);
#undef CASE_VALUE
                    case FieldDescriptor::CPPTYPE_ENUM: {
                        auto enumValue = ref->GetEnum(msg, field);
                        if (options.always_print_enums_as_ints) {
                            value = enumValue->number();
                        } else {
                            value = enumValue->name();
                        }
                        break;
                    }

                    case FieldDescriptor::CPPTYPE_MESSAGE: {
                        if (!ref->HasField(msg, field)) {
                            value = nullptr;
                            break;
                        }
                        auto subMsg = &ref->GetMessage(msg, field);
                        value = pb2json(*subMsg, options);
                        break;
                    }

                    case FieldDescriptor::CPPTYPE_STRING: {
                        auto stringValue = ref->GetString(msg, field);

                        switch (field->type()) {
                            case FieldDescriptor::TYPE_STRING: {
                                value = stringValue;
                                break;
                            }

                            case FieldDescriptor::TYPE_BYTES: {
                                auto hexStr = hexEncode(stringValue);
                                value = hexStr;
                                break;
                            }

                            default: {
                                break;
                            }
                        }
                        break;
                    }

                    default: {
                        break;
                    }
                }
                json_obj[field_name] = value;
            }
            return json_obj;
        }

        string pb2jsonString(const Message &msg, const PrintOptions &options) {
            auto x = pb2json(msg, options);
            return x.dump(options.indent);
        }
    }
}
