load("//tools:plugin.bzl", "cc_proto_plugin")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "file_util",
    srcs = ["file_util.cc"],
    hdrs = ["file_util.h"],
    deps = [
        "@boost//:filesystem",
        "@boost//:smart_ptr",
        "@glog",
    ],
)

cc_binary(
    name = "pb_code_generator",
    srcs = [
        "pb_code_generator.cc",
        "pb_code_generator.h",
    ],
    data = [
        "//src/template:void_data_maker.cc",
        "//src/template:void_data_maker.h",
    ],
    linkstatic = 1,
    deps = [
        ":file_util",
        "@com_google_absl//absl/strings",
        "@com_google_protobuf//:protoc_lib",
        "@glog",
        "@rapidjson",
    ],
)

cc_proto_plugin(
    name = "pb_code_generator_plugin",
    src = "//src/proto:proto_file",
    outs = [
        "void_data_maker.cc",
        "void_data_maker.h",
    ],
    linkstatic = 1,
    plugin = ":pb_code_generator",
    protoc = "@com_google_protobuf//:protoc",
    deps = [
        "//src/proto:demo_proto",
        "@boost//:smart_ptr",
        "@com_google_absl//absl/strings",
        "@glog",
        "@rapidjson",
    ],
)
