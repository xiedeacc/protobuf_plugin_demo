package(default_visibility = ["//visibility:public"])

load(
    "@org_tensorflow//tensorflow/core/platform/default:build_config.bzl",
    "cc_proto_library",
)

cc_proto_library(
    name = "demo_proto",
    srcs = [
        "demo.proto",
    ],
    cc_libs = ["@com_google_protobuf//:protobuf"],
    visibility = ["//visibility:public"],
)

filegroup(
    name = "proto_file",
    srcs = ["demo.proto"],
)
