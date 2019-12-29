#include "V_Dip_Swell_Interrupt.h"

extern double A_result_800half;
extern double A_VoltagedipDepth, A_VoltagedipLastVoltageResult;
extern int A_VoltagedipDurationTime, A_VoltagedipDurationLastTime;
extern int A_VoltagedipDurationStartTime;
extern double A_VoltagedipVoltageTemp;
extern char A_voltagedipstartflag;
extern double A_VoltageswellVoltageTemp, A_VoltageswellVoltageResult, A_VoltageswellLastVoltageResult;
extern int A_VoltageswellDurationTime, A_VoltageswellDurationLastTime, A_VoltageswellDurationStartTime;
extern char A_voltageswellstartflag;
extern double A_VoltageinterruptVoltageTemp, A_VoltageinterruptVoltageResult, A_VoltageinterruptLastVoltageResult;
extern int A_VoltageinterruptionDurationTime, A_VoltageinterruptionDurationLastTime,A_VoltageinterruptionDurationStartTime;
extern char  A_voltageinterruptstartflag;

/******************************************************************************************/
void A_voltagedipcalculation(void)  //��ѹ�轵
{
	/*updata the Ures*/
	if (A_result_800half< A_VoltagedipVoltageTemp)
		A_VoltagedipVoltageTemp = A_result_800half;   //������
											
	A_VoltagedipDepth = A_VoltagedipVoltageTemp;
	A_VoltagedipDurationTime = clock() - A_VoltagedipDurationStartTime;  //ʵʱʱ��

	if ((A_result_800half >= (VoltagedipThreshold + 0.02*DeclaredInputVoltageUdin)) || (A_result_800half<VoltageinterruptThreshold))       //��ѹ�ݽ�����
	{
		A_VoltagedipLastVoltageResult = A_VoltagedipDepth;//���һ��
		A_VoltagedipDurationLastTime = A_VoltagedipDurationTime;
		A_VoltagedipDurationTime = 0;
		A_voltagedipstartflag = 0;
		A_VoltagedipDepth = 0;
	}
}

void A_voltageswellcalculation(void)
{
	/*updata the Umax*/
	if (A_result_800half>A_VoltageswellVoltageTemp)
		A_VoltageswellVoltageTemp = A_result_800half;     //ѡ���������
															 /*supply voltage swells ended*/
		A_VoltageswellVoltageResult = A_VoltageswellVoltageTemp;
	    A_VoltageswellDurationTime = clock() - A_VoltageswellDurationStartTime;
	
	if ((A_result_800half <= (VoltageswellThreshold - 0.02*DeclaredInputVoltageUdin)))  //��ѹ��������
	{
		A_VoltageswellDurationLastTime = A_VoltageswellDurationTime;
		A_VoltageswellLastVoltageResult = A_VoltageswellVoltageResult;
		A_voltageswellstartflag = 0;
		A_VoltageswellDurationTime = 0;      //ʱ��
		A_VoltageswellVoltageResult = 0;
	}
}

void A_voltageinterruptioncalculation(void)
{
	/*updata the Ures*/
	if (A_result_800half<A_VoltageinterruptVoltageTemp)
		A_VoltageinterruptVoltageTemp = A_result_800half;
	/*supply voltage interruptions ended*/
	A_VoltageinterruptVoltageResult = A_VoltageinterruptVoltageTemp;//�жϲ����ѹ
	A_VoltageinterruptionDurationTime = clock() - A_VoltageinterruptionDurationStartTime;
	
	if ((A_result_800half >= (VoltageinterruptThreshold + 0.02*DeclaredInputVoltageUdin)))  //��ѹ�жϽ���
	{
		A_VoltageinterruptionDurationLastTime = A_VoltageinterruptionDurationTime;
		A_VoltageinterruptLastVoltageResult = A_VoltageinterruptVoltageResult;
		A_voltageinterruptstartflag=0;
		A_VoltageinterruptionDurationTime = 0;
		A_VoltageinterruptVoltageResult = 0;
	}
}
/***************************************************************************************/
/*voltage A_dip/A_swell/interruption detection*/
char A_dip[100], A_swell[100], A_interrupt[100];

void A_voltagedipswellinterruptiondetection(void)
{
	SYSTEMTIME Diptime, Swelltime, Interrupttime;
	
	/*supply voltage dips occurred*/
	if (A_voltagedipstartflag == 0) //��ǰ�޵�ѹ�轵
	{
		if ((A_result_800half < VoltagedipThreshold) && (A_result_800half>VoltageinterruptThreshold))  //��⵽��ѹ�轵
		{
			
			GetLocalTime(&Diptime);
			sprintf_s(A_dip, "%2d-%2d-%2d %2d:%2d:%2d:%2d\n", Diptime.wYear, Diptime.wMonth, Diptime.wDay,
			Diptime.wHour, Diptime.wMinute, Diptime.wSecond, Diptime.wMilliseconds);
			A_VoltagedipDurationStartTime = clock();
			
			A_voltagedipstartflag = 1;
			/*Initialize the Ures*/
			A_VoltagedipVoltageTemp = A_result_800half;
		}
	}
	if (A_voltageswellstartflag == 0)
	{
		/*supply voltage swells occurred*/
		if ((A_result_800half> VoltageswellThreshold))   //��⵽��ѹ����
		{
			GetLocalTime(&Swelltime);
			sprintf_s(A_swell, "%2d-%2d-%2d %2d:%2d:%2d:%2d\n", Swelltime.wYear, Swelltime.wMonth, Swelltime.wDay,
			Swelltime.wHour, Swelltime.wMinute, Swelltime.wSecond, Swelltime.wMilliseconds);
			A_VoltageswellDurationStartTime = clock();
			A_voltageswellstartflag = 1;
			/*Initialize the Umax*/
			A_VoltageswellVoltageTemp = A_result_800half;
		}
	}
	if ( A_voltageinterruptstartflag== 0)
	{
		/*supply voltage interruptions occurred*/
		if ((A_result_800half < VoltageinterruptThreshold))   //��⵽��ѹ�ж�

		{
			GetLocalTime(&Interrupttime);
			sprintf_s(A_interrupt, "%2d-%2d-%2d %2d:%2d:%2d:%2d\n", Interrupttime.wYear,
			Interrupttime.wMonth, Interrupttime.wDay, Interrupttime.wHour,
			Interrupttime.wMinute, Interrupttime.wSecond, Interrupttime.wMilliseconds);
			A_VoltageinterruptionDurationStartTime = clock();        //�жϿ�ʼʱ��
			A_voltageinterruptstartflag = 1;
			/*Initialize the Ures*/
			A_VoltageinterruptVoltageTemp = A_result_800half;
		}
	}
}

extern double B_result_800half;
extern double B_VoltagedipDepth, B_VoltagedipLastVoltageResult;
extern int B_VoltagedipDurationTime, B_VoltagedipDurationLastTime;
extern int B_VoltagedipDurationStartTime;
extern double B_VoltagedipVoltageTemp;
extern char B_voltagedipstartflag;
extern double B_VoltageswellVoltageTemp, B_VoltageswellVoltageResult, B_VoltageswellLastVoltageResult;
extern int B_VoltageswellDurationTime, B_VoltageswellDurationLastTime, B_VoltageswellDurationStartTime;
extern char B_voltageswellstartflag;
extern double B_VoltageinterruptVoltageTemp, B_VoltageinterruptVoltageResult, B_VoltageinterruptLastVoltageResult;
extern int B_VoltageinterruptionDurationTime, B_VoltageinterruptionDurationLastTime, B_VoltageinterruptionDurationStartTime;
extern char  B_voltageinterruptstartflag;
/******************************************************************************************/
void B_voltagedipcalculation(void)  //��ѹ�轵
{
	/*updata the Ures*/
	if (B_result_800half < B_VoltagedipVoltageTemp)
		B_VoltagedipVoltageTemp = B_result_800half;   //������

	B_VoltagedipDepth = B_VoltagedipVoltageTemp;
	B_VoltagedipDurationTime = clock() - B_VoltagedipDurationStartTime;  //ʵʱʱ��
	if ((B_result_800half >= (VoltagedipThreshold + 0.02*DeclaredInputVoltageUdin)) || (B_result_800half < VoltageinterruptThreshold))       //��ѹ�ݽ�����
	{
		B_VoltagedipLastVoltageResult = B_VoltagedipDepth;//���һ��
		B_VoltagedipDurationLastTime = B_VoltagedipDurationTime;
		B_VoltagedipDurationTime = 0;
		B_voltagedipstartflag = 0;
		B_VoltagedipDepth = 0;
	}
}
void B_voltageswellcalculation(void)
{
	/*updata the Umax*/
	if (B_result_800half > B_VoltageswellVoltageTemp)
		B_VoltageswellVoltageTemp = B_result_800half;     //ѡ���������
															 /*supply voltage swells ended*/
	B_VoltageswellVoltageResult = B_VoltageswellVoltageTemp;
	B_VoltageswellDurationTime = clock() - B_VoltageswellDurationStartTime;

	if ((B_result_800half <= (VoltageswellThreshold - 0.02*DeclaredInputVoltageUdin)))  //��ѹ��������
	{
		B_VoltageswellDurationLastTime = B_VoltageswellDurationTime;
		B_VoltageswellLastVoltageResult = B_VoltageswellVoltageResult;
		B_voltageswellstartflag = 0;
		B_VoltageswellDurationTime = 0;      //ʱ��
		B_VoltageswellVoltageResult = 0;
	}
}
void B_voltageinterruptioncalculation(void)
{
	/*updata the Ures*/
	if (B_result_800half < B_VoltageinterruptVoltageTemp)
		B_VoltageinterruptVoltageTemp = B_result_800half;
	/*supply voltage interruptions ended*/
	B_VoltageinterruptVoltageResult = B_VoltageinterruptVoltageTemp;//�жϲ����ѹ
	B_VoltageinterruptionDurationTime = clock() - B_VoltageinterruptionDurationStartTime;

	if ((B_result_800half >= (VoltageinterruptThreshold + 0.02*DeclaredInputVoltageUdin)))  //��ѹ�жϽ���
	{
		B_VoltageinterruptionDurationLastTime = B_VoltageinterruptionDurationTime;
		B_VoltageinterruptLastVoltageResult = B_VoltageinterruptVoltageResult;
		B_voltageinterruptstartflag = 0;
		B_VoltageinterruptionDurationTime = 0;
		B_VoltageinterruptVoltageResult = 0;
	}
}
/***************************************************************************************/
/*voltage B_dip/B_swell/interruption detection*/
char B_dip[100], B_swell[100], B_interrupt[100];
void B_voltagedipswellinterruptiondetection(void)
{
	SYSTEMTIME Diptime, Swelltime, Interrupttime;

	/*supply voltage dips occurred*/
	if (B_voltagedipstartflag == 0) //��ǰ�޵�ѹ�轵
	{
		if ((B_result_800half < VoltagedipThreshold) && (B_result_800half > VoltageinterruptThreshold))  //��⵽��ѹ�轵
		{
			GetLocalTime(&Diptime);
			sprintf_s(B_dip, "%2d-%2d-%2d %2d:%2d:%2d:%2d\n", Diptime.wYear, Diptime.wMonth, Diptime.wDay,
				Diptime.wHour, Diptime.wMinute, Diptime.wSecond, Diptime.wMilliseconds);
			B_VoltagedipDurationStartTime = clock();
			B_voltagedipstartflag = 1;
			/*Initialize the Ures*/
			B_VoltagedipVoltageTemp = B_result_800half;
		}
	}
	if (B_voltageswellstartflag == 0)
	{
		/*supply voltage swells occurred*/
		if ((B_result_800half > VoltageswellThreshold))   //��⵽��ѹ����
		{
			GetLocalTime(&Swelltime);
			sprintf_s(B_swell, "%2d-%2d-%2d %2d:%2d:%2d:%2d\n", Swelltime.wYear, Swelltime.wMonth, Swelltime.wDay,
				Swelltime.wHour, Swelltime.wMinute, Swelltime.wSecond, Swelltime.wMilliseconds);
			B_VoltageswellDurationStartTime = clock();
			B_voltageswellstartflag = 1;
			/*Initialize the Umax*/
			B_VoltageswellVoltageTemp = B_result_800half;
		}
	}
	if (B_voltageinterruptstartflag == 0)
	{
		/*supply voltage interruptions occurred*/
		if ((B_result_800half < VoltageinterruptThreshold))   //��⵽��ѹ�ж�

		{
			GetLocalTime(&Interrupttime);
			sprintf_s(B_interrupt, "%2d-%2d-%2d %2d:%2d:%2d:%2d\n", Interrupttime.wYear,
				Interrupttime.wMonth, Interrupttime.wDay, Interrupttime.wHour,
				Interrupttime.wMinute, Interrupttime.wSecond, Interrupttime.wMilliseconds);
			B_VoltageinterruptionDurationStartTime = clock();        //�жϿ�ʼʱ��
			B_voltageinterruptstartflag = 1;
			/*Initialize the Ures*/
			B_VoltageinterruptVoltageTemp = B_result_800half;
		}
	}
}
extern double C_result_800half;
extern double C_VoltagedipDepth, C_VoltagedipLastVoltageResult;
extern int C_VoltagedipDurationTime, C_VoltagedipDurationLastTime;
extern int C_VoltagedipDurationStartTime;
extern double C_VoltagedipVoltageTemp;
extern char C_voltagedipstartflag;
extern double C_VoltageswellVoltageTemp, C_VoltageswellVoltageResult, C_VoltageswellLastVoltageResult;
extern int C_VoltageswellDurationTime, C_VoltageswellDurationLastTime, C_VoltageswellDurationStartTime;
extern char C_voltageswellstartflag;
extern double C_VoltageinterruptVoltageTemp, C_VoltageinterruptVoltageResult, C_VoltageinterruptLastVoltageResult;
extern int C_VoltageinterruptionDurationTime, C_VoltageinterruptionDurationLastTime, C_VoltageinterruptionDurationStartTime;
extern char  C_voltageinterruptstartflag;
/******************************************************************************************/

void C_voltagedipcalculation(void)  //��ѹ�轵
{
	/*updata the Ures*/
	if (C_result_800half < C_VoltagedipVoltageTemp)
		C_VoltagedipVoltageTemp = C_result_800half;   //������

	C_VoltagedipDepth = C_VoltagedipVoltageTemp;
	C_VoltagedipDurationTime = clock() - C_VoltagedipDurationStartTime;  //ʵʱʱ��
	if ((C_result_800half >= (VoltagedipThreshold + 0.02*DeclaredInputVoltageUdin)) || (C_result_800half < VoltageinterruptThreshold))       //��ѹ�ݽ�����
	{
		C_VoltagedipLastVoltageResult = C_VoltagedipDepth;//���һ��
		C_VoltagedipDurationLastTime = C_VoltagedipDurationTime;
		C_VoltagedipDurationTime = 0;
		C_voltagedipstartflag = 0;
		C_VoltagedipDepth = 0;
	}
}

void C_voltageswellcalculation(void)
{
	/*updata the Umax*/
	if (C_result_800half > C_VoltageswellVoltageTemp)
		C_VoltageswellVoltageTemp = C_result_800half;     //ѡ���������
															 /*supply voltage swells ended*/
	C_VoltageswellVoltageResult = C_VoltageswellVoltageTemp;
	C_VoltageswellDurationTime = clock() - C_VoltageswellDurationStartTime;

	if ((C_result_800half <= (VoltageswellThreshold - 0.02*DeclaredInputVoltageUdin)))  //��ѹ��������
	{
		C_VoltageswellDurationLastTime = C_VoltageswellDurationTime;
		C_VoltageswellLastVoltageResult = C_VoltageswellVoltageResult;
		C_voltageswellstartflag = 0;
		C_VoltageswellDurationTime = 0;      //ʱ��
		C_VoltageswellVoltageResult = 0;
	}
}

void C_voltageinterruptioncalculation(void)
{
	/*updata the Ures*/
	if (C_result_800half < C_VoltageinterruptVoltageTemp)
		C_VoltageinterruptVoltageTemp = C_result_800half;
	/*supply voltage interruptions ended*/
	C_VoltageinterruptVoltageResult = C_VoltageinterruptVoltageTemp;//�жϲ����ѹ
	C_VoltageinterruptionDurationTime = clock() - C_VoltageinterruptionDurationStartTime;

	if ((C_result_800half >= (VoltageinterruptThreshold + 0.02*DeclaredInputVoltageUdin)))  //��ѹ�жϽ���
	{
		C_VoltageinterruptionDurationLastTime = C_VoltageinterruptionDurationTime;
		C_VoltageinterruptLastVoltageResult = C_VoltageinterruptVoltageResult;
		C_voltageinterruptstartflag = 0;
		C_VoltageinterruptionDurationTime = 0;
		C_VoltageinterruptVoltageResult = 0;
	}
}

/***************************************************************************************/
/*voltage C_dip/C_swell/interruption detection*/
char C_dip[100], C_swell[100], C_interrupt[100];
void C_voltagedipswellinterruptiondetection(void)
{
	SYSTEMTIME Diptime, Swelltime, Interrupttime;
	/*supply voltage dips occurred*/
	if (C_voltagedipstartflag == 0) //��ǰ�޵�ѹ�轵
	{
		if ((C_result_800half < VoltagedipThreshold) && (C_result_800half > VoltageinterruptThreshold))  //��⵽��ѹ�轵
		{
			GetLocalTime(&Diptime);
			sprintf_s(C_dip, "%2d-%2d-%2d %2d:%2d:%2d:%2d\n", Diptime.wYear, Diptime.wMonth, Diptime.wDay,
				Diptime.wHour, Diptime.wMinute, Diptime.wSecond, Diptime.wMilliseconds);
			C_VoltagedipDurationStartTime = clock();
			C_voltagedipstartflag = 1;
			/*Initialize the Ures*/
			C_VoltagedipVoltageTemp = C_result_800half;
		}
	}
	if (C_voltageswellstartflag == 0)
	{
		/*supply voltage swells occurred*/
		if ((C_result_800half > VoltageswellThreshold))   //��⵽��ѹ����
		{
			GetLocalTime(&Swelltime);
			sprintf_s(C_swell, "%2d-%2d-%2d %2d:%2d:%2d:%2d\n", Swelltime.wYear, Swelltime.wMonth, Swelltime.wDay,
				Swelltime.wHour, Swelltime.wMinute, Swelltime.wSecond, Swelltime.wMilliseconds);
			C_VoltageswellDurationStartTime = clock();
			C_voltageswellstartflag = 1;
			/*Initialize the Umax*/
			C_VoltageswellVoltageTemp = C_result_800half;
		}
	}
	if (C_voltageinterruptstartflag == 0)
	{
		/*supply voltage interruptions occurred*/
		if ((C_result_800half < VoltageinterruptThreshold))   //��⵽��ѹ�ж�

		{
			GetLocalTime(&Interrupttime);
			sprintf_s(C_interrupt, "%2d-%2d-%2d %2d:%2d:%2d:%2d\n", Interrupttime.wYear,
				Interrupttime.wMonth, Interrupttime.wDay, Interrupttime.wHour,
				Interrupttime.wMinute, Interrupttime.wSecond, Interrupttime.wMilliseconds);
			C_VoltageinterruptionDurationStartTime = clock();        //�жϿ�ʼʱ��
			C_voltageinterruptstartflag = 1;
			/*Initialize the Ures*/
			C_VoltageinterruptVoltageTemp = C_result_800half;
		}
	}
}