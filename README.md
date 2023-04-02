proto2json

# Why do I create this repo?
Protobuf serializes type int64 as string. And the official rejected to provide an option
to disable this conversion. But I really need this feature. 

* https://github.com/protocolbuffers/protobuf/pull/5035
* https://github.com/protocolbuffers/protobuf/issues/5015
* https://github.com/google/gnostic/pull/355
* https://github.com/jhump/protoreflect/issues/510
* https://github.com/protocolbuffers/protobuf/issues/2679

# install
1. add python bin to `$PATH`
2. install conan: `pip install conan==1.53`
3. run `update-conan` to install dependencies.

# How to use this library?
```cpp
    User user = randUser();
    weiyinfu::proto2json::PrintOptions options;
    auto s = pb2jsonString(user, options);
```