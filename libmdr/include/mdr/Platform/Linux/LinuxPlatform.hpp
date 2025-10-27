#pragma once
#include <mdr-c/Platform/Linux/LinuxPlatform.h>

namespace mdr
{
    struct MDRConnectionLinux
    {
        ::MDRConnectionLinux* pConn;
        MDRConnectionLinux() : pConn(mdrConnectionLinuxCreate()) {}
        operator ::MDRConnectionLinux*() const { return pConn; }
        operator ::MDRConnection*() const { return mdrConnectionLinuxGet(pConn); }
        ~MDRConnectionLinux() { mdrConnectionLinuxDestroy(pConn); }
    };
}