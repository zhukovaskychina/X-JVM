//
// sun.nio.ch.Net / SocketDispatcher — 与 OpenJDK Unix Net.c / SocketDispatcher.c 行为对齐的精简实现。
//

#include "NioChNativeSupport.h"

#include "../ClassLoader.h"
#include "../JavaThread.h"
#include "../LocalVariableTables.h"
#include "../Object.h"
#include "../Slots.h"
#include "../heap/ClassMember.h"
#include "../heap/JavaClass.h"
#include "../heap/JavaHeap.h"
#include "../heap/StringConstantPools.h"

#include <cerrno>
#include <climits>
#include <cstdint>
#include <cstring>

#ifndef _WIN32
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <limits.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#endif

namespace Runtime {
namespace Native {
namespace {

using Heap::FieldInfo;

constexpr int IOS_UNAVAILABLE = -2;
constexpr int IOS_INTERRUPTED = -3;

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

FieldInfo* findInstanceField(JavaClass* start, const char* name, const char* desc) {
    for (JavaClass* jc = start; jc; jc = jc->getSuperClass()) {
        for (FieldInfo* fi : jc->getFieldInfoList()) {
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

void setSlotInt(Object* obj, int slotId, int v) {
    if (!obj) {
        return;
    }
    Slots* base = obj->getFields();
    if (!base) {
        return;
    }
    base[slotId].setNums(v);
}

void setSlotRef(Object* obj, int slotId, Object* ref) {
    if (!obj) {
        return;
    }
    Slots* base = obj->getFields();
    if (!base) {
        return;
    }
    base[slotId].setRefs(ref);
}

int getSlotInt(Object* obj, int slotId) {
    if (!obj) {
        return 0;
    }
    Slots* base = obj->getFields();
    if (!base) {
        return 0;
    }
    return base[slotId].getNums();
}

Object* getSlotRef(Object* obj, int slotId) {
    if (!obj) {
        return nullptr;
    }
    Slots* base = obj->getFields();
    if (!base) {
        return nullptr;
    }
    return base[slotId].getRefs();
}

Object* makeJavaString(ClassLoader* cl, JavaHeap* heap, const std::string& utf8) {
    if (!cl) {
        return nullptr;
    }
    return Heap::JString::getJString()->getJString(cl, utf8, heap);
}

int getFdFromFileDescriptor(Object* fdObj) {
    if (!fdObj) {
        return -1;
    }
    FieldInfo* f = findInstanceField(fdObj->getJavaClass(), "fd", "I");
    if (!f) {
        return -1;
    }
    return getSlotInt(fdObj, f->getSlotId());
}

void setFdOnFileDescriptor(Object* fdObj, int fd) {
    if (!fdObj) {
        return;
    }
    FieldInfo* f = findInstanceField(fdObj->getJavaClass(), "fd", "I");
    if (!f) {
        return;
    }
    setSlotInt(fdObj, f->getSlotId(), fd);
}

int ipv4BytesToHolderInt(const uint8_t b[4]) {
    return (static_cast<int>(b[0] & 255) << 24) | (static_cast<int>(b[1] & 255) << 16) |
           (static_cast<int>(b[2] & 255) << 8) | (static_cast<int>(b[3] & 255));
}

Object* makeInet4Address(ClassLoader* cl, JavaHeap* heap, const std::string& hostOrNumeric, const uint8_t ipv4[4]) {
    if (!cl) {
        return nullptr;
    }
    JavaClass* holderCl = cl->loadClass("java/net/InetAddress$InetAddressHolder");
    JavaClass* inet4Cl = cl->loadClass("java/net/Inet4Address");
    if (!holderCl || !inet4Cl) {
        return nullptr;
    }
    Object* holder = holderCl->createNewJavaObjectInstance();
    Object* inet4 = inet4Cl->createNewJavaObjectInstance();
    if (!holder || !inet4) {
        return nullptr;
    }
    Object* hostStr = makeJavaString(cl, heap, hostOrNumeric);
    FieldInfo* hOrig = findInstanceField(holderCl, "originalHostName", "Ljava/lang/String;");
    FieldInfo* hHost = findInstanceField(holderCl, "hostName", "Ljava/lang/String;");
    FieldInfo* hAddr = findInstanceField(holderCl, "address", "I");
    FieldInfo* hFam = findInstanceField(holderCl, "family", "I");
    if (!hOrig || !hHost || !hAddr || !hFam) {
        return nullptr;
    }
    setSlotRef(holder, hOrig->getSlotId(), hostStr);
    setSlotRef(holder, hHost->getSlotId(), hostStr);
    setSlotInt(holder, hAddr->getSlotId(), ipv4BytesToHolderInt(ipv4));
    setSlotInt(holder, hFam->getSlotId(), 1);
    FieldInfo* iHolder = findInstanceField(inet4Cl, "holder", "Ljava/net/InetAddress$InetAddressHolder;");
    if (!iHolder) {
        return nullptr;
    }
    setSlotRef(inet4, iHolder->getSlotId(), holder);
    return inet4;
}

Object* makeInet6Address(ClassLoader* cl, JavaHeap* heap, const std::string& hostOrNumeric, const uint8_t ipv6[16],
                         uint32_t scopeId) {
    if (!cl) {
        return nullptr;
    }
    JavaClass* holderCl = cl->loadClass("java/net/InetAddress$InetAddressHolder");
    JavaClass* holder6Cl = cl->loadClass("java/net/Inet6Address$Inet6AddressHolder");
    JavaClass* inet6Cl = cl->loadClass("java/net/Inet6Address");
    JavaClass* barrCl = cl->loadClass("[B");
    if (!holderCl || !holder6Cl || !inet6Cl || !barrCl) {
        return nullptr;
    }
    Object* holder = holderCl->createNewJavaObjectInstance();
    Object* holder6 = holder6Cl->createNewJavaObjectInstance();
    Object* inet6 = inet6Cl->createNewJavaObjectInstance();
    if (!holder || !holder6 || !inet6) {
        return nullptr;
    }
    Object* hostStr = makeJavaString(cl, heap, hostOrNumeric);
    FieldInfo* hOrig = findInstanceField(holderCl, "originalHostName", "Ljava/lang/String;");
    FieldInfo* hHost = findInstanceField(holderCl, "hostName", "Ljava/lang/String;");
    FieldInfo* hAddr = findInstanceField(holderCl, "address", "I");
    FieldInfo* hFam = findInstanceField(holderCl, "family", "I");
    if (!hOrig || !hHost || !hAddr || !hFam) {
        return nullptr;
    }
    setSlotRef(holder, hOrig->getSlotId(), hostStr);
    setSlotRef(holder, hHost->getSlotId(), hostStr);
    setSlotInt(holder, hAddr->getSlotId(), 0);
    setSlotInt(holder, hFam->getSlotId(), 2);
    Object* ipArr = barrCl->createArray(16);
    if (!ipArr || ipArr->getArrayLength() != 16) {
        return nullptr;
    }
    std::memcpy(ipArr->getData(), ipv6, 16);
    FieldInfo* h6ip = findInstanceField(holder6Cl, "ipaddress", "[B");
    FieldInfo* h6sc = findInstanceField(holder6Cl, "scope_id", "I");
    FieldInfo* h6scs = findInstanceField(holder6Cl, "scope_id_set", "Z");
    FieldInfo* h6if = findInstanceField(holder6Cl, "scope_ifname", "Ljava/net/NetworkInterface;");
    FieldInfo* h6ifs = findInstanceField(holder6Cl, "scope_ifname_set", "Z");
    if (!h6ip || !h6sc || !h6scs || !h6if || !h6ifs) {
        return nullptr;
    }
    setSlotRef(holder6, h6ip->getSlotId(), ipArr);
    setSlotInt(holder6, h6sc->getSlotId(), static_cast<int>(scopeId));
    setSlotInt(holder6, h6scs->getSlotId(), scopeId != 0 ? 1 : 0);
    setSlotRef(holder6, h6if->getSlotId(), nullptr);
    setSlotInt(holder6, h6ifs->getSlotId(), 0);
    FieldInfo* iHolder = findInstanceField(inet6Cl, "holder", "Ljava/net/InetAddress$InetAddressHolder;");
    FieldInfo* iHolder6 = findInstanceField(inet6Cl, "holder6", "Ljava/net/Inet6Address$Inet6AddressHolder;");
    if (!iHolder || !iHolder6) {
        return nullptr;
    }
    setSlotRef(inet6, iHolder->getSlotId(), holder);
    setSlotRef(inet6, iHolder6->getSlotId(), holder6);
    return inet6;
}

Object* inetFromSockaddr(ClassLoader* cl, JavaHeap* heap, const struct sockaddr_storage& ss, int* outPort) {
    if (ss.ss_family == AF_INET) {
        const auto& sin = *reinterpret_cast<const struct sockaddr_in*>(&ss);
        uint8_t b[4];
        std::memcpy(b, &sin.sin_addr, 4);
        char host[INET_ADDRSTRLEN];
        std::string h = "0.0.0.0";
        if (inet_ntop(AF_INET, &sin.sin_addr, host, sizeof(host)) != nullptr) {
            h = std::string(host);
        }
        if (outPort) {
            *outPort = static_cast<int>(ntohs(sin.sin_port));
        }
        return makeInet4Address(cl, heap, h, b);
    }
    if (ss.ss_family == AF_INET6) {
        const auto& sin6 = *reinterpret_cast<const struct sockaddr_in6*>(&ss);
        uint8_t b[16];
        std::memcpy(b, &sin6.sin6_addr, 16);
        char host[INET6_ADDRSTRLEN];
        std::string h = "::";
        if (inet_ntop(AF_INET6, &sin6.sin6_addr, host, sizeof(host)) != nullptr) {
            h = std::string(host);
        }
        if (outPort) {
            *outPort = static_cast<int>(ntohs(sin6.sin6_port));
        }
        return makeInet6Address(cl, heap, h, b, sin6.sin6_scope_id);
    }
    if (outPort) {
        *outPort = 0;
    }
    return nullptr;
}

bool fillSockaddrIn6FromInet6Java(Object* addr, int port, struct sockaddr_in6* out) {
    if (!addr || !out) {
        return false;
    }
    FieldInfo* h6f = findInstanceField(addr->getJavaClass(), "holder6", "Ljava/net/Inet6Address$Inet6AddressHolder;");
    if (!h6f) {
        return false;
    }
    Object* holder6 = getSlotRef(addr, h6f->getSlotId());
    if (!holder6) {
        return false;
    }
    JavaClass* h6cl = holder6->getJavaClass();
    FieldInfo* ipf = findInstanceField(h6cl, "ipaddress", "[B");
    FieldInfo* scf = findInstanceField(h6cl, "scope_id", "I");
    if (!ipf || !scf) {
        return false;
    }
    Object* ipArr = getSlotRef(holder6, ipf->getSlotId());
    if (!ipArr || ipArr->getJavaClass()->getThisClassName() != "[B" || ipArr->getArrayLength() != 16) {
        return false;
    }
    std::memset(out, 0, sizeof(*out));
    out->sin6_family = AF_INET6;
    out->sin6_port = htons(static_cast<uint16_t>(port));
    std::memcpy(&out->sin6_addr, ipArr->getData(), 16);
    out->sin6_scope_id = static_cast<uint32_t>(getSlotInt(holder6, scf->getSlotId()));
    return true;
}

bool inetAddressToSockaddr(Object* addr, int port, bool preferIPv6, struct sockaddr_storage* ss, socklen_t* outLen) {
    if (!addr || !ss || !outLen) {
        return false;
    }
    FieldInfo* hf = findInstanceField(addr->getJavaClass(), "holder", "Ljava/net/InetAddress$InetAddressHolder;");
    if (!hf) {
        return false;
    }
    Object* holder = getSlotRef(addr, hf->getSlotId());
    if (!holder) {
        return false;
    }
    JavaClass* hcl = holder->getJavaClass();
    FieldInfo* af = findInstanceField(hcl, "address", "I");
    FieldInfo* ff = findInstanceField(hcl, "family", "I");
    if (!af || !ff) {
        return false;
    }
    const int fam = getSlotInt(holder, ff->getSlotId());
    if (fam == 1) {
        const int packed = getSlotInt(holder, af->getSlotId());
        auto* sin = reinterpret_cast<struct sockaddr_in*>(ss);
        std::memset(sin, 0, sizeof(*sin));
        sin->sin_family = AF_INET;
        sin->sin_port = htons(static_cast<uint16_t>(port));
        sin->sin_addr.s_addr = htonl(static_cast<uint32_t>(packed));
        *outLen = sizeof(struct sockaddr_in);
        return true;
    }
    if (fam == 2) {
        auto* sin6 = reinterpret_cast<struct sockaddr_in6*>(ss);
        if (!fillSockaddrIn6FromInet6Java(addr, port, sin6)) {
            return false;
        }
        *outLen = sizeof(struct sockaddr_in6);
        return true;
    }
    (void)preferIPv6;
    return false;
}

Object* makeInetSocketAddress(ClassLoader* cl, JavaHeap* heap, Object* inetAddr, int port) {
    if (!cl || !inetAddr) {
        return nullptr;
    }
    JavaClass* holderCl = cl->loadClass("java/net/InetSocketAddress$InetSocketAddressHolder");
    JavaClass* isaCl = cl->loadClass("java/net/InetSocketAddress");
    if (!holderCl || !isaCl) {
        return nullptr;
    }
    Object* h = holderCl->createNewJavaObjectInstance();
    Object* isa = isaCl->createNewJavaObjectInstance();
    if (!h || !isa) {
        return nullptr;
    }
    FieldInfo* hn = findInstanceField(holderCl, "hostname", "Ljava/lang/String;");
    FieldInfo* ha = findInstanceField(holderCl, "addr", "Ljava/net/InetAddress;");
    FieldInfo* hp = findInstanceField(holderCl, "port", "I");
    FieldInfo* ihf = findInstanceField(isaCl, "holder", "Ljava/net/InetSocketAddress$InetSocketAddressHolder;");
    if (!hn || !ha || !hp || !ihf) {
        return nullptr;
    }
    setSlotRef(h, hn->getSlotId(), nullptr);
    setSlotRef(h, ha->getSlotId(), inetAddr);
    setSlotInt(h, hp->getSlotId(), port);
    setSlotRef(isa, ihf->getSlotId(), h);
    return isa;
}

int convertReadWriteReturn(ssize_t n, bool reading) {
    if (n >= 0) {
        return static_cast<int>(n);
    }
    if (errno == EINTR) {
        return IOS_INTERRUPTED;
    }
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
        return IOS_UNAVAILABLE;
    }
    if (reading && (errno == ECONNRESET || errno == EPIPE)) {
        return -1;
    }
    return -1;
}

long convertLongReadWriteReturn(ssize_t n, bool reading) {
    return static_cast<long>(convertReadWriteReturn(n, reading));
}

} // namespace

#ifndef _WIN32

void native_Net_initIDs(const NativeCallContext&) {}

void native_Net_isIPv6Available0(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(1);
    }
}

void native_Net_isReusePortAvailable0(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(1);
    }
}

void native_Net_isExclusiveBindAvailable(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(-1);
    }
}

void native_Net_shouldSetBothIPv4AndIPv6Options0(const NativeCallContext& ctx) {
    if (ctx.caller) {
#if defined(__linux__)
        ctx.caller->getOperandStack()->pushInt(1);
#else
        ctx.caller->getOperandStack()->pushInt(0);
#endif
    }
}

void native_Net_canIPv6SocketJoinIPv4Group0(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(1);
    }
}

void native_Net_canJoin6WithIPv4Group0(const NativeCallContext& ctx) {
    if (ctx.caller) {
#if defined(__APPLE__) || defined(__linux__)
        ctx.caller->getOperandStack()->pushInt(1);
#else
        ctx.caller->getOperandStack()->pushInt(0);
#endif
    }
}

void native_Net_canUseIPv6OptionsWithIPv4LocalAddress0(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(1);
    }
}

void native_Net_socket0(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    const int preferIPv6 = lvt->getInt(0);
    const int stream = lvt->getInt(1);
    const int reuse = lvt->getInt(2);
    (void)lvt->getInt(3);
    const int type = stream ? SOCK_STREAM : SOCK_DGRAM;
    const int domain = (preferIPv6 != 0) ? AF_INET6 : AF_INET;
    const int fd = static_cast<int>(::socket(domain, type, 0));
    if (fd < 0) {
        ctx.caller->getOperandStack()->pushInt(-1);
        return;
    }
    if (domain == AF_INET6) {
        int v6only = 0;
        (void)::setsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY, &v6only, sizeof(v6only));
    }
    if (reuse) {
        int one = 1;
        (void)::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
    }
    ctx.caller->getOperandStack()->pushInt(fd);
}

void native_Net_bind0(const NativeCallContext& ctx) {
    if (!ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    Object* fdObj = lvt->getRef(0);
    const int preferIPv6 = lvt->getInt(1);
    (void)lvt->getInt(2);
    Object* addr = lvt->getRef(3);
    const int port = lvt->getInt(4);
    const int sfd = getFdFromFileDescriptor(fdObj);
    if (sfd < 0 || !addr) {
        return;
    }
    struct sockaddr_storage ss {};
    socklen_t len = sizeof(ss);
    if (!inetAddressToSockaddr(addr, port, preferIPv6 != 0, &ss, &len)) {
        return;
    }
    (void)::bind(sfd, reinterpret_cast<struct sockaddr*>(&ss), len);
}

void native_Net_listen(const NativeCallContext& ctx) {
    if (!ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    Object* fdObj = lvt->getRef(0);
    const int backlog = lvt->getInt(1);
    const int sfd = getFdFromFileDescriptor(fdObj);
    if (sfd >= 0) {
        (void)::listen(sfd, backlog < 1 ? 50 : backlog);
    }
}

void native_Net_connect0(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    const int preferIPv6 = lvt->getInt(0);
    Object* fdObj = lvt->getRef(1);
    Object* addr = lvt->getRef(2);
    const int port = lvt->getInt(3);
    const int sfd = getFdFromFileDescriptor(fdObj);
    if (sfd < 0 || !addr) {
        ctx.caller->getOperandStack()->pushInt(IOS_UNAVAILABLE);
        return;
    }
    struct sockaddr_storage ss {};
    socklen_t len = sizeof(ss);
    if (!inetAddressToSockaddr(addr, port, preferIPv6 != 0, &ss, &len)) {
        ctx.caller->getOperandStack()->pushInt(IOS_UNAVAILABLE);
        return;
    }
    const int r = ::connect(sfd, reinterpret_cast<struct sockaddr*>(&ss), len);
    if (r == 0) {
        ctx.caller->getOperandStack()->pushInt(1);
        return;
    }
    if (errno == EINPROGRESS) {
        ctx.caller->getOperandStack()->pushInt(IOS_UNAVAILABLE);
        return;
    }
    if (errno == EINTR) {
        ctx.caller->getOperandStack()->pushInt(IOS_INTERRUPTED);
        return;
    }
    ctx.caller->getOperandStack()->pushInt(IOS_UNAVAILABLE);
}

void native_Net_accept(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    Object* lfdObj = lvt->getRef(0);
    Object* newFdObj = lvt->getRef(1);
    Object* arrObj = lvt->getRef(2);
    const int ls = getFdFromFileDescriptor(lfdObj);
    if (ls < 0 || !newFdObj || !arrObj || arrObj->getJavaClass()->getThisClassName() != "[Ljava/net/InetSocketAddress;" ||
        arrObj->getArrayLength() < 1) {
        ctx.caller->getOperandStack()->pushInt(IOS_UNAVAILABLE);
        return;
    }
    struct sockaddr_storage peer {};
    socklen_t plen = sizeof(peer);
    const int cs = ::accept(ls, reinterpret_cast<struct sockaddr*>(&peer), &plen);
    if (cs < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            ctx.caller->getOperandStack()->pushInt(IOS_UNAVAILABLE);
            return;
        }
        if (errno == EINTR) {
            ctx.caller->getOperandStack()->pushInt(IOS_INTERRUPTED);
            return;
        }
        ctx.caller->getOperandStack()->pushInt(IOS_UNAVAILABLE);
        return;
    }
    setFdOnFileDescriptor(newFdObj, cs);
    ClassLoader* cl = loaderFromCallee(ctx);
    JavaHeap* heap = heapFromCallee(ctx);
    int rport = 0;
    Object* ia = inetFromSockaddr(cl, heap, peer, &rport);
    Object* isa = makeInetSocketAddress(cl, heap, ia, rport);
    auto** refs = reinterpret_cast<Object**>(arrObj->getData());
    refs[0] = isa;
    ctx.caller->getOperandStack()->pushInt(1);
}

void native_Net_shutdown(const NativeCallContext& ctx) {
    if (!ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    Object* fdObj = lvt->getRef(0);
    const int how = lvt->getInt(1);
    const int sfd = getFdFromFileDescriptor(fdObj);
    if (sfd >= 0) {
        int k = SHUT_RDWR;
        if (how == 0) {
            k = SHUT_RD;
        } else if (how == 1) {
            k = SHUT_WR;
        }
        if (::shutdown(sfd, k) < 0 && errno != ENOTCONN) {
            // ignore
        }
    }
}

void native_Net_localPort(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    Object* fdObj = ctx.callee->getLocalVariableTables()->getRef(0);
    const int sfd = getFdFromFileDescriptor(fdObj);
    struct sockaddr_storage ss {};
    socklen_t len = sizeof(ss);
    if (sfd < 0 || ::getsockname(sfd, reinterpret_cast<struct sockaddr*>(&ss), &len) != 0) {
        ctx.caller->getOperandStack()->pushInt(-1);
        return;
    }
    int p = 0;
    if (ss.ss_family == AF_INET) {
        p = static_cast<int>(ntohs(reinterpret_cast<struct sockaddr_in*>(&ss)->sin_port));
    } else if (ss.ss_family == AF_INET6) {
        p = static_cast<int>(ntohs(reinterpret_cast<struct sockaddr_in6*>(&ss)->sin6_port));
    }
    ctx.caller->getOperandStack()->pushInt(p);
}

void native_Net_localInetAddress(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    Object* fdObj = ctx.callee->getLocalVariableTables()->getRef(0);
    const int sfd = getFdFromFileDescriptor(fdObj);
    ClassLoader* cl = loaderFromCallee(ctx);
    JavaHeap* heap = heapFromCallee(ctx);
    struct sockaddr_storage ss {};
    socklen_t len = sizeof(ss);
    if (sfd < 0 || ::getsockname(sfd, reinterpret_cast<struct sockaddr*>(&ss), &len) != 0) {
        ctx.caller->getOperandStack()->pushRef(nullptr);
        return;
    }
    ctx.caller->getOperandStack()->pushRef(inetFromSockaddr(cl, heap, ss, nullptr));
}

void native_Net_remotePort(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    Object* fdObj = ctx.callee->getLocalVariableTables()->getRef(0);
    const int sfd = getFdFromFileDescriptor(fdObj);
    struct sockaddr_storage ss {};
    socklen_t len = sizeof(ss);
    if (sfd < 0 || ::getpeername(sfd, reinterpret_cast<struct sockaddr*>(&ss), &len) != 0) {
        ctx.caller->getOperandStack()->pushInt(-1);
        return;
    }
    int p = 0;
    if (ss.ss_family == AF_INET) {
        p = static_cast<int>(ntohs(reinterpret_cast<struct sockaddr_in*>(&ss)->sin_port));
    } else if (ss.ss_family == AF_INET6) {
        p = static_cast<int>(ntohs(reinterpret_cast<struct sockaddr_in6*>(&ss)->sin6_port));
    }
    ctx.caller->getOperandStack()->pushInt(p);
}

void native_Net_remoteInetAddress(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    Object* fdObj = ctx.callee->getLocalVariableTables()->getRef(0);
    const int sfd = getFdFromFileDescriptor(fdObj);
    ClassLoader* cl = loaderFromCallee(ctx);
    JavaHeap* heap = heapFromCallee(ctx);
    struct sockaddr_storage ss {};
    socklen_t len = sizeof(ss);
    if (sfd < 0 || ::getpeername(sfd, reinterpret_cast<struct sockaddr*>(&ss), &len) != 0) {
        ctx.caller->getOperandStack()->pushRef(nullptr);
        return;
    }
    ctx.caller->getOperandStack()->pushRef(inetFromSockaddr(cl, heap, ss, nullptr));
}

void native_Net_getIntOption0(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    Object* fdObj = lvt->getRef(0);
    (void)lvt->getInt(1);
    const int level = lvt->getInt(2);
    const int opt = lvt->getInt(3);
    const int sfd = getFdFromFileDescriptor(fdObj);
    if (sfd < 0) {
        ctx.caller->getOperandStack()->pushInt(-1);
        return;
    }
    if (level == SOL_SOCKET && opt == SO_LINGER) {
        struct linger lg {};
        socklen_t lglen = sizeof(lg);
        if (::getsockopt(sfd, SOL_SOCKET, SO_LINGER, &lg, &lglen) != 0) {
            ctx.caller->getOperandStack()->pushInt(-1);
            return;
        }
        ctx.caller->getOperandStack()->pushInt(lg.l_onoff ? static_cast<int>(lg.l_linger) : -1);
        return;
    }
    int val = 0;
    socklen_t vlen = sizeof(val);
    if (::getsockopt(sfd, level, opt, &val, &vlen) != 0) {
        ctx.caller->getOperandStack()->pushInt(-1);
        return;
    }
    ctx.caller->getOperandStack()->pushInt(val);
}

void native_Net_setIntOption0(const NativeCallContext& ctx) {
    if (!ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    Object* fdObj = lvt->getRef(0);
    (void)lvt->getInt(1);
    const int level = lvt->getInt(2);
    const int opt = lvt->getInt(3);
    const int arg = lvt->getInt(4);
    (void)lvt->getInt(5);
    const int sfd = getFdFromFileDescriptor(fdObj);
    if (sfd < 0) {
        return;
    }
    if (level == SOL_SOCKET && opt == SO_LINGER) {
        struct linger lg {};
        if (arg >= 0) {
            lg.l_onoff = 1;
            lg.l_linger = static_cast<decltype(lg.l_linger)>(arg);
        }
        (void)::setsockopt(sfd, SOL_SOCKET, SO_LINGER, &lg, sizeof(lg));
        return;
    }
    int v = arg;
    (void)::setsockopt(sfd, level, opt, &v, sizeof(v));
}

void native_Net_poll(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    Object* fdObj = lvt->getRef(0);
    const int events = lvt->getInt(1);
    long timeoutMs = lvt->getLong(2);
    const int sfd = getFdFromFileDescriptor(fdObj);
    if (sfd < 0) {
        ctx.caller->getOperandStack()->pushInt(0);
        return;
    }
    if (timeoutMs < -1) {
        timeoutMs = -1;
    }
    if (timeoutMs > INT_MAX) {
        timeoutMs = INT_MAX;
    }
    struct pollfd pfd {};
    pfd.fd = sfd;
    pfd.events = static_cast<short>(events);
    const int rv = ::poll(&pfd, 1, static_cast<int>(timeoutMs));
    if (rv >= 0) {
        ctx.caller->getOperandStack()->pushInt(static_cast<int>(static_cast<unsigned short>(pfd.revents)));
        return;
    }
    if (errno == EINTR) {
        ctx.caller->getOperandStack()->pushInt(0);
        return;
    }
    ctx.caller->getOperandStack()->pushInt(0);
}

void native_Net_pollConnect(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    Object* fdObj = lvt->getRef(0);
    long timeoutMs = lvt->getLong(1);
    const int sfd = getFdFromFileDescriptor(fdObj);
    if (sfd < 0) {
        ctx.caller->getOperandStack()->pushInt(0);
        return;
    }
    if (timeoutMs < -1) {
        timeoutMs = -1;
    }
    if (timeoutMs > INT_MAX) {
        timeoutMs = INT_MAX;
    }
    struct pollfd poller {};
    poller.fd = sfd;
    poller.events = POLLOUT;
    const int result = ::poll(&poller, 1, static_cast<int>(timeoutMs));
    if (result > 0) {
        int error = 0;
        socklen_t n = sizeof(error);
        if (::getsockopt(sfd, SOL_SOCKET, SO_ERROR, &error, &n) != 0 || error != 0) {
            ctx.caller->getOperandStack()->pushInt(0);
            return;
        }
        if ((poller.revents & POLLHUP) != 0) {
            ctx.caller->getOperandStack()->pushInt(0);
            return;
        }
        ctx.caller->getOperandStack()->pushInt(1);
        return;
    }
    ctx.caller->getOperandStack()->pushInt(0);
}

void native_Net_available(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    Object* fdObj = ctx.callee->getLocalVariableTables()->getRef(0);
    const int sfd = getFdFromFileDescriptor(fdObj);
    int n = 0;
    if (sfd >= 0) {
        (void)::ioctl(sfd, FIONREAD, &n);
    }
    ctx.caller->getOperandStack()->pushInt(n);
}

void native_Net_sendOOB(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    Object* fdObj = lvt->getRef(0);
    const int b = lvt->getInt(1);
    const int sfd = getFdFromFileDescriptor(fdObj);
    int out = -1;
    if (sfd >= 0) {
        const char c = static_cast<char>(b & 255);
        out = static_cast<int>(::send(sfd, &c, 1, MSG_OOB));
    }
    ctx.caller->getOperandStack()->pushInt(out);
}

void native_Net_discardOOB(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(0);
    }
    (void)ctx;
}

void native_Net_joinOrDrop4(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(0);
    }
    (void)ctx;
}

void native_Net_blockOrUnblock4(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(0);
    }
    (void)ctx;
}

void native_Net_joinOrDrop6(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(0);
    }
    (void)ctx;
}

void native_Net_blockOrUnblock6(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(0);
    }
    (void)ctx;
}

void native_Net_setInterface4(const NativeCallContext& ctx) {
    (void)ctx;
}

void native_Net_getInterface4(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(-1);
    }
}

void native_Net_setInterface6(const NativeCallContext& ctx) {
    (void)ctx;
}

void native_Net_getInterface6(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(-1);
    }
}

void native_Net_pollinValue(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(POLLIN);
    }
}

void native_Net_polloutValue(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(POLLOUT);
    }
}

void native_Net_pollerrValue(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(POLLERR);
    }
}

void native_Net_pollhupValue(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(POLLHUP);
    }
}

void native_Net_pollnvalValue(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(POLLNVAL);
    }
}

void native_Net_pollconnValue(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(POLLOUT);
    }
}

void native_SocketDispatcher_read0(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    Object* fdObj = lvt->getRef(0);
    const long addrLong = lvt->getLong(1);
    const int len = lvt->getInt(3);
    void* buf = reinterpret_cast<void*>(static_cast<uintptr_t>(addrLong));
    const int sfd = getFdFromFileDescriptor(fdObj);
    if (sfd < 0 || !buf || len <= 0) {
        ctx.caller->getOperandStack()->pushInt(-1);
        return;
    }
    const ssize_t n = ::read(sfd, buf, static_cast<size_t>(len));
    ctx.caller->getOperandStack()->pushInt(convertReadWriteReturn(n, true));
}

void native_SocketDispatcher_readv0(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    Object* fdObj = lvt->getRef(0);
    const long iovAddr = lvt->getLong(1);
    const int iovcnt = lvt->getInt(3);
    auto* iov = reinterpret_cast<struct iovec*>(static_cast<uintptr_t>(iovAddr));
    const int sfd = getFdFromFileDescriptor(fdObj);
    if (sfd < 0 || !iov || iovcnt <= 0) {
        ctx.caller->getOperandStack()->pushLong(-1);
        return;
    }
#if defined(IOV_MAX)
    const int cnt = iovcnt > IOV_MAX ? IOV_MAX : iovcnt;
#else
    const int cnt = iovcnt;
#endif
    const ssize_t n = ::readv(sfd, iov, cnt);
    ctx.caller->getOperandStack()->pushLong(convertLongReadWriteReturn(n, true));
}

void native_SocketDispatcher_write0(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    Object* fdObj = lvt->getRef(0);
    const long addrLong = lvt->getLong(1);
    const int len = lvt->getInt(3);
    void* buf = reinterpret_cast<void*>(static_cast<uintptr_t>(addrLong));
    const int sfd = getFdFromFileDescriptor(fdObj);
    if (sfd < 0 || !buf || len <= 0) {
        ctx.caller->getOperandStack()->pushInt(-1);
        return;
    }
    const ssize_t n = ::write(sfd, buf, static_cast<size_t>(len));
    ctx.caller->getOperandStack()->pushInt(convertReadWriteReturn(n, false));
}

void native_SocketDispatcher_writev0(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    auto* lvt = ctx.callee->getLocalVariableTables();
    Object* fdObj = lvt->getRef(0);
    const long iovAddr = lvt->getLong(1);
    const int iovcnt = lvt->getInt(3);
    auto* iov = reinterpret_cast<struct iovec*>(static_cast<uintptr_t>(iovAddr));
    const int sfd = getFdFromFileDescriptor(fdObj);
    if (sfd < 0 || !iov || iovcnt <= 0) {
        ctx.caller->getOperandStack()->pushLong(-1);
        return;
    }
#if defined(IOV_MAX)
    const int cnt = iovcnt > IOV_MAX ? IOV_MAX : iovcnt;
#else
    const int cnt = iovcnt;
#endif
    const ssize_t n = ::writev(sfd, iov, cnt);
    ctx.caller->getOperandStack()->pushLong(convertLongReadWriteReturn(n, false));
}

#else

void native_Net_initIDs(const NativeCallContext&) {}
void native_Net_isIPv6Available0(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(0);
    }
}
void native_Net_isReusePortAvailable0(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(0);
    }
}
void native_Net_isExclusiveBindAvailable(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(-1);
    }
}
void native_Net_shouldSetBothIPv4AndIPv6Options0(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(0);
    }
}
void native_Net_canIPv6SocketJoinIPv4Group0(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(0);
    }
}
void native_Net_canJoin6WithIPv4Group0(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(0);
    }
}
void native_Net_canUseIPv6OptionsWithIPv4LocalAddress0(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(0);
    }
}
void native_Net_socket0(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(-1);
    }
    (void)ctx;
}
void native_Net_bind0(const NativeCallContext& ctx) {
    (void)ctx;
}
void native_Net_listen(const NativeCallContext& ctx) {
    (void)ctx;
}
void native_Net_connect0(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(-1);
    }
}
void native_Net_accept(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(-1);
    }
    (void)ctx;
}
void native_Net_shutdown(const NativeCallContext& ctx) {
    (void)ctx;
}
void native_Net_localPort(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(-1);
    }
}
void native_Net_localInetAddress(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushRef(nullptr);
    }
}
void native_Net_remotePort(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(-1);
    }
}
void native_Net_remoteInetAddress(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushRef(nullptr);
    }
}
void native_Net_getIntOption0(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(-1);
    }
}
void native_Net_setIntOption0(const NativeCallContext& ctx) {
    (void)ctx;
}
void native_Net_poll(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(0);
    }
}
void native_Net_pollConnect(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(0);
    }
}
void native_Net_available(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(0);
    }
}
void native_Net_sendOOB(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(-1);
    }
}
void native_Net_discardOOB(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(0);
    }
}
void native_Net_joinOrDrop4(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(0);
    }
}
void native_Net_blockOrUnblock4(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(0);
    }
}
void native_Net_joinOrDrop6(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(0);
    }
}
void native_Net_blockOrUnblock6(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(0);
    }
}
void native_Net_setInterface4(const NativeCallContext& ctx) {
    (void)ctx;
}
void native_Net_getInterface4(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(-1);
    }
}
void native_Net_setInterface6(const NativeCallContext& ctx) {
    (void)ctx;
}
void native_Net_getInterface6(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(-1);
    }
}
void native_Net_pollinValue(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(0);
    }
}
void native_Net_polloutValue(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(0);
    }
}
void native_Net_pollerrValue(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(0);
    }
}
void native_Net_pollhupValue(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(0);
    }
}
void native_Net_pollnvalValue(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(0);
    }
}
void native_Net_pollconnValue(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(0);
    }
}
void native_SocketDispatcher_read0(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(-1);
    }
}
void native_SocketDispatcher_readv0(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushLong(0);
    }
}
void native_SocketDispatcher_write0(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(-1);
    }
}
void native_SocketDispatcher_writev0(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushLong(0);
    }
}

#endif

} // namespace Native
} // namespace Runtime
