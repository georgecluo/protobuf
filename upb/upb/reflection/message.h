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

#ifndef UPB_REFLECTION_MESSAGE_H_
#define UPB_REFLECTION_MESSAGE_H_

#include <stddef.h>

#include "upb/upb/collections/map.h"
#include "upb/upb/mem/arena.h"
#include "upb/upb/message/types.h"
#include "upb/upb/message/value.h"  // IWYU pragma: export
#include "upb/upb/reflection/common.h"

// Must be last.
#include "upb/upb/port/def.inc"

#ifdef __cplusplus
extern "C" {
#endif

// Returns a mutable pointer to a map, array, or submessage value. If the given
// arena is non-NULL this will construct a new object if it was not previously
// present. May not be called for primitive fields.
UPB_API upb_MutableMessageValue upb_Message_Mutable(upb_Message* msg,
                                                    const upb_FieldDef* f,
                                                    upb_Arena* a);

// Returns the field that is set in the oneof, or NULL if none are set.
UPB_API const upb_FieldDef* upb_Message_WhichOneof(const upb_Message* msg,
                                                   const upb_OneofDef* o);

// Clear all data and unknown fields.
void upb_Message_ClearByDef(upb_Message* msg, const upb_MessageDef* m);

// Clears any field presence and sets the value back to its default.
UPB_API void upb_Message_ClearFieldByDef(upb_Message* msg,
                                         const upb_FieldDef* f);

// May only be called for fields where upb_FieldDef_HasPresence(f) == true.
UPB_API bool upb_Message_HasFieldByDef(const upb_Message* msg,
                                       const upb_FieldDef* f);

// Returns the value in the message associated with this field def.
UPB_API upb_MessageValue upb_Message_GetFieldByDef(const upb_Message* msg,
                                                   const upb_FieldDef* f);

// Sets the given field to the given value. For a msg/array/map/string, the
// caller must ensure that the target data outlives |msg| (by living either in
// the same arena or a different arena that outlives it).
//
// Returns false if allocation fails.
UPB_API bool upb_Message_SetFieldByDef(upb_Message* msg, const upb_FieldDef* f,
                                       upb_MessageValue val, upb_Arena* a);

// Iterate over present fields.
//
// size_t iter = kUpb_Message_Begin;
// const upb_FieldDef *f;
// upb_MessageValue val;
// while (upb_Message_Next(msg, m, ext_pool, &f, &val, &iter)) {
//   process_field(f, val);
// }
//
// If ext_pool is NULL, no extensions will be returned.  If the given symtab
// returns extensions that don't match what is in this message, those extensions
// will be skipped.

#define kUpb_Message_Begin -1

bool upb_Message_Next(const upb_Message* msg, const upb_MessageDef* m,
                      const upb_DefPool* ext_pool, const upb_FieldDef** f,
                      upb_MessageValue* val, size_t* iter);

// Clears all unknown field data from this message and all submessages.
UPB_API bool upb_Message_DiscardUnknown(upb_Message* msg,
                                        const upb_MessageDef* m, int maxdepth);

#ifdef __cplusplus
} /* extern "C" */
#endif

#include "upb/upb/port/undef.inc"

#endif /* UPB_REFLECTION_MESSAGE_H_ */
