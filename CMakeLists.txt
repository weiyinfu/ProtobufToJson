cmake_minimum_required(VERSION 3.21)
project(protobufToJson)

set(CMAKE_CXX_STANDARD 20)
set(CONAN_DISABLE_CHECK_COMPILER 1)
include(build/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(protobufToJson main.cpp)
# proto v2测试
add_executable(protobuf_v2 150-protobuf/01-protobuf和json.cpp 150-protobuf/gen/haha.pb.cc 150-protobuf/proto2json/proto2json.cpp)
target_link_libraries(protobuf_v2  ${CONAN_LIBS})

# proto v3测试
add_executable(protobuf_v3 150-protobuf/02-protobuf的optional的默认值.cpp 150-protobuf/gen/haha.pb.cc 150-protobuf/proto2json/proto2json.cpp)
target_link_libraries(protobuf_v3  ${CONAN_LIBS})

# proto v3大规模测试
add_executable(protobuf_v3_test_many 150-protobuf/03-充分测试proto和json转换.cpp 150-protobuf/gen3/testMany.pb.cc 150-protobuf/proto2json/proto2json.cpp)
target_link_libraries(protobuf_v3_test_many  ${CONAN_LIBS})


# proto v3大规模测试
add_executable(protobuf_v3_json 150-protobuf/001-usejson.cpp)
target_link_libraries(protobuf_v3_json  ${CONAN_LIBS})