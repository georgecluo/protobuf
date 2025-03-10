# Copyright (c) 2009-2021, Google LLC
# All rights reserved.
#
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file or at
# https://developers.google.com/open-source/licenses/bsd

load(
    "//upb/bazel:build_defs.bzl",
    "UPB_DEFAULT_COPTS",
    "UPB_DEFAULT_CPPOPTS",
)
load(
    "//upb/bazel:upb_minitable_proto_library.bzl",
    "upb_minitable_proto_library",
)
load(
    "//upb/bazel:upb_proto_library.bzl",
    "upb_proto_library",
    "upb_proto_reflection_library",
)
load(
    "//upb/upbc:bootstrap_compiler.bzl",
    "bootstrap_cc_binary",
    "bootstrap_cc_library",
    "bootstrap_upb_proto_library",
)

# begin:google_only
# package(default_applicable_licenses = ["//upb:license"])
# end:google_only

licenses(["notice"])

proto_library(
    name = "code_generator_request",
    srcs = ["code_generator_request.proto"],
    visibility = ["//upb:friends"],
    deps = ["//:compiler_plugin_proto"],
)

upb_proto_library(
    name = "code_generator_request_upb_proto",
    visibility = ["//upb:friends"],
    deps = [":code_generator_request"],
)

upb_proto_reflection_library(
    name = "code_generator_request_upb_proto_reflection",
    visibility = ["//upb:friends"],
    deps = [":code_generator_request"],
)

upb_minitable_proto_library(
    name = "code_generator_request_upb_minitable_proto",
    visibility = ["//upb:friends"],
    deps = [":code_generator_request"],
)

bootstrap_upb_proto_library(
    name = "plugin_upb_proto",
    base_dir = "",
    # TODO(b/289127200): Export 'net/proto2/proto/descriptor.upb.h' and remove "-layering_check".
    features = ["-layering_check"],
    google3_src_files = [
        "net/proto2/compiler/proto/profile.proto",
        "third_party/protobuf/compiler/plugin.proto",
    ],
    google3_src_rules = [
        "//net/proto2/proto:descriptor_proto_source",
        "//net/proto2/compiler/proto:profile.proto",
        "//third_party/protobuf/compiler:plugin_proto_source",
    ],
    oss_src_files = ["google/protobuf/compiler/plugin.proto"],
    oss_src_rules = [
        "//:descriptor_proto_srcs",
        "//src/google/protobuf/compiler:plugin_proto_src",
    ],
    oss_strip_prefix = "third_party/protobuf/github/bootstrap/src",
    proto_lib_deps = ["//:compiler_plugin_proto"],
    visibility = ["//upb:friends"],
    deps = ["//upb:descriptor_upb_proto"],
)

upb_proto_reflection_library(
    name = "plugin_upb_proto_reflection",
    visibility = ["//upb:friends"],
    deps = ["//:compiler_plugin_proto"],
)

bootstrap_cc_library(
    name = "common",
    srcs = [
        "common.cc",
    ],
    hdrs = [
        "common.h",
    ],
    bootstrap_deps = [
        "//upb:reflection",
    ],
    copts = UPB_DEFAULT_CPPOPTS,
    visibility = ["//upb/protos_generator:__pkg__"],
    deps = [
        "//upb:mini_table",
        "//upb:mini_table_internal",
        "//upb:port",
        "@com_google_absl//absl/strings",
    ],
)

bootstrap_cc_library(
    name = "file_layout",
    srcs = [
        "file_layout.cc",
    ],
    hdrs = [
        "file_layout.h",
    ],
    bootstrap_deps = [
        ":common",
        "//upb:reflection",
        "//upb:descriptor_upb_proto",
    ],
    copts = UPB_DEFAULT_CPPOPTS,
    visibility = ["//upb/protos_generator:__pkg__"],
    deps = [
        "//upb:base",
        "//upb:mini_descriptor",
        "//upb:mini_table",
        "//upb:mini_table_internal",
        "//upb:port",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "keywords",
    srcs = [
        "keywords.cc",
    ],
    hdrs = [
        "keywords.h",
    ],
    copts = UPB_DEFAULT_CPPOPTS,
    visibility = ["//upb/protos_generator:__pkg__"],
)

bootstrap_cc_library(
    name = "plugin",
    hdrs = [
        "plugin.h",
    ],
    bootstrap_deps = [
        ":plugin_upb_proto",
        "//upb:descriptor_upb_proto",
        "//upb:reflection",
    ],
    copts = UPB_DEFAULT_CPPOPTS,
    visibility = ["//upb/protos_generator:__pkg__"],
    deps = [
        "//upb:port",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/strings",
    ],
)

bootstrap_cc_library(
    name = "names",
    srcs = [
        "names.cc",
    ],
    hdrs = [
        "names.h",
    ],
    bootstrap_deps = [
        "//upb:reflection",
    ],
    copts = UPB_DEFAULT_CPPOPTS,
    visibility = ["//upb/protos_generator:__pkg__"],
    deps = [
        "//:protobuf",
        "//src/google/protobuf/compiler:code_generator",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/strings",
    ],
)

cc_binary(
    name = "libupbc.so",
    srcs = ["upbc_so.c"],
    copts = UPB_DEFAULT_COPTS + ["-DUPB_BUILD_API"],
    linkshared = 1,
    linkstatic = 1,
    visibility = ["//visibility:public"],
    deps = [
        ":upbdev",
        "//upb:port",
    ],
)

cc_library(
    name = "upbdev",
    srcs = [
        "code_generator_request.c",
        "code_generator_request.h",
        "get_used_fields.c",
        "upbdev.c",
    ],
    hdrs = [
        "get_used_fields.h",
        "upbdev.h",
    ],
    copts = UPB_DEFAULT_COPTS,
    visibility = ["//visibility:private"],
    deps = [
        ":code_generator_request_upb_proto",
        ":code_generator_request_upb_proto_reflection",
        ":plugin_upb_proto",
        ":plugin_upb_proto_reflection",
        "//upb:base",
        "//upb:descriptor_upb_proto",
        "//upb:json",
        "//upb:mem",
        "//upb:mini_descriptor",
        "//upb:mini_table",
        "//upb:port",
        "//upb:reflection",
        "//upb:reflection_internal",
        "//upb:wire",
    ],
)

bootstrap_cc_binary(
    name = "protoc-gen-upb",
    srcs = ["protoc-gen-upb.cc"],
    bootstrap_deps = [
        ":common",
        ":file_layout",
        ":names",
        ":plugin",
        ":plugin_upb_proto",
        "//upb:descriptor_upb_proto",
        "//upb:reflection",
    ],
    copts = UPB_DEFAULT_CPPOPTS,
    visibility = ["//visibility:public"],
    deps = [
        "//upb:base",
        "//upb:mem",
        "//upb:mini_table_internal",
        "//upb:port",
        "//upb:wire_types",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/strings",
    ],
)

bootstrap_cc_binary(
    name = "protoc-gen-upb_minitable",
    srcs = ["protoc-gen-upb_minitable.cc"],
    bootstrap_deps = [
        ":common",
        ":file_layout",
        ":names",
        ":plugin",
        ":plugin_upb_proto",
        "//upb:descriptor_upb_proto",
        "//upb:reflection",
    ],
    copts = UPB_DEFAULT_CPPOPTS,
    visibility = ["//visibility:public"],
    deps = [
        "//upb:base",
        "//upb:mem",
        "//upb:mini_table_internal",
        "//upb:port",
        "//upb:wire_types",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/strings",
    ],
)

cc_binary(
    name = "protoc-gen-upbdefs",
    srcs = [
        "protoc-gen-upbdefs.cc",
    ],
    copts = UPB_DEFAULT_CPPOPTS,
    # To work around the following link error from ABSL:
    # /usr/bin/x86_64-linux-gnu-ld: bazel-out/k8-opt-exec-2B5CBBC6-ST-c1776f9924ec/bin/external/com_google_absl/absl/time/libtime.a(duration.o): undefined reference to symbol 'floor@@GLIBC_2.2.5'
    # /usr/bin/x86_64-linux-gnu-ld: /opt/manylinux/2014/x86_64/lib64/libm.so.6: error adding symbols: DSO missing from command line
    # clang-14: error: linker command failed with exit code 1 (use -v to see invocation)
    linkopts = ["-lm"],
    visibility = ["//visibility:public"],
    deps = [
        ":common",
        ":file_layout",
        ":plugin",
        "//upb:descriptor_upb_proto",
        "//upb:reflection",
        "//upb/upb/util:def_to_proto",
    ],
)

cc_binary(
    name = "protoc-gen-upbdev",
    srcs = [
        "protoc-gen-upbdev.cc",
        "subprocess.cc",
        "subprocess.h",
    ],
    copts = UPB_DEFAULT_CPPOPTS,
    target_compatible_with = select({
        "@platforms//os:windows": ["@platforms//:incompatible"],
        "//conditions:default": [],
    }),
    visibility = ["//visibility:public"],
    deps = [
        ":plugin_upb_proto",
        ":upbdev",
        "//upb:port",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/strings",
    ],
)
