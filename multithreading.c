#include"globals.h"
#include"multithreading.h"
#include"graphics.h"
#include"exploration.h"
#include"input.h"

typedef struct wsThreads {
	HANDLE handles[1];
	DWORD threadIDs[1];
} wsThreads;
wsThreads threads;

DWORD WINAPI wsThreadMainStart(LPVOID lp_param) {
	printf("Main thread started\n");
	
	// The meat and potatoes!
	while(true) {
		
	}
}
void wsThreadingInit() {
	threads.handles[0] = CreateThread(NULL, 0, &wsThreadMainStart, NULL, 0, &threads.threadIDs[0]);
	if(threads.handles[0] == NULL)	printf("%s\n", GetLastError());
}
void wsThreadingTerminate() {
	for(size_t i = 0; i < 0; i++) {
		CloseHandle(threads.handles[i]);
		printf("Thread %d closed\n", i);
	}
}