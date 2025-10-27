#pragma once
#include <mdr-c/Platform/Windows/WindowsPlatform.h>

namespace mdr
{
    struct MDRConnectionWindows
    {
        ::MDRConnectionWindows* pConn;
        MDRConnectionWindows() : pConn(mdrConnectionWindowsCreate()) {}
        operator ::MDRConnectionWindows*() const { return pConn; }
        operator ::MDRConnection*() const { return mdrConnectionWindowsGet(pConn); }
        ~MDRConnectionWindows() { mdrConnectionWindowsDestroy(pConn); }
    };
}