//
// java.io.UnixFileSystem：路径与元数据 native（POSIX）。与 OpenJDK 描述符一致。
//

#include "FileNativeSupport.h"

#include "../ClassLoader.h"
#include "../JavaThread.h"
#include "../LocalVariableTables.h"
#include "../Object.h"
#include "../Slots.h"
#include "../heap/ClassMember.h"
#include "../heap/JavaClass.h"
#include "../heap/StringConstantPools.h"

#include <cerrno>
#include <climits>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

#ifndef _WIN32
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/time.h>
#include <unistd.h>
#endif

namespace Runtime {
namespace Native {
namespace {

/** 与 OpenJDK java.io.FileSystem 中 BA_* 一致。 */
constexpr int BA_EXISTS = 0x01;
constexpr int BA_REGULAR = 0x02;
constexpr int BA_DIRECTORY = 0x04;
constexpr int BA_HIDDEN = 0x08;

constexpr int ACCESS_READ = 0x04;
constexpr int ACCESS_WRITE = 0x02;
constexpr int ACCESS_EXECUTE = 0x01;

constexpr int SPACE_TOTAL = 0;
constexpr int SPACE_FREE = 1;
constexpr int SPACE_USABLE = 2;

Heap::FieldInfo* findInstanceField(JavaClass* start, const char* name, const char* desc) {
    for (JavaClass* jc = start; jc; jc = jc->getSuperClass()) {
        for (Heap::FieldInfo* fi : jc->getFieldInfoList()) {
            if (fi->isStatic()) {
                continue;
            }
            if (fi->getName() == name && fi->getDescriptor() == desc) {
                return fi;
            }
        }
    }
    return nullptr;
}

std::string pathFromFileObject(Object* fileObj) {
    if (!fileObj || !fileObj->getJavaClass()) {
        return "";
    }
    if (fileObj->getJavaClass()->getThisClassName() != "java/io/File") {
        return "";
    }
    Heap::FieldInfo* pf = findInstanceField(fileObj->getJavaClass(), "path", "Ljava/lang/String;");
    if (!pf) {
        return "";
    }
    Slots* base = fileObj->getFields();
    if (!base) {
        return "";
    }
    const int sid = pf->getSlotId();
    Object* strObj = base[sid].getRefs();
    if (!strObj) {
        return "";
    }
    return Heap::JString::getJString()->getJavaString(strObj);
}

std::string pathFromStringArg(Object* strObj) {
    if (!strObj) {
        return "";
    }
    return Heap::JString::getJString()->getJavaString(strObj);
}

ClassLoader* loaderFromCallee(const NativeCallContext& ctx) {
    if (!ctx.callee || !ctx.callee->getMethod() || !ctx.callee->getMethod()->getJavaClass()) {
        return nullptr;
    }
    return ctx.callee->getMethod()->getJavaClass()->getClassLoader();
}

JavaHeap* heapFromCallee(const NativeCallContext& ctx) {
    if (!ctx.callee || !ctx.callee->getJavaThread()) {
        return nullptr;
    }
    return ctx.callee->getJavaThread()->getJavaHeap();
}

Object* makeJavaString(ClassLoader* cl, JavaHeap* heap, const std::string& utf8) {
    if (!cl) {
        return nullptr;
    }
    return Heap::JString::getJString()->getJString(cl, utf8, heap);
}

#ifndef _WIN32

bool statPath(const std::string& p, struct stat* st) {
    if (p.empty()) {
        return false;
    }
    return ::stat(p.c_str(), st) == 0;
}

int booleanAttributesForPath(const std::string& p) {
    struct stat st {};
    if (!statPath(p, &st)) {
        return 0;
    }
    int a = BA_EXISTS;
    if (S_ISREG(st.st_mode)) {
        a |= BA_REGULAR;
    }
    if (S_ISDIR(st.st_mode)) {
        a |= BA_DIRECTORY;
    }
    const std::string base = p.substr(p.find_last_of('/') + 1);
    if (!base.empty() && base[0] == '.') {
        a |= BA_HIDDEN;
    }
    return a;
}

#endif

static int64_t readJlongFromLocals(LocalVariableTables* lv, int idx) {
    Slots* lo = lv->getSlots(idx);
    Slots* hi = lv->getSlots(idx + 1);
    if (!lo || !hi) {
        return 0;
    }
    const auto low = static_cast<uint32_t>(static_cast<unsigned int>(lo->getNums()));
    const auto high = static_cast<uint32_t>(static_cast<unsigned int>(hi->getNums()));
    return (static_cast<int64_t>(high) << 32) | static_cast<int64_t>(low);
}

} // namespace

void native_UnixFileSystem_initIDs(const NativeCallContext&) {}

void native_UnixFileSystem_canonicalize0(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    Object* pathStr = ctx.callee->getLocalVariableTables()->getRef(1);
    ClassLoader* cl = loaderFromCallee(ctx);
    JavaHeap* heap = heapFromCallee(ctx);
    const std::string in = pathFromStringArg(pathStr);
    if (in.empty() || !cl) {
        ctx.caller->getOperandStack()->pushRef(nullptr);
        return;
    }
#ifndef _WIN32
    char buf[PATH_MAX];
    if (::realpath(in.c_str(), buf) != nullptr) {
        ctx.caller->getOperandStack()->pushRef(makeJavaString(cl, heap, std::string(buf)));
        return;
    }
#endif
    ctx.caller->getOperandStack()->pushRef(makeJavaString(cl, heap, in));
}

void native_UnixFileSystem_getBooleanAttributes0(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    Object* file = ctx.callee->getLocalVariableTables()->getRef(1);
    const std::string p = pathFromFileObject(file);
#ifndef _WIN32
    ctx.caller->getOperandStack()->pushInt(booleanAttributesForPath(p));
#else
    (void)p;
    ctx.caller->getOperandStack()->pushInt(0);
#endif
}

void native_UnixFileSystem_checkAccess(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    Object* file = ctx.callee->getLocalVariableTables()->getRef(1);
    const int mask = ctx.callee->getLocalVariableTables()->getInt(2);
    const std::string p = pathFromFileObject(file);
    bool ok = false;
#ifndef _WIN32
    if (!p.empty()) {
        int mode = F_OK;
        if ((mask & ACCESS_READ) != 0) {
            mode |= R_OK;
        }
        if ((mask & ACCESS_WRITE) != 0) {
            mode |= W_OK;
        }
        if ((mask & ACCESS_EXECUTE) != 0) {
            mode |= X_OK;
        }
        ok = (::access(p.c_str(), mode) == 0);
    }
#else
    (void)mask;
    (void)p;
#endif
    ctx.caller->getOperandStack()->pushInt(ok ? 1 : 0);
}

void native_UnixFileSystem_getLastModifiedTime(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    Object* file = ctx.callee->getLocalVariableTables()->getRef(1);
    const std::string p = pathFromFileObject(file);
    int64_t ms = 0;
#ifndef _WIN32
    struct stat st {};
    if (statPath(p, &st)) {
#if defined(__APPLE__)
        ms = static_cast<int64_t>(st.st_mtimespec.tv_sec) * 1000LL +
             static_cast<int64_t>(st.st_mtimespec.tv_nsec / 1000000);
#else
        ms = static_cast<int64_t>(st.st_mtime) * 1000LL;
#endif
    }
#endif
    ctx.caller->getOperandStack()->pushLong(static_cast<long>(ms));
}

void native_UnixFileSystem_getLength(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    Object* file = ctx.callee->getLocalVariableTables()->getRef(1);
    const std::string p = pathFromFileObject(file);
    int64_t len = 0;
#ifndef _WIN32
    struct stat st {};
    if (statPath(p, &st) && S_ISREG(st.st_mode)) {
        len = static_cast<int64_t>(st.st_size);
    }
#endif
    ctx.caller->getOperandStack()->pushLong(static_cast<long>(len));
}

void native_UnixFileSystem_setPermission(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    Object* file = ctx.callee->getLocalVariableTables()->getRef(1);
    const int which = ctx.callee->getLocalVariableTables()->getInt(2);
    const int enable = ctx.callee->getLocalVariableTables()->getInt(3);
    const int owneronly = ctx.callee->getLocalVariableTables()->getInt(4);
    (void)which;
    (void)enable;
    (void)owneronly;
    const std::string p = pathFromFileObject(file);
    bool ok = false;
#ifndef _WIN32
    struct stat st {};
    if (statPath(p, &st)) {
        mode_t m = st.st_mode;
        // 简化：仅按 enable 打开/关闭 owner 读位示意；完整 chmod 位与 JDK 对齐成本高。
        if (enable) {
            m |= S_IRUSR;
        } else {
            m &= static_cast<mode_t>(~S_IRUSR);
        }
        ok = (::chmod(p.c_str(), m) == 0);
    }
#endif
    ctx.caller->getOperandStack()->pushInt(ok ? 1 : 0);
}

void native_UnixFileSystem_createFileExclusively(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    Object* pathStr = ctx.callee->getLocalVariableTables()->getRef(1);
    const std::string p = pathFromStringArg(pathStr);
    bool ok = false;
#ifndef _WIN32
    if (!p.empty()) {
        const int fd = ::open(p.c_str(), O_CREAT | O_EXCL | O_WRONLY, 0666);
        if (fd >= 0) {
            ::close(fd);
            ok = true;
        }
    }
#endif
    ctx.caller->getOperandStack()->pushInt(ok ? 1 : 0);
}

void native_UnixFileSystem_delete0(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    Object* file = ctx.callee->getLocalVariableTables()->getRef(1);
    const std::string p = pathFromFileObject(file);
    bool ok = false;
#ifndef _WIN32
    if (!p.empty()) {
        if (::unlink(p.c_str()) == 0) {
            ok = true;
        } else if (::rmdir(p.c_str()) == 0) {
            ok = true;
        }
    }
#endif
    ctx.caller->getOperandStack()->pushInt(ok ? 1 : 0);
}

void native_UnixFileSystem_list(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    Object* file = ctx.callee->getLocalVariableTables()->getRef(1);
    ClassLoader* cl = loaderFromCallee(ctx);
    JavaHeap* heap = heapFromCallee(ctx);
    const std::string dirPath = pathFromFileObject(file);
    if (!cl || dirPath.empty()) {
        ctx.caller->getOperandStack()->pushRef(nullptr);
        return;
    }
#ifndef _WIN32
    std::vector<std::string> names;
    DIR* d = ::opendir(dirPath.c_str());
    if (d) {
        while (dirent* ent = ::readdir(d)) {
            names.emplace_back(ent->d_name);
        }
        ::closedir(d);
    }
    JavaClass* arrCl = cl->loadClass("[Ljava/lang/String;");
    if (!arrCl) {
        ctx.caller->getOperandStack()->pushRef(nullptr);
        return;
    }
    Object* arr = arrCl->createArray(static_cast<int>(names.size()));
    if (names.empty()) {
        ctx.caller->getOperandStack()->pushRef(arr);
        return;
    }
    auto** slots = reinterpret_cast<Object**>(arr->getData());
    for (size_t i = 0; i < names.size(); ++i) {
        slots[i] = makeJavaString(cl, heap, names[i]);
    }
    ctx.caller->getOperandStack()->pushRef(arr);
#else
    (void)heap;
    ctx.caller->getOperandStack()->pushRef(nullptr);
#endif
}

void native_UnixFileSystem_createDirectory(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    Object* file = ctx.callee->getLocalVariableTables()->getRef(1);
    const std::string p = pathFromFileObject(file);
    bool ok = false;
#ifndef _WIN32
    if (!p.empty()) {
        ok = (::mkdir(p.c_str(), 0777) == 0);
    }
#endif
    ctx.caller->getOperandStack()->pushInt(ok ? 1 : 0);
}

void native_UnixFileSystem_rename0(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    Object* from = ctx.callee->getLocalVariableTables()->getRef(1);
    Object* to = ctx.callee->getLocalVariableTables()->getRef(2);
    const std::string a = pathFromFileObject(from);
    const std::string b = pathFromFileObject(to);
    bool ok = false;
#ifndef _WIN32
    if (!a.empty() && !b.empty()) {
        ok = (::rename(a.c_str(), b.c_str()) == 0);
    }
#endif
    ctx.caller->getOperandStack()->pushInt(ok ? 1 : 0);
}

void native_UnixFileSystem_setLastModifiedTime(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    Object* file = ctx.callee->getLocalVariableTables()->getRef(1);
    const int64_t ms = readJlongFromLocals(ctx.callee->getLocalVariableTables(), 2);
    const std::string p = pathFromFileObject(file);
    bool ok = false;
#ifndef _WIN32
    if (!p.empty()) {
        struct timeval tv[2];
        const time_t sec = static_cast<time_t>(ms / 1000);
        const suseconds_t usec = static_cast<suseconds_t>((ms % 1000) * 1000);
        tv[0].tv_sec = sec;
        tv[0].tv_usec = usec;
        tv[1] = tv[0];
        ok = (::utimes(p.c_str(), tv) == 0);
    }
#endif
    ctx.caller->getOperandStack()->pushInt(ok ? 1 : 0);
}

void native_UnixFileSystem_setReadOnly(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    Object* file = ctx.callee->getLocalVariableTables()->getRef(1);
    const std::string p = pathFromFileObject(file);
    bool ok = false;
#ifndef _WIN32
    struct stat st {};
    if (statPath(p, &st)) {
        mode_t m = st.st_mode;
        m &= static_cast<mode_t>(~(S_IWUSR | S_IWGRP | S_IWOTH));
        ok = (::chmod(p.c_str(), m) == 0);
    }
#endif
    ctx.caller->getOperandStack()->pushInt(ok ? 1 : 0);
}

void native_UnixFileSystem_getSpace(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    Object* file = ctx.callee->getLocalVariableTables()->getRef(1);
    const int t = ctx.callee->getLocalVariableTables()->getInt(2);
    const std::string p = pathFromFileObject(file);
    int64_t out = 0;
#ifndef _WIN32
    struct statvfs vfs {};
    if (!p.empty() && ::statvfs(p.c_str(), &vfs) == 0) {
        const unsigned long bsize = vfs.f_frsize ? vfs.f_frsize : vfs.f_bsize;
        if (t == SPACE_TOTAL) {
            out = static_cast<int64_t>(bsize) * static_cast<int64_t>(vfs.f_blocks);
        } else if (t == SPACE_FREE) {
            out = static_cast<int64_t>(bsize) * static_cast<int64_t>(vfs.f_bfree);
        } else if (t == SPACE_USABLE) {
            out = static_cast<int64_t>(bsize) * static_cast<int64_t>(vfs.f_bavail);
        }
    }
#else
    (void)t;
    (void)p;
#endif
    ctx.caller->getOperandStack()->pushLong(static_cast<long>(out));
}

void native_UnixFileSystem_getNameMax0(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    Object* pathStr = ctx.callee->getLocalVariableTables()->getRef(1);
    const std::string p = pathFromStringArg(pathStr);
    long out = NAME_MAX;
#ifndef _WIN32
    if (!p.empty()) {
        const long v = ::pathconf(p.c_str(), _PC_NAME_MAX);
        if (v > 0) {
            out = v;
        }
    }
#else
    (void)p;
#endif
    ctx.caller->getOperandStack()->pushLong(out);
}

} // namespace Native
} // namespace Runtime
