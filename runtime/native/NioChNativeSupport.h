//
// sun.nio.ch.Net / sun.nio.ch.SocketDispatcher — JDK 17+ 默认 NioSocketImpl 依赖的 POSIX native。
//

#ifndef XJVM_NIO_CH_NATIVE_SUPPORT_H
#define XJVM_NIO_CH_NATIVE_SUPPORT_H

#include "NativeMethodRegistry.h"

namespace Runtime {
namespace Native {

void native_Net_initIDs(const NativeCallContext& ctx);
void native_Net_isIPv6Available0(const NativeCallContext& ctx);
void native_Net_isReusePortAvailable0(const NativeCallContext& ctx);
void native_Net_isExclusiveBindAvailable(const NativeCallContext& ctx);
void native_Net_shouldSetBothIPv4AndIPv6Options0(const NativeCallContext& ctx);
void native_Net_canIPv6SocketJoinIPv4Group0(const NativeCallContext& ctx);
void native_Net_canJoin6WithIPv4Group0(const NativeCallContext& ctx);
void native_Net_canUseIPv6OptionsWithIPv4LocalAddress0(const NativeCallContext& ctx);
void native_Net_socket0(const NativeCallContext& ctx);
void native_Net_bind0(const NativeCallContext& ctx);
void native_Net_listen(const NativeCallContext& ctx);
void native_Net_connect0(const NativeCallContext& ctx);
void native_Net_accept(const NativeCallContext& ctx);
void native_Net_shutdown(const NativeCallContext& ctx);
void native_Net_localPort(const NativeCallContext& ctx);
void native_Net_localInetAddress(const NativeCallContext& ctx);
void native_Net_remotePort(const NativeCallContext& ctx);
void native_Net_remoteInetAddress(const NativeCallContext& ctx);
void native_Net_getIntOption0(const NativeCallContext& ctx);
void native_Net_setIntOption0(const NativeCallContext& ctx);
void native_Net_poll(const NativeCallContext& ctx);
void native_Net_pollConnect(const NativeCallContext& ctx);
void native_Net_available(const NativeCallContext& ctx);
void native_Net_sendOOB(const NativeCallContext& ctx);
void native_Net_discardOOB(const NativeCallContext& ctx);
void native_Net_joinOrDrop4(const NativeCallContext& ctx);
void native_Net_blockOrUnblock4(const NativeCallContext& ctx);
void native_Net_joinOrDrop6(const NativeCallContext& ctx);
void native_Net_blockOrUnblock6(const NativeCallContext& ctx);
void native_Net_setInterface4(const NativeCallContext& ctx);
void native_Net_getInterface4(const NativeCallContext& ctx);
void native_Net_setInterface6(const NativeCallContext& ctx);
void native_Net_getInterface6(const NativeCallContext& ctx);
void native_Net_pollinValue(const NativeCallContext& ctx);
void native_Net_polloutValue(const NativeCallContext& ctx);
void native_Net_pollerrValue(const NativeCallContext& ctx);
void native_Net_pollhupValue(const NativeCallContext& ctx);
void native_Net_pollnvalValue(const NativeCallContext& ctx);
void native_Net_pollconnValue(const NativeCallContext& ctx);

void native_SocketDispatcher_read0(const NativeCallContext& ctx);
void native_SocketDispatcher_readv0(const NativeCallContext& ctx);
void native_SocketDispatcher_write0(const NativeCallContext& ctx);
void native_SocketDispatcher_writev0(const NativeCallContext& ctx);

} // namespace Native
} // namespace Runtime

#endif
