#include "workthread.h"
#include "pcap.h"
#include <math.h>
#include "fftw3.h"
#include "tool.h"
#include "V_Dip_Swell_Interrupt.h"

extern FILE *fp;
extern CRITICAL_SECTION g_cs;
extern HANDLE handlePcap1, handlePcap2, handlePcap3;
extern HANDLE handleFFT_AThread, handleFFT_BThread, handleFFT_CThread, handleA_FlickerThread, handleA_HalfPeriodThread;
extern HANDLE handleB_FlickerThread, handleB_HalfPeriodThread, handleC_FlickerThread, handleC_HalfPeriodThread;
int A_packet_number, B_packet_number, C_packet_number;
short an_buffer[AN_BUFFER_880kLEN];
short an_buffer_cur[AN_BUFFER_880kLEN];
unsigned long an_buffer_idx_A = 0;
unsigned long an_buffer_8800flag_A = 0;
int index_8800_A = 0;

u_int A_err_flag, B_err_flag, C_err_flag;
u_short  A_err_current, B_err_current, C_err_current;
unsigned long  A_err_sum, B_err_sum, C_err_sum;
u_char error_value;
extern int loss_open, A_flicker_open, A_voltage_dipswellinterrupt_open;
extern int B_flicker_open, B_voltage_dipswellinterrupt_open, C_flicker_open, C_voltage_dipswellinterrupt_open;;
char A_fre_flag = 0;
u_char A_flag1 = 0, A_flag2 = 0, B_flag1 = 0, B_flag2 = 0, C_flag1 = 0, C_flag2 = 0;
u_short A_flag, B_flag, C_flag, stand_flag, A_temp, B_temp, C_temp, A_temp_last, B_temp_last, C_temp_last;
u_char error_flag = 0;
extern int packet_offset;
void ethernet_protocol_packet_callback1(u_char *user_data,
	const struct pcap_pkthdr *packet_header,
	const u_char *packet_content)
{
	an_point *sample;
	(VOID)(user_data);
	sample = (an_point *)(packet_content + packet_offset);

	if (!A_flag1)  //ִֻ��һ�εõ���ʼֵ
	{
		A_flag = ntohs(sample->stand_flag);
		stand_flag = (A_flag - 1) % mod_value;
		A_flag1 = 1;
	}
	A_temp = ntohs(sample->stand_flag);
	if (loss_open)
	{
		if (A_temp != ((A_temp_last + 1) % mod_value))
		{
			error_flag = 1;
			A_err_current = (((short)(A_temp - A_temp_last - 1)) % mod_value);
			A_err_sum += A_err_current;
			A_err_flag++;
		}
	}
	A_temp_last = A_temp;
	if (A_temp == stand_flag)
	{
		A_flag2 = 1;
	}
	if (A_flag2)
	{
		if (an_buffer_idx_A < AN_BUFFER_880kLEN)   //     880000 1000����   20s
		{
			//ntohs�����ǽ�һ��16λ���������ֽ�˳��ת��Ϊ�����ֽ�˳�� 
			#if (accuracy == 16)
			{
				an_buffer[an_buffer_idx_A] = ntohs(sample->an_ch0);//A��ѹ
				an_buffer_cur[an_buffer_idx_A] = ntohs(sample->an_ch1);//A����
			}
			#else
			{ 
			an_buffer[an_buffer_idx_A] = ntohl(sample->an_ch0)/4;//A��ѹ
			an_buffer_cur[an_buffer_idx_A] = ntohl(sample->an_ch0)/4;//A����
			}
			#endif
			an_buffer_idx_A++;
			/********************************************�����ڼ���**********************************************************************************/
			if (an_buffer_idx_A % 400 == 0) //400(�������)
			{
				ResumeThread(handleA_HalfPeriodThread);
			}
			if (an_buffer_idx_A % Plus_8000 == 0)// ��Լ10����  ���������߳�
			{
				index_8800_A = an_buffer_idx_A / Plus_8000;
				an_buffer_8800flag_A = 1;
				ResumeThread(handleFFT_AThread);//����
			}
		}
		else
		{
			an_buffer_idx_A = 0;
			#if (accuracy == 16)
			{
				an_buffer[an_buffer_idx_A] = (short)ntohs(sample->an_ch0);
				an_buffer_cur[an_buffer_idx_A] = (short)ntohs(sample->an_ch1);
			}
			#else
			{
				an_buffer[an_buffer_idx_A] = ntohl(sample->an_ch0) / 4;
				an_buffer_cur[an_buffer_idx_A] = ntohl(sample->an_ch1)/4;
			}
			#endif
			an_buffer_idx_A++;
		}
	}
	A_packet_number = 1;
}

char B_fre_flag = 0;
int index_8800_B = 0;
unsigned long an_buffer_idx_B = 0;
short an_buffer_b[AN_BUFFER_880kLEN];
short an_buffer_b_cur[AN_BUFFER_880kLEN];
unsigned long an_buffer_8800flag_B = 0;
void ethernet_protocol_packet_callback2(u_char *user_data,
	const struct pcap_pkthdr *packet_header,
	const u_char *packet_content)
{
	an_point *sample;
	(VOID)(user_data);
	sample = (an_point *)(packet_content + packet_offset);
	if (!B_flag1)  //ִֻ��һ�εõ���ʼֵ
	{
		B_flag = ntohs(sample->stand_flag);
		B_flag1 = 1;
	}
	B_temp = ntohs(sample->stand_flag);
	if (loss_open)
	{
		if (B_temp != ((B_temp_last + 1) % mod_value))
		{
			error_flag = 1;
			B_err_current = (((short)(B_temp - B_temp_last - 1)) % mod_value);
			B_err_sum += B_err_current;
			B_err_flag++;
		}
	}
	B_temp_last = B_temp;
	if (B_temp == stand_flag)
	{
		B_flag2 = 1;
	}
	if (B_flag2)
	{
		if (an_buffer_idx_B < AN_BUFFER_880kLEN)   //     880000 1000����   20s
		{
			//ntohs�����ǽ�һ��16λ���������ֽ�˳��ת��Ϊ�����ֽ�˳�� 
			#if (accuracy == 16)
			{
				an_buffer_b[an_buffer_idx_B] = ntohs(sample->an_ch2);//B��ѹ
				an_buffer_b_cur[an_buffer_idx_B] = ntohs(sample->an_ch3);//B��ѹ
			}
			#else
			{
				an_buffer_b[an_buffer_idx_B] = ntohl(sample->an_ch0) / 4;//B��ѹ
				an_buffer_b_cur[an_buffer_idx_B] = ntohl(sample->an_ch0) / 4;//B��ѹ
			}
			#endif
			an_buffer_idx_B++;
			/********************************************�����ڼ���**********************************************************************************/
			if (an_buffer_idx_B % 400 == 0) //400(�������)
			{
				ResumeThread(handleB_HalfPeriodThread);
			}
			if (an_buffer_idx_B % Plus_8000 == 0)// ��Լ10����  ���������߳�
			{
				index_8800_B = an_buffer_idx_B / Plus_8000;
				an_buffer_8800flag_B = 1;
				ResumeThread(handleFFT_BThread);//����
			}
		}
		else
		{
			an_buffer_idx_B = 0;
			#if (accuracy == 16)
			{
				an_buffer_b[an_buffer_idx_B] = ntohs(sample->an_ch2);//B��ѹ
				an_buffer_b_cur[an_buffer_idx_B] = ntohs(sample->an_ch3);//B��ѹ
			}
			#else
			{
				an_buffer_b[an_buffer_idx_B] = ntohl(sample->an_ch3) / 4;//B��ѹ
				an_buffer_b_cur[an_buffer_idx_B] = ntohl(sample->an_ch0) / 4;//B��ѹ
			}
			#endif
			an_buffer_idx_B++;
		}
	}
	B_packet_number = 1;
}

char C_fre_flag = 0;
int index_8800_C = 0;
unsigned long an_buffer_idx_C = 0;
short an_buffer_c[AN_BUFFER_880kLEN];
short an_buffer_c_cur[AN_BUFFER_880kLEN];
unsigned long an_buffer_8800flag_C = 0;
void ethernet_protocol_packet_callback3(u_char *user_data,
	const struct pcap_pkthdr *packet_header,
	const u_char *packet_content)
{
	an_point *sample;
	(VOID)(user_data);
	sample = (an_point *)(packet_content + packet_offset);

	if (!C_flag1)  //ִֻ��һ�εõ���ʼֵ
	{
		C_flag = ntohs(sample->stand_flag) - 1;
		C_flag1 = 1;
	}
	C_temp = ntohs(sample->stand_flag);
	if (loss_open)
	{
		if (C_temp != ((C_temp_last + 1) % mod_value))
		{
			error_flag = 1;
			C_err_current = (((short)(C_temp - C_temp_last - 1)) % mod_value);
			C_err_sum += C_err_current;
			C_err_flag++;
		}
	}
	C_temp_last = C_temp;
	if (C_temp == stand_flag)
	{
		C_flag2 = 1;
	}
	if (C_flag2)
	{
		if (an_buffer_idx_C < AN_BUFFER_880kLEN)   //     880000 1000����   20s
		{
			//ntohs�����ǽ�һ��16λ���������ֽ�˳��ת��Ϊ�����ֽ�˳�� 
			#if (accuracy == 16)
			{
				an_buffer_c[an_buffer_idx_C] = ntohs(sample->an_ch4);//C��ѹ
				an_buffer_c_cur[an_buffer_idx_C] = ntohs(sample->an_ch5);//C��ѹ
			}
			#else
			{
				an_buffer_c[an_buffer_idx_C] = ntohl(sample->an_ch0) / 4;//C��ѹ  
				an_buffer_c_cur[an_buffer_idx_C] = ntohl(sample->an_ch0) / 4;//C��ѹ  
			}
			#endif
			an_buffer_idx_C++;
			/********************************************�����ڼ���**********************************************************************************/
			if (an_buffer_idx_C % 400 == 0) //400(�������)
			{
				ResumeThread(handleC_HalfPeriodThread);
			}
			if (an_buffer_idx_C % Plus_8000 == 0)// ��Լ10����  ���������߳�
			{
				index_8800_C = an_buffer_idx_C / Plus_8000;
				an_buffer_8800flag_C = 1;
				ResumeThread(handleFFT_CThread);//����
			}
		}
		else
		{
			an_buffer_idx_C = 0;
			#if (accuracy == 16)
			{
				an_buffer_c[an_buffer_idx_C] = ntohs(sample->an_ch4);//C��ѹ
				an_buffer_c_cur[an_buffer_idx_C] = ntohs(sample->an_ch5);//C��ѹ
			}
			#else
			{
				an_buffer_c[an_buffer_idx_C] = ntohl(sample->an_ch0) / 4;//C��ѹ  
				an_buffer_c_cur[an_buffer_idx_C] = ntohl(sample->an_ch0) / 4;//C��ѹ  
			}
			#endif
			an_buffer_idx_C++;
		}
	}
	C_packet_number = 1;
}

double A_rms = 0, B_rms = 0, C_rms = 0;//A���ѹ��Чֵ
double A_cur_rms, B_cur_rms, C_cur_rms;//A�������Чֵ
double A_active_power;//A�й�����
double A_reactive_power;//A�޹�����
double A_apparent_power = 0;
double A_active_power_meter = 0;//�й����
double A_reactive_power_meter = 0;//�޹����
double THDU = 0;
double A_fre;
int pointfre = 0;
int A_FFT_Number=0;
double fuzhi_a[HarmonicWave] = { 0.0 };
double fuzhi_a_cur[HarmonicWave] = { 0.0 };
int A_HarmonicIndex = 0;
double fuzhi_a_temp[HarmonicWaveParam][HarmonicWave] = { 0.0 };
double fuzhi_a_ave[HarmonicWave] = { 0.0 };
double fuzhi_a_cur_temp[HarmonicWaveParam][HarmonicWave] = { 0.0 };
double fuzhi_a_cur_ave[HarmonicWave] = { 0.0 };
double fftw_phase_a_vol[AN_BUFFER_LEN_8000];
double fftw_phase_a_cur[AN_BUFFER_LEN_8000];
double fftw_phase_differ[AN_BUFFER_LEN_8000];//��ѹ������������
u_long A_FFT = 0, B_FFT = 0, C_FFT = 0;
double A_jibophase[100], B_jibophase[100], C_jibophase[100];
DWORD WINAPI FFT_AThreadFunc(LPVOID param)
{
	int i, j;
	double fftw_ampout_a_fre[AN_FFT_LEN_8000];
	double fftw_ampout_fuzhi[PeriodPointMax * 10];
	double fftw_ampout_cur_fuzhi[PeriodPointMax * 10];
	short buffer_8800_a[Plus_8000];
	short buffer_8800_cur[Plus_8000];
	
	fftw_complex *in, *out;
	fftw_plan p;
	fftw_complex *in_fuzhi, *out_fuzhi, *in_ia_fuzhi, *out_ia_fuzhi;
	fftw_plan p_fuzhi, p_ia_fuzhi;

	EnterCriticalSection(&g_cs);
	in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * AN_BUFFER_LEN_8000);  //4000
	out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * AN_BUFFER_LEN_8000);
	in_fuzhi = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * PeriodPointMax * 10);  //4000
	out_fuzhi = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * PeriodPointMax * 10);
	in_ia_fuzhi = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * PeriodPointMax * 10);
	out_ia_fuzhi = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * PeriodPointMax * 10);
	LeaveCriticalSection(&g_cs);

	double hannn[FFT_8000] = { 0 };
	double alpha_a[HarmonicWave] = { 0 };
	double alpha_cur[HarmonicWave] = { 0 };
	double sum_vol, sum_cur, reactive_power_temp, active_power_temp;
	double deta_a[HarmonicWave] = { 0 };
	int index;
	double THDU_temp = 0;
	double THDI_temp = 0;
	while (1)
	{
		if (an_buffer_8800flag_A == 1)
		{
			sum_vol = 0; sum_cur = 0; active_power_temp = 0; reactive_power_temp = 0; THDU_temp = 0; THDI_temp = 0;
			for (int h = 0; h < Plus_8000; h++)
			{
				buffer_8800_a[h] = an_buffer[Plus_8000 * (index_8800_A - 1) + h];   //�������10������
				buffer_8800_cur[h] = an_buffer_cur[Plus_8000 * (index_8800_A - 1) + h];
			}
			for (i = 0; i < AN_BUFFER_LEN_8000; i++)
			{
				hannn[i] = 0.5 - 0.5 *cos(2 * PI* i / FFT_8000);
				in[i][0] = buffer_8800_a[i] * hannn[i];  //��8800���ǰ8000����FFT�Ӻ�����
			}
			EnterCriticalSection(&g_cs);
			p = fftw_plan_dft_1d(AN_BUFFER_LEN_8000, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
			fftw_execute(p);
			fftw_destroy_plan(p);
			LeaveCriticalSection(&g_cs);
			
			fftw_ampout_a_fre[9] = sqrt(out[9][0] * out[9][0] + out[9][1] * out[9][1]) / (FFT_8000 / 2);
			fftw_ampout_a_fre[10] = sqrt(out[10][0] * out[10][0] + out[10][1] * out[10][1]) / (FFT_8000 / 2);
			fftw_ampout_a_fre[11] = sqrt(out[11][0] * out[11][0] + out[11][1] * out[11][1]) / (FFT_8000 / 2);
			//--------------г����������ֵ-------------------
			//�����߲�ֵ   Ƶ�׷ֱ���5Hz
			/************************************************************Ƶ��**************************************************************************/
			alpha_a[0] = (fftw_ampout_a_fre[11] - fftw_ampout_a_fre[9]) / fftw_ampout_a_fre[10];
			//Ƶ��������ʽ
			deta_a[0] = 0.6666 * alpha_a[0]
				- 0.073 * alpha_a[0] * alpha_a[0] * alpha_a[0]
				+ 0.0126 * alpha_a[0] * alpha_a[0] * alpha_a[0] * alpha_a[0] * alpha_a[0];
			//Ƶ��������ʽ
			//���Ĺ�ʽ11    0.2=1/5=2000��/10000Hz   N/fs  ����Ƶ��40000Hz-25us
			A_fre = (deta_a[0] + 10) / 0.2;
			if ((A_fre > 47.5) && (A_fre < 55.0))
				A_fre_flag = 1;
			else
				A_fre_flag = 0;
			/*****************************************************************��ѹ������Чֵ �й������ڹ��� �й����޹����******************************************************************************/
		if(A_fre_flag)
		{ 
			pointfre = (int)(40000 / A_fre + 0.5);
			A_FFT_Number = (int)(double(40000 / A_fre) * 10 + 0.5);
			if ((A_FFT_Number % 2) != 0)
				A_FFT_Number++;
			A_FFT++;
			for (i = 0; i < A_FFT_Number; i++)  //800������
			{
				sum_vol += buffer_8800_a[i] / factor_factor * buffer_8800_a[i] / factor_factor;
				sum_cur += buffer_8800_cur[i] / factor_factor * buffer_8800_cur[i] / factor_factor;
				active_power_temp += buffer_8800_a[i] / factor_factor * buffer_8800_cur[i] / factor_factor;//˲ʱ�й�����         ��ѹ˲ʱֵ*����˲ʱֵ(û�й�һ��)
			}
			A_active_power = active_power_temp / A_FFT_Number  ;//�й�����       ˲ʱ�й����ʵ��ۼ�
			A_active_power_meter += A_active_power * 0.2 / 3600 / 1000;//�й����         ��λKW.h
			A_rms = sqrt((double)(sum_vol / A_FFT_Number));   //��ѹ��Чֵ     ������
			A_cur_rms = sqrt((double)(sum_cur / A_FFT_Number)) ;   //������Чֵ      ������
			A_apparent_power = A_rms * A_cur_rms;//���ڹ���          ��ѹ��Чֵ*������Чֵ
/******************************************��ѹ����  ֱ������+г����ֵ***************************************************/
			for (i = 0; i < A_FFT_Number; i++)
			{
				in_fuzhi[i][0] = buffer_8800_a[i];
				in_ia_fuzhi[i][0] = buffer_8800_cur[i];
			}
			EnterCriticalSection(&g_cs);
			p_fuzhi = fftw_plan_dft_1d(A_FFT_Number, in_fuzhi, out_fuzhi, FFTW_FORWARD, FFTW_ESTIMATE);
			p_ia_fuzhi = fftw_plan_dft_1d(A_FFT_Number, in_ia_fuzhi, out_ia_fuzhi, FFTW_FORWARD, FFTW_ESTIMATE);
			fftw_execute(p_fuzhi);
			fftw_execute(p_ia_fuzhi);
			fftw_destroy_plan(p_fuzhi);
			fftw_destroy_plan(p_ia_fuzhi);
			LeaveCriticalSection(&g_cs);
			//fftw_ampout_fuzhi[0] = sqrt(out_fuzhi[0][0] * out_fuzhi[0][0] + out_fuzhi[0][1] * out_fuzhi[0][1]) / (A_FFT_Number);
			//fftw_ampout_cur_fuzhi[0] = sqrt(out_ia_fuzhi[0][0] * out_ia_fuzhi[0][0] + out_ia_fuzhi[0][1] * out_ia_fuzhi[0][1]) / (A_FFT_Number);	
			fftw_ampout_fuzhi[0] = out_fuzhi[0][0] / A_FFT_Number;
			fftw_ampout_cur_fuzhi[0] = out_ia_fuzhi[0][0] / A_FFT_Number;
			for (j = 1; j < 420; j++)
			{
				fftw_ampout_fuzhi[j] = sqrt(out_fuzhi[j][0] * out_fuzhi[j][0] + out_fuzhi[j][1] * out_fuzhi[j][1]) / (A_FFT_Number / 2);
				fftw_ampout_cur_fuzhi[j] = sqrt(out_ia_fuzhi[j][0] * out_ia_fuzhi[j][0] + out_ia_fuzhi[j][1] * out_ia_fuzhi[j][1]) / (A_FFT_Number / 2);	
			}
			for (j = 0; j < (HarmonicWave); j++)       //A���ѹ��������+г��   BC���ѹ����+г��
			{
				index = j * 10;
				//fuzhi_a[j] = fftw_ampout_fuzhi[index] / factor_factor;
				fuzhi_a_temp[A_HarmonicIndex][j]= fftw_ampout_fuzhi[index] / factor_factor;

				fftw_phase_a_vol[j] = atan2(out_fuzhi[index][1], out_fuzhi[index][0]) + PI / 2;
				//fuzhi_a_cur[j] = fftw_ampout_cur_fuzhi[index] / factor_factor;
				fuzhi_a_cur_temp[A_HarmonicIndex][j] = fftw_ampout_cur_fuzhi[index] / factor_factor;
				fftw_phase_a_cur[j] = atan2(out_ia_fuzhi[index][1], out_ia_fuzhi[index][0]) + PI / 2;
				/*************************************************A�๦��������*****************************************************************************/
				if (fabs(fftw_phase_a_vol[j] - fftw_phase_a_cur[j]) > PI)
				{
					fftw_phase_differ[j] = 2 * PI - fabs(fftw_phase_a_vol[j] - fftw_phase_a_cur[j]);
				}
				else
				{
					fftw_phase_differ[j] = fabs(fftw_phase_a_vol[j] - fftw_phase_a_cur[j]);
				}
			}
			//г����ƽ��ֵ
			A_HarmonicIndex++;
			if (A_HarmonicIndex == HarmonicWaveParam)
			{
				A_HarmonicIndex = 0;
				for (int j = 0; j < HarmonicWave; j++)
				{
					for (int i = 0; i < HarmonicWaveParam;i++)
					{
						fuzhi_a_ave[j] += fuzhi_a_temp[i][j];
						fuzhi_a_cur_ave[j] += fuzhi_a_cur_temp[i][j];
					}
					fuzhi_a_ave[j] = fuzhi_a_ave[j] / HarmonicWaveParam;
					fuzhi_a[j] = fuzhi_a_ave[j];    //���ս��
					fuzhi_a_ave[j] = 0;
					fuzhi_a_cur_ave[j] = fuzhi_a_cur_ave[j] / HarmonicWaveParam;
					fuzhi_a_cur[j] = fuzhi_a_cur_ave[j];    //���ս��
					fuzhi_a_cur_ave[j] = 0;
				}
			}
			A_jibophase[A_FFT% phase_param2] = fftw_phase_a_vol[1];
			for (j = 2; j < HarmonicWave; j++)
			{
				THDU_temp += fuzhi_a[j] * fuzhi_a[j];
				THDI_temp += fuzhi_a_cur[j] * fuzhi_a_cur[j];
			}
			THDU = sqrt(THDU_temp) / fuzhi_a[1] * 100;
			reactive_power_temp += fuzhi_a[1] * fuzhi_a_cur[1] * sin(fftw_phase_differ[1]);//����ϵ��
	        //�޹�����
			A_reactive_power = reactive_power_temp / 2;
			//�޹����
			A_reactive_power_meter += A_reactive_power * 0.2 / 3600 / 1000;
			an_buffer_8800flag_A = 0;
           }
		}
		SuspendThread(handleFFT_AThread);//����
	}
	fftw_free(in);
	fftw_free(out);
	fftw_free(in_fuzhi);
	fftw_free(in_ia_fuzhi);
	fftw_free(out_fuzhi);
	fftw_free(out_ia_fuzhi);
	return 0;
}
double B_fre;
int B_FFT_Number = 0;
double B_active_power;//A�й�����
double B_reactive_power;//A�޹�����
double B_apparent_power = 0;
double B_active_power_meter = 0;//�й����
double B_reactive_power_meter = 0;//�޹����
double B_THDU, B_THDI;
double fuzhi_b[HarmonicWave] = { 0.0 };
double fuzhi_b_cur[HarmonicWave] = { 0.0 };
int B_HarmonicIndex = 0;
double fuzhi_b_temp[HarmonicWaveParam][HarmonicWave] = { 0.0 };
double fuzhi_b_ave[HarmonicWave] = { 0.0 };
double fuzhi_b_cur_temp[HarmonicWaveParam][HarmonicWave] = { 0.0 };
double fuzhi_b_cur_ave[HarmonicWave] = { 0.0 };
double fftw_phase_differ_b[AN_BUFFER_LEN_8000];//��ѹ������������
double fftw_phase_b[AN_BUFFER_LEN_8000];
double fftw_phase_b_cur[HarmonicWave];
DWORD WINAPI FFT_BThreadFunc(LPVOID param)
{
	int i, j;
	double fftw_ampout_b[AN_FFT_LEN_8000];
	double fftw_ampout_b_fuzhi[PeriodPointMax * 10];	
	double fftw_ampout_bi_fuzhi[PeriodPointMax * 10];
	short buffer_8800_b[Plus_8000];
	short buffer_8800_b_cur[Plus_8000];

	fftw_complex *in_fre_b, *out_fre_b;
	fftw_plan p_fre_b;
	fftw_complex *in_b_fuzhi, *out_b_fuzhi, *in_bi_fuzhi, *out_bi_fuzhi;
	fftw_plan p_b_fuzhi, p_bi_fuzhi;
	EnterCriticalSection(&g_cs);
	in_fre_b = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * AN_BUFFER_LEN_8000);
	out_fre_b = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * AN_BUFFER_LEN_8000);
	in_b_fuzhi = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * PeriodPointMax * 10);
	out_b_fuzhi = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * PeriodPointMax * 10);	
	in_bi_fuzhi = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * PeriodPointMax * 10);
	out_bi_fuzhi = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * PeriodPointMax * 10);
	LeaveCriticalSection(&g_cs);
	double hannn_b[FFT_8000] = { 0 };
	double alpha_b[HarmonicWave] = { 0 };
	double alpha_b_cur[HarmonicWave] = { 0 };
	double deta_b[HarmonicWave] = { 0 };
	double B_sum_vol, B_sum_cur, B_reactive_power_temp, B_active_power_temp;
	
	int index;
	double B_THDU_temp = 0;
	double B_THDI_temp = 0;
	while (1)
	{
		if (an_buffer_8800flag_B == 1)
		{	
			B_sum_vol = 0; B_sum_cur = 0; B_active_power_temp = 0; B_reactive_power_temp = 0; B_THDU_temp = 0; B_THDI_temp = 0;
			for (int h = 0; h < Plus_8000; h++)
			{
				buffer_8800_b[h] = an_buffer_b[Plus_8000 * (index_8800_B - 1) + h];
				buffer_8800_b_cur[h] = an_buffer_b_cur[Plus_8000 * (index_8800_B - 1) + h];
				in_b_fuzhi[h][0] = buffer_8800_b[h];
				in_bi_fuzhi[h][0] = buffer_8800_b_cur[h];
			}
			for (i = 0; i < AN_BUFFER_LEN_8000; i++)
			{
				hannn_b[i] = 0.5 - 0.5 *cos(2 * PI* i / FFT_8000);
				in_fre_b[i][0] = buffer_8800_b[i] * hannn_b[i];  //��8800���ǰ8000����FFT�Ӻ�����
			}

			EnterCriticalSection(&g_cs);
			p_fre_b = fftw_plan_dft_1d(AN_BUFFER_LEN_8000, in_fre_b, out_fre_b, FFTW_FORWARD, FFTW_ESTIMATE);
			fftw_execute(p_fre_b);
			fftw_destroy_plan(p_fre_b);
			LeaveCriticalSection(&g_cs);
			
			fftw_ampout_b[9] = sqrt(out_fre_b[9][0] * out_fre_b[9][0] + out_fre_b[9][1] * out_fre_b[9][1]) / (FFT_8000 / 2);
			fftw_ampout_b[10] = sqrt(out_fre_b[10][0] * out_fre_b[10][0] + out_fre_b[10][1] * out_fre_b[10][1]) / (FFT_8000 / 2);
			fftw_ampout_b[11] = sqrt(out_fre_b[11][0] * out_fre_b[11][0] + out_fre_b[11][1] * out_fre_b[11][1]) / (FFT_8000 / 2);
			//--------------г����������ֵ-------------------
			//�����߲�ֵ   Ƶ�׷ֱ���5Hz
			/************************************************************Ƶ��**************************************************************************/
			alpha_b[0] = (fftw_ampout_b[11] - fftw_ampout_b[9]) / fftw_ampout_b[10];
			//Ƶ��������ʽ
			deta_b[0] = 0.6666 * alpha_b[0]
				- 0.073 * alpha_b[0] * alpha_b[0] * alpha_b[0]
				+ 0.0126 * alpha_b[0] * alpha_b[0] * alpha_b[0] * alpha_b[0] * alpha_b[0];
			//Ƶ��������ʽ
			//���Ĺ�ʽ11    0.2=1/5=2000��/10000Hz   N/fs  ����Ƶ��40000Hz-25us
			B_fre = (deta_b[0] + 10) / 0.2;
			if ((B_fre > 47.5) && (B_fre < 55.0))
				B_fre_flag = 1;
			else
				B_fre_flag = 0;

			if (B_fre_flag)
			{
				B_FFT_Number= (int)(double(40000 / B_fre) * 10 + 0.5);
				if ((B_FFT_Number % 2) != 0)
					B_FFT_Number++;
				B_FFT++;

			EnterCriticalSection(&g_cs);
			p_b_fuzhi = fftw_plan_dft_1d(B_FFT_Number, in_b_fuzhi, out_b_fuzhi, FFTW_FORWARD, FFTW_ESTIMATE);
			p_bi_fuzhi = fftw_plan_dft_1d(B_FFT_Number, in_bi_fuzhi, out_bi_fuzhi, FFTW_FORWARD, FFTW_ESTIMATE);
			fftw_execute(p_b_fuzhi);
			fftw_execute(p_bi_fuzhi);
			fftw_destroy_plan(p_b_fuzhi);
			fftw_destroy_plan(p_bi_fuzhi);
			LeaveCriticalSection(&g_cs);
			
			/*fftw_ampout_b_fuzhi[0] = sqrt(out_b_fuzhi[0][0] * out_b_fuzhi[0][0] + out_b_fuzhi[0][1] * out_b_fuzhi[0][1]) / (B_FFT_Number);
			fftw_ampout_bi_fuzhi[0] = sqrt(out_bi_fuzhi[0][0] * out_bi_fuzhi[0][0] + out_bi_fuzhi[0][1] * out_bi_fuzhi[0][1]) / (B_FFT_Number);*/
			fftw_ampout_b_fuzhi[0] = out_b_fuzhi[0][0] / B_FFT_Number;
			fftw_ampout_bi_fuzhi[0] = out_bi_fuzhi[0][0] / B_FFT_Number;

			for (j = 1; j < 420; j++)
			{
				fftw_ampout_b_fuzhi[j] = sqrt(out_b_fuzhi[j][0] * out_b_fuzhi[j][0] + out_b_fuzhi[j][1] * out_b_fuzhi[j][1]) / (B_FFT_Number / 2);
				fftw_ampout_bi_fuzhi[j] = sqrt(out_bi_fuzhi[j][0] * out_bi_fuzhi[j][0] + out_bi_fuzhi[j][1] * out_bi_fuzhi[j][1]) / (B_FFT_Number / 2);
			}
			for (j = 0; j < (HarmonicWave ); j++)       //��ѹ����+г��
			{
				index = j * 10;
				//fuzhi_b[j] = fftw_ampout_b_fuzhi[index] / factor_factor;
				fuzhi_b_temp[B_HarmonicIndex][j] = fftw_ampout_b_fuzhi[index] / factor_factor;
				fftw_phase_b[j] = atan2(out_b_fuzhi[index][1], out_b_fuzhi[index][0]) + PI / 2;
				//fuzhi_b_cur[j] = fftw_ampout_bi_fuzhi[index] / factor_factor;
				fuzhi_b_cur_temp[B_HarmonicIndex][j] = fftw_ampout_bi_fuzhi[index] / factor_factor;
				fftw_phase_b_cur[j] = atan2(out_bi_fuzhi[index][1], out_bi_fuzhi[index][0]) + PI / 2;
				/*************************************************A�๦��������*****************************************************************************/
				if (fabs(fftw_phase_b[j] - fftw_phase_b_cur[j]) > PI)
				{
					fftw_phase_differ_b[j] = 2 * PI - fabs(fftw_phase_b[j] - fftw_phase_b_cur[j]);
				}
				else
				{
					fftw_phase_differ_b[j] = fabs(fftw_phase_b[j] - fftw_phase_b_cur[j]);
				}
			}
			//г����ƽ��ֵ
			B_HarmonicIndex++;
			if (B_HarmonicIndex == HarmonicWaveParam)
			{
				B_HarmonicIndex = 0;
				for (int j = 0; j < HarmonicWave; j++)
				{
					for (int i = 0; i < HarmonicWaveParam; i++)
					{
						fuzhi_b_ave[j] += fuzhi_b_temp[i][j];
						fuzhi_b_cur_ave[j] += fuzhi_b_cur_temp[i][j];
					}
					fuzhi_b_ave[j] = fuzhi_b_ave[j] / HarmonicWaveParam;
					fuzhi_b[j] = fuzhi_b_ave[j];    //���ս��
					fuzhi_b_ave[j] = 0;

					fuzhi_b_cur_ave[j] = fuzhi_b_cur_ave[j] / HarmonicWaveParam;
					fuzhi_b_cur[j] = fuzhi_b_cur_ave[j];    //���ս��
					fuzhi_b_cur_ave[j] = 0;
				}
			}

			B_jibophase[B_FFT% phase_param2] = fftw_phase_b[1];
			for (i = 0; i < B_FFT_Number; i++)  //800������
			{
				B_sum_vol += buffer_8800_b[i] / factor_factor * buffer_8800_b[i] / factor_factor;
				B_sum_cur += buffer_8800_b_cur[i] / factor_factor * buffer_8800_b_cur[i] / factor_factor;
				B_active_power_temp += buffer_8800_b[i] / factor_factor * buffer_8800_b_cur[i] / factor_factor;//˲ʱ�й�����   
			}
			B_active_power = B_active_power_temp / B_FFT_Number;//�й�����       ˲ʱ�й����ʵ��ۼ�
			B_active_power_meter += B_active_power * 0.2 / 3600 / 1000;//�й����         ��λKW.h
			B_rms = sqrt((double)(B_sum_vol / B_FFT_Number));   //��ѹ��Чֵ     ������
			B_cur_rms = sqrt((double)(B_sum_cur / B_FFT_Number));   //������Чֵ      ������
			B_apparent_power = B_rms * B_cur_rms;//���ڹ���          ��ѹ��Чֵ*������Чֵ

			for (j = 2; j < HarmonicWave; j++)
			{
				B_THDU_temp += fuzhi_b[j] * fuzhi_b[j];
				B_THDI_temp += fuzhi_b_cur[j] * fuzhi_b_cur[j];
			}
			B_THDU = sqrt(B_THDU_temp) / fuzhi_b[1] * 100;
			B_reactive_power_temp += fuzhi_b[1] * fuzhi_b_cur[1] * sin(fftw_phase_differ_b[1]);//����ϵ��
			//�޹�����
			B_reactive_power = B_reactive_power_temp / 2;
			//�޹����
			B_reactive_power_meter += B_reactive_power * 0.2 / 3600 / 1000;
			an_buffer_8800flag_B = 0;
			}
		}
		SuspendThread(handleFFT_BThread);//����
	}
	fftw_free(in_b_fuzhi);
	fftw_free(out_b_fuzhi);
	return 0;
}
double C_fre;
int C_FFT_Number = 0;
double C_active_power;//A�й�����
double C_reactive_power;//A�޹�����
double C_apparent_power = 0;
double C_active_power_meter = 0;//�й����
double C_reactive_power_meter = 0;//�޹����
double C_THDU, C_THDI;
double fuzhi_c[HarmonicWave] = { 0.0 };
double fuzhi_c_cur[HarmonicWave] = { 0.0 };
int C_HarmonicIndex = 0;
double fuzhi_c_temp[HarmonicWaveParam][HarmonicWave] = { 0.0 };
double fuzhi_c_ave[HarmonicWave] = { 0.0 };
double fuzhi_c_cur_temp[HarmonicWaveParam][HarmonicWave] = { 0.0 };
double fuzhi_c_cur_ave[HarmonicWave] = { 0.0 };
double fftw_phase_differ_c[AN_BUFFER_LEN_8000];//��ѹ������������
double fftw_phase_c[AN_BUFFER_LEN_8000];
double fftw_phase_c_cur[HarmonicWave];
DWORD WINAPI FFT_CThreadFunc(LPVOID param)
{
	int i, j;
	double fftw_ampout_c[AN_FFT_LEN_8000];
	double fftw_ampout_c_fuzhi[PeriodPointMax * 10];
	double fftw_ampout_ci_fuzhi[PeriodPointMax * 10];
	short buffer_8800_c[Plus_8000];
	short buffer_8800_c_cur[Plus_8000];
	fftw_complex *in_c_fre, *out_c_fre;
	fftw_plan p_c_fre;
	fftw_complex *in_c_fuzhi, *out_c_fuzhi, *in_ci_fuzhi, *out_ci_fuzhi;
	fftw_plan p_c_fuzhi, p_ci_fuzhi;

	EnterCriticalSection(&g_cs);
	in_c_fre = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * AN_BUFFER_LEN_8000);  //4000
	out_c_fre = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * AN_BUFFER_LEN_8000);
	in_c_fuzhi = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * PeriodPointMax * 10);
	out_c_fuzhi = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * PeriodPointMax * 10);
	in_ci_fuzhi = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * PeriodPointMax * 10);
	out_ci_fuzhi = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * PeriodPointMax * 10);
	LeaveCriticalSection(&g_cs);

	double hannn_c[FFT_8000] = { 0 };
	double alpha_c[HarmonicWave] = { 0 };
	double alpha_c_cur[HarmonicWave] = { 0 };
	double deta_c[HarmonicWave] = { 0 };
	double C_sum_vol, C_sum_cur, C_reactive_power_temp, C_active_power_temp;
	int index;
	double C_THDU_temp = 0;
	double C_THDI_temp = 0;
	while (1)
	{
		if (an_buffer_8800flag_C == 1)
		{
			C_sum_vol = 0; C_sum_cur = 0; C_reactive_power_temp = 0; C_active_power_temp = 0; C_THDU_temp = 0; C_THDI_temp = 0;
			for (int h = 0; h < Plus_8000; h++)
			{
				buffer_8800_c[h] = an_buffer_c[Plus_8000 * (index_8800_C - 1) + h];
				buffer_8800_c_cur[h] = an_buffer_c_cur[Plus_8000 * (index_8800_C - 1) + h];
				in_c_fuzhi[h][0] = buffer_8800_c[h];
				in_ci_fuzhi[h][0] = buffer_8800_c_cur[h];
			}
			for (i = 0; i < AN_BUFFER_LEN_8000; i++)
			{
				hannn_c[i] = 0.5 - 0.5 *cos(2 * PI* i / FFT_8000);
				in_c_fre[i][0] = buffer_8800_c[i] * hannn_c[i];  //��8800���ǰ8000����FFT�Ӻ�����
			}

			EnterCriticalSection(&g_cs);
			p_c_fre = fftw_plan_dft_1d(AN_BUFFER_LEN_8000, in_c_fre, out_c_fre, FFTW_FORWARD, FFTW_ESTIMATE);
			fftw_execute(p_c_fre);
			fftw_destroy_plan(p_c_fre);
			LeaveCriticalSection(&g_cs);
			
			fftw_ampout_c[9] = sqrt(out_c_fre[9][0] * out_c_fre[9][0] + out_c_fre[9][1] * out_c_fre[9][1]) / (FFT_8000 / 2);
			fftw_ampout_c[10] = sqrt(out_c_fre[10][0] * out_c_fre[10][0] + out_c_fre[10][1] * out_c_fre[10][1]) / (FFT_8000 / 2);
			fftw_ampout_c[11] = sqrt(out_c_fre[11][0] * out_c_fre[11][0] + out_c_fre[11][1] * out_c_fre[11][1]) / (FFT_8000 / 2);
			//--------------г����������ֵ-------------------
			//�����߲�ֵ   Ƶ�׷ֱ���5Hz
			/************************************************************Ƶ��**************************************************************************/
			alpha_c[0] = (fftw_ampout_c[11] - fftw_ampout_c[9]) / fftw_ampout_c[10];
			//Ƶ��������ʽ
			deta_c[0] = 0.6666 * alpha_c[0]
				- 0.073 * alpha_c[0] * alpha_c[0] * alpha_c[0]
				+ 0.0126 * alpha_c[0] * alpha_c[0] * alpha_c[0] * alpha_c[0] * alpha_c[0];
			//Ƶ��������ʽ
			//���Ĺ�ʽ11    0.2=1/5=2000��/10000Hz   N/fs  ����Ƶ��40000Hz-25us
			C_fre = (deta_c[0] + 10) / 0.2;
			if ((C_fre > 47.5) && (C_fre < 55.0))
				C_fre_flag = 1;
			else
				C_fre_flag = 0;

			if (C_fre_flag)
			{
				pointfre = (int)(40000 / C_fre + 0.5);
				C_FFT_Number = (int)(double(40000 / C_fre) * 10 + 0.5);
				if ((C_FFT_Number % 2) != 0)
					C_FFT_Number++;
				C_FFT++;

				EnterCriticalSection(&g_cs);
				p_c_fuzhi = fftw_plan_dft_1d(C_FFT_Number, in_c_fuzhi, out_c_fuzhi, FFTW_FORWARD, FFTW_ESTIMATE);
				p_ci_fuzhi = fftw_plan_dft_1d(C_FFT_Number, in_ci_fuzhi, out_ci_fuzhi, FFTW_FORWARD, FFTW_ESTIMATE);
				fftw_execute(p_c_fuzhi);
				fftw_execute(p_ci_fuzhi);
				fftw_destroy_plan(p_c_fuzhi);
				fftw_destroy_plan(p_ci_fuzhi);
				LeaveCriticalSection(&g_cs);
				
				/*fftw_ampout_c_fuzhi[0] = sqrt(out_c_fuzhi[0][0] * out_c_fuzhi[0][0] + out_c_fuzhi[0][1] * out_c_fuzhi[0][1]) / (C_FFT_Number);
				fftw_ampout_ci_fuzhi[0] = sqrt(out_ci_fuzhi[0][0] * out_ci_fuzhi[0][0] + out_ci_fuzhi[0][1] * out_ci_fuzhi[0][1]) / (C_FFT_Number);*/
				fftw_ampout_c_fuzhi[0] = out_c_fuzhi[0][0] / C_FFT_Number;
				fftw_ampout_ci_fuzhi[0] = out_ci_fuzhi[0][0] / C_FFT_Number;
				for (j = 1; j < 420; j++)
				{
					fftw_ampout_c_fuzhi[j] = sqrt(out_c_fuzhi[j][0] * out_c_fuzhi[j][0] + out_c_fuzhi[j][1] * out_c_fuzhi[j][1]) / (C_FFT_Number / 2);
					fftw_ampout_ci_fuzhi[j] = sqrt(out_ci_fuzhi[j][0] * out_ci_fuzhi[j][0] + out_ci_fuzhi[j][1] * out_ci_fuzhi[j][1]) / (C_FFT_Number / 2);
				}
				for (j = 0; j < (HarmonicWave); j++)       //A���ѹ��������+г��   BC���ѹ����+г��
				{
					index = j * 10;
					//fuzhi_c[j] = fftw_ampout_c_fuzhi[index] / factor_factor;
					fuzhi_c_temp[C_HarmonicIndex][j] = fftw_ampout_c_fuzhi[index] / factor_factor;
					fftw_phase_c[j] = atan2(out_c_fuzhi[index][1], out_c_fuzhi[index][0]) + PI / 2;
					//fuzhi_c_cur[j] = fftw_ampout_ci_fuzhi[index] / factor_factor;
					fuzhi_c_cur_temp[C_HarmonicIndex][j] = fftw_ampout_ci_fuzhi[index] / factor_factor;
					fftw_phase_c_cur[j] = atan2(out_ci_fuzhi[index][1], out_ci_fuzhi[index][0]) + PI / 2;
					if (fabs(fftw_phase_c[j] - fftw_phase_c_cur[j]) > PI)
					{
						fftw_phase_differ_c[j] = 2 * PI - fabs(fftw_phase_c[j] - fftw_phase_c_cur[j]);
					}
					else
					{
						fftw_phase_differ_c[j] = fabs(fftw_phase_c[j] - fftw_phase_c_cur[j]);
					}
				}
				//г����ƽ��ֵ
				C_HarmonicIndex++;
				if (C_HarmonicIndex == HarmonicWaveParam)
				{
					C_HarmonicIndex = 0;
					for (int j = 0; j < HarmonicWave; j++)
					{
						for (int i = 0; i < HarmonicWaveParam; i++)
						{
							fuzhi_c_ave[j] += fuzhi_c_temp[i][j];
							fuzhi_c_cur_ave[j] += fuzhi_c_cur_temp[i][j];
						}
						fuzhi_c_ave[j] = fuzhi_c_ave[j] / HarmonicWaveParam;
						fuzhi_c[j] = fuzhi_c_ave[j];    //���ս��
						fuzhi_c_ave[j] = 0;
						fuzhi_c_cur_ave[j] = fuzhi_c_cur_ave[j] / HarmonicWaveParam;
						fuzhi_c_cur[j] = fuzhi_c_cur_ave[j];    //���ս��
						fuzhi_c_cur_ave[j] = 0;
					}
				}
				C_jibophase[C_FFT%phase_param2] = fftw_phase_c[1];
				for (i = 0; i < C_FFT_Number; i++)  //800������
				{
					C_sum_vol += buffer_8800_c[i] / factor_factor * buffer_8800_c[i] / factor_factor;
					C_sum_cur += buffer_8800_c_cur[i] / factor_factor * buffer_8800_c_cur[i] / factor_factor;
					C_active_power_temp += buffer_8800_c[i] / factor_factor * buffer_8800_c_cur[i] / factor_factor;
				}

				C_active_power = C_active_power_temp / C_FFT_Number;//�й�����       ˲ʱ�й����ʵ��ۼ�
				C_active_power_meter += C_active_power * 0.2 / 3600 / 1000;//�й����         ��λKW.h
				C_rms = sqrt((double)(C_sum_vol / C_FFT_Number));   //��ѹ��Чֵ     ������
				C_cur_rms = sqrt((double)(C_sum_cur / C_FFT_Number));   //������Чֵ      ������
				C_apparent_power = C_rms * C_cur_rms;//���ڹ���          ��ѹ��Чֵ*������Чֵ
				for (j = 2; j < HarmonicWave; j++)
				{
					C_THDU_temp += fuzhi_c[j] * fuzhi_c[j];
					C_THDI_temp += fuzhi_c_cur[j] * fuzhi_c_cur[j];
				}
				C_THDU = sqrt(C_THDU_temp) / fuzhi_c[1] * 100;
				//---------------------------------------------------
				C_reactive_power_temp += fuzhi_c[1] * fuzhi_c_cur[1] * sin(fftw_phase_differ_c[1]);//����ϵ��
				//�޹�����
				C_reactive_power = C_reactive_power_temp / 2;
				//�޹����
				C_reactive_power_meter += C_reactive_power * 0.2 / 3600 / 1000;
				an_buffer_8800flag_C = 0;
			}
		}
		SuspendThread(handleFFT_CThread);//����
	}
	fftw_free(in_c_fuzhi);
	fftw_free(out_c_fuzhi);
	fftw_free(in_ci_fuzhi);
	fftw_free(out_ci_fuzhi);
	return 0;
}
double A_reg_result_1000half_buffer[1000] = { 0.0f };
double B_reg_result_1000half_buffer[1000] = { 0.0f };
double C_reg_result_1000half_buffer[1000] = { 0.0f };
u_char A_flicker_finished_flag = 0;
u_char B_flicker_finished_flag = 0;
u_char C_flicker_finished_flag = 0;
u_char A_reg_1000fullflag = 0;
u_char B_reg_1000fullflag = 0;
u_char C_reg_1000fullflag = 0;
double A_reg_result_1000half[1000] = { 0.0f };//
double B_reg_result_1000half[1000] = { 0.0f };//
double C_reg_result_1000half[1000] = { 0.0f };//
//-----------------------------------------���俽��----------------------------------------------------------------//
void A_FlickerDataCopy()
{
	if (A_reg_1000fullflag && (!A_flicker_finished_flag))        //A_flicker_finished_flag���俽����1�������߳̽�����0
	{
		memcpy(A_reg_result_1000half_buffer, A_reg_result_1000half, 1000 * sizeof(double));
		A_flicker_finished_flag = 1;
		A_reg_1000fullflag = 0;
		ResumeThread(handleA_FlickerThread);
	}
}
void B_FlickerDataCopy()
{
	if (B_reg_1000fullflag && (!B_flicker_finished_flag))        //A_flicker_finished_flag���俽����1�������߳̽�����0
	{
		memcpy(B_reg_result_1000half_buffer, B_reg_result_1000half, 1000 * sizeof(double));
		B_flicker_finished_flag = 1;
		B_reg_1000fullflag = 0;
		ResumeThread(handleB_FlickerThread);
	}
}
void C_FlickerDataCopy()
{
	if (C_reg_1000fullflag && (!C_flicker_finished_flag))        //A_flicker_finished_flag���俽����1�������߳̽�����0
	{
		memcpy(C_reg_result_1000half_buffer, C_reg_result_1000half, 1000 * sizeof(double));
		C_flicker_finished_flag = 1;
		C_reg_1000fullflag = 0;
		ResumeThread(handleC_FlickerThread);
	}
}

double A_Ppointone;
double A_Pone;
double A_Pthree;
double A_Pten;
double A_Pfifty;
double A_InstantaneousFlickerValue;
double A_ShorttimeFlickerValue = 0;
double A_LongtimeFlickerValue = 0;
extern double voltagefluctuation[2451];
//����------��Чֵ�첨��
double A_tiaozhibo_f=0.0f;
double A_V_fluctuation;
int A_shanbianCount;
unsigned int A_instantaneousflickervaluecnt = 0;
int A_tester;
DWORD WINAPI A_FlickerThreadFunc(LPVOID param)
{
	double instantaneousflickervaluetemp = 0.0f;
	double instantaneousflickervaluebuffer[60] = { 0 };
	double shorttimeflickervaluebuffer[12];
	unsigned int shorttimeflickervaluecnt = 0;
	double fftw_ampout_flick[300];
	fftw_complex *in_flick, *out_flick;

	EnterCriticalSection(&g_cs);
	in_flick = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * 1000);
	out_flick = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * 1000);
	LeaveCriticalSection(&g_cs);
	fftw_plan p_flick;
	double longtimeflickervaluetemp = 0;
	double temper;
	while (1)
	{
		if (A_flicker_finished_flag == 1)
		{
			for (int h = 0; h < 1000; h++)
			{
				in_flick[h][0] = A_reg_result_1000half_buffer[h];
			}
			EnterCriticalSection(&g_cs);
			p_flick = fftw_plan_dft_1d(1000, in_flick, out_flick, FFTW_FORWARD, FFTW_ESTIMATE);
			fftw_execute(p_flick);
			fftw_destroy_plan(p_flick);
			LeaveCriticalSection(&g_cs);
			
			for (int h = 0; h < 300; h++)
			{
				//ȡģ--��ֵ    /1000??????
				fftw_ampout_flick[h] = sqrt(out_flick[h][0] * out_flick[h][0] + out_flick[h][1] * out_flick[h][1]) / 1000;
				//fprintf(fp, "%d %2.8f\n", h,fftw_ampout_flick[h]);
			}
			for (int h = 5; h < 249; h++)
			{
				if (temper < fftw_ampout_flick[h])
				{
					temper = fftw_ampout_flick[h];
					A_tester = h;
				}
			}
			if (temper < 0.0005)
				A_tiaozhibo_f = 0.0;
			else
			{
				A_tiaozhibo_f = double(A_tester) / 10;
				A_V_fluctuation = 2 * fftw_ampout_flick[A_tester] / fftw_ampout_flick[0];
			}
			temper = 0;
			/*˲ʱ����ֵ
			Ƶ�׷ֱ���Ϊ0.1Hz��k��ȡֵΪ245��
			���ݵ�λ˲ʱ����ֵ��Ӧ ��ѹ����ֵϵ���� �����ĳ��Ƶ���µ�˲ʱ����ֵ��
			��Ƶ�ʶ�Ӧ��˲ʱ����ֵ֮�ͼ�Ϊ��ǰ��ѹ�ź����ж�Ӧ��˲ʱ����ֵ

			Ƶ��fi�Ķ���Ƶ�׷�ֵ��Ӧ���Ǹ�Ƶ�������ҵ�ѹ�벨�����������ϵĵ�ѹ����ֵ 2 * fftw_ampout_flick[5 + h]* 100��
			�ٽ���ֵ���Ծ���FFT���������ֱ�����������ѹ fftw_ampout_flick[0] �����ܵõ�Ƶ�� fi�µĵ�ѹ���� d(i)��
			d(i)=2 * fftw_ampout_flick[5 + h]* 100 / fftw_ampout_flick[0]
			���Ը�Ƶ���²���һ����λ˲ʱ�����Ӹж�����ĵ�ѹ���� divoltagefluctuation[h * 10]
			voltagefluctuation[0]��Ӧ0.5Hz   0.5+0.01*i
			voltagefluctuation[2450]��Ӧ25Hz
			��ƽ�����������Ƶ�� fi�¶�Ӧ��˲ʱ����ֵ Si��
			��ʽ���£�
			*/
			//��Ƶ�� ���
			for (int h = 0; h < 246; h++)
			{
				instantaneousflickervaluetemp += pow(2 * fftw_ampout_flick[5 + h] / voltagefluctuation[h * 10] / fftw_ampout_flick[0], 2)
					/ (sin(PI*(0.5 + 0.1*h) / 100) / (PI*(0.5 + 0.1*h) / 100));
			}
			/*instantaneousflickervaluetemp = pow(2 * fftw_ampout_flick[tester] *100  / voltagefluctuation[(tester - 5) * 10] / fftw_ampout_flick[0], 2)
				  / (sin(PI*(0.5 + 0.1*(tester - 5)) / 100) / (PI*(0.5 + 0.1*(tester - 5)) / 100));*/
		    //if ((instantaneousflickervaluetemp < 1.1f) && (instantaneousflickervaluetemp > 0.9f))
			{
				A_InstantaneousFlickerValue = instantaneousflickervaluetemp;
				instantaneousflickervaluebuffer[A_instantaneousflickervaluecnt] = instantaneousflickervaluetemp;  //��¼60��˲ʱ����ֵ  10min
				A_instantaneousflickervaluecnt++;
			}
			A_shanbianCount = A_instantaneousflickervaluecnt;
			instantaneousflickervaluetemp = 0;
			if (A_instantaneousflickervaluecnt >= 60)
			{
				A_instantaneousflickervaluecnt = 0;
				/*
				void qsort( void *base, size_t num, size_t width, int (__cdecl *compare )
				��һ������ base �� ��Ҫ�����Ŀ��������������Ҳ�������ɿ�ʼ����ĵ�ַ����Ϊ����д&s[i]�����ı��ʽ��
				�ڶ������� num �� ���������Ŀ������Ԫ�ظ���
				���������� width �ǵ���Ԫ�صĴ�С������Ŀ��������ÿһ��Ԫ�س��ȣ����Ƽ�ʹ��sizeof(s[0]�������ı��ʽ
				���ĸ����� compare �����úܶ��˾��÷ǳ�����ıȽϺ�������
				int compare (const void *elem1, const void *elem2 ) );
				*/
				//��60��˲ʱ����ֵ��������
				qsort(instantaneousflickervaluebuffer, 60, sizeof(double), compar);
				//P0.1 P1 P3 P10 P50�ֱ�Ϊͳ�������ڳ���0.1% 1% 3% 10% 50%ʱ��ȵĸ��ʷֲ�ˮƽֵ
				//A_Ppointone=buffer[59]+0.06*(buffer[58]-buffer[59])
				A_Ppointone = instantaneousflickervaluebuffer[60 - 1] + 0.06*
					(instantaneousflickervaluebuffer[60 - 2] - instantaneousflickervaluebuffer[60 - 1]);
				//A_Pone=buffer[59]+0.6*(buffer[58]-buffer[59])
				A_Pone = instantaneousflickervaluebuffer[60 - 1] + 0.6*
					(instantaneousflickervaluebuffer[60 - 2] - instantaneousflickervaluebuffer[60 - 1]);
				//A_Pthree=buffer[58]+0.8*(buffer[57]-buffer[58])
				A_Pthree = instantaneousflickervaluebuffer[60 - 2] + 0.8*
					(instantaneousflickervaluebuffer[60 - 3] - instantaneousflickervaluebuffer[60 - 2]);
				//A_Pten=buffer[53]
				A_Pten = instantaneousflickervaluebuffer[60 - 7];
				//A_Pfifty=buffer[29]
				A_Pfifty = instantaneousflickervaluebuffer[60 - 31];
				//��ʱ����ֵ
				//A_ShorttimeFlickerValue=��0.0314*A_Ppointone+0.0525* A_Pone+0.065*A_Pthree+0.28*A_Pten+0.08*A_Pfifty��~1/2
				A_ShorttimeFlickerValue = sqrt(Kpointone*A_Ppointone + Kone * A_Pone + Kthree * A_Pthree + Kten * A_Pten + Kfifty * A_Pfifty);
				shorttimeflickervaluebuffer[shorttimeflickervaluecnt] = A_ShorttimeFlickerValue;  //�������12����ʱ����ֵ
				shorttimeflickervaluecnt++;
				if (shorttimeflickervaluecnt >= 12)
				{
					shorttimeflickervaluecnt = 0;
					/*obtain long-time flicker value*/
					for (int k = 0; k < 12; k++)
					{
						longtimeflickervaluetemp += pow(shorttimeflickervaluebuffer[k], 3) / 12;
					}
					//��ʱ����ֵ
					A_LongtimeFlickerValue = pow(longtimeflickervaluetemp, 1.0 / 3.0);
					longtimeflickervaluetemp = 0;
				}
			}
			A_flicker_finished_flag = 0;
		}
		SuspendThread(handleA_FlickerThread);
	}
	fftw_free(in_flick);
	fftw_free(out_flick);
	return 0;
}
double B_Ppointone;
double B_Pone;
double B_Pthree;
double B_Pten;
double B_Pfifty;
double B_InstantaneousFlickerValue;
double B_ShorttimeFlickerValue = 0;
double B_LongtimeFlickerValue = 0;
//����------��Чֵ�첨��
double B_tiaozhibo_f = 0.0f;
double B_V_fluctuation;
int B_shanbianCount;
unsigned int B_instantaneousflickervaluecnt = 0;
int B_tester;
DWORD WINAPI B_FlickerThreadFunc(LPVOID param)
{
	double instantaneousflickervaluetemp = 0.0f;
	double instantaneousflickervaluebuffer[60] = { 0 };
	double shorttimeflickervaluebuffer[12];
	unsigned int shorttimeflickervaluecnt = 0;
	double fftw_ampout_flick[300];
	fftw_complex *in_flick, *out_flick;
	EnterCriticalSection(&g_cs);
	in_flick = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * 1000);
	out_flick = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * 1000);
	LeaveCriticalSection(&g_cs);
	fftw_plan p_flick;
	double longtimeflickervaluetemp = 0;
	double temper;
	while (1)
	{
		if (B_flicker_finished_flag == 1)
		{
			for (int h = 0; h < 1000; h++)
			{
				in_flick[h][0] = B_reg_result_1000half_buffer[h];
			}
			EnterCriticalSection(&g_cs);
			p_flick = fftw_plan_dft_1d(1000, in_flick, out_flick, FFTW_FORWARD, FFTW_ESTIMATE);
			fftw_execute(p_flick);
			fftw_destroy_plan(p_flick);
			LeaveCriticalSection(&g_cs);
			
			for (int h = 0; h < 300; h++)
			{
				//ȡģ--��ֵ    /1000??????
				fftw_ampout_flick[h] = sqrt(out_flick[h][0] * out_flick[h][0] + out_flick[h][1] * out_flick[h][1]) / 1000;
				//fprintf(fp, "%d %2.8f\n", h,fftw_ampout_flick[h]);
			}
			for (int h = 5; h < 249; h++)
			{
				if (temper < fftw_ampout_flick[h])
				{
					temper = fftw_ampout_flick[h];
					B_tester = h;
				}
			}
			if (temper < 0.0005)
				B_tiaozhibo_f = 0.0;
			else
			{
				B_tiaozhibo_f = double(B_tester) / 10;
				B_V_fluctuation = 2 * fftw_ampout_flick[B_tester] / fftw_ampout_flick[0];
			}
			temper = 0;
			//��Ƶ�� ���
			for (int h = 0; h < 246; h++)
			{
				instantaneousflickervaluetemp += pow(2 * fftw_ampout_flick[5 + h]  / voltagefluctuation[h * 10] / fftw_ampout_flick[0], 2)
					/ (sin(PI*(0.5 + 0.1*h) / 100) / (PI*(0.5 + 0.1*h) / 100));
			}
			/*instantaneousflickervaluetemp = pow(2 * fftw_ampout_flick[tester] *100  / voltagefluctuation[(tester - 5) * 10] / fftw_ampout_flick[0], 2)
				  / (sin(PI*(0.5 + 0.1*(tester - 5)) / 100) / (PI*(0.5 + 0.1*(tester - 5)) / 100));*/
				  //if ((instantaneousflickervaluetemp < 1.1f) && (instantaneousflickervaluetemp > 0.9f))
			{
				B_InstantaneousFlickerValue = instantaneousflickervaluetemp;
				instantaneousflickervaluebuffer[B_instantaneousflickervaluecnt] = instantaneousflickervaluetemp;  //��¼60��˲ʱ����ֵ  10min
				B_instantaneousflickervaluecnt++;
			}
			B_shanbianCount = B_instantaneousflickervaluecnt;
			instantaneousflickervaluetemp = 0;
			if (B_instantaneousflickervaluecnt >= 60)
			{
				B_instantaneousflickervaluecnt = 0;
				//��60��˲ʱ����ֵ��������
				qsort(instantaneousflickervaluebuffer, 60, sizeof(double), compar);
				//P0.1 P1 P3 P10 P50�ֱ�Ϊͳ�������ڳ���0.1% 1% 3% 10% 50%ʱ��ȵĸ��ʷֲ�ˮƽֵ
				//B_Ppointone=buffer[59]+0.06*(buffer[58]-buffer[59])
				B_Ppointone = instantaneousflickervaluebuffer[60 - 1] + 0.06*
					(instantaneousflickervaluebuffer[60 - 2] - instantaneousflickervaluebuffer[60 - 1]);
				//B_Pone=buffer[59]+0.6*(buffer[58]-buffer[59])
				B_Pone = instantaneousflickervaluebuffer[60 - 1] + 0.6*
					(instantaneousflickervaluebuffer[60 - 2] - instantaneousflickervaluebuffer[60 - 1]);
				//B_Pthree=buffer[58]+0.8*(buffer[57]-buffer[58])
				B_Pthree = instantaneousflickervaluebuffer[60 - 2] + 0.8*
					(instantaneousflickervaluebuffer[60 - 3] - instantaneousflickervaluebuffer[60 - 2]);
				//B_Pten=buffer[53]
				B_Pten = instantaneousflickervaluebuffer[60 - 7];
				//B_Pfifty=buffer[29]
				B_Pfifty = instantaneousflickervaluebuffer[60 - 31];
				//��ʱ����ֵ
				//B_ShorttimeFlickerValue=��0.0314*B_Ppointone+0.0525* B_Pone+0.065*B_Pthree+0.28*B_Pten+0.08*B_Pfifty��~1/2
				B_ShorttimeFlickerValue = sqrt(Kpointone*B_Ppointone + Kone * B_Pone + Kthree * B_Pthree + Kten * B_Pten + Kfifty * B_Pfifty);
				shorttimeflickervaluebuffer[shorttimeflickervaluecnt] = B_ShorttimeFlickerValue;  //�������12����ʱ����ֵ
				shorttimeflickervaluecnt++;
				if (shorttimeflickervaluecnt >= 12)
				{
					shorttimeflickervaluecnt = 0;
					/*obtain long-time flicker value*/
					for (int k = 0; k < 12; k++)
					{
						longtimeflickervaluetemp += pow(shorttimeflickervaluebuffer[k], 3) / 12;
					}
					B_LongtimeFlickerValue = pow(longtimeflickervaluetemp, 1.0 / 3.0);
					longtimeflickervaluetemp = 0;
				}
			}
			B_flicker_finished_flag = 0;
		}
		SuspendThread(handleB_FlickerThread);
	}
	fftw_free(in_flick);
	fftw_free(out_flick);
	return 0;
}
double C_Ppointone;
double C_Pone;
double C_Pthree;
double C_Pten;
double C_Pfifty;
double C_InstantaneousFlickerValue;
double C_ShorttimeFlickerValue = 0;
double C_LongtimeFlickerValue = 0;
//����------��Чֵ�첨��
double C_tiaozhibo_f = 0.0f;
double C_V_fluctuation;
int C_shanbianCount;
int C_tester;
unsigned int C_instantaneousflickervaluecnt = 0;
DWORD WINAPI C_FlickerThreadFunc(LPVOID param)
{
	double instantaneousflickervaluetemp = 0.0f;
	double instantaneousflickervaluebuffer[60] = { 0 };
	double shorttimeflickervaluebuffer[12];
	unsigned int shorttimeflickervaluecnt = 0;
	double fftw_ampout_flick[300];
	fftw_complex *in_flick, *out_flick;
	EnterCriticalSection(&g_cs);
	in_flick = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * 1000);
	out_flick = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * 1000);
	LeaveCriticalSection(&g_cs);
	fftw_plan p_flick;
	double longtimeflickervaluetemp = 0;
	double temper;
	while (1)
	{
		if (C_flicker_finished_flag == 1)
		{
			for (int h = 0; h < 1000; h++)
			{
				in_flick[h][0] = C_reg_result_1000half_buffer[h];
			}
			EnterCriticalSection(&g_cs);
			p_flick = fftw_plan_dft_1d(1000, in_flick, out_flick, FFTW_FORWARD, FFTW_ESTIMATE);
			fftw_execute(p_flick);
			fftw_destroy_plan(p_flick);
			LeaveCriticalSection(&g_cs);
			
			for (int h = 0; h < 300; h++)
			{
				//ȡģ--��ֵ    /1000??????
				fftw_ampout_flick[h] = sqrt(out_flick[h][0] * out_flick[h][0] + out_flick[h][1] * out_flick[h][1]) / 1000;
				//fprintf(fp, "%d %2.8f\n", h,fftw_ampout_flick[h]);
			}
			for (int h = 5; h < 249; h++)
			{
				if (temper < fftw_ampout_flick[h])
				{
					temper = fftw_ampout_flick[h];
					C_tester = h;
				}
			}
			if (temper < 0.0005)
				C_tiaozhibo_f = 0.0;
			else
			{
				C_tiaozhibo_f = double(C_tester) / 10;
				C_V_fluctuation = 2 * fftw_ampout_flick[C_tester] / fftw_ampout_flick[0];
			}
			temper = 0;
			//��Ƶ�� ���
			for (int h = 0; h < 246; h++)
			{
				instantaneousflickervaluetemp += pow(2 * fftw_ampout_flick[5 + h]  / voltagefluctuation[h * 10] / fftw_ampout_flick[0], 2)
					/ (sin(PI*(0.5 + 0.1*h) / 100) / (PI*(0.5 + 0.1*h) / 100));
			}
			/*instantaneousflickervaluetemp = pow(2 * fftw_ampout_flick[tester] *100  / voltagefluctuation[(tester - 5) * 10] / fftw_ampout_flick[0], 2)
				  / (sin(PI*(0.5 + 0.1*(tester - 5)) / 100) / (PI*(0.5 + 0.1*(tester - 5)) / 100));*/
				  //if ((instantaneousflickervaluetemp < 1.1f) && (instantaneousflickervaluetemp > 0.9f))
			{
				C_InstantaneousFlickerValue = instantaneousflickervaluetemp;
				instantaneousflickervaluebuffer[C_instantaneousflickervaluecnt] = instantaneousflickervaluetemp;  //��¼60��˲ʱ����ֵ  10min
				C_instantaneousflickervaluecnt++;
			}
			C_shanbianCount = C_instantaneousflickervaluecnt;
			instantaneousflickervaluetemp = 0;
			if (C_instantaneousflickervaluecnt >= 60)
			{
				C_instantaneousflickervaluecnt = 0;
				//��60��˲ʱ����ֵ��������
				qsort(instantaneousflickervaluebuffer, 60, sizeof(double), compar);
				//P0.1 P1 P3 P10 P50�ֱ�Ϊͳ�������ڳ���0.1% 1% 3% 10% 50%ʱ��ȵĸ��ʷֲ�ˮƽֵ
				//C_Ppointone=buffer[59]+0.06*(buffer[58]-buffer[59])
				C_Ppointone = instantaneousflickervaluebuffer[60 - 1] + 0.06*
					(instantaneousflickervaluebuffer[60 - 2] - instantaneousflickervaluebuffer[60 - 1]);
				//C_Pone=buffer[59]+0.6*(buffer[58]-buffer[59])
				C_Pone = instantaneousflickervaluebuffer[60 - 1] + 0.6*
					(instantaneousflickervaluebuffer[60 - 2] - instantaneousflickervaluebuffer[60 - 1]);
				//C_Pthree=buffer[58]+0.8*(buffer[57]-buffer[58])
				C_Pthree = instantaneousflickervaluebuffer[60 - 2] + 0.8*
					(instantaneousflickervaluebuffer[60 - 3] - instantaneousflickervaluebuffer[60 - 2]);
				//C_Pten=buffer[53]
				C_Pten = instantaneousflickervaluebuffer[60 - 7];
				//C_Pfifty=buffer[29]
				C_Pfifty = instantaneousflickervaluebuffer[60 - 31];
				//��ʱ����ֵ
				//C_ShorttimeFlickerValue=��0.0314*C_Ppointone+0.0525* C_Pone+0.065*C_Pthree+0.28*C_Pten+0.08*C_Pfifty��~1/2
				C_ShorttimeFlickerValue = sqrt(Kpointone*C_Ppointone + Kone * C_Pone + Kthree * C_Pthree + Kten * C_Pten + Kfifty * C_Pfifty);
				shorttimeflickervaluebuffer[shorttimeflickervaluecnt] = C_ShorttimeFlickerValue;  //�������12����ʱ����ֵ
				shorttimeflickervaluecnt++;
				if (shorttimeflickervaluecnt >= 12)
				{
					shorttimeflickervaluecnt = 0;
					/*obtain long-time flicker value*/
					for (int k = 0; k < 12; k++)
					{
						longtimeflickervaluetemp += pow(shorttimeflickervaluebuffer[k], 3) / 12;
					}
					C_LongtimeFlickerValue = pow(longtimeflickervaluetemp, 1.0 / 3.0);
					longtimeflickervaluetemp = 0;
				}
			}
			C_flicker_finished_flag = 0;
		}
		SuspendThread(handleC_FlickerThread);
	}
	fftw_free(in_flick);
	fftw_free(out_flick);
	return 0;
}
/*�����ݽ��жϱ�־*/
char A_voltagedipstartflag = 0;
char A_voltageswellstartflag = 0;
char A_voltageinterruptstartflag = 0;
/*��ѹʵʱֵ+���ս��ֵ*/
double A_VoltagedipVoltageTemp = 0.0f; //�ݽ�ʵʱֵ
double A_VoltagedipDepth = 0.0f;   //�ݽ�����ֵ
double A_VoltageswellVoltageTemp = 0.0f; //����ʵʱֵ
double A_VoltageswellVoltageResult = 0.0f;      //��������ֵ
double A_VoltageinterruptVoltageTemp = 0.0f;//�ж�ʵʱֵ
double A_VoltageinterruptVoltageResult = 0.0f;//�ж�����ֵ
/*��һ�ε�ѹֵ*/
double A_VoltageswellLastVoltageResult = 0.0f;
double A_VoltageinterruptLastVoltageResult = 0.0f;
double A_VoltagedipLastVoltageResult = 0.0f;
/*��ʼʱ��*/
int A_VoltageswellDurationStartTime = 0;
int A_VoltagedipDurationStartTime = 0.0f;
int A_VoltageinterruptionDurationStartTime = 0.0f;
/*ʵʱ����ʱ��*/
int A_VoltageswellDurationTime = 0;
int A_VoltagedipDurationTime = 0;
int A_VoltageinterruptionDurationTime = 0.0f;
/*��һ�γ���ʱ��*/
int A_VoltageswellDurationLastTime = 0;
int A_VoltagedipDurationLastTime = 0;
int A_VoltageinterruptionDurationLastTime = 0.0f;

int A_index_400 = 0;
int A_reg_1000cnt = 0;
double A_result_800half = 0, A_result_400half = 0;

DWORD WINAPI A_HalfThreadFunc(LPVOID param)
{
	double sum_period = 0.0f, sum_half = 0.0f, sum_half_last = 0.0f;
	int h;
	while (1)
	{
		A_index_400 = an_buffer_idx_A / 400;
		if (A_index_400 == 0)
		{
			for (h = 0; h < 400; h++)
			{
				sum_half += an_buffer[AN_BUFFER_880kLEN - 400 + h] / factor_factor * an_buffer[AN_BUFFER_880kLEN - 400 + h] / factor_factor;
			}
		}
		else
		{
			for (h = 0; h < 400; h++)
			{
				sum_half += an_buffer[400 * (A_index_400 - 1) + h] / factor_factor * an_buffer[400 * (A_index_400 - 1) + h] / factor_factor;
			}
		}
		A_result_400half = sqrt((double)(sum_half / 400));   // �벨��Чֵ ������A���ѹ�ķ����� ����һ��
		A_result_800half = sqrt((double)((sum_half + sum_half_last) / PeriodPoint));
		sum_half_last = sum_half;
		sum_half = 0.0f;
		if (A_voltage_dipswellinterrupt_open)
		{
			A_voltagedipswellinterruptiondetection();  //��ѹ�������ݽ����жϼ��  voltage A_dip/A_swell/interrupy

			if (A_voltagedipstartflag) //�ݽ�
			{
				A_voltagedipcalculation();
			}
			if (A_voltageswellstartflag)  //����
			{
				A_voltageswellcalculation();
			}
			if ( A_voltageinterruptstartflag) //��ѹ�ж�
			{
				A_voltageinterruptioncalculation();
			}
		}
		if (A_flicker_open)
		{
			A_reg_result_1000half[A_reg_1000cnt] = A_result_400half;  //���1000���벨��Чֵ
			A_reg_1000cnt++;
			if (A_reg_1000cnt == 1000)
			{
				A_reg_1000fullflag = 1;
				A_FlickerDataCopy();   //�������ݲ� �ָ������߳�
				A_reg_1000cnt = 0;
			}
		}
		SuspendThread(handleA_HalfPeriodThread);
	}
	return 0;
}
/*�����ݽ��жϱ�־*/
char B_voltagedipstartflag = 0;
char B_voltageswellstartflag = 0;
char B_voltageinterruptstartflag = 0;
/*��ѹʵʱֵ+���ս��ֵ*/
double B_VoltagedipVoltageTemp = 0.0f; //�ݽ�ʵʱֵ
double B_VoltagedipDepth = 0.0f;   //�ݽ�����ֵ
double B_VoltageswellVoltageTemp = 0.0f; //����ʵʱֵ
double B_VoltageswellVoltageResult = 0.0f;      //��������ֵ
double B_VoltageinterruptVoltageTemp = 0.0f;//�ж�ʵʱֵ
double B_VoltageinterruptVoltageResult = 0.0f;//�ж�����ֵ
/*��һ�ε�ѹֵ*/
double B_VoltageswellLastVoltageResult = 0.0f;
double B_VoltageinterruptLastVoltageResult = 0.0f;
double B_VoltagedipLastVoltageResult = 0.0f;
/*��ʼʱ��*/
int B_VoltageswellDurationStartTime = 0;
int B_VoltagedipDurationStartTime = 0.0f;
int B_VoltageinterruptionDurationStartTime = 0.0f;
/*ʵʱ����ʱ��*/
int B_VoltageswellDurationTime = 0;
int B_VoltagedipDurationTime = 0;
int B_VoltageinterruptionDurationTime = 0.0f;
/*��һ�γ���ʱ��*/
int B_VoltageswellDurationLastTime = 0;
int B_VoltagedipDurationLastTime = 0;
int B_VoltageinterruptionDurationLastTime = 0.0f;

int B_index_400 = 0;//ȫ�ֱ���
int B_reg_1000cnt = 0;
double B_result_800half = 0, B_result_400half = 0;
DWORD WINAPI B_HalfThreadFunc(LPVOID param)
{
	double sum_period = 0.0f, sum_half = 0.0f, sum_half_last = 0.0f;
	int h;
	while (1)
	{
		B_index_400 = an_buffer_idx_B / 400;
		if (B_index_400 == 0)
		{
			for (h = 0; h < 400; h++)
			{
				sum_half += an_buffer_b[AN_BUFFER_880kLEN - 400 + h] / factor_factor * an_buffer_b[AN_BUFFER_880kLEN - 400 + h] / factor_factor;
			}
		}
		else
		{
			for (h = 0; h < 400; h++)
			{
				sum_half += an_buffer_b[400 * (B_index_400 - 1) + h] / factor_factor * an_buffer_b[400 * (B_index_400 - 1) + h] / factor_factor;
			}
		}
		B_result_400half = sqrt((double)(sum_half / 400));   // �벨��Чֵ ������B���ѹ�ķ����� ����һ��
		B_result_800half = sqrt((double)((sum_half + sum_half_last) / PeriodPoint));
		sum_half_last = sum_half;
		sum_half = 0.0f;
		if (B_voltage_dipswellinterrupt_open)
		{
			B_voltagedipswellinterruptiondetection();  //��ѹ�������ݽ����жϼ��  voltage A_dip/A_swell/interrupy

			if (B_voltagedipstartflag) //�ݽ�
			{
				B_voltagedipcalculation();
			}
			if (B_voltageswellstartflag)  //����
			{
				B_voltageswellcalculation();
			}
			if (B_voltageinterruptstartflag) //��ѹ�ж�
			{
				B_voltageinterruptioncalculation();
			}
		}
		if (B_flicker_open)
		{
			B_reg_result_1000half[B_reg_1000cnt] = B_result_400half;  //���1000���벨��Чֵ
			B_reg_1000cnt++;
			if (B_reg_1000cnt == 1000)
			{
				B_reg_1000fullflag = 1;
				B_FlickerDataCopy();   //�������ݲ� �ָ������߳�
				B_reg_1000cnt = 0;
			}
		}
		SuspendThread(handleB_HalfPeriodThread);
	}
	return 0;
}
/*�����ݽ��жϱ�־*/
char C_voltagedipstartflag = 0;
char C_voltageswellstartflag = 0;
char C_voltageinterruptstartflag = 0;
/*��ѹʵʱֵ+���ս��ֵ*/
double C_VoltagedipVoltageTemp = 0.0f; //�ݽ�ʵʱֵ
double C_VoltagedipDepth = 0.0f;   //�ݽ�����ֵ
double C_VoltageswellVoltageTemp = 0.0f; //����ʵʱֵ
double C_VoltageswellVoltageResult = 0.0f;      //��������ֵ
double C_VoltageinterruptVoltageTemp = 0.0f;//�ж�ʵʱֵ
double C_VoltageinterruptVoltageResult = 0.0f;//�ж�����ֵ
/*��һ�ε�ѹֵ*/
double C_VoltageswellLastVoltageResult = 0.0f;
double C_VoltageinterruptLastVoltageResult = 0.0f;
double C_VoltagedipLastVoltageResult = 0.0f;
/*��ʼʱ��*/
int C_VoltageswellDurationStartTime = 0;
int C_VoltagedipDurationStartTime = 0.0f;
int C_VoltageinterruptionDurationStartTime = 0.0f;
/*ʵʱ����ʱ��*/
int C_VoltageswellDurationTime = 0;
int C_VoltagedipDurationTime = 0;
int C_VoltageinterruptionDurationTime = 0.0f;
/*��һ�γ���ʱ��*/
int C_VoltageswellDurationLastTime = 0;
int C_VoltagedipDurationLastTime = 0;
int C_VoltageinterruptionDurationLastTime = 0.0f;

int C_index_400 = 0;//ȫ�ֱ���
int C_reg_1000cnt = 0;
double C_result_800half = 0, C_result_400half = 0;
DWORD WINAPI C_HalfThreadFunc(LPVOID param)
{
	double sum_period = 0.0f, sum_half = 0.0f, sum_half_last = 0.0f;
	int h;
	while (1)
	{
		C_index_400 = an_buffer_idx_C / 400;
		if (C_index_400 == 0)
		{
			for (h = 0; h < 400; h++)
			{
				sum_half += an_buffer_c[AN_BUFFER_880kLEN - 400 + h] / factor_factor * an_buffer_c[AN_BUFFER_880kLEN - 400 + h] / factor_factor;
			}
		}
		else
		{
			for (h = 0; h < 400; h++)
			{
				sum_half += an_buffer_c[400 * (C_index_400 - 1) + h] / factor_factor * an_buffer_c[400 * (C_index_400 - 1) + h] / factor_factor;
			}
		}
		C_result_400half = sqrt((double)(sum_half / 400));   // �벨��Чֵ ������C���ѹ�ķ����� ����һ��
		C_result_800half = sqrt((double)((sum_half + sum_half_last) / PeriodPoint));
		sum_half_last = sum_half;
		sum_half = 0.0f;
		if (C_voltage_dipswellinterrupt_open)
		{
			C_voltagedipswellinterruptiondetection();  //��ѹ�������ݽ����жϼ��  voltage A_dip/A_swell/interrupy

			if (C_voltagedipstartflag) //�ݽ�
			{
				C_voltagedipcalculation();
			}
			if (C_voltageswellstartflag)  //����
			{
				C_voltageswellcalculation();
			}
			if (C_voltageinterruptstartflag) //��ѹ�ж�
			{
				C_voltageinterruptioncalculation();
			}
		}
		if (C_flicker_open)
		{
			C_reg_result_1000half[C_reg_1000cnt] = C_result_400half;  //���1000���벨��Чֵ
			C_reg_1000cnt++;
			if (C_reg_1000cnt == 1000)
			{
				C_reg_1000fullflag = 1;
				C_FlickerDataCopy();   //�������ݲ� �ָ������߳�
				C_reg_1000cnt = 0;
			}
		}
		SuspendThread(handleC_HalfPeriodThread);
	}
	return 0;
}
measuring_results_union measuring_results;
void indicators2union(void)
{
	measuring_results.indicators_array_double[0]  = A_fre;//A��Ƶ��
	measuring_results.indicators_array_double[1]  = A_rms;//A���ѹ��Чֵ
	measuring_results.indicators_array_double[2]  = A_cur_rms;//A�������Чֵ
	measuring_results.indicators_array_double[3]  = A_active_power;//A���й�����
	measuring_results.indicators_array_double[4]  = A_reactive_power;//A���޹�����
	measuring_results.indicators_array_double[5]  = A_apparent_power;//A�����ڹ���
	measuring_results.indicators_array_double[6]  = A_fre;//A��Ƶ��
	measuring_results.indicators_array_double[7]  = A_rms;//A���ѹ��Чֵ
	measuring_results.indicators_array_double[8]  = A_cur_rms;//A�������Чֵ
	measuring_results.indicators_array_double[9]  = A_active_power;//A���й�����
	measuring_results.indicators_array_double[10] = A_reactive_power;//A���޹�����
	measuring_results.indicators_array_double[11] = A_apparent_power;//A�����ڹ���
	measuring_results.indicators_array_double[12] = A_cur_rms;//A�������Чֵ
	measuring_results.indicators_array_double[13] = A_active_power;//A���й�����
	measuring_results.indicators_array_double[14] = A_reactive_power;//A���޹�����
	measuring_results.indicators_array_double[15] = A_apparent_power;//A�����ڹ���
	measuring_results.indicators_array_double[16] = A_fre;//A��Ƶ��
	measuring_results.indicators_array_double[17] = A_rms;//A���ѹ��Чֵ
	measuring_results.indicators_array_double[18] = A_cur_rms;//A�������Чֵ
	measuring_results.indicators_array_double[19] = A_active_power;//A���й�����
	measuring_results.indicators_array_double[20] = A_reactive_power;//A���޹�����
	measuring_results.indicators_array_double[21] = A_rms;//A���ѹ��Чֵ
	measuring_results.indicators_array_double[22] = A_cur_rms;//A�������Чֵ
	measuring_results.indicators_array_double[23] = A_active_power;//A���й�����
	measuring_results.indicators_array_double[24] = A_reactive_power;//A���޹�����
	measuring_results.indicators_array_double[25] = A_apparent_power;//A�����ڹ���
	measuring_results.indicators_array_double[26] = A_fre;//A��Ƶ��
	measuring_results.indicators_array_double[27] = A_rms;//A���ѹ��Чֵ
	measuring_results.indicators_array_double[28] = A_cur_rms;//A�������Чֵ
	measuring_results.indicators_array_double[29] = A_active_power;//A���й�����
}
DWORD WINAPI SocketThreadFunc(LPVOID param)
{
	WSADATA data;
	WORD w = MAKEWORD(2, 0);//�����׽��ְ汾
	::WSAStartup(w, &data);//��ʼ���׽��ֿ�

	SOCKET s;
	s = ::socket(AF_INET, SOCK_DGRAM, 0);//����UDP�׽���  SOCK_DGRAM:UDP�׽���

	sockaddr_in addr, addr2;//�����׽��ֵ�ַ�ṹ����
	int n = sizeof(addr2);//��ַ�ṹ������С

	//����
	addr.sin_family = AF_INET;  //AF_INET6
	addr.sin_port = htons(1401);
	inet_pton(AF_INET, "192.168.1.20", (void*)&addr.sin_addr.S_un.S_addr);
	//�ͻ���
	addr2.sin_family = AF_INET;
	addr2.sin_port = htons(8084);
	inet_pton(AF_INET, "192.168.1.255", (void*)&addr2.sin_addr.S_un.S_addr);

	::bind(s, (sockaddr*)&addr, sizeof(addr));
	while (1)
	{
		indicators2union();
		::sendto(s, measuring_results.indicators_array_char, sizeof(measuring_results.indicators_array_char), 0, (sockaddr*)&addr2, n);
		::Sleep(3000);
	}
	return 0;
}
u_short stand_temp;
u_long A_FFT_last;
double BA_phase[100], CA_phase[100];

double uneg = 0;
double uneg_param1, uneg_param2;
double a1 = 0.0f, a2 = 0.0f, a3 = 0.0f, a4 = 0.0f;
double BA_phase_average = 0, CA_phase_average = 0;
DWORD WINAPI CheckThreadFunc(LPVOID param)
{
	ResumeThread(handlePcap1);
	Sleep(10);
	ResumeThread(handlePcap2);
	ResumeThread(handlePcap3);
	Sleep(2000);
	loss_open = 1;
	while (1)
	{	
		if ((!A_flicker_open)&&(!A_voltage_dipswellinterrupt_open)&& (!B_flicker_open) && (!B_voltage_dipswellinterrupt_open)&& (!C_flicker_open) && (!C_voltage_dipswellinterrupt_open))
		{
			if (error_flag)
			{
				SuspendThread(handlePcap1);
				SuspendThread(handlePcap2);
				SuspendThread(handlePcap3);
				EnterCriticalSection(&g_cs);
				stand_temp = maxValue(A_temp, B_temp, C_temp);
				stand_flag = (stand_temp + 10000) % 65536;

				an_buffer_idx_A = minValue(index_8800_A, index_8800_B, index_8800_C)*Plus_8000;
				an_buffer_idx_B = an_buffer_idx_A;
				an_buffer_idx_C = an_buffer_idx_A;
				A_FFT = minValue(A_FFT, B_FFT, C_FFT);
				B_FFT = A_FFT;
				C_FFT = A_FFT;
				//A_flag1 = 0;
				A_flag2 = 0;
				B_flag1 = 0;
				B_flag2 = 0;
				C_flag1 = 0;
				C_flag2 = 0;
				error_flag = 0;
				LeaveCriticalSection(&g_cs);
				ResumeThread(handlePcap1);
				ResumeThread(handlePcap2);
				ResumeThread(handlePcap3);
			}
		}
		if ((A_FFT>0)&&(A_FFT %( phase_param1+3)) == 0)
		{
			if (A_FFT != A_FFT_last)
			{ 
				for (int i = 0; i < phase_param1; i++)
				{
					BA_phase[i] = B_jibophase[i] - A_jibophase[i];
					if (BA_phase[i] > PI)
						BA_phase[i] -= 2 * PI;
					CA_phase[i] = C_jibophase[i] - A_jibophase[i];
					if (CA_phase[i] < -PI)
						CA_phase[i] += 2 * PI;
				
					//fprintf(fp, "%d  %d BA:%5.6f, CA:%5.6f   %5.6f   %5.6f\n", A_FFT,i,BA_phase[i] / PI * 180, CA_phase[i] / PI * 180, BA_phase_average / PI * 180, CA_phase_average / PI * 180);
					BA_phase_average = average(BA_phase, phase_param1);
					CA_phase_average = average(CA_phase, phase_param1);
					a1 = fuzhi_a[1] - 0.5   * fuzhi_b[1] * cos(BA_phase_average) - 0.866 * fuzhi_b[1] * sin(BA_phase_average) - 0.5   * fuzhi_c[1] * cos(CA_phase_average) + 0.866 * fuzhi_c[1] * sin(CA_phase_average);
					a2 = 0 + 0.866 * fuzhi_b[1] * cos(BA_phase_average) - 0.5   * fuzhi_b[1] * sin(BA_phase_average) - 0.866 * fuzhi_c[1] * cos(CA_phase_average) - 0.5   * fuzhi_c[1] * sin(CA_phase_average);
					//-------------------------------------------------------------------
					a3 = fuzhi_a[1] - 0.5   * fuzhi_b[1] * cos(BA_phase_average) + 0.866 * fuzhi_b[1] * sin(BA_phase_average) - 0.5   * fuzhi_c[1] * cos(CA_phase_average) - 0.866 * fuzhi_c[1] * sin(CA_phase_average);
					a4 = 0 - 0.866 * fuzhi_b[1] * cos(BA_phase_average) - 0.5   * fuzhi_b[1] * sin(BA_phase_average) + 0.866 * fuzhi_c[1] * cos(CA_phase_average) - 0.5   * fuzhi_c[1] * sin(CA_phase_average);//fuxu
					uneg_param1 = sqrt(a3*a3 + a4 * a4);
					uneg_param2 = sqrt(a1*a1 + a2 * a2);
					uneg = uneg_param1 / uneg_param2;//����
				}
			}
			A_FFT_last = A_FFT;
		}
		::Sleep(1);
	}
}
VOID OneMinuteTimerCallbackFunc(PVOID lpParamter, BOOLEAN TimerOrWaitFired)
{
	error_flag = 1;
}