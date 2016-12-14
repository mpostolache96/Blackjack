#pragma once
#include "stdafx.h"

HANDLE getHandleForRead(const char *fileName);
HANDLE getHandleForWrite(const char *fileName);
bool Write(HANDLE hFile, void* buffer, DWORD size, DWORD address);
BYTE* read(HANDLE hFile, DWORD address, DWORD size = -1);
