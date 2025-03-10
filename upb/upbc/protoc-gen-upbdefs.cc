// Protocol Buffers - Google's data interchange format
// Copyright 2023 Google LLC.  All rights reserved.
// https://developers.google.com/protocol-buffers/
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google LLC nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <memory>

#include "google/protobuf/descriptor.upb.h"
#include "upb/upb/reflection/def.hpp"
#include "upb/upb/util/def_to_proto.h"
#include "upb/upbc/common.h"
#include "upb/upbc/file_layout.h"
#include "upb/upbc/plugin.h"

namespace upbc {
namespace {

std::string DefInitSymbol(upb::FileDefPtr file) {
  return ToCIdent(file.name()) + "_upbdefinit";
}

static std::string DefHeaderFilename(upb::FileDefPtr file) {
  return StripExtension(file.name()) + ".upbdefs.h";
}

static std::string DefSourceFilename(upb::FileDefPtr file) {
  return StripExtension(file.name()) + ".upbdefs.c";
}

void GenerateMessageDefAccessor(upb::MessageDefPtr d, Output& output) {
  output("UPB_INLINE const upb_MessageDef *$0_getmsgdef(upb_DefPool *s) {\n",
         ToCIdent(d.full_name()));
  output("  _upb_DefPool_LoadDefInit(s, &$0);\n", DefInitSymbol(d.file()));
  output("  return upb_DefPool_FindMessageByName(s, \"$0\");\n", d.full_name());
  output("}\n");
  output("\n");
}

void WriteDefHeader(upb::FileDefPtr file, Output& output) {
  EmitFileWarning(file.name(), output);

  output(
      "#ifndef $0_UPBDEFS_H_\n"
      "#define $0_UPBDEFS_H_\n\n"
      "#include \"upb/upb/reflection/def.h\"\n"
      "#include \"upb/upb/reflection/internal/def_pool.h\"\n"
      "#include \"upb/upb/port/def.inc\"\n"
      "#ifdef __cplusplus\n"
      "extern \"C\" {\n"
      "#endif\n\n",
      ToPreproc(file.name()));

  output("#include \"upb/upb/reflection/def.h\"\n");
  output("\n");
  output("#include \"upb/upb/port/def.inc\"\n");
  output("\n");

  output("extern _upb_DefPool_Init $0;\n", DefInitSymbol(file));
  output("\n");

  for (auto msg : SortedMessages(file)) {
    GenerateMessageDefAccessor(msg, output);
  }

  output(
      "#ifdef __cplusplus\n"
      "}  /* extern \"C\" */\n"
      "#endif\n"
      "\n"
      "#include \"upb/upb/port/undef.inc\"\n"
      "\n"
      "#endif  /* $0_UPBDEFS_H_ */\n",
      ToPreproc(file.name()));
}

void WriteDefSource(upb::FileDefPtr file, Output& output) {
  EmitFileWarning(file.name(), output);

  output("#include \"upb/upb/reflection/def.h\"\n");
  output("#include \"$0\"\n", DefHeaderFilename(file));
  output("#include \"$0\"\n", MiniTableHeaderFilename(file));
  output("\n");

  for (int i = 0; i < file.dependency_count(); i++) {
    output("extern _upb_DefPool_Init $0;\n", DefInitSymbol(file.dependency(i)));
  }

  upb::Arena arena;
  google_protobuf_FileDescriptorProto* file_proto =
      upb_FileDef_ToProto(file.ptr(), arena.ptr());
  size_t serialized_size;
  const char* serialized = google_protobuf_FileDescriptorProto_serialize(
      file_proto, arena.ptr(), &serialized_size);
  absl::string_view file_data(serialized, serialized_size);

  output("static const char descriptor[$0] = {", serialized_size);

  // C90 only guarantees that strings can be up to 509 characters, and some
  // implementations have limits here (for example, MSVC only allows 64k:
  // https://docs.microsoft.com/en-us/cpp/error-messages/compiler-errors-1/fatal-error-c1091.
  // So we always emit an array instead of a string.
  for (size_t i = 0; i < serialized_size;) {
    for (size_t j = 0; j < 25 && i < serialized_size; ++i, ++j) {
      output("'$0', ", absl::CEscape(file_data.substr(i, 1)));
    }
    output("\n");
  }
  output("};\n\n");

  output("static _upb_DefPool_Init *deps[$0] = {\n",
         file.dependency_count() + 1);
  for (int i = 0; i < file.dependency_count(); i++) {
    output("  &$0,\n", DefInitSymbol(file.dependency(i)));
  }
  output("  NULL\n");
  output("};\n");
  output("\n");

  output("_upb_DefPool_Init $0 = {\n", DefInitSymbol(file));
  output("  deps,\n");
  output("  &$0,\n", FileLayoutName(file));
  output("  \"$0\",\n", file.name());
  output("  UPB_STRINGVIEW_INIT(descriptor, $0)\n", file_data.size());
  output("};\n");
}

void GenerateFile(upb::FileDefPtr file, Plugin* plugin) {
  Output h_def_output;
  WriteDefHeader(file, h_def_output);
  plugin->AddOutputFile(DefHeaderFilename(file), h_def_output.output());

  Output c_def_output;
  WriteDefSource(file, c_def_output);
  plugin->AddOutputFile(DefSourceFilename(file), c_def_output.output());
}

}  // namespace
}  // namespace upbc

int main(int argc, char** argv) {
  upbc::Plugin plugin;
  if (!plugin.parameter().empty()) {
    plugin.SetError(
        absl::StrCat("Expected no parameters, got: ", plugin.parameter()));
    return 0;
  }
  plugin.GenerateFiles(
      [&](upb::FileDefPtr file) { upbc::GenerateFile(file, &plugin); });
  return 0;
}
