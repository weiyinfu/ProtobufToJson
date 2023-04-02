protoc --cpp_out=gen haha.proto

# proto3和proto2的区别

proto3更加倡导"约定大于配置"，简化语法。

所有字段默认是required，所以删除了required关键字。

proto3删除了optional，optional处于实验阶段。

```
This file contains proto3 optional fields, but --experimental_allow_proto3_optional was not set.
```

https://blog.csdn.net/ymzhu385/article/details/122307593

# 使用protobuf的注意事项

protoc与protobuf的版本必须对应。

# protobuf数据转成JSON
虽然protobuf官方库里面提供了proto转JSON的方法，但是那种方法效率较低（需要先序列化成二进制，然后从二进制里面解析出来）。  
https://github.com/Kelvin-001/Google_Protocol_Buffers-to-JSON/blob/main/google_protocol_buffer-to-jansson/pb2json.h
# 解析proto文件格式，转换成JSON
https://github.com/thinkcn/proto2json

# Github上的讨论
