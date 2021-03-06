workspace(name = "protobuf_plugin_demo")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository", "new_git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

#git_repository(
#name = "rules_cc",
#commit = "40548a2974f1aea06215272d9c2b47a14a24e556",
#remote = "https://github.com/bazelbuild/rules_cc.git",
#)

git_repository(
    name = "build_bazel_apple_support",
    remote = "https://github.com/bazelbuild/apple_support.git",
    tag = "0.9.1",
)

load(
    "@build_bazel_apple_support//lib:repositories.bzl",
    "apple_support_dependencies",
)

apple_support_dependencies()

git_repository(
    name = "build_bazel_rules_apple",
    remote = "https://github.com/bazelbuild/rules_apple.git",
    tag = "0.21.2",
)

git_repository(
    name = "io_bazel_rules_go",
    remote = "https://github.com/bazelbuild/rules_go.git",
    tag = "v0.25.1",
)

load("@io_bazel_rules_go//go:deps.bzl", "go_register_toolchains", "go_rules_dependencies")

go_rules_dependencies()

go_register_toolchains(version = "1.15.5")

git_repository(
    name = "io_bazel_rules_closure",
    remote = "https://github.com/bazelbuild/rules_closure.git",
    tag = "0.11.0",
)

git_repository(
    name = "rules_protobuf",
    remote = "https://github.com/pubref/rules_protobuf.git",
    tag = "v0.8.2",
)

git_repository(
    name = "rules_proto",
    commit = "f7a30f6f80006b591fa7c437fe5a951eb10bcbcf",
    remote = "https://github.com/bazelbuild/rules_proto.git",
)

load("@rules_proto//proto:repositories.bzl", "rules_proto_dependencies", "rules_proto_toolchains")

rules_proto_dependencies()

rules_proto_toolchains()

git_repository(
    name = "bazel_skylib",
    remote = "https://github.com/bazelbuild/bazel-skylib.git",
    tag = "1.0.3",
)

git_repository(
    name = "com_google_protobuf",
    remote = "https://github.com/protocolbuffers/protobuf.git",
    tag = "v3.15.1",
)

#####################################################################
# grpc dependencies
#####################################################################
git_repository(
    name = "com_google_protobuf",
    remote = "https://github.com/protocolbuffers/protobuf.git",
    tag = "v3.15.1",
)

git_repository(
    name = "upb",
    commit = "eb0fdda14b7b211872507a66f7d988f7c24a44c9",
    remote = "https://github.com/protocolbuffers/upb.git",
)

git_repository(
    name = "com_github_grpc_grpc",
    remote = "https://github.com/grpc/grpc.git",
    tag = "v1.35.0",
)

git_repository(
    name = "envoy",
    remote = "https://github.com/envoyproxy/envoy.git",
    tag = "v1.17.0",
)

load("@envoy//bazel:api_binding.bzl", "envoy_api_binding")

envoy_api_binding()

load("@envoy//bazel:api_repositories.bzl", "envoy_api_dependencies")

envoy_api_dependencies()

load("@com_github_grpc_grpc//bazel:grpc_deps.bzl", "grpc_deps", "grpc_test_only_deps")
load("@com_github_grpc_grpc//bazel:grpc_extra_deps.bzl", "grpc_extra_deps")

grpc_deps()

grpc_extra_deps()

###################################################################

git_repository(
    name = "rapidjson",
    remote = "https://github.com/xiedeacc/rapidjson.git",
    tag = "v1.1.0",
)

git_repository(
    name = "boost",
    remote = "https://github.com/xiedeacc/boost.git",
    tag = "v1.69.0",
)

git_repository(
    name = "cc_grpc_library_demo",
    remote = "https://github.com/xiedeacc/cc_grpc_library_demo.git",
    commit = "829dbf14fdff70a44989ad6e53dc4aeb4cee190c",
)

git_repository(
    name = "com_github_gflags_gflags",
    remote = "https://github.com/xiedeacc/gflags.git",
    tag = "v2.2.2",
)

git_repository(
    name = "glog",
    remote = "https://github.com/xiedeacc/glog.git",
    tag = "v0.3.7",
)

bind(
    name = "gflags",
    actual = "@com_github_gflags_gflags//:gflags",
)
