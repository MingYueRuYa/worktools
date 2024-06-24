#pragma once

#pragma comment (lib ,"dbgeng.lib")


#include "inc/dbgeng.h"


#include <windows.h>

#define KDEXT_64BIT
#define DBG_COMMAND_EXCEPTION            ((DWORD   )0x40010009L)
#include <wdbgexts.h>
#include <dbgeng.h>

WINDBG_EXTENSION_APIS64 ExtensionApis;