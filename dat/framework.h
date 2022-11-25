#pragma once

#define WIN32_LEAN_AND_MEAN             // Selten verwendete Komponenten aus Windows-Headern ausschließen
// Windows-Headerdateien

#define DAT_WELCOME_MSG R"(
 _____     ______     ______    
/\  __-.  /\  __ \   /\  ___\   
\ \ \/\ \ \ \  __ \  \ \ \____  
 \ \____-  \ \_\ \_\  \ \_____\ 
  \/____/   \/_/\/_/   \/_____/                                                      
)"

#include <cinttypes>

typedef unsigned char u8;
typedef unsigned short ushort;
typedef unsigned long ulong;
typedef unsigned short u16;
typedef unsigned long u32;
typedef unsigned long long u64;

#define BASE_OF(X) (DWORD_PTR)X
#define PTR_OF(X) (PVOID)X
#define RESOLVE_INSTR(addr, io, il) ( addr + *reinterpret_cast<int32_t*>( addr + io ) + il )

#include <vector>

#include <windows.h>
#include <Psapi.h>
#include <TlHelp32.h>

typedef EXCEPTION_POINTERS exception_ptrs;

#include "log.h"
#include "dac.h"
#include "hooks.h"
