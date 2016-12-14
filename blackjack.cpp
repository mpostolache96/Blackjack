#include "stdafx.h"
#include "Players.h"
#include "blakcjack.h"



HANDLE getHandleForRead(const char *fileName) {
	HANDLE hFile;
	hFile = CreateFileA(fileName,// file to open
		GENERIC_READ,          // open for reading
		FILE_SHARE_READ,       // share for reading
		NULL,                  // default security
		OPEN_EXISTING,         // existing file only
		NULL, // normal file
		NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		printf("invalid = %d %s\n", GetLastError(), fileName);
	return hFile;
}

HANDLE getHandleForWrite(const char *fileName) {
	HANDLE hFile;

	hFile = CreateFileA(fileName,// file to open
		GENERIC_WRITE,          // open for writing
		NULL,       // share for writing
		NULL,                  // default security
		CREATE_ALWAYS,
		NULL, // normal file
		NULL);

	if (hFile == INVALID_HANDLE_VALUE)
		printf("invalid = %d %s\n", GetLastError(), fileName);
	return hFile;
}

bool Write(HANDLE hFile, void* buffer, DWORD size, DWORD address) {
	DWORD  dwBytesRead = 0;
	DWORD nrBytesWrite;
	LARGE_INTEGER add;
	add.HighPart = 0;
	add.LowPart = address;

	if (hFile == INVALID_HANDLE_VALUE) {
		printf("handle invalid - write = %d\n", GetLastError());
		return NULL;
	}
	if (address == -1) {
		add.LowPart = 0;
		SetFilePointerEx(hFile, add, 0, FILE_END);
	}
	else SetFilePointerEx(hFile, add, 0, FILE_BEGIN);

	if (false == WriteFile(hFile,
		buffer,
		size,
		&nrBytesWrite,
		NULL)) {
		printf("couldnt write file%d\n", GetLastError());
		return false;
	}

	if (nrBytesWrite != size) {
		printf("write too litle\n");
		return false;
	}

	return true;
}

BYTE* read(HANDLE hFile, DWORD address, DWORD size) {
	DWORD nrBytesRead, nrBytesToRead;
	DWORD  dwBytesRead = 0;
	BYTE* buffer;
	LARGE_INTEGER add;

	if (size == -1)
		size = GetFileSize(hFile, 0);
	nrBytesToRead = size;
	add.HighPart = 0;
	add.LowPart = address;

	if (hFile == INVALID_HANDLE_VALUE) {
		//printf("handle invalid - read = %d", GetLastError());
		return NULL;
	}

	buffer = (BYTE*)malloc(nrBytesToRead + 1);
	SetFilePointerEx(hFile, add, 0, FILE_BEGIN);

	if (false == ReadFile(hFile,
		buffer,
		nrBytesToRead,
		&nrBytesRead,
		NULL)) {
		//printf("couldnt read file = %d", GetLastError());
		return NULL;
	}

	if (nrBytesRead < nrBytesToRead) {
		//printf("incorrect number of bytes read = %d\n", GetLastError());
		return NULL;
	}

	return buffer;
}

int main()
{
	game mGame;
    return 0;
}
