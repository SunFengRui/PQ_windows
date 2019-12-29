#include "workthread.h"


extern CRITICAL_SECTION g_cs;
/*
A相 右1 inum=4,IP=16
B相 右2 inum=3,IP=17
C相 右3 inum=5,IP=18
*/
int inum = 5, inum2 = 1, inum3 = 2;

#if (accuracy==16)
char packet_filter[] = "ip and udp and ether[29] & 0xff = 0x0a";
int packet_offset = 42;   //42 16
#else
char packet_filter[] = "ether[12] = 0x88 and ether[13] = 0xbb";
int packet_offset = 16;  //42 16
#endif

DWORD WINAPI Pcap1ThreadFunc(LPVOID param)
{
	EnterCriticalSection(&g_cs);
	pcap_if_t *alldevs;
	pcap_if_t *d;

	int i = 0;
	pcap_t *adhandle;
	char errbuf[PCAP_ERRBUF_SIZE];
	u_int netmask;
	struct bpf_program fcode;
	/* Retrieve the device list */
	//获取网络设备列表
	if (pcap_findalldevs(&alldevs, errbuf) == -1)
	{
		//fprintf(stderr, "Error in pcap_findalldevs: %s\n", errbuf);
		exit(1);
	}
	/* Print the list */
	//显示网络设备列表
	//格式：  数字. 网卡名 （网卡描述）
	for (d = alldevs; d; d = d->next)
	{
		++i;
	}
	if (i == 0)
	{
		printf("\nNo interfaces found! Make sure WinPcap is installed.\n");
		//return 1;
	}
	/* Check if the user specified a valid adapter */
	if (inum < 1 || inum > i)
	{
		printf("\nAdapter number out of range.\n");

		/* Free the device list */
		pcap_freealldevs(alldevs);
		//return 1;
	}

	/* Jump to the selected adapter */
	for (d = alldevs, i = 0; i < inum - 1; d = d->next, i++);

	/* Open the adapter */
	//打开网卡捕获数据包
	if ((adhandle = pcap_open_live(d->name,	// name of the device 
		65536,			// portion of the packet to capture. 捕获最大字节数
						// 65536 grants that the whole packet will be captured on all the MACs.
		1,				// promiscuous mode (nonzero means promiscuous) 开启混杂模式
		1000,			// read timeout 读取的超时时间 1000ms
		errbuf			// error buffer
	)) == NULL)
	{
		//fprintf(stderr, "\nUnable to open the adapter. %s is not supported by WinPcap\n");
		/* Free the device list */
		pcap_freealldevs(alldevs);
		//return 1;
	}

	/* Check the link layer. We support only Ethernet for simplicity. */
	if (pcap_datalink(adhandle) != DLT_EN10MB)
	{
		//fprintf(stderr, "\nThis program works only on Ethernet networks.\n");
		/* Free the device list */
		pcap_freealldevs(alldevs);
		//return 1;
	}

	if (d->addresses != NULL)
		/* Retrieve the mask of the first address of the interface */
		netmask = ((struct sockaddr_in *)(d->addresses->netmask))->sin_addr.S_un.S_addr;
	else
		/* If the interface is without addresses we suppose to be in a C class network */
		netmask = 0xffffff;

	//滤出Udp协议且发送端的Ip地址为“192.168.1.10”的数据包。
	//char packet_filter[] = "ip and udp and ether[29] & 0xff = 0x0a";

	//compile the filter
	//编译过滤规则
	if (pcap_compile(adhandle, &fcode, packet_filter, 1, netmask) < 0)
	{
		//fprintf(stderr, "\nUnable to compile the packet filter. Check the syntax.\n");
		/* Free the device list */
		pcap_freealldevs(alldevs);
		//return 1;
	}

	//set the filter
	//设置过滤规则
	if (pcap_setfilter(adhandle, &fcode) < 0)
	{
		//fprintf(stderr, "\nError setting the filter.\n");
		/* Free the device list */
		pcap_freealldevs(alldevs);
		//return 1;
	}

	printf("\nlistening on %s...\n", d->description);

	/* At this point, we don't need any more the device list. Free it */
	pcap_freealldevs(alldevs);

	pcap_setbuff(adhandle, 100000);

	LeaveCriticalSection(&g_cs);
	/* start the capture */
	/*使用pcap_loop函数来循环捕获数据包，pcap_loop函数模型为
	//pcap_loop（pcap_t *p，int cnt，pcap_handler callback，u_char *user）
	//第一个参数是句柄adhandle，第二个参数cnt设置为 - 1表示无限循环捕获，
	//第三个参数是回调函数，第四个参数一般设置为NULL。*/
	pcap_loop(adhandle, -1, ethernet_protocol_packet_callback1, NULL);
	return 0;
}
DWORD WINAPI Pcap2ThreadFunc(LPVOID param)
{
	EnterCriticalSection(&g_cs);
	pcap_if_t *alldevs;
	pcap_if_t *d;

	int i = 0;
	pcap_t *adhandle;
	char errbuf[PCAP_ERRBUF_SIZE];
	u_int netmask;
	//滤出Udp协议且发送端的Ip地址为“192.168.1.10”的数据包。

	//char packet_filter[] = "ip and udp";
	struct bpf_program fcode;

	/* Retrieve the device list */
	//获取网络设备列表
	if (pcap_findalldevs(&alldevs, errbuf) == -1)
	{
		//fprintf(stderr, "Error in pcap_findalldevs: %s\n", errbuf);
		exit(1);
	}

	/* Print the list */
	//显示网络设备列表
	//格式：  数字. 网卡名 （网卡描述）
	for (d = alldevs; d; d = d->next)
	{
		++i;
	}

	if (i == 0)
	{
		printf("\nNo interfaces found! Make sure WinPcap is installed.\n");
		//return 1;
	}
	/* Check if the user specified a valid adapter */
	if (inum2 < 1 || inum2 > i)
	{
		printf("\nAdapter number out of range.\n");

		/* Free the device list */
		pcap_freealldevs(alldevs);
		//return 1;
	}

	/* Jump to the selected adapter */
	for (d = alldevs, i = 0; i < inum2 - 1; d = d->next, i++);

	/* Open the adapter */
	//打开网卡捕获数据包
	if ((adhandle = pcap_open_live(d->name,	// name of the device 
		65536,			// portion of the packet to capture. 捕获最大字节数
						// 65536 grants that the whole packet will be captured on all the MACs.
		1,				// promiscuous mode (nonzero means promiscuous) 开启混杂模式
		1000,			// read timeout 读取的超时时间 1000ms
		errbuf			// error buffer
	)) == NULL)
	{
		//fprintf(stderr, "\nUnable to open the adapter. %s is not supported by WinPcap\n");
		/* Free the device list */
		pcap_freealldevs(alldevs);
		//return 1;
	}

	/* Check the link layer. We support only Ethernet for simplicity. */
	if (pcap_datalink(adhandle) != DLT_EN10MB)
	{
		//fprintf(stderr, "\nThis program works only on Ethernet networks.\n");
		/* Free the device list */
		pcap_freealldevs(alldevs);
		//return 1;
	}

	if (d->addresses != NULL)
		/* Retrieve the mask of the first address of the interface */
		netmask = ((struct sockaddr_in *)(d->addresses->netmask))->sin_addr.S_un.S_addr;
	else
		/* If the interface is without addresses we suppose to be in a C class network */
		netmask = 0xffffff;

	//滤出Udp协议且发送端的Ip地址为“192.168.1.10”的数据包。
	//char packet_filter[] = "ip and udp and ether[29] & 0xff = 0x0a";

	//compile the filter
	//编译过滤规则
	if (pcap_compile(adhandle, &fcode, packet_filter, 1, netmask) < 0)
	{
		//fprintf(stderr, "\nUnable to compile the packet filter. Check the syntax.\n");
		/* Free the device list */
		pcap_freealldevs(alldevs);
		//return 1;
	}

	//set the filter
	//设置过滤规则
	if (pcap_setfilter(adhandle, &fcode) < 0)
	{
		//fprintf(stderr, "\nError setting the filter.\n");
		/* Free the device list */
		pcap_freealldevs(alldevs);
		//return 1;
	}

	printf("\nlistening on %s...\n", d->description);

	/* At this point, we don't need any more the device list. Free it */
	pcap_freealldevs(alldevs);

	pcap_setbuff(adhandle, 100000);

	LeaveCriticalSection(&g_cs);
	/* start the capture */
	/*使用pcap_loop函数来循环捕获数据包，pcap_loop函数模型为
	//pcap_loop（pcap_t *p，int cnt，pcap_handler callback，u_char *user）
	//第一个参数是句柄adhandle，第二个参数cnt设置为 - 1表示无限循环捕获，
	//第三个参数是回调函数，第四个参数一般设置为NULL。*/
	pcap_loop(adhandle, -1, ethernet_protocol_packet_callback2, NULL);
	return 0;
}
DWORD WINAPI Pcap3ThreadFunc(LPVOID param)
{
	EnterCriticalSection(&g_cs);
	pcap_if_t *alldevs;
	pcap_if_t *d;
	int i = 0;
	pcap_t *adhandle;
	char errbuf[PCAP_ERRBUF_SIZE];
	u_int netmask;
	//滤出Udp协议且发送端的Ip地址为“192.168.1.10”的数据包。

	//char packet_filter[] = "ip and udp";
	struct bpf_program fcode;

	/* Retrieve the device list */
	//获取网络设备列表
	if (pcap_findalldevs(&alldevs, errbuf) == -1)
	{
		//fprintf(stderr, "Error in pcap_findalldevs: %s\n", errbuf);
		exit(1);
	}

	/* Print the list */
	//显示网络设备列表
	//格式：  数字. 网卡名 （网卡描述）
	for (d = alldevs; d; d = d->next)
	{
		++i;
	}

	if (i == 0)
	{
		printf("\nNo interfaces found! Make sure WinPcap is installed.\n");
		//return 1;
	}
	/* Check if the user specified a valid adapter */
	if (inum3 < 1 || inum3 > i)
	{
		printf("\nAdapter number out of range.\n");

		/* Free the device list */
		pcap_freealldevs(alldevs);
		//return 1;
	}

	/* Jump to the selected adapter */
	for (d = alldevs, i = 0; i < inum3 - 1; d = d->next, i++);

	/* Open the adapter */
	//打开网卡捕获数据包
	if ((adhandle = pcap_open_live(d->name,	// name of the device 
		65536,			// portion of the packet to capture. 捕获最大字节数
						// 65536 grants that the whole packet will be captured on all the MACs.
		1,				// promiscuous mode (nonzero means promiscuous) 开启混杂模式
		1000,			// read timeout 读取的超时时间 1000ms
		errbuf			// error buffer
	)) == NULL)
	{
		//fprintf(stderr, "\nUnable to open the adapter. %s is not supported by WinPcap\n");
		/* Free the device list */
		pcap_freealldevs(alldevs);
		//return 1;
	}

	/* Check the link layer. We support only Ethernet for simplicity. */
	if (pcap_datalink(adhandle) != DLT_EN10MB)
	{
		//fprintf(stderr, "\nThis program works only on Ethernet networks.\n");
		/* Free the device list */
		pcap_freealldevs(alldevs);
		//return 1;
	}

	if (d->addresses != NULL)
		/* Retrieve the mask of the first address of the interface */
		netmask = ((struct sockaddr_in *)(d->addresses->netmask))->sin_addr.S_un.S_addr;
	else
		/* If the interface is without addresses we suppose to be in a C class network */
		netmask = 0xffffff;

	//滤出Udp协议且发送端的Ip地址为“192.168.1.10”的数据包。
	//char packet_filter[] = "ip and udp and ether[29] & 0xff = 0x0a";

	//compile the filter
	//编译过滤规则
	if (pcap_compile(adhandle, &fcode, packet_filter, 1, netmask) < 0)
	{
		//fprintf(stderr, "\nUnable to compile the packet filter. Check the syntax.\n");
		/* Free the device list */
		pcap_freealldevs(alldevs);
		//return 1;
	}

	//set the filter
	//设置过滤规则
	if (pcap_setfilter(adhandle, &fcode) < 0)
	{
		//fprintf(stderr, "\nError setting the filter.\n");
		/* Free the device list */
		pcap_freealldevs(alldevs);
		//return 1;
	}

	printf("\nlistening on %s...\n", d->description);

	/* At this point, we don't need any more the device list. Free it */
	pcap_freealldevs(alldevs);

	pcap_setbuff(adhandle, 100000);

	LeaveCriticalSection(&g_cs);
	/* start the capture */
	/*使用pcap_loop函数来循环捕获数据包，pcap_loop函数模型为
	//pcap_loop（pcap_t *p，int cnt，pcap_handler callback，u_char *user）
	//第一个参数是句柄adhandle，第二个参数cnt设置为 - 1表示无限循环捕获，
	//第三个参数是回调函数，第四个参数一般设置为NULL。*/
	pcap_loop(adhandle, -1, ethernet_protocol_packet_callback3, NULL);
	return 0;
}