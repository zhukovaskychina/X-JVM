//
// java.io.UnixFileSystem 等 File 相关 native（POSIX；Windows 下为保守桩）。
//

#ifndef XJVM_FILE_NATIVE_SUPPORT_H
#define XJVM_FILE_NATIVE_SUPPORT_H

#include "NativeMethodRegistry.h"

namespace Runtime {
namespace Native {

void native_UnixFileSystem_initIDs(const NativeCallContext& ctx);
void native_UnixFileSystem_canonicalize0(const NativeCallContext& ctx);
void native_UnixFileSystem_getBooleanAttributes0(const NativeCallContext& ctx);
void native_UnixFileSystem_checkAccess(const NativeCallContext& ctx);
void native_UnixFileSystem_getLastModifiedTime(const NativeCallContext& ctx);
void native_UnixFileSystem_getLength(const NativeCallContext& ctx);
void native_UnixFileSystem_setPermission(const NativeCallContext& ctx);
void native_UnixFileSystem_createFileExclusively(const NativeCallContext& ctx);
void native_UnixFileSystem_delete0(const NativeCallContext& ctx);
void native_UnixFileSystem_list(const NativeCallContext& ctx);
void native_UnixFileSystem_createDirectory(const NativeCallContext& ctx);
void native_UnixFileSystem_rename0(const NativeCallContext& ctx);
void native_UnixFileSystem_setLastModifiedTime(const NativeCallContext& ctx);
void native_UnixFileSystem_setReadOnly(const NativeCallContext& ctx);
void native_UnixFileSystem_getSpace(const NativeCallContext& ctx);
void native_UnixFileSystem_getNameMax0(const NativeCallContext& ctx);

} // namespace Native
} // namespace Runtime

#endif
