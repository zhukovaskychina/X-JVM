//
// java.net / java.io.FileDescriptor 等与套接字、解析相关的 native（POSIX）。
//

#ifndef XJVM_NETWORK_NATIVE_SUPPORT_H
#define XJVM_NETWORK_NATIVE_SUPPORT_H

#include "NativeMethodRegistry.h"

namespace Runtime {
namespace Native {

void native_InetAddress_init(const NativeCallContext& ctx);
void native_Inet4Address_init(const NativeCallContext& ctx);
void native_Inet6Address_init(const NativeCallContext& ctx);

void native_Inet4AddressImpl_getLocalHostName(const NativeCallContext& ctx);
void native_Inet4AddressImpl_lookupAllHostAddr(const NativeCallContext& ctx);
void native_Inet4AddressImpl_getHostByAddr(const NativeCallContext& ctx);
void native_Inet4AddressImpl_isReachable0(const NativeCallContext& ctx);

void native_Inet6AddressImpl_getLocalHostName(const NativeCallContext& ctx);
void native_Inet6AddressImpl_lookupAllHostAddr(const NativeCallContext& ctx);
void native_Inet6AddressImpl_getHostByAddr(const NativeCallContext& ctx);
void native_Inet6AddressImpl_isReachable0(const NativeCallContext& ctx);

void native_PlainSocketImpl_socketCreate(const NativeCallContext& ctx);
void native_PlainSocketImpl_socketConnect(const NativeCallContext& ctx);
void native_PlainSocketImpl_socketBind(const NativeCallContext& ctx);
void native_PlainSocketImpl_socketListen(const NativeCallContext& ctx);
void native_PlainSocketImpl_socketAccept(const NativeCallContext& ctx);
void native_PlainSocketImpl_socketAvailable(const NativeCallContext& ctx);
void native_PlainSocketImpl_socketClose0(const NativeCallContext& ctx);
void native_PlainSocketImpl_socketShutdown(const NativeCallContext& ctx);
void native_PlainSocketImpl_initProto(const NativeCallContext& ctx);
void native_PlainSocketImpl_socketSetOption0(const NativeCallContext& ctx);
void native_PlainSocketImpl_socketGetOption(const NativeCallContext& ctx);
void native_PlainSocketImpl_socketSendUrgentData(const NativeCallContext& ctx);

void native_SocketInputStream_socketRead0(const NativeCallContext& ctx);
void native_SocketInputStream_init(const NativeCallContext& ctx);

void native_SocketOutputStream_socketWrite0(const NativeCallContext& ctx);
void native_SocketOutputStream_init(const NativeCallContext& ctx);

void native_NetworkInterface_init(const NativeCallContext& ctx);
void native_NetworkInterface_getAll(const NativeCallContext& ctx);
void native_NetworkInterface_getByName0(const NativeCallContext& ctx);
void native_NetworkInterface_getByIndex0(const NativeCallContext& ctx);
void native_NetworkInterface_boundInetAddress0(const NativeCallContext& ctx);
void native_NetworkInterface_getByInetAddress0(const NativeCallContext& ctx);
void native_NetworkInterface_isUp0(const NativeCallContext& ctx);
void native_NetworkInterface_isLoopback0(const NativeCallContext& ctx);
void native_NetworkInterface_supportsMulticast0(const NativeCallContext& ctx);
void native_NetworkInterface_isP2P0(const NativeCallContext& ctx);
void native_NetworkInterface_getMacAddr0(const NativeCallContext& ctx);
void native_NetworkInterface_getMTU0(const NativeCallContext& ctx);

void native_DefaultProxySelector_init(const NativeCallContext& ctx);
void native_DefaultProxySelector_getSystemProxies(const NativeCallContext& ctx);

void native_ResolverConfigurationImpl_fallbackDomain0(const NativeCallContext& ctx);

void native_FileDescriptor_initIDs(const NativeCallContext& ctx);
void native_FileDescriptor_sync(const NativeCallContext& ctx);
void native_FileDescriptor_getHandle(const NativeCallContext& ctx);
void native_FileDescriptor_getAppend(const NativeCallContext& ctx);
void native_FileDescriptor_close0(const NativeCallContext& ctx);

} // namespace Native
} // namespace Runtime

#endif
