#pragma once

#include <QtWidgets/QWidget>

#include "qchartglobal.h"
#include "chartsnamespace.h"
#include "qchartview.h"
#include "qlineseries.h"
#include "qsplineseries.h"
#include "workthread.h"

#include "ui_pq.h"

#include "qtimer.h"
#include <QStandardItemModel>
#include "DList.h"
class PQ : public QWidget
{
	Q_OBJECT

public:
	PQ(QWidget *parent = Q_NULLPTR);

private:
	Ui::PQClass ui;
	QTimer* t;
	QTimer* t2;
	const int max_count = 10;
	double x_min = 0;
	double x_max = 5.0;
	double y_min = 1.0;
	double y_max = 3.0;
	//QList以链表形式存储一组元素。默认为空链表，我们可以使用<<操作符添加元素：
	QList<double>  data;
	QSplineSeries *series;
	QLineSeries *series2;
	QChart *chart;
	QChartView *chartView;
	QStandardItemModel *model;
	int count = 0;
private slots:
	void update(void);
	void curve(void);
	void shuaxin(void);
	void A_flicker_shuaxin(void);
	void B_flicker_shuaxin(void);
	void C_flicker_shuaxin(void);
	void fuwei(void);
};
