#include "mainwindow.hpp"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    connect(&chartTimer, &QTimer::timeout, this, &MainWindow::readData);

    QChartView *chartView = new QChartView(this->chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    ui->chartLayout->addWidget(chartView);

    this->chartTimer.start(100);


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::readData() {
    chart->addPoint(ui->SetpointSlider->value(), 0, 0);
}

