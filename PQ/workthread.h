#pragma once

#include "netfunction.h"

#define accuracy 18
#define mod_value 65536
#define factor_factor (6553.6)   //104857.6
#define FFT_8000 8000
#define HarmonicWave 40
#define HarmonicWaveParam 5
#define PeriodPoint 800
#define PeriodPointMax 880
#define AN_BUFFER_LEN_8000	(8000)
#define Plus_8000 8800
#define AN_FFT_LEN_8000		(AN_BUFFER_LEN_8000)
#define AN_BUFFER_880kLEN	(880*1000)

#define PI 3.1415926
#define phase_param1  10
#define phase_param2  (phase_param1+5)

//短时闪变计算系数
#define Kpointone                      0.0314
#define Kone                           0.0525
#define Kthree                         0.065
#define Kten                           0.28
#define Kfifty                         0.08
typedef struct
{
	#if (accuracy==16)
		u_short an_ch0;
		u_short an_ch1;
		u_short an_ch2;
		u_short an_ch3;
		u_short an_ch4;
		u_short an_ch5;
		u_short stand_flag;
	#else
		long an_ch3;
		long an_ch2;
		long an_ch1;
		long an_ch0;
		long a1;
		long a2;
		long a3;
		long a4;
		long a5;
		long a6;
		u_short check;
		u_short stand_flag;

		long an_ch4;
		long an_ch5;
	#endif
}an_point;
typedef struct
{
	u_short an_ch0;
	u_short an_ch1;
	u_short an_ch2;
	u_short an_ch3;
	u_short an_ch4;
	u_short an_ch5;
	u_short stand_flag;
}an_point_f;

typedef struct
{
	
	long an_ch3;
	long an_ch2;
	long an_ch1;
	long an_ch0;
	long a1;
	long a2;
	long a3;
	long a4;
	long a5;
	long a6;
	u_short check;
	u_short stand_flag;
	long an_ch4;
	long an_ch5;
}an_point_z;

#pragma pack(2)
typedef struct
{
	u_short  pkt_idx;//2
	u_long   pkt_timestamp_send;//4
	u_long   pkt_timestamp0;//4
	u_long   pkt_timestamp1;//4
	u_short  pkt_freq;//2
	u_short  dummy1;//2
	u_long   pkt_timestamp_freq;//4
	u_long   pkt_timestamp_freq1;//4
	u_short  pkt_idx1;//2
	an_point sample_point;   //每次接收数据*10
	u_long   pkt_timestamp_freq2;//4
}an_channels;
#pragma pack()

typedef union _measuring_results_union
{
	double  indicators_array_double[30];
	char    indicators_array_char[240];
}measuring_results_union;

typedef unsigned   char    u8_t;
typedef signed     char    s8_t;
typedef unsigned   short   u16_t;
typedef signed     short   s16_t;
typedef unsigned   long    u32_t;
typedef signed     long    s32_t;

struct ether_header
{
	u8_t ether_dhost[6];
	u8_t ether_shost[6];
	u16_t ether_type;
};
/* 4 bytes IP address */
typedef struct ip_address
{
	u_char byte1;
	u_char byte2;
	u_char byte3;
	u_char byte4;
}ip_address;

/* IPv4 header */
typedef struct ip_header
{
	u_char	ver_ihl;		// Version (4 bits) + Internet header length (4 bits)
	u_char	tos;			// Type of service 
	u_short tlen;			// Total length 
	u_short identification; // Identification
	u_short flags_fo;		// Flags (3 bits) + Fragment offset (13 bits)
	u_char	ttl;			// Time to live
	u_char	proto;			// Protocol
	u_short crc;			// Header checksum
	ip_address	saddr;		// Source address
	ip_address	daddr;		// Destination address
	u_int	op_pad;			// Option + Padding
}ip_header;

/* UDP--ͷ*/
typedef struct udp_header
{
	u_short sport;			// Source port
	u_short dport;			// Destination port
	u_short len;			// Datagram length
	u_short crc;			// Checksum
}udp_header;

void ethernet_protocol_packet_callback1(u_char *user_data,
	const struct pcap_pkthdr *packet_header,
	const u_char *packet_content);
void ethernet_protocol_packet_callback2(u_char *user_data,
	const struct pcap_pkthdr *packet_header,
	const u_char *packet_content);
void ethernet_protocol_packet_callback3(u_char *user_data,
	const struct pcap_pkthdr *packet_header,
	const u_char *packet_content);



DWORD WINAPI FFT_AThreadFunc(LPVOID param);
DWORD WINAPI FFT_BThreadFunc(LPVOID param);
DWORD WINAPI FFT_CThreadFunc(LPVOID param);
DWORD WINAPI A_FlickerThreadFunc(LPVOID param);
DWORD WINAPI A_HalfThreadFunc(LPVOID param);
DWORD WINAPI B_FlickerThreadFunc(LPVOID param);
DWORD WINAPI B_HalfThreadFunc(LPVOID param);
DWORD WINAPI C_FlickerThreadFunc(LPVOID param);
DWORD WINAPI C_HalfThreadFunc(LPVOID param);
DWORD WINAPI SocketThreadFunc(LPVOID param);
DWORD WINAPI CheckThreadFunc(LPVOID param);
VOID OneMinuteTimerCallbackFunc(PVOID lpParamter, BOOLEAN TimerOrWaitFired);