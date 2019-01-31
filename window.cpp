#include <QtWidgets>

#include "window.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QValueAxis>

QT_CHARTS_USE_NAMESPACE

Window::Window(){}

Window::Window(QPointF max)
{
    QList<QPointF> mapList;
    QList<QPointF> timeList;
    this->grid = new QGridLayout;

    grid->addWidget(createTimeChartGroup(timeList), 0, 0);
    grid->addWidget(createMapChartGroup(mapList, max), 0, 1);
    setLayout(grid);

    setWindowTitle(tr("Spot your mate"));
    resize(1000, 600);
}

void Window::setWidget(QString string, QList<QPointF> list, QPointF max){
    if(string.compare("time")==0) {
        //grid->removeWidget();
        grid->addWidget(createTimeChartGroup(list), 0, 0);
    }

    if(string.compare("map")==0)
        grid->addWidget(createMapChartGroup(list, max), 0, 1);
    setLayout(grid);
}

QGridLayout *Window::getGrid(){
    return this->grid;
}

QGroupBox *Window::createTimeChartGroup(QList<QPointF> points)
{
    QGroupBox *groupBox = new QGroupBox(tr("Temporal chart"));

    QLineSeries *series = new QLineSeries();

    for(QList<QPointF>::iterator i=points.begin(); i!=points.end(); i++)
        *series << *i;

    QChart *chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->setTitle("Number of people in area");

    QValueAxis *axisX = new QValueAxis;
    axisX->setRange(0, 60);
    axisX->setTickCount(13);
    chart->setAxisX(axisX, series);


    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(chartView);
    groupBox->setLayout(layout);

    return groupBox;
}

QGroupBox *Window::createMapChartGroup(QList<QPointF> points, QPointF max)
{
    QGroupBox *groupBox = new QGroupBox(tr("Map"));

    QScatterSeries *series = new QScatterSeries();

    for(QList<QPointF>::iterator i=points.begin(); i!=points.end(); i++)
        *series << *i;

    QChart *chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->axisX()->setRange(0.0, max);
    chart->axisY()->setRange(0.0, max);
    chart->setTitle("People in the area");

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(chartView);
    groupBox->setLayout(layout);

    return groupBox;
}

