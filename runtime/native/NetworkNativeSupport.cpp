//
// java.net / java.io.FileDescriptor 套接字与地址解析 native（POSIX 客户端为主）。
//

#include "NetworkNativeSupport.h"

#include "../ClassLoader.h"
#include "../JavaThread.h"
#include "../LocalVariableTables.h"
#include "../Object.h"
#include "../Slots.h"
#include "../heap/ClassMember.h"
#include "../heap/JavaClass.h"
#include "../heap/StringConstantPools.h"

#include <cerrno>
#include <cstring>
#include <string>
#include <utility>
#include <vector>

#ifndef _WIN32
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#endif

namespace Runtime {
namespace Native {
namespace {

using Heap::FieldInfo;

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

std::string stringFromArg(Object* strObj) {
    if (!strObj) {
        return "";
    }
    return Heap::JString::getJString()->getJavaString(strObj);
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

Object* getSocketImplFdObject(Object* socketImpl) {
    if (!socketImpl) {
        return nullptr;
    }
    FieldInfo* f = findInstanceField(socketImpl->getJavaClass(), "fd", "Ljava/io/FileDescriptor;");
    if (!f) {
        return nullptr;
    }
    return getSlotRef(socketImpl, f->getSlotId());
}

int ipv4BytesToHolderInt(const uint8_t b[4]) {
    return (static_cast<int>(b[0] & 255) << 24) | (static_cast<int>(b[1] & 255) << 16) |
           (static_cast<int>(b[2] & 255) << 8) | (static_cast<int>(b[3] & 255));
}

/** 构造 java.net.Inet4Address + InetAddress$InetAddressHolder（与 JDK 21 字段布局一致）。 */
Object* makeInet4Address(ClassLoader* cl, JavaHeap* heap, const std::string& hostOrNumeric,
                         const uint8_t ipv4[4]) {
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
    setSlotInt(holder, hFam->getSlotId(), 1); // IPv4

    FieldInfo* iHolder = findInstanceField(inet4Cl, "holder", "Ljava/net/InetAddress$InetAddressHolder;");
    if (!iHolder) {
        return nullptr;
    }
    setSlotRef(inet4, iHolder->getSlotId(), holder);
    return inet4;
}

/** InetAddress.IPv6 == 2；16 字节网络序地址 + 可选 sin6_scope_id。 */
Object* makeInet6Address(ClassLoader* cl, JavaHeap* heap, const std::string& hostOrNumeric,
                         const uint8_t ipv6[16], uint32_t scopeId) {
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

#ifndef _WIN32

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

Object* makeInet4FromSockaddrIn(ClassLoader* cl, JavaHeap* heap, const struct sockaddr_in& sin) {
    uint8_t b[4];
    std::memcpy(b, &sin.sin_addr, 4);
    char host[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, &sin.sin_addr, host, sizeof(host)) == nullptr) {
        return makeInet4Address(cl, heap, "0.0.0.0", b);
    }
    return makeInet4Address(cl, heap, std::string(host), b);
}

Object* makeInet6FromSockaddrIn6(ClassLoader* cl, JavaHeap* heap, const struct sockaddr_in6& sin6) {
    uint8_t b[16];
    std::memcpy(b, &sin6.sin6_addr, 16);
    char host[INET6_ADDRSTRLEN];
    std::string h = "::";
    if (inet_ntop(AF_INET6, &sin6.sin6_addr, host, sizeof(host)) != nullptr) {
        h = std::string(host);
    }
    return makeInet6Address(cl, heap, h, b, sin6.sin6_scope_id);
}

#endif

} // namespace

void native_InetAddress_init(const NativeCallContext&) {}

void native_Inet4Address_init(const NativeCallContext&) {}

void native_Inet6Address_init(const NativeCallContext&) {}

void native_Inet4AddressImpl_getLocalHostName(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    ClassLoader* cl = loaderFromCallee(ctx);
    JavaHeap* heap = heapFromCallee(ctx);
    ctx.caller->getOperandStack()->pushRef(makeJavaString(cl, heap, "localhost"));
}

void native_Inet4AddressImpl_lookupAllHostAddr(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    Object* hostObj = ctx.callee->getLocalVariableTables()->getRef(1);
    ClassLoader* cl = loaderFromCallee(ctx);
    JavaHeap* heap = heapFromCallee(ctx);
    const std::string host = stringFromArg(hostObj);
    if (host.empty() || !cl) {
        ctx.caller->getOperandStack()->pushRef(nullptr);
        return;
    }
#ifndef _WIN32
    addrinfo hint {};
    hint.ai_family = AF_UNSPEC;
    hint.ai_socktype = SOCK_STREAM;
    addrinfo* res = nullptr;
    const int gai = ::getaddrinfo(host.c_str(), nullptr, &hint, &res);
    if (gai != 0 || !res) {
        ctx.caller->getOperandStack()->pushRef(nullptr);
        if (res) {
            ::freeaddrinfo(res);
        }
        return;
    }
    std::vector<Object*> addrs;
    for (addrinfo* p = res; p; p = p->ai_next) {
        if (p->ai_family == AF_INET) {
            const auto* sin = reinterpret_cast<struct sockaddr_in*>(p->ai_addr);
            if (Object* ia = makeInet4FromSockaddrIn(cl, heap, *sin)) {
                addrs.push_back(ia);
            }
        } else if (p->ai_family == AF_INET6) {
            const auto* sin6 = reinterpret_cast<struct sockaddr_in6*>(p->ai_addr);
            if (Object* ia = makeInet6FromSockaddrIn6(cl, heap, *sin6)) {
                addrs.push_back(ia);
            }
        }
    }
    ::freeaddrinfo(res);
    JavaClass* arrCl = cl->loadClass("[Ljava/net/InetAddress;");
    if (!arrCl || addrs.empty()) {
        ctx.caller->getOperandStack()->pushRef(nullptr);
        return;
    }
    Object* arr = arrCl->createArray(static_cast<int>(addrs.size()));
    auto** refs = reinterpret_cast<Object**>(arr->getData());
    for (size_t i = 0; i < addrs.size(); ++i) {
        refs[i] = addrs[i];
    }
    ctx.caller->getOperandStack()->pushRef(arr);
#else
    (void)heap;
    ctx.caller->getOperandStack()->pushRef(nullptr);
#endif
}

void native_Inet4AddressImpl_getHostByAddr(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    Object* ba = ctx.callee->getLocalVariableTables()->getRef(1);
    ClassLoader* cl = loaderFromCallee(ctx);
    JavaHeap* heap = heapFromCallee(ctx);
    if (!ba || !ba->getJavaClass() || ba->getJavaClass()->getThisClassName() != "[B" || !cl) {
        ctx.caller->getOperandStack()->pushRef(nullptr);
        return;
    }
    const int n = ba->getArrayLength();
    auto* raw = reinterpret_cast<uint8_t*>(ba->getData());
    if (n != 4 || !raw) {
        ctx.caller->getOperandStack()->pushRef(nullptr);
        return;
    }
    char buf[64];
    std::snprintf(buf, sizeof(buf), "%u.%u.%u.%u", raw[0] & 255u, raw[1] & 255u, raw[2] & 255u, raw[3] & 255u);
    ctx.caller->getOperandStack()->pushRef(makeJavaString(cl, heap, std::string(buf)));
}

void native_Inet4AddressImpl_isReachable0(const NativeCallContext& ctx) {
    if (!ctx.caller) {
        return;
    }
    ctx.caller->getOperandStack()->pushInt(0);
}

void native_Inet6AddressImpl_getLocalHostName(const NativeCallContext& ctx) {
    native_Inet4AddressImpl_getLocalHostName(ctx);
}

void native_Inet6AddressImpl_lookupAllHostAddr(const NativeCallContext& ctx) {
    native_Inet4AddressImpl_lookupAllHostAddr(ctx);
}

void native_Inet6AddressImpl_getHostByAddr(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    Object* ba = ctx.callee->getLocalVariableTables()->getRef(1);
    ClassLoader* cl = loaderFromCallee(ctx);
    JavaHeap* heap = heapFromCallee(ctx);
    if (!ba || !ba->getJavaClass() || ba->getJavaClass()->getThisClassName() != "[B" || !cl) {
        ctx.caller->getOperandStack()->pushRef(nullptr);
        return;
    }
    const int n = ba->getArrayLength();
    auto* raw = reinterpret_cast<uint8_t*>(ba->getData());
#ifndef _WIN32
    if (n != 16 || !raw) {
        ctx.caller->getOperandStack()->pushRef(nullptr);
        return;
    }
    char buf[INET6_ADDRSTRLEN];
    if (inet_ntop(AF_INET6, raw, buf, sizeof(buf)) == nullptr) {
        ctx.caller->getOperandStack()->pushRef(nullptr);
        return;
    }
    ctx.caller->getOperandStack()->pushRef(makeJavaString(cl, heap, std::string(buf)));
#else
    (void)heap;
    (void)n;
    (void)raw;
    ctx.caller->getOperandStack()->pushRef(nullptr);
#endif
}

void native_Inet6AddressImpl_isReachable0(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(0);
    }
}

void native_PlainSocketImpl_socketCreate(const NativeCallContext& ctx) {
    if (!ctx.callee) {
        return;
    }
#ifndef _WIN32
    Object* impl = ctx.callee->getLocalVariableTables()->getRef(0);
    Object* fdObj = getSocketImplFdObject(impl);
    if (!fdObj) {
        return;
    }
    int sfd = ::socket(AF_INET6, SOCK_STREAM, 0);
    if (sfd >= 0) {
        int v6only = 0;
        (void)::setsockopt(sfd, IPPROTO_IPV6, IPV6_V6ONLY, &v6only, sizeof(v6only));
    } else {
        sfd = ::socket(AF_INET, SOCK_STREAM, 0);
    }
    if (sfd < 0) {
        return;
    }
    setFdOnFileDescriptor(fdObj, sfd);
#else
    (void)ctx;
#endif
}

void native_PlainSocketImpl_socketConnect(const NativeCallContext& ctx) {
    if (!ctx.callee) {
        return;
    }
#ifndef _WIN32
    Object* impl = ctx.callee->getLocalVariableTables()->getRef(0);
    Object* addr = ctx.callee->getLocalVariableTables()->getRef(1);
    const int port = ctx.callee->getLocalVariableTables()->getInt(2);
    (void)ctx.callee->getLocalVariableTables()->getInt(3); // timeout — 当前用阻塞 connect，忽略
    Object* fdObj = getSocketImplFdObject(impl);
    if (!fdObj || !addr) {
        return;
    }
    FieldInfo* hf = findInstanceField(addr->getJavaClass(), "holder", "Ljava/net/InetAddress$InetAddressHolder;");
    if (!hf) {
        return;
    }
    Object* holder = getSlotRef(addr, hf->getSlotId());
    if (!holder) {
        return;
    }
    JavaClass* hcl = holder->getJavaClass();
    FieldInfo* af = findInstanceField(hcl, "address", "I");
    FieldInfo* ff = findInstanceField(hcl, "family", "I");
    if (!af || !ff) {
        return;
    }
    const int fam = getSlotInt(holder, ff->getSlotId());
    const int sfd = getFdFromFileDescriptor(fdObj);
    if (sfd < 0) {
        return;
    }
    if (fam == 1) {
        const int packed = getSlotInt(holder, af->getSlotId());
        struct sockaddr_in sin {};
        sin.sin_family = AF_INET;
        sin.sin_port = htons(static_cast<uint16_t>(port));
        sin.sin_addr.s_addr = htonl(static_cast<uint32_t>(packed));
        if (::connect(sfd, reinterpret_cast<struct sockaddr*>(&sin), sizeof(sin)) != 0) {
            return;
        }
    } else if (fam == 2) {
        struct sockaddr_in6 sin6 {};
        if (!fillSockaddrIn6FromInet6Java(addr, port, &sin6)) {
            return;
        }
        if (::connect(sfd, reinterpret_cast<struct sockaddr*>(&sin6), sizeof(sin6)) != 0) {
            return;
        }
    } else {
        return;
    }
    FieldInfo* portF = findInstanceField(impl->getJavaClass(), "port", "I");
    if (portF) {
        setSlotInt(impl, portF->getSlotId(), port);
    }
#else
    (void)ctx;
#endif
}

void native_PlainSocketImpl_socketBind(const NativeCallContext& ctx) {
#ifndef _WIN32
    Object* impl = ctx.callee->getLocalVariableTables()->getRef(0);
    Object* addr = ctx.callee->getLocalVariableTables()->getRef(1);
    const int port = ctx.callee->getLocalVariableTables()->getInt(2);
    Object* fdObj = getSocketImplFdObject(impl);
    if (!fdObj || !addr) {
        return;
    }
    FieldInfo* hf = findInstanceField(addr->getJavaClass(), "holder", "Ljava/net/InetAddress$InetAddressHolder;");
    Object* holder = hf ? getSlotRef(addr, hf->getSlotId()) : nullptr;
    if (!holder) {
        return;
    }
    FieldInfo* af = findInstanceField(holder->getJavaClass(), "address", "I");
    FieldInfo* ff = findInstanceField(holder->getJavaClass(), "family", "I");
    if (!af || !ff) {
        return;
    }
    const int fam = getSlotInt(holder, ff->getSlotId());
    const int sfd = getFdFromFileDescriptor(fdObj);
    if (sfd < 0) {
        return;
    }
    if (fam == 1) {
        const int packed = getSlotInt(holder, af->getSlotId());
        struct sockaddr_in sin {};
        sin.sin_family = AF_INET;
        sin.sin_port = htons(static_cast<uint16_t>(port));
        sin.sin_addr.s_addr = htonl(static_cast<uint32_t>(packed));
        (void)::bind(sfd, reinterpret_cast<struct sockaddr*>(&sin), sizeof(sin));
    } else if (fam == 2) {
        struct sockaddr_in6 sin6 {};
        if (fillSockaddrIn6FromInet6Java(addr, port, &sin6)) {
            (void)::bind(sfd, reinterpret_cast<struct sockaddr*>(&sin6), sizeof(sin6));
        }
    }
#else
    (void)ctx;
#endif
}

void native_PlainSocketImpl_socketListen(const NativeCallContext& ctx) {
#ifndef _WIN32
    Object* impl = ctx.callee->getLocalVariableTables()->getRef(0);
    const int backlog = ctx.callee->getLocalVariableTables()->getInt(1);
    Object* fdObj = getSocketImplFdObject(impl);
    const int sfd = fdObj ? getFdFromFileDescriptor(fdObj) : -1;
    if (sfd >= 0) {
        (void)::listen(sfd, backlog < 1 ? 50 : backlog);
    }
#else
    (void)ctx;
#endif
}

void native_PlainSocketImpl_socketAccept(const NativeCallContext& ctx) {
#ifndef _WIN32
    Object* impl = ctx.callee->getLocalVariableTables()->getRef(0);
    Object* child = ctx.callee->getLocalVariableTables()->getRef(1);
    Object* lfdObj = getSocketImplFdObject(impl);
    if (!lfdObj || !child) {
        return;
    }
    const int ls = getFdFromFileDescriptor(lfdObj);
    if (ls < 0) {
        return;
    }
    struct sockaddr_storage peer {};
    socklen_t plen = sizeof(peer);
    const int cs = ::accept(ls, reinterpret_cast<struct sockaddr*>(&peer), &plen);
    if (cs < 0) {
        return;
    }
    Object* cfdObj = getSocketImplFdObject(child);
    if (!cfdObj) {
        ::close(cs);
        return;
    }
    setFdOnFileDescriptor(cfdObj, cs);
    ClassLoader* cl = loaderFromCallee(ctx);
    JavaHeap* heap = heapFromCallee(ctx);
    Object* peerInet = nullptr;
    if (peer.ss_family == AF_INET) {
        peerInet = makeInet4FromSockaddrIn(cl, heap, *reinterpret_cast<struct sockaddr_in*>(&peer));
    } else if (peer.ss_family == AF_INET6) {
        peerInet = makeInet6FromSockaddrIn6(cl, heap, *reinterpret_cast<struct sockaddr_in6*>(&peer));
    }
    FieldInfo* af = findInstanceField(child->getJavaClass(), "address", "Ljava/net/InetAddress;");
    FieldInfo* pf = findInstanceField(child->getJavaClass(), "port", "I");
    if (af) {
        setSlotRef(child, af->getSlotId(), peerInet);
    }
    if (pf) {
        int peerPort = 0;
        if (peer.ss_family == AF_INET) {
            peerPort = static_cast<int>(
                ntohs(reinterpret_cast<struct sockaddr_in*>(&peer)->sin_port));
        } else if (peer.ss_family == AF_INET6) {
            peerPort = static_cast<int>(
                ntohs(reinterpret_cast<struct sockaddr_in6*>(&peer)->sin6_port));
        }
        setSlotInt(child, pf->getSlotId(), peerPort);
    }
#else
    (void)ctx;
#endif
}

void native_PlainSocketImpl_socketAvailable(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    int n = 0;
#ifndef _WIN32
    Object* impl = ctx.callee->getLocalVariableTables()->getRef(0);
    Object* fdObj = getSocketImplFdObject(impl);
    const int sfd = fdObj ? getFdFromFileDescriptor(fdObj) : -1;
    if (sfd >= 0) {
        ::ioctl(sfd, FIONREAD, &n);
    }
#endif
    ctx.caller->getOperandStack()->pushInt(n);
}

void native_PlainSocketImpl_socketClose0(const NativeCallContext& ctx) {
#ifndef _WIN32
    Object* impl = ctx.callee->getLocalVariableTables()->getRef(0);
    (void)ctx.callee->getLocalVariableTables()->getInt(1);
    Object* fdObj = getSocketImplFdObject(impl);
    const int sfd = fdObj ? getFdFromFileDescriptor(fdObj) : -1;
    if (sfd >= 0) {
        ::close(sfd);
        setFdOnFileDescriptor(fdObj, -1);
    }
#else
    (void)ctx;
#endif
}

void native_PlainSocketImpl_socketShutdown(const NativeCallContext& ctx) {
#ifndef _WIN32
    Object* impl = ctx.callee->getLocalVariableTables()->getRef(0);
    const int how = ctx.callee->getLocalVariableTables()->getInt(1);
    Object* fdObj = getSocketImplFdObject(impl);
    const int sfd = fdObj ? getFdFromFileDescriptor(fdObj) : -1;
    if (sfd >= 0) {
        int k = SHUT_RDWR;
        if (how == 0) {
            k = SHUT_RD;
        } else if (how == 1) {
            k = SHUT_WR;
        }
        (void)::shutdown(sfd, k);
    }
#else
    (void)ctx;
#endif
}

void native_PlainSocketImpl_initProto(const NativeCallContext&) {}

void native_PlainSocketImpl_socketSetOption0(const NativeCallContext& ctx) {
#ifndef _WIN32
    Object* impl = ctx.callee->getLocalVariableTables()->getRef(0);
    const int opt = ctx.callee->getLocalVariableTables()->getInt(1);
    (void)ctx.callee->getLocalVariableTables()->getInt(2); // on
    Object* val = ctx.callee->getLocalVariableTables()->getRef(3);
    Object* fdObj = getSocketImplFdObject(impl);
    const int sfd = fdObj ? getFdFromFileDescriptor(fdObj) : -1;
    if (sfd < 0) {
        return;
    }
    // java.net.SocketOptions.SO_TIMEOUT == 4102 (0x1006)
    if (opt == 4102 && val) {
        FieldInfo* vf = findInstanceField(val->getJavaClass(), "value", "I");
        if (!vf) {
            return;
        }
        const int ms = getSlotInt(val, vf->getSlotId());
        struct timeval tv {};
        tv.tv_sec = ms / 1000;
        tv.tv_usec = (ms % 1000) * 1000;
        (void)::setsockopt(sfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
        (void)::setsockopt(sfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
    }
    if (opt == 0x0001 && val) { // TCP_NODELAY
        int flag = 1;
        if (FieldInfo* vf = findInstanceField(val->getJavaClass(), "value", "Z")) {
            flag = getSlotInt(val, vf->getSlotId()) ? 1 : 0;
        } else if (FieldInfo* vi = findInstanceField(val->getJavaClass(), "value", "I")) {
            flag = getSlotInt(val, vi->getSlotId()) ? 1 : 0;
        }
        (void)::setsockopt(sfd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));
    }
#else
    (void)ctx;
#endif
}

void native_PlainSocketImpl_socketGetOption(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    Object* impl = ctx.callee->getLocalVariableTables()->getRef(0);
    const int opt = ctx.callee->getLocalVariableTables()->getInt(1);
    (void)ctx.callee->getLocalVariableTables()->getRef(2);
#ifndef _WIN32
    Object* fdObj = getSocketImplFdObject(impl);
    const int sfd = fdObj ? getFdFromFileDescriptor(fdObj) : -1;
    if (sfd < 0) {
        ctx.caller->getOperandStack()->pushInt(-1);
        return;
    }
    if (opt == 1) { // SocketOptions.TCP_NODELAY
        int v = 0;
        socklen_t len = sizeof(v);
        if (::getsockopt(sfd, IPPROTO_TCP, TCP_NODELAY, &v, &len) != 0) {
            ctx.caller->getOperandStack()->pushInt(-1);
            return;
        }
        ctx.caller->getOperandStack()->pushInt(v != 0 ? 1 : 0);
        return;
    }
    if (opt == 4102) { // SO_TIMEOUT — 与 SO_RCVTIMEO 一致
        struct timeval tv {};
        socklen_t len = sizeof(tv);
        if (::getsockopt(sfd, SOL_SOCKET, SO_RCVTIMEO, &tv, &len) != 0) {
            ctx.caller->getOperandStack()->pushInt(-1);
            return;
        }
        const int ms = static_cast<int>(tv.tv_sec) * 1000 + static_cast<int>(tv.tv_usec) / 1000;
        ctx.caller->getOperandStack()->pushInt(ms);
        return;
    }
    if (opt == 128) { // SO_LINGER
        struct linger lg {};
        socklen_t len = sizeof(lg);
        if (::getsockopt(sfd, SOL_SOCKET, SO_LINGER, &lg, &len) != 0) {
            ctx.caller->getOperandStack()->pushInt(-1);
            return;
        }
        ctx.caller->getOperandStack()->pushInt(lg.l_onoff ? static_cast<int>(lg.l_linger) : -1);
        return;
    }
#else
    (void)impl;
    (void)opt;
#endif
    ctx.caller->getOperandStack()->pushInt(-1);
}

void native_PlainSocketImpl_socketSendUrgentData(const NativeCallContext& ctx) {
#ifndef _WIN32
    Object* impl = ctx.callee->getLocalVariableTables()->getRef(0);
    const int data = ctx.callee->getLocalVariableTables()->getInt(1);
    Object* fdObj = getSocketImplFdObject(impl);
    const int sfd = fdObj ? getFdFromFileDescriptor(fdObj) : -1;
    if (sfd >= 0) {
        const char c = static_cast<char>(data & 255);
        (void)::send(sfd, &c, 1, MSG_OOB);
    }
#else
    (void)ctx;
#endif
}

void native_SocketInputStream_socketRead0(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
#ifndef _WIN32
    Object* fdObj = ctx.callee->getLocalVariableTables()->getRef(1);
    Object* bufArr = ctx.callee->getLocalVariableTables()->getRef(2);
    const int off = ctx.callee->getLocalVariableTables()->getInt(3);
    const int len = ctx.callee->getLocalVariableTables()->getInt(4);
    (void)ctx.callee->getLocalVariableTables()->getInt(5);
    if (!bufArr || bufArr->getJavaClass()->getThisClassName() != "[B" || !fdObj) {
        ctx.caller->getOperandStack()->pushInt(-1);
        return;
    }
    const int sfd = getFdFromFileDescriptor(fdObj);
    if (sfd < 0) {
        ctx.caller->getOperandStack()->pushInt(-1);
        return;
    }
    auto* raw = reinterpret_cast<uint8_t*>(bufArr->getData());
    const int alen = bufArr->getArrayLength();
    if (!raw || off < 0 || len < 0 || off + len > alen) {
        ctx.caller->getOperandStack()->pushInt(-1);
        return;
    }
    const ssize_t n = ::recv(sfd, raw + off, static_cast<size_t>(len), 0);
    ctx.caller->getOperandStack()->pushInt(static_cast<int>(n));
#else
    ctx.caller->getOperandStack()->pushInt(-1);
#endif
}

void native_SocketInputStream_init(const NativeCallContext&) {}

void native_SocketOutputStream_socketWrite0(const NativeCallContext& ctx) {
#ifndef _WIN32
    if (!ctx.callee) {
        return;
    }
    Object* fdObj = ctx.callee->getLocalVariableTables()->getRef(1);
    Object* bufArr = ctx.callee->getLocalVariableTables()->getRef(2);
    const int off = ctx.callee->getLocalVariableTables()->getInt(3);
    const int len = ctx.callee->getLocalVariableTables()->getInt(4);
    if (!bufArr || bufArr->getJavaClass()->getThisClassName() != "[B" || !fdObj) {
        return;
    }
    const int sfd = getFdFromFileDescriptor(fdObj);
    auto* raw = reinterpret_cast<uint8_t*>(bufArr->getData());
    const int alen = bufArr->getArrayLength();
    if (sfd < 0 || !raw || off < 0 || len < 0 || off + len > alen) {
        return;
    }
    (void)::send(sfd, raw + off, static_cast<size_t>(len), 0);
#else
    (void)ctx;
#endif
}

void native_SocketOutputStream_init(const NativeCallContext&) {}

void native_NetworkInterface_init(const NativeCallContext&) {}

void native_NetworkInterface_getAll(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    ClassLoader* cl = loaderFromCallee(ctx);
    if (!cl) {
        ctx.caller->getOperandStack()->pushRef(nullptr);
        return;
    }
    JavaClass* arrCl = cl->loadClass("[Ljava/net/NetworkInterface;");
    if (!arrCl) {
        ctx.caller->getOperandStack()->pushRef(nullptr);
        return;
    }
    Object* arr = arrCl->createArray(0);
    ctx.caller->getOperandStack()->pushRef(arr);
}

void native_NetworkInterface_getByName0(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushRef(nullptr);
    }
    (void)ctx;
}

void native_NetworkInterface_getByIndex0(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushRef(nullptr);
    }
    (void)ctx;
}

void native_NetworkInterface_boundInetAddress0(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(0);
    }
    (void)ctx;
}

void native_NetworkInterface_getByInetAddress0(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushRef(nullptr);
    }
    (void)ctx;
}

void native_NetworkInterface_isUp0(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(0);
    }
    (void)ctx;
}

void native_NetworkInterface_isLoopback0(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(0);
    }
    (void)ctx;
}

void native_NetworkInterface_supportsMulticast0(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(0);
    }
    (void)ctx;
}

void native_NetworkInterface_isP2P0(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(0);
    }
    (void)ctx;
}

void native_NetworkInterface_getMacAddr0(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushRef(nullptr);
    }
    (void)ctx;
}

void native_NetworkInterface_getMTU0(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(0);
    }
    (void)ctx;
}

void native_DefaultProxySelector_init(const NativeCallContext& ctx) {
    if (ctx.caller) {
        ctx.caller->getOperandStack()->pushInt(1);
    }
}

void native_DefaultProxySelector_getSystemProxies(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    ClassLoader* cl = loaderFromCallee(ctx);
    if (!cl) {
        ctx.caller->getOperandStack()->pushRef(nullptr);
        return;
    }
    JavaClass* arrCl = cl->loadClass("[Ljava/net/Proxy;");
    if (!arrCl) {
        ctx.caller->getOperandStack()->pushRef(nullptr);
        return;
    }
    Object* arr = arrCl->createArray(0);
    ctx.caller->getOperandStack()->pushRef(arr);
}

void native_ResolverConfigurationImpl_fallbackDomain0(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    ClassLoader* cl = loaderFromCallee(ctx);
    JavaHeap* heap = heapFromCallee(ctx);
    ctx.caller->getOperandStack()->pushRef(makeJavaString(cl, heap, ""));
}

void native_FileDescriptor_initIDs(const NativeCallContext&) {}

void native_FileDescriptor_sync(const NativeCallContext& ctx) {
#ifndef _WIN32
    Object* self = ctx.callee->getLocalVariableTables()->getRef(0);
    const int fd = getFdFromFileDescriptor(self);
    if (fd >= 0) {
        (void)::fsync(fd);
    }
#else
    (void)ctx;
#endif
}

void native_FileDescriptor_getHandle(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    const int fd = ctx.callee->getLocalVariableTables()->getInt(0);
    ctx.caller->getOperandStack()->pushLong(static_cast<long>(fd));
}

void native_FileDescriptor_getAppend(const NativeCallContext& ctx) {
    if (!ctx.caller || !ctx.callee) {
        return;
    }
    (void)ctx.callee->getLocalVariableTables()->getInt(0);
    ctx.caller->getOperandStack()->pushInt(0);
}

void native_FileDescriptor_close0(const NativeCallContext& ctx) {
#ifndef _WIN32
    Object* self = ctx.callee->getLocalVariableTables()->getRef(0);
    const int fd = getFdFromFileDescriptor(self);
    if (fd >= 0) {
        ::close(fd);
        setFdOnFileDescriptor(self, -1);
    }
#else
    (void)ctx;
#endif
}

} // namespace Native
} // namespace Runtime
