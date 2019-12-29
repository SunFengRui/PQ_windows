#pragma once
#include "pcap.h"

DWORD WINAPI Pcap1ThreadFunc(LPVOID param);
DWORD WINAPI Pcap2ThreadFunc(LPVOID param);
DWORD WINAPI Pcap3ThreadFunc(LPVOID param);
