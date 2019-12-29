#include "pq.h"


#pragma execution_character_set("utf-8")
PQ::PQ(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	setWindowTitle(tr("������������"));

	t = new QTimer;
	connect(t, SIGNAL(timeout()), this, SLOT(update()));
	t->start(1000);

	//�������	
	series = new QSplineSeries();
	series2 = new QLineSeries();
	chart = new QChart();
	chart->legend()->hide();
	chart->addSeries(series);
	chart->addSeries(series2);
	chart->setTitle("A���ѹ��Чֵ");
	chart->createDefaultAxes();
	chart->axisY()->setRange(y_min, y_max);
	chart->axisX()->setRange(x_min, x_max);
	chartView = new QChartView(chart);
	chartView->setRenderHint(QPainter::Antialiasing);
	QGridLayout *baseLayout = new QGridLayout(); //������ʾ���������񲼾�
	baseLayout->addWidget(chartView, 1, 0);
	ui.curve->setLayout(baseLayout);
	t2 = new QTimer;
	connect(t2, SIGNAL(timeout()), this, SLOT(curve()));
	t2->start(10);

	model = new QStandardItemModel();
	ui.tableView->setModel(model);
	model->setColumnCount(2);
	model->setHeaderData(0, Qt::Horizontal, "Ƶ��");
	model->setHeaderData(1, Qt::Horizontal, "��Чֵ");
	model->setRowCount(10);
	model->setHeaderData(0, Qt::Vertical, "0");
	model->setHeaderData(1, Qt::Vertical, "1");
	model->setHeaderData(2, Qt::Vertical, "2");
	model->setHeaderData(3, Qt::Vertical, "3");
	model->setHeaderData(4, Qt::Vertical, "4");
	model->setHeaderData(5, Qt::Vertical, "5");
	model->setHeaderData(6, Qt::Vertical, "6");
	model->setHeaderData(7, Qt::Vertical, "7");
	model->setHeaderData(8, Qt::Vertical, "8");
	model->setHeaderData(9, Qt::Vertical, "9");


	connect(ui.updateButton, SIGNAL(clicked()), this, SLOT(shuaxin()));
	connect(ui.flicker_updateButton, SIGNAL(clicked()), this, SLOT(A_flicker_shuaxin()));
	connect(ui.B_flicker_updateButton, SIGNAL(clicked()), this, SLOT(B_flicker_shuaxin()));
	connect(ui.C_flicker_updateButton, SIGNAL(clicked()), this, SLOT(C_flicker_shuaxin()));
	connect(ui.fuwei, SIGNAL(clicked()), this, SLOT(fuwei()));
}
QString tempStr;
extern int A_packet_number, B_packet_number, C_packet_number;
extern double A_rms, B_rms, C_rms, A_cur_rms, B_cur_rms, C_cur_rms;
extern double A_fre, B_fre, C_fre, A_active_power, A_reactive_power, A_apparent_power, A_active_power_meter, A_reactive_power_meter;
extern double B_active_power, B_reactive_power, B_apparent_power, B_active_power_meter, B_reactive_power_meter;
extern double C_active_power, C_reactive_power, C_apparent_power, C_active_power_meter, C_reactive_power_meter;
extern double fuzhi_a[HarmonicWave], fuzhi_b[HarmonicWave], fuzhi_c[HarmonicWave];
extern double fftw_phase_differ[AN_BUFFER_LEN_8000], fuzhi_a_cur[HarmonicWave], THDU;
extern double B_THDU, C_THDU, fftw_phase_differ_b[AN_BUFFER_LEN_8000], fuzhi_b_cur[HarmonicWave], fftw_phase_differ_c[AN_BUFFER_LEN_8000], fuzhi_c_cur[HarmonicWave];

extern double A_VoltagedipDepth, A_VoltagedipLastVoltageResult, A_VoltageswellVoltageResult, A_VoltageswellLastVoltageResult, A_VoltageinterruptVoltageResult, A_VoltageinterruptLastVoltageResult;
extern int A_VoltageswellDurationTime, A_VoltageinterruptionDurationTime, A_VoltagedipDurationTime;
extern char A_dip[100], A_swell[100], A_interrupt[100];
extern char A_voltageswellstartflag, A_voltagedipstartflag, A_voltageinterruptstartflag;
extern int A_VoltagedipDurationLastTime, A_VoltageswellDurationLastTime, A_VoltageinterruptionDurationLastTime;
extern double B_VoltagedipDepth, B_VoltagedipLastVoltageResult, B_VoltageswellVoltageResult, B_VoltageswellLastVoltageResult, B_VoltageinterruptVoltageResult, B_VoltageinterruptLastVoltageResult;
extern int B_VoltageswellDurationTime, B_VoltageinterruptionDurationTime, B_VoltagedipDurationTime;
extern char B_dip[100], B_swell[100], B_interrupt[100];
extern char B_voltageswellstartflag, B_voltagedipstartflag, B_voltageinterruptstartflag;
extern int B_VoltagedipDurationLastTime, B_VoltageswellDurationLastTime, B_VoltageinterruptionDurationLastTime;
extern double C_VoltagedipDepth, C_VoltagedipLastVoltageResult, C_VoltageswellVoltageResult, C_VoltageswellLastVoltageResult, C_VoltageinterruptVoltageResult, C_VoltageinterruptLastVoltageResult;
extern int C_VoltageswellDurationTime, C_VoltageinterruptionDurationTime, C_VoltagedipDurationTime;
extern char C_dip[100], C_swell[100], C_interrupt[100];
extern char C_voltageswellstartflag, C_voltagedipstartflag, C_voltageinterruptstartflag;
extern int C_VoltagedipDurationLastTime, C_VoltageswellDurationLastTime, C_VoltageinterruptionDurationLastTime;

extern double A_tiaozhibo_f, A_V_fluctuation, A_InstantaneousFlickerValue, A_ShorttimeFlickerValue, A_LongtimeFlickerValue;
extern int A_shanbianCount;
extern double B_tiaozhibo_f, B_V_fluctuation, B_InstantaneousFlickerValue, B_ShorttimeFlickerValue, B_LongtimeFlickerValue;
extern int B_shanbianCount;
extern double C_tiaozhibo_f, C_V_fluctuation, C_InstantaneousFlickerValue, C_ShorttimeFlickerValue, C_LongtimeFlickerValue;
extern int C_shanbianCount;
extern double  uneg, uneg_param1, uneg_param2;
int loss_open, A_flicker_open, A_voltage_dipswellinterrupt_open;
int B_flicker_open, B_voltage_dipswellinterrupt_open;
int C_flicker_open, C_voltage_dipswellinterrupt_open;
extern int A_FFT_Number, pointfre;
extern u_long A_FFT, B_FFT, C_FFT;
extern char start_time[200];
extern long start, finish;
long totaltime;
char total_time[100];
int day, hour, minute, sec;
extern double fftw_phase_a_vol[HarmonicWave], fftw_phase_a_cur[HarmonicWave], fftw_phase_b[HarmonicWave], fftw_phase_c[HarmonicWave];
extern u_short A_flag, B_flag, C_flag;
extern u_int A_err_flag, B_err_flag, C_err_flag;
extern u_short A_err_current, B_err_current, C_err_current;
extern unsigned long A_err_sum, B_err_sum, C_err_sum;
extern double A_result_800half, A_result_400half, B_result_800half, B_result_400half, C_result_800half, C_result_400half;
extern double BA_phase_average, CA_phase_average;
extern unsigned long an_buffer_idx_A, an_buffer_idx_B, an_buffer_idx_C;
extern int threadsum;
extern int core_num;
extern DList *list_f;
double f_array[10];
void PQ::update(void)
{
	double active_power_meter_tmp, reactive_power_meter_tmp;
	double B_active_power_meter_tmp, B_reactive_power_meter_tmp;
	double C_active_power_meter_tmp, C_reactive_power_meter_tmp;
	char tmp[100];
	//loss_open = ui.loss_comboBox->currentIndex();
	A_flicker_open = ui.flicker_comboBox->currentIndex();
	B_flicker_open = ui.B_flicker_comboBox->currentIndex();
	C_flicker_open = ui.C_flicker_comboBox->currentIndex();
	A_voltage_dipswellinterrupt_open = ui.voltage_dipswellinterrupt_comboBox->currentIndex();
	B_voltage_dipswellinterrupt_open = ui.B_voltage_dipswellinterrupt_comboBox->currentIndex();
	C_voltage_dipswellinterrupt_open = ui.C_voltage_dipswellinterrupt_comboBox->currentIndex();

	ui.test->setText(tempStr.setNum(core_num));
	//ʱ��
	{
		ui.System_Start_Time->setText(start_time);
		finish = clock();
		totaltime = (finish - start) / CLOCKS_PER_SEC;
		day = totaltime / (60 * 60 * 24);
		totaltime = totaltime % (60 * 60 * 24);
		hour = totaltime / (60 * 60);
		totaltime = totaltime % (60 * 60);
		minute = totaltime / 60;
		totaltime = totaltime % 60;
		sec = totaltime;
		sprintf_s(total_time, "%2d��%2dʱ%2d��%2d��\n", day, hour, minute, sec);
		ui.total_time->setText(total_time);
	}
	ui.threadsum->setText(tempStr.setNum(threadsum));
	sprintf(tmp, "%.4f", A_result_800half);
	ui.result_800->setText(tmp);
	sprintf(tmp, "%.4f", A_result_400half);
	ui.result_400->setText(tmp);
	sprintf(tmp, "%.4f", B_result_800half);
	ui.B_result_800->setText(tmp);
	sprintf(tmp, "%.4f", B_result_400half);
	ui.B_result_400->setText(tmp);
	sprintf(tmp, "%.4f", C_result_800half);
	ui.C_result_800->setText(tmp);
	sprintf(tmp, "%.4f", C_result_400half);
	ui.C_result_400->setText(tmp);
	ui.A_FFT_flag->setText(tempStr.setNum(A_FFT));
	ui.B_FFT_flag->setText(tempStr.setNum(B_FFT));
	ui.C_FFT_flag->setText(tempStr.setNum(C_FFT));
	ui.index_a->setText(tempStr.setNum(an_buffer_idx_A));
	ui.index_b->setText(tempStr.setNum(an_buffer_idx_B));
	ui.index_c->setText(tempStr.setNum(an_buffer_idx_C));

	static double test_data = 50.0;
	test_data = test_data + 0.01;
	ChangeData(list_f, test_data);

		struct node *temp = list_f->head;
		for (int i = 0; i < list_f->len; i++)
		{
			f_array[i] = temp->data;
			temp = temp->next;
		}
		sprintf(tmp, "%.3f", f_array[0]);
		model->setItem(0, 0, new QStandardItem(tmp));
		sprintf(tmp, "%.3f", f_array[1]);
		model->setItem(1, 0, new QStandardItem(tmp));
		sprintf(tmp, "%.3f", f_array[2]);
		model->setItem(2, 0, new QStandardItem(tmp));
		sprintf(tmp, "%.3f", f_array[3]);
		model->setItem(3, 0, new QStandardItem(tmp));
		sprintf(tmp, "%.3f", f_array[4]);
		model->setItem(4, 0, new QStandardItem(tmp));
		sprintf(tmp, "%.3f", f_array[5]);
		model->setItem(5, 0, new QStandardItem(tmp));
		sprintf(tmp, "%.3f", f_array[6]);
		model->setItem(6, 0, new QStandardItem(tmp));
		sprintf(tmp, "%.3f", f_array[7]);
		model->setItem(7, 0, new QStandardItem(tmp));
		sprintf(tmp, "%.3f", f_array[8]);
		model->setItem(8, 0, new QStandardItem(tmp));
		sprintf(tmp, "%.3f", f_array[9]);
		model->setItem(9, 0, new QStandardItem(tmp));

		model->setItem(0, 1, new QStandardItem("3"));

	//��λ
	{
		sprintf(tmp, "%.3f", fftw_phase_a_vol[1] / PI * 180);
		ui.A_jiboxiangwei->setText(tmp);
		sprintf(tmp, "%.3f", fftw_phase_b[1] / PI * 180);
		ui.B_jiboxiangwei->setText(tmp);
		sprintf(tmp, "%.3f", fftw_phase_c[1] / PI * 180);
		ui.C_jiboxiangwei->setText(tmp);
		sprintf(tmp, "%.4f", BA_phase_average / PI * 180);
		ui.BA_xiangweicha->setText(tmp);
		sprintf(tmp, "%.4f", CA_phase_average / PI * 180);
		ui.CA_xiangweicha->setText(tmp);
	}
	ui.A_flag->setText(tempStr.setNum(A_flag));
	ui.B_flag->setText(tempStr.setNum(B_flag));
	ui.C_flag->setText(tempStr.setNum(C_flag));

	//A�����ֵ
	{
		//A���ѹ��Чֵ
		sprintf(tmp, "%.7f", A_rms);
		ui.V_rms->setText(tmp);
		//A�������Чֵ
		sprintf(tmp, "%.7f", A_cur_rms);
		ui.I_rms->setText(tmp);
		//Ƶ��
		sprintf(tmp, "%.7f", A_fre);
		ui.FHz->setText(tmp);
		//�й�����
		sprintf(tmp, "%.7f", A_active_power);
		ui.PW->setText(tmp);
		//�й����
		active_power_meter_tmp = A_active_power_meter;
		sprintf(tmp, "%.9f", active_power_meter_tmp);
		ui.PT->setText(tmp);
		//�޹���ѹ
		sprintf(tmp, "%.7f", A_reactive_power);
		ui.QVar->setText(tmp);
		//�޹����
		reactive_power_meter_tmp = A_reactive_power_meter;
		sprintf(tmp, "%.9f", reactive_power_meter_tmp);
		ui.QT->setText(tmp);
		//���ڹ���
		sprintf(tmp, "%.7f", A_apparent_power);
		ui.SVA->setText(tmp);

		//THDU
		sprintf(tmp, "%.5f%%", THDU);
		ui.Thdu->setText(tmp);
		//��ѹ������λ��
		sprintf(tmp, "%.6f", fftw_phase_differ[1] / PI * 180);
		ui.VI_xiangweicha->setText(tmp);
		//��������
		sprintf(tmp, "%.6f", cos(fftw_phase_differ[1]));
		ui.VI_cos->setText(tmp);
		//��ѹ������ֵ
		sprintf(tmp, "%.9f", fuzhi_a[1]);
		ui.V_jibofuzhi->setText(tmp);
		//����������ֵ
		sprintf(tmp, "%.9f", fuzhi_a_cur[1]);
		ui.I_jibofuzhi->setText(tmp);
		//��ѹ����������ֵ��
		sprintf(tmp, "%.9f", fuzhi_a[1] / fuzhi_a_cur[1]);
		ui.VI_jibofuzhibi->setText(tmp);
	}
	//A��ֱ������+г��
	{
		//ֱ������
		sprintf(tmp, "%.8f", fuzhi_a[0]);
		ui.zhiliufenliang->setText(tmp);
		//����г����ֵ
		sprintf(tmp, "%.9f", fuzhi_a[2]);
		ui.ercixiebofuzhi->setText(tmp);
		//����г����ֵ
		sprintf(tmp, "%.9f", fuzhi_a[3]);
		ui.sancixiebofuzhi->setText(tmp);
		//�Ĵ�г����ֵ
		sprintf(tmp, "%.9f", fuzhi_a[4]);
		ui.sicixiebofuzhi->setText(tmp);
		//���г����ֵ
		sprintf(tmp, "%.9f", fuzhi_a[5]);
		ui.wucixiebofuzhi->setText(tmp);
		//����г����ֵ
		sprintf(tmp, "%.9f", fuzhi_a[6]);
		ui.liucixiebofuzhi->setText(tmp);
		//�ߴ�г����ֵ
		sprintf(tmp, "%.9f", fuzhi_a[7]);
		ui.qicixiebofuzhi->setText(tmp);
		//�˴�г����ֵ
		sprintf(tmp, "%.9f", fuzhi_a[8]);
		ui.bacixiebofuzhi->setText(tmp);
		//�Ŵ�г����ֵ
		sprintf(tmp, "%.9f", fuzhi_a[9]);
		ui.jiucixiebofuzhi->setText(tmp);
		//ʮ��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_a[10]);
		ui.shicixiebofuzhi->setText(tmp);
		//ʮһ��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_a[11]);
		ui.shiyicixiebofuzhi->setText(tmp);
		//ʮ����г����ֵ
		sprintf(tmp, "%.9f", fuzhi_a[12]);
		ui.shiercixiebofuzhi->setText(tmp);
		//ʮ����г����ֵ
		sprintf(tmp, "%.9f", fuzhi_a[13]);
		ui.shisancixiebofuzhi->setText(tmp);
		//ʮ�Ĵ�г����ֵ
		sprintf(tmp, "%.9f", fuzhi_a[14]);
		ui.shisicixiebofuzhi->setText(tmp);
		//ʮ���г����ֵ
		sprintf(tmp, "%.9f", fuzhi_a[15]);
		ui.shiwucixiebofuzhi->setText(tmp);
		//ʮ����г����ֵ
		sprintf(tmp, "%.9f", fuzhi_a[16]);
		ui.shiliucixiebofuzhi->setText(tmp);
		//ʮ�ߴ�г����ֵ
		sprintf(tmp, "%.9f", fuzhi_a[17]);
		ui.shiqicixiebofuzhi->setText(tmp);
		//18��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_a[18]);
		ui.shibacixiebofuzhi->setText(tmp);
		//19��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_a[19]);
		ui.shijiucixiebofuzhi->setText(tmp);
		//20��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_a[20]);
		ui.ershicixiebofuzhi->setText(tmp);

		//21��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_a[21]);
		ui.ershiyicixiebofuzhi->setText(tmp);
		//22��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_a[22]);
		ui.ershiercixiebofuzhi->setText(tmp);
		//23��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_a[23]);
		ui.ershisancixiebofuzhi->setText(tmp);
		//24��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_a[24]);
		ui.ershisicixiebofuzhi->setText(tmp);
		//25��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_a[25]);
		ui.ershiwucixiebofuzhi->setText(tmp);
		//26��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_a[26]);
		ui.ershiliucixiebofuzhi->setText(tmp);
		//27��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_a[27]);
		ui.ershiqicixiebofuzhi->setText(tmp);
		//28��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_a[28]);
		ui.ershibacixiebofuzhi->setText(tmp);
		//29��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_a[29]);
		ui.ershijiucixiebofuzhi->setText(tmp);
		//30��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_a[30]);
		ui.sanshicixiebofuzhi->setText(tmp);

		//31��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_a[31]);
		ui.sanshiyicixiebofuzhi->setText(tmp);
		//32��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_a[32]);
		ui.sanshiercixiebofuzhi->setText(tmp);
		//33��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_a[33]);
		ui.sanshisancixiebofuzhi->setText(tmp);
		//34��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_a[34]);
		ui.sanshisicixiebofuzhi->setText(tmp);
		//35��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_a[35]);
		ui.sanshiwucixiebofuzhi->setText(tmp);
		//36��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_a[36]);
		ui.sanshiliucixiebofuzhi->setText(tmp);
		//37��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_a[37]);
		ui.sanshiqicixiebofuzhi->setText(tmp);
		//38��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_a[38]);
		ui.sanshibacixiebofuzhi->setText(tmp);
		//39��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_a[39]);
		ui.sanshijiucixiebofuzhi->setText(tmp);
	}

	//B�����ֵ
	{

		//A���ѹ��Чֵ
		sprintf(tmp, "%.7f", B_rms);
		ui.B_rms->setText(tmp);
		//A�������Чֵ
		sprintf(tmp, "%.7f", B_cur_rms);
		ui.B_I_rms->setText(tmp);
		//Ƶ��
		sprintf(tmp, "%.7f", B_fre);
		ui.B_FHz->setText(tmp);
		//�й�����
		sprintf(tmp, "%.7f", B_active_power);
		ui.B_PW->setText(tmp);
		//�й����
		B_active_power_meter_tmp = B_active_power_meter;
		sprintf(tmp, "%.9f", B_active_power_meter_tmp);
		ui.B_PT->setText(tmp);
		//�޹���ѹ
		sprintf(tmp, "%.7f", B_reactive_power);
		ui.B_QVar->setText(tmp);
		//�޹����
		B_reactive_power_meter_tmp = B_reactive_power_meter;
		sprintf(tmp, "%.9f", B_reactive_power_meter_tmp);
		ui.B_QT->setText(tmp);
		//���ڹ���
		sprintf(tmp, "%.7f", B_apparent_power);
		ui.B_SVA->setText(tmp);

		//THDU
		sprintf(tmp, "%.5f%%", B_THDU);
		ui.B_Thdu->setText(tmp);
		//��ѹ������λ��
		sprintf(tmp, "%.6f", fftw_phase_differ_b[1] / PI * 180);
		ui.B_VI_xiangweicha->setText(tmp);
		//��������
		sprintf(tmp, "%.6f", cos(fftw_phase_differ_b[1]));
		ui.B_VI_cos->setText(tmp);
		//��ѹ������ֵ
		sprintf(tmp, "%.9f", fuzhi_b[1]);
		ui.B_V_jibofuzhi->setText(tmp);
		//����������ֵ
		sprintf(tmp, "%.9f", fuzhi_b_cur[1]);
		ui.B_I_jibofuzhi->setText(tmp);
		//��ѹ����������ֵ��
		sprintf(tmp, "%.9f", fuzhi_b[1] / fuzhi_b_cur[1]);
		ui.B_VI_jibofuzhibi->setText(tmp);

	}
	//B��
	{
		//BC��ֱ������+г��
		//Bֱ������
		sprintf(tmp, "%.8f", fuzhi_b[0]);
		ui.B_zhiliufenliang->setText(tmp);
		//B��ѹ������ֵ
		sprintf(tmp, "%.9f", fuzhi_b[1]);
		ui.B_V_jibofuzhi->setText(tmp);
		//����г����ֵ
		sprintf(tmp, "%.9f", fuzhi_b[2]);
		ui.B_ercixiebofuzhi->setText(tmp);
		//����г����ֵ
		sprintf(tmp, "%.9f", fuzhi_b[3]);
		ui.B_sancixiebofuzhi->setText(tmp);
		//�Ĵ�г����ֵ
		sprintf(tmp, "%.9f", fuzhi_b[4]);
		ui.B_sicixiebofuzhi->setText(tmp);
		//���г����ֵ
		sprintf(tmp, "%.9f", fuzhi_b[5]);
		ui.B_wucixiebofuzhi->setText(tmp);
		//����г����ֵ
		sprintf(tmp, "%.9f", fuzhi_b[6]);
		ui.B_liucixiebofuzhi->setText(tmp);
		//�ߴ�г����ֵ
		sprintf(tmp, "%.9f", fuzhi_b[7]);
		ui.B_qicixiebofuzhi->setText(tmp);
		//�˴�г����ֵ
		sprintf(tmp, "%.9f", fuzhi_b[8]);
		ui.B_bacixiebofuzhi->setText(tmp);
		//�Ŵ�г����ֵ
		sprintf(tmp, "%.9f", fuzhi_b[9]);
		ui.B_jiucixiebofuzhi->setText(tmp);
		//ʮ��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_b[10]);
		ui.B_shicixiebofuzhi->setText(tmp);
		//ʮһ��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_b[11]);
		ui.B_shiyicixiebofuzhi->setText(tmp);
		//ʮ����г����ֵ
		sprintf(tmp, "%.9f", fuzhi_b[12]);
		ui.B_shiercixiebofuzhi->setText(tmp);
		//ʮ����г����ֵ
		sprintf(tmp, "%.9f", fuzhi_b[13]);
		ui.B_shisancixiebofuzhi->setText(tmp);
		//ʮ�Ĵ�г����ֵ
		sprintf(tmp, "%.9f", fuzhi_b[14]);
		ui.B_shisicixiebofuzhi->setText(tmp);
		//ʮ���г����ֵ
		sprintf(tmp, "%.9f", fuzhi_b[15]);
		ui.B_shiwucixiebofuzhi->setText(tmp);
		//ʮ����г����ֵ
		sprintf(tmp, "%.9f", fuzhi_b[16]);
		ui.B_shiliucixiebofuzhi->setText(tmp);
		//ʮ�ߴ�г����ֵ
		sprintf(tmp, "%.9f", fuzhi_b[17]);
		ui.B_shiqicixiebofuzhi->setText(tmp);
		//18��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_b[18]);
		ui.B_shibacixiebofuzhi->setText(tmp);
		//19��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_b[19]);
		ui.B_shijiucixiebofuzhi->setText(tmp);
		//20��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_b[20]);
		ui.B_ershicixiebofuzhi->setText(tmp);

		//21��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_b[21]);
		ui.B_ershiyicixiebofuzhi->setText(tmp);
		//22��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_b[22]);
		ui.B_ershiercixiebofuzhi->setText(tmp);
		//23��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_b[23]);
		ui.B_ershisancixiebofuzhi->setText(tmp);
		//24��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_b[24]);
		ui.B_ershisicixiebofuzhi->setText(tmp);
		//25��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_b[25]);
		ui.B_ershiwucixiebofuzhi->setText(tmp);
		//26��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_b[26]);
		ui.B_ershiliucixiebofuzhi->setText(tmp);
		//27��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_b[27]);
		ui.B_ershiqicixiebofuzhi->setText(tmp);
		//28��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_b[28]);
		ui.B_ershibacixiebofuzhi->setText(tmp);
		//29��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_b[29]);
		ui.B_ershijiucixiebofuzhi->setText(tmp);
		//30��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_b[30]);
		ui.B_sanshicixiebofuzhi->setText(tmp);

		//31��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_b[31]);
		ui.B_sanshiyicixiebofuzhi->setText(tmp);
		//32��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_b[32]);
		ui.B_sanshiercixiebofuzhi->setText(tmp);
		//33��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_b[33]);
		ui.B_sanshisancixiebofuzhi->setText(tmp);
		//34��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_b[34]);
		ui.B_sanshisicixiebofuzhi->setText(tmp);
		//35��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_b[35]);
		ui.B_sanshiwucixiebofuzhi->setText(tmp);
		//36��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_b[36]);
		ui.B_sanshiliucixiebofuzhi->setText(tmp);
		//37��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_b[37]);
		ui.B_sanshiqicixiebofuzhi->setText(tmp);
		//38��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_b[38]);
		ui.B_sanshibacixiebofuzhi->setText(tmp);
		//39��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_b[39]);
		ui.B_sanshijiucixiebofuzhi->setText(tmp);
	}
	//C�����ֵ
	{

		//A���ѹ��Чֵ
		sprintf(tmp, "%.7f", C_rms);
		ui.C_rms->setText(tmp);
		//A�������Чֵ
		sprintf(tmp, "%.7f", C_cur_rms);
		ui.C_I_rms->setText(tmp);
		//Ƶ��
		sprintf(tmp, "%.7f", C_fre);
		ui.C_FHz->setText(tmp);
		//�й�����
		sprintf(tmp, "%.7f", C_active_power);
		ui.C_PW->setText(tmp);
		//�й����
		C_active_power_meter_tmp = C_active_power_meter;
		sprintf(tmp, "%.9f", C_active_power_meter_tmp);
		ui.C_PT->setText(tmp);
		//�޹���ѹ
		sprintf(tmp, "%.7f", C_reactive_power);
		ui.C_QVar->setText(tmp);
		//�޹����
		C_reactive_power_meter_tmp = C_reactive_power_meter;
		sprintf(tmp, "%.9f", C_reactive_power_meter_tmp);
		ui.C_QT->setText(tmp);
		//���ڹ���
		sprintf(tmp, "%.7f", C_apparent_power);
		ui.C_SVA->setText(tmp);

		//THDU
		sprintf(tmp, "%.5f%%", C_THDU);
		ui.C_Thdu->setText(tmp);
		//��ѹ������λ��
		sprintf(tmp, "%.6f", fftw_phase_differ_c[1] / PI * 180);
		ui.C_VI_xiangweicha->setText(tmp);
		//��������
		sprintf(tmp, "%.6f", cos(fftw_phase_differ_c[1]));
		ui.C_VI_cos->setText(tmp);
		//��ѹ������ֵ
		sprintf(tmp, "%.9f", fuzhi_c[1]);
		ui.C_V_jibofuzhi->setText(tmp);
		//����������ֵ
		sprintf(tmp, "%.9f", fuzhi_c_cur[1]);
		ui.C_I_jibofuzhi->setText(tmp);
		//��ѹ����������ֵ��
		sprintf(tmp, "%.9f", fuzhi_c[1] / fuzhi_c_cur[1]);
		ui.C_VI_jibofuzhibi->setText(tmp);
	}
	//C��
	{
		//Cֱ������
		sprintf(tmp, "%.8f", fuzhi_c[0]);
		ui.C_zhiliufenliang->setText(tmp);
		//C��ѹ������ֵ
		sprintf(tmp, "%.9f", fuzhi_c[1]);
		ui.C_V_jibofuzhi->setText(tmp);
		//����г����ֵ
		sprintf(tmp, "%.9f", fuzhi_c[2]);
		ui.C_ercixiebofuzhi->setText(tmp);
		//����г����ֵ
		sprintf(tmp, "%.9f", fuzhi_c[3]);
		ui.C_sancixiebofuzhi->setText(tmp);
		//�Ĵ�г����ֵ
		sprintf(tmp, "%.9f", fuzhi_c[4]);
		ui.C_sicixiebofuzhi->setText(tmp);
		//���г����ֵ
		sprintf(tmp, "%.9f", fuzhi_c[5]);
		ui.C_wucixiebofuzhi->setText(tmp);
		//����г����ֵ
		sprintf(tmp, "%.9f", fuzhi_c[6]);
		ui.C_liucixiebofuzhi->setText(tmp);
		//�ߴ�г����ֵ
		sprintf(tmp, "%.9f", fuzhi_c[7]);
		ui.C_qicixiebofuzhi->setText(tmp);
		//�˴�г����ֵ
		sprintf(tmp, "%.9f", fuzhi_c[8]);
		ui.C_bacixiebofuzhi->setText(tmp);
		//�Ŵ�г����ֵ
		sprintf(tmp, "%.9f", fuzhi_c[9]);
		ui.C_jiucixiebofuzhi->setText(tmp);
		//ʮ��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_c[10]);
		ui.C_shicixiebofuzhi->setText(tmp);

		//ʮһ��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_c[11]);
		ui.C_shiyicixiebofuzhi->setText(tmp);
		//ʮ����г����ֵ
		sprintf(tmp, "%.9f", fuzhi_c[12]);
		ui.C_shiercixiebofuzhi->setText(tmp);
		//ʮ����г����ֵ
		sprintf(tmp, "%.9f", fuzhi_c[13]);
		ui.C_shisancixiebofuzhi->setText(tmp);
		//ʮ�Ĵ�г����ֵ
		sprintf(tmp, "%.9f", fuzhi_c[14]);
		ui.C_shisicixiebofuzhi->setText(tmp);
		//ʮ���г����ֵ
		sprintf(tmp, "%.9f", fuzhi_c[15]);
		ui.C_shiwucixiebofuzhi->setText(tmp);
		//ʮ����г����ֵ
		sprintf(tmp, "%.9f", fuzhi_c[16]);
		ui.C_shiliucixiebofuzhi->setText(tmp);
		//ʮ�ߴ�г����ֵ
		sprintf(tmp, "%.9f", fuzhi_c[17]);
		ui.C_shiqicixiebofuzhi->setText(tmp);
		//18��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_c[18]);
		ui.C_shibacixiebofuzhi->setText(tmp);
		//19��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_c[19]);
		ui.C_shijiucixiebofuzhi->setText(tmp);
		//20��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_c[20]);
		ui.C_ershicixiebofuzhi->setText(tmp);

		//21��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_c[21]);
		ui.C_ershiyicixiebofuzhi->setText(tmp);
		//22��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_c[22]);
		ui.C_ershiercixiebofuzhi->setText(tmp);
		//23��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_c[23]);
		ui.C_ershisancixiebofuzhi->setText(tmp);
		//24��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_c[24]);
		ui.C_ershisicixiebofuzhi->setText(tmp);
		//25��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_c[25]);
		ui.C_ershiwucixiebofuzhi->setText(tmp);
		//26��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_c[26]);
		ui.C_ershiliucixiebofuzhi->setText(tmp);
		//27��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_c[27]);
		ui.C_ershiqicixiebofuzhi->setText(tmp);
		//28��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_c[28]);
		ui.C_ershibacixiebofuzhi->setText(tmp);
		//29��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_c[29]);
		ui.C_ershijiucixiebofuzhi->setText(tmp);
		//30��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_c[30]);
		ui.C_sanshicixiebofuzhi->setText(tmp);

		//31��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_c[31]);
		ui.C_sanshiyicixiebofuzhi->setText(tmp);
		//32��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_c[32]);
		ui.C_sanshiercixiebofuzhi->setText(tmp);
		//33��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_c[33]);
		ui.C_sanshisancixiebofuzhi->setText(tmp);
		//34��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_c[34]);
		ui.C_sanshisicixiebofuzhi->setText(tmp);
		//35��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_c[35]);
		ui.C_sanshiwucixiebofuzhi->setText(tmp);
		//36��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_c[36]);
		ui.C_sanshiliucixiebofuzhi->setText(tmp);
		//37��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_c[37]);
		ui.C_sanshiqicixiebofuzhi->setText(tmp);
		//38��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_c[38]);
		ui.C_sanshibacixiebofuzhi->setText(tmp);
		//39��г����ֵ
		sprintf(tmp, "%.9f", fuzhi_c[39]);
		ui.C_sanshijiucixiebofuzhi->setText(tmp);
	}
	//A��ѹ�轵
	//if ((A_VoltagedipDurationLastTime > 20)||(A_VoltagedipDurationLastTime==0))
	{
		ui.V_Dip_StartTime->setText(A_dip);
		ui.V_Dip_Flag->setText(tempStr.setNum(A_voltagedipstartflag));
		//��ѹ�轵���
		sprintf(tmp, "%.6f", A_VoltagedipDepth);
		ui.V_Dip_Depth->setText(tmp);
		//���һ���轵���
		sprintf(tmp, "%.6f", A_VoltagedipLastVoltageResult);
		ui.Last_V_Dip_Depth->setText(tmp);
		//��ѹ�轵ʵʱʱ��
		ui.V_Dip_Time->setText(tempStr.setNum(A_VoltagedipDurationTime));
		//���һ�ε�ѹ�轵ʱ��
		ui.V_Dip_LastTime->setText(tempStr.setNum(A_VoltagedipDurationLastTime));
	}
	//A��ѹ����
	//if ((A_VoltageswellDurationLastTime > 20) || (A_VoltageswellDurationLastTime == 0))
	{
		ui.V_Swell_StartTime->setText(A_swell);
		ui.V_Swell_Flag->setText(tempStr.setNum(A_voltageswellstartflag));
		//��ѹ������ѹ
		sprintf(tmp, "%.6f", A_VoltageswellVoltageResult);
		ui.V_Swell->setText(tmp);
		//���һ�ε�ѹ������ѹ
		sprintf(tmp, "%.6f", A_VoltageswellLastVoltageResult);
		ui.Last_V_Swell->setText(tmp);
		//��ѹ����ʵʱʱ��
		ui.V_Swell_Time->setText(tempStr.setNum(A_VoltageswellDurationTime));
		//���һ�ε�ѹ��������ʱ��
		ui.V_Swell_LastTime->setText(tempStr.setNum(A_VoltageswellDurationLastTime));
	}
	//A��ѹ�ж�
	//if ((A_VoltageinterruptionDurationLastTime > 20) || (A_VoltageinterruptionDurationLastTime == 0))
	{
		ui.V_Interrupt_StartTime->setText(A_interrupt);
		ui.V_Interrupt_Flag->setText(tempStr.setNum(A_voltageinterruptstartflag));
		//��ѹ�жϲ����ѹ
		sprintf(tmp, "%.6f", A_VoltageinterruptVoltageResult);
		ui.V_Interrupt->setText(tmp);
		//���һ�ε�ѹ�жϲ����ѹ
		sprintf(tmp, "%.6f", A_VoltageinterruptLastVoltageResult);
		ui.Last_V_Interrupt->setText(tmp);
		//��ѹ�ж�ʵʱʱ��
		ui.V_Interrupt_Time->setText(tempStr.setNum(A_VoltageinterruptionDurationTime));
		//���һ�ε�ѹ�жϳ���ʱ��
		ui.V_Interrupt_LasttTime->setText(tempStr.setNum(A_VoltageinterruptionDurationLastTime));
	}
	//B��ѹ�轵
//if ((B_VoltagedipDurationLastTime > 20)||(B_VoltagedipDurationLastTime==0))
	{
		ui.B_V_Dip_StartTime->setText(B_dip);
		ui.B_V_Dip_Flag->setText(tempStr.setNum(B_voltagedipstartflag));
		//��ѹ�轵���
		sprintf(tmp, "%.6f", B_VoltagedipDepth);
		ui.B_V_Dip_Depth->setText(tmp);
		//���һ���轵���
		sprintf(tmp, "%.6f", B_VoltagedipLastVoltageResult);
		ui.B_Last_V_Dip_Depth->setText(tmp);
		//��ѹ�轵ʵʱʱ��
		ui.B_V_Dip_Time->setText(tempStr.setNum(B_VoltagedipDurationTime));
		//���һ�ε�ѹ�轵ʱ��
		ui.B_V_Dip_LastTime->setText(tempStr.setNum(B_VoltagedipDurationLastTime));
	}
	//B��ѹ����
	//if ((B_VoltageswellDurationLastTime > 20) || (B_VoltageswellDurationLastTime == 0))
	{
		ui.B_V_Swell_StartTime->setText(B_swell);
		ui.V_Swell_Flag->setText(tempStr.setNum(B_voltageswellstartflag));
		//��ѹ������ѹ
		sprintf(tmp, "%.6f", B_VoltageswellVoltageResult);
		ui.B_V_Swell->setText(tmp);
		//���һ�ε�ѹ������ѹ
		sprintf(tmp, "%.6f", B_VoltageswellLastVoltageResult);
		ui.B_Last_V_Swell->setText(tmp);
		//��ѹ����ʵʱʱ��
		ui.B_V_Swell_Time->setText(tempStr.setNum(B_VoltageswellDurationTime));
		//���һ�ε�ѹ��������ʱ��
		ui.B_V_Swell_LastTime->setText(tempStr.setNum(B_VoltageswellDurationLastTime));
	}
	//B��ѹ�ж�
	//if ((B_VoltageinterruptionDurationLastTime > 20) || (B_VoltageinterruptionDurationLastTime == 0))
	{
		ui.B_V_Interrupt_StartTime->setText(B_interrupt);
		ui.B_V_Interrupt_Flag->setText(tempStr.setNum(B_voltageinterruptstartflag));
		//��ѹ�жϲ����ѹ
		sprintf(tmp, "%.6f", B_VoltageinterruptVoltageResult);
		ui.B_V_Interrupt->setText(tmp);
		//���һ�ε�ѹ�жϲ����ѹ
		sprintf(tmp, "%.6f", B_VoltageinterruptLastVoltageResult);
		ui.B_Last_V_Interrupt->setText(tmp);
		//��ѹ�ж�ʵʱʱ��
		ui.B_V_Interrupt_Time->setText(tempStr.setNum(B_VoltageinterruptionDurationTime));
		//���һ�ε�ѹ�жϳ���ʱ��
		ui.B_V_Interrupt_LasttTime->setText(tempStr.setNum(B_VoltageinterruptionDurationLastTime));
	}
	//C��ѹ�轵
//if ((C_VoltagedipDurationLastTime > 20)||(C_VoltagedipDurationLastTime==0))
	{
		ui.C_V_Dip_StartTime->setText(C_dip);
		ui.C_V_Dip_Flag->setText(tempStr.setNum(C_voltagedipstartflag));
		//��ѹ�轵���
		sprintf(tmp, "%.6f", C_VoltagedipDepth);
		ui.C_V_Dip_Depth->setText(tmp);
		//���һ���轵���
		sprintf(tmp, "%.6f", C_VoltagedipLastVoltageResult);
		ui.C_Last_V_Dip_Depth->setText(tmp);
		//��ѹ�轵ʵʱʱ��
		ui.C_V_Dip_Time->setText(tempStr.setNum(C_VoltagedipDurationTime));
		//���һ�ε�ѹ�轵ʱ��
		ui.C_V_Dip_LastTime->setText(tempStr.setNum(C_VoltagedipDurationLastTime));
	}
	//C��ѹ����
	//if ((C_VoltageswellDurationLastTime > 20) || (C_VoltageswellDurationLastTime == 0))
	{
		ui.C_V_Swell_StartTime->setText(C_swell);
		ui.C_V_Swell_Flag->setText(tempStr.setNum(C_voltageswellstartflag));
		//��ѹ������ѹ
		sprintf(tmp, "%.6f", C_VoltageswellVoltageResult);
		ui.C_V_Swell->setText(tmp);
		//���һ�ε�ѹ������ѹ
		sprintf(tmp, "%.6f", C_VoltageswellLastVoltageResult);
		ui.C_Last_V_Swell->setText(tmp);
		//��ѹ����ʵʱʱ��
		ui.C_V_Swell_Time->setText(tempStr.setNum(C_VoltageswellDurationTime));
		//���һ�ε�ѹ��������ʱ��
		ui.C_V_Swell_LastTime->setText(tempStr.setNum(C_VoltageswellDurationLastTime));
	}
	//C��ѹ�ж�
	//if ((C_VoltageinterruptionDurationLastTime > 20) || (C_VoltageinterruptionDurationLastTime == 0))
	{
		ui.C_V_Interrupt_StartTime->setText(C_interrupt);
		ui.C_V_Interrupt_Flag->setText(tempStr.setNum(C_voltageinterruptstartflag));
		//��ѹ�жϲ����ѹ
		sprintf(tmp, "%.6f", C_VoltageinterruptVoltageResult);
		ui.C_V_Interrupt->setText(tmp);
		//���һ�ε�ѹ�жϲ����ѹ
		sprintf(tmp, "%.6f", C_VoltageinterruptLastVoltageResult);
		ui.C_Last_V_Interrupt->setText(tmp);
		//��ѹ�ж�ʵʱʱ��
		ui.C_V_Interrupt_Time->setText(tempStr.setNum(C_VoltageinterruptionDurationTime));
		//���һ�ε�ѹ�жϳ���ʱ��
		ui.C_V_Interrupt_LasttTime->setText(tempStr.setNum(C_VoltageinterruptionDurationLastTime));
	}
	//�����ѹ��ƽ���
	sprintf(tmp, "%.9f", uneg_param1);
	ui.uneg_param1->setText(tmp);
	sprintf(tmp, "%.9f", uneg_param2);
	ui.uneg_param2->setText(tmp);
	sprintf(tmp, "%.5f%%", uneg * 100);
	ui.sanxiangdianyabupinghengdu->setText(tmp);
	//A����
	{
		//���Ʋ�Ƶ��
		sprintf(tmp, "%.1f", A_tiaozhibo_f);
		ui.A_tiaozhibo_f->setText(tmp);
		//���Ʋ�ϵ��
		sprintf(tmp, "%.5f%%", A_V_fluctuation * 100);
		ui.tiaozhibo_xishu->setText(tmp);
		//��ʱ����������
		ui.shanbianjisuancishu->setText(tempStr.setNum(A_shanbianCount));
		//˲ʱ����ֵ
		sprintf(tmp, "%.7f", A_InstantaneousFlickerValue);
		ui.shunshishanbianzhi->setText(tmp);
		//��ʱ����ֵ
		sprintf(tmp, "%.7f", A_ShorttimeFlickerValue);
		ui.duanshishanbianzhi->setText(tmp);
		//��ʱ����ֵ
		sprintf(tmp, "%.7f", A_LongtimeFlickerValue);
		ui.changshishanbianzhi->setText(tmp);
	}
	//B����
	{
		//���Ʋ�Ƶ��
		sprintf(tmp, "%.1f", B_tiaozhibo_f);
		ui.B_tiaozhibo_f->setText(tmp);
		//���Ʋ�ϵ��
		sprintf(tmp, "%.5f%%", B_V_fluctuation * 100);
		ui.B_tiaozhibo_xishu->setText(tmp);
		//��ʱ����������
		ui.B_shanbianjisuancishu->setText(tempStr.setNum(B_shanbianCount));
		//˲ʱ����ֵ
		sprintf(tmp, "%.7f", B_InstantaneousFlickerValue);
		ui.B_shunshishanbianzhi->setText(tmp);
		//��ʱ����ֵ
		sprintf(tmp, "%.7f", B_ShorttimeFlickerValue);
		ui.B_duanshishanbianzhi->setText(tmp);
		//��ʱ����ֵ
		sprintf(tmp, "%.7f", B_LongtimeFlickerValue);
		ui.B_changshishanbianzhi->setText(tmp);
	}
	//C����
	{
		//���Ʋ�Ƶ��
		sprintf(tmp, "%.1f", C_tiaozhibo_f);
		ui.C_tiaozhibo_f->setText(tmp);
		//���Ʋ�ϵ��
		sprintf(tmp, "%.5f%%", C_V_fluctuation * 100);
		ui.C_tiaozhibo_xishu->setText(tmp);
		//��ʱ����������
		ui.C_shanbianjisuancishu->setText(tempStr.setNum(C_shanbianCount));
		//˲ʱ����ֵ
		sprintf(tmp, "%.7f", C_InstantaneousFlickerValue);
		ui.C_shunshishanbianzhi->setText(tmp);
		//��ʱ����ֵ
		sprintf(tmp, "%.7f", C_ShorttimeFlickerValue);
		ui.C_duanshishanbianzhi->setText(tmp);
		//��ʱ����ֵ
		sprintf(tmp, "%.7f", C_LongtimeFlickerValue);
		ui.C_changshishanbianzhi->setText(tmp);
	}

	ui.message->setText("Qt <br/> ������������");
	ui.A_receivecount->setText(tempStr.setNum(A_packet_number));
	ui.B_receivecount->setText(tempStr.setNum(B_packet_number));
	ui.C_receivecount->setText(tempStr.setNum(C_packet_number));
	//�������
	{
		ui.A_loss->setText(tempStr.setNum(A_err_flag));
		ui.A_loss_current->setText(tempStr.setNum(A_err_current));
		ui.A_loss_sum->setText(tempStr.setNum(A_err_sum));
		ui.B_loss->setText(tempStr.setNum(B_err_flag));
		ui.B_loss_current->setText(tempStr.setNum(B_err_current));
		ui.B_loss_sum->setText(tempStr.setNum(B_err_sum));
		ui.C_loss->setText(tempStr.setNum(C_err_flag));
		ui.C_loss_current->setText(tempStr.setNum(C_err_current));
		ui.C_loss_sum->setText(tempStr.setNum(C_err_sum));
	}
	//����һ���ڵ���
	ui.point_count->setText(tempStr.setNum(pointfre));
	ui.FFT_Number->setText(tempStr.setNum(A_FFT_Number));

	A_packet_number = 0;
	B_packet_number = 0;
	C_packet_number = 0;
}
void PQ::shuaxin(void)
{
	memset(A_dip, 0, sizeof(A_dip));
	A_VoltagedipLastVoltageResult = 0;
	A_VoltagedipDurationLastTime = 0;
	memset(A_swell, 0, sizeof(A_swell));
	A_VoltageswellLastVoltageResult = 0;
	A_VoltageswellDurationLastTime = 0;
	memset(A_interrupt, 0, sizeof(A_interrupt));
	A_VoltageinterruptLastVoltageResult = 0;
	A_VoltageinterruptionDurationLastTime = 0;

	memset(B_dip, 0, sizeof(B_dip));
	B_VoltagedipLastVoltageResult = 0;
	B_VoltagedipDurationLastTime = 0;
	memset(B_swell, 0, sizeof(B_swell));
	B_VoltageswellLastVoltageResult = 0;
	B_VoltageswellDurationLastTime = 0;
	memset(B_interrupt, 0, sizeof(B_interrupt));
	B_VoltageinterruptLastVoltageResult = 0;
	B_VoltageinterruptionDurationLastTime = 0;

	memset(C_dip, 0, sizeof(C_dip));
	C_VoltagedipLastVoltageResult = 0;
	C_VoltagedipDurationLastTime = 0;
	memset(C_swell, 0, sizeof(C_swell));
	C_VoltageswellLastVoltageResult = 0;
	C_VoltageswellDurationLastTime = 0;
	memset(C_interrupt, 0, sizeof(C_interrupt));
	C_VoltageinterruptLastVoltageResult = 0;
	C_VoltageinterruptionDurationLastTime = 0;

	A_err_flag = 0;
	B_err_flag = 0;
	C_err_flag = 0;
	A_err_current = 0;
	B_err_current = 0;
	C_err_current = 0;
	A_err_sum = 0;
	B_err_sum = 0;
	C_err_sum = 0;
}
extern unsigned int A_instantaneousflickervaluecnt;
extern int A_shanbianCount;
void PQ::A_flicker_shuaxin(void)
{
	A_instantaneousflickervaluecnt = 0;
	A_shanbianCount = 0;
	A_tiaozhibo_f = 0;
	A_V_fluctuation = 0;
	A_InstantaneousFlickerValue = 0;
	A_ShorttimeFlickerValue = 0;
	A_LongtimeFlickerValue = 0;
}
extern unsigned int B_instantaneousflickervaluecnt;
extern int B_shanbianCount;
void PQ::B_flicker_shuaxin(void)
{
	B_instantaneousflickervaluecnt = 0;
	B_shanbianCount = 0;
	B_tiaozhibo_f = 0;
	B_V_fluctuation = 0;
	B_InstantaneousFlickerValue = 0;
	B_ShorttimeFlickerValue = 0;
	B_LongtimeFlickerValue = 0;
}
extern unsigned int C_instantaneousflickervaluecnt;
extern int C_shanbianCount;
void PQ::C_flicker_shuaxin(void)
{
	C_instantaneousflickervaluecnt = 0;
	C_shanbianCount = 0;
	C_tiaozhibo_f = 0;
	C_V_fluctuation = 0;
	C_InstantaneousFlickerValue = 0;
	C_ShorttimeFlickerValue = 0;
	C_LongtimeFlickerValue = 0;
}
void PQ::fuwei(void)
{
	A_fre = 0;
	B_fre = 0;
	C_fre = 0;

	A_result_800half = 0;
	A_result_400half = 0;
	A_FFT = 0;
	B_FFT = 0;
	C_FFT = 0;
	an_buffer_idx_A = 0;
	an_buffer_idx_B = 0;
	an_buffer_idx_C = 0;
	fftw_phase_a_vol[1] = 0;
	fftw_phase_b[1] = 0;
	fftw_phase_c[1] = 0;
	BA_phase_average = 0;
	CA_phase_average = 0;
	A_flag = 0;
	B_flag = 0;
	C_flag = 0;
	A_rms = 0;
	A_cur_rms = 0;
	A_active_power = 0;
	A_reactive_power = 0;
	A_apparent_power = 0;
	THDU = 0;
	fftw_phase_differ[1] = 0;
	fuzhi_a[1] = 0;
	fuzhi_a_cur[1] = 0;
	fuzhi_a[0] = 0;
	B_rms = 0;
	B_cur_rms = 0;
	B_active_power = 0;
	B_reactive_power = 0;
	B_apparent_power = 0;
	B_THDU = 0;
	fftw_phase_differ_b[1] = 0;
	fuzhi_b[1] = 0;
	fuzhi_b_cur[1] = 0;
	fuzhi_b[0] = 0;
	fuzhi_b[1] = 0;
	C_rms = 0;
	C_cur_rms = 0;
	C_active_power = 0;
	C_reactive_power = 0;
	C_apparent_power = 0;
	C_THDU = 0;
	fftw_phase_differ_c[1] = 0;
	fuzhi_c[1] = 0;
	fuzhi_c_cur[1] = 0;
	fuzhi_c[0] = 0;
	fuzhi_c[1] = 0;
	uneg_param1 = 0;
	uneg_param2 = 0;
	uneg = 0;
	A_packet_number = 0;
	B_packet_number = 0;
	C_packet_number = 0;
	pointfre = 0;
	A_FFT_Number = 0;
	C_reactive_power = 0;
	C_reactive_power = 0;
}

void PQ::curve(void)
{
	double m_y =A_rms;
	//int m_y = an_buffer_idx_A;
	count++;
	if (count > max_count)
	{
		data.pop_front();
		data << m_y;
		series->clear();
		//series2->clear();
		for (double i = 0; i < x_max; i+=0.1)
		{
			series->append(i, data.at(i));
			//series2->append(i, data.at(i) + 1);
		}
	}
	else
	{
		data << m_y;
		series->clear();
		series2->clear();
		for (int i = 0; i < count; i++)
		{
			series->append(i, data.at(i));
			//series2->append(i, data.at(i) + 1);
		}
	}


}