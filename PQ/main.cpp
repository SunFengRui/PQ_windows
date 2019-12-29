#include "pq.h"
#include <QtWidgets/QApplication>
#include "time.h"
#include <Windows.h>
#include "DList.h"

#pragma execution_character_set("utf-8")
HANDLE handlePcap1, handlePcap2, handlePcap3;
HANDLE handleFFT_AThread, handleA_FlickerThread, handleA_HalfPeriodThread, handleSocketThread;
HANDLE handleFFT_BThread, handleB_FlickerThread, handleB_HalfPeriodThread;
HANDLE handleFFT_CThread, handleC_FlickerThread, handleC_HalfPeriodThread;
HANDLE handle_CheckThread;
HANDLE timer_queue, timer;
CRITICAL_SECTION g_cs;
SYSTEMTIME Start_time;
char start_time[200];
long start, finish;
FILE *fp;
int core_num;
int threadsum = 0;

DList *list_f;

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	PQ w;
	SYSTEM_INFO SyetemInfo;
	GetSystemInfo(&SyetemInfo);
	core_num = SyetemInfo.dwNumberOfProcessors;
	fp = fopen("D:\\text.txt", "w+");//可读可写 每次内容都清0,文件不必存在

	GetLocalTime(&Start_time);
	sprintf_s(start_time, "%4d年%2d月%2d日%2d时%2d分%2d秒\n", Start_time.wYear, Start_time.wMonth, Start_time.wDay, Start_time.wHour, Start_time.wMinute, Start_time.wSecond);
	start = clock();
	InitializeCriticalSection(&g_cs);

	timer_queue = CreateTimerQueue();
	CreateTimerQueueTimer(&timer, timer_queue, OneMinuteTimerCallbackFunc, NULL, 1000 * 60, 1000 * 60 * 1, WT_EXECUTEINTIMERTHREAD);


	//SetThreadAffinityMask(handlePcap1, 0x00000001);
	//SetThreadAffinityMask(handlePcap2, 0x00000002);
	//SetThreadAffinityMask(handlePcap3, 0x00000004);
	//SetThreadAffinityMask(handleFFT_AThread, 0x00000008);
	//SetThreadAffinityMask(handleA_FlickerThread, 0x00000008);
	//SetThreadAffinityMask(handleA_HalfPeriodThread, 0x00000008);
	//SetThreadAffinityMask(handleFFT_BThread, 0x00000010);
	//SetThreadAffinityMask(handleB_FlickerThread, 0x00000010);
	//SetThreadAffinityMask(handleB_HalfPeriodThread, 0x00000010);
	//SetThreadAffinityMask(handleFFT_CThread, 0x00000020);
	//SetThreadAffinityMask(handleC_FlickerThread, 0x00000020);
	//SetThreadAffinityMask(handleC_HalfPeriodThread, 0x00000020);
	//SetThreadAffinityMask(handleSocketThread, 0x00000040);
	//SetThreadAffinityMask(handle_CheckThread, 0x00000080);

	EnterCriticalSection(&g_cs);
	handlePcap1 = CreateThread(NULL, 0, Pcap1ThreadFunc, 0, CREATE_SUSPENDED, NULL);	
	SetThreadPriority(handlePcap1, THREAD_PRIORITY_HIGHEST);
	if (handlePcap1 != NULL)
		threadsum++;
	handlePcap2 = CreateThread(NULL, 0, Pcap2ThreadFunc, 0, CREATE_SUSPENDED, NULL);
	SetThreadPriority(handlePcap2, THREAD_PRIORITY_HIGHEST);
	
	if (handlePcap2 != NULL)
		threadsum++;
	handlePcap3 = CreateThread(NULL, 0, Pcap3ThreadFunc, 0, CREATE_SUSPENDED, NULL);
	SetThreadPriority(handlePcap3, THREAD_PRIORITY_HIGHEST);
	
	if (handlePcap3 != NULL)
		threadsum++;

	handleFFT_AThread = CreateThread(NULL, 0, FFT_AThreadFunc, 0, CREATE_SUSPENDED, NULL);	
	if (handleFFT_AThread != NULL)
		threadsum++;
	handleFFT_BThread = CreateThread(NULL, 0, FFT_BThreadFunc, 0, CREATE_SUSPENDED, NULL);
	
	if (handleFFT_BThread != NULL)
		threadsum++;
	handleFFT_CThread = CreateThread(NULL, 0, FFT_CThreadFunc, 0, CREATE_SUSPENDED, NULL);
	
	if (handleFFT_CThread != NULL)
		threadsum++;

	handle_CheckThread = CreateThread(NULL, 0, CheckThreadFunc, 0, 0, NULL);
	if (handle_CheckThread != NULL)
		threadsum++;
	handleSocketThread = CreateThread(NULL, 0, SocketThreadFunc, 0, 0, NULL);
	if (handleSocketThread != NULL)
		threadsum++;
	CloseHandle(handle_CheckThread);
	CloseHandle(handleSocketThread);

	handleA_FlickerThread = CreateThread(NULL, 0, A_FlickerThreadFunc, 0, 0, NULL);
	if (handleA_FlickerThread != NULL)
		threadsum++;
	handleB_FlickerThread = CreateThread(NULL, 0, B_FlickerThreadFunc, 0, 0, NULL);
	if (handleB_FlickerThread != NULL)
		threadsum++;
	handleC_FlickerThread = CreateThread(NULL, 0, C_FlickerThreadFunc, 0, 0, NULL);
	if (handleC_FlickerThread != NULL)
		threadsum++;
	handleA_HalfPeriodThread = CreateThread(NULL, 0, A_HalfThreadFunc, 0, CREATE_SUSPENDED, NULL);
	if (handleA_HalfPeriodThread != NULL)
		threadsum++;
	handleB_HalfPeriodThread = CreateThread(NULL, 0, B_HalfThreadFunc, 0, CREATE_SUSPENDED, NULL);
	if (handleB_HalfPeriodThread != NULL)
		threadsum++;
	handleC_HalfPeriodThread = CreateThread(NULL, 0, C_HalfThreadFunc, 0, CREATE_SUSPENDED, NULL);
	if (handleC_HalfPeriodThread != NULL)
		threadsum++;

	


	LeaveCriticalSection(&g_cs);

	create_list(&list_f,10,50.0);
	
	w.show();

	return a.exec();
}

