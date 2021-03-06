load("//tools:plugin.bzl", "cc_proto_plugin")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "attr",
    hdrs = ["attr.h"],
    deps = [
        "@com_google_absl//absl/container:inlined_vector",
        "@glog",
    ],
)

cc_library(
    name = "base_type_memory",
    hdrs = ["base_type_memory.h"],
    deps = [
        "//external:gtest",
        "@boost//:smart_ptr",
        "@com_google_absl//absl/time",
        ":attr",
    ],
)

cc_test(
    name = "base_type_memory_test",
    srcs = ["base_type_memory_test.cc"],
    deps = [
        ":base_type_memory",
        "@glog",
    ],
)

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
    src = "@cc_grpc_library_demo//:foo_bar_files",
    outs = [
        "void_data_maker.cc",
        "void_data_maker.h",
    ],
    linkstatic = 1,
    plugin = ":pb_code_generator",
    protoc = "@com_google_protobuf//:protoc",
    data = [
        "//src/template:void_data_maker.cc",
        "//src/template:void_data_maker.h",
    ],
    proto_file = "@cc_grpc_library_demo//:foo_proto",
    deps = [
        "@cc_grpc_library_demo//:cc_bar_proto",
        "@cc_grpc_library_demo//:cc_foo_proto",
        "@boost//:smart_ptr",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/time",
        "@com_google_absl//absl/synchronization",
        "@glog",
        ":base_type_memory",
        "@rapidjson",
    ],
)
