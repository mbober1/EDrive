#include "mainwindow.hpp"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : 
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    chart(new Chart(100)) // 100 points
{
    ui->setupUi(this);

    connect(&chartTimer, &QTimer::timeout, this, &MainWindow::readData);

    // setup chart
    QChartView *chartView = new QChartView(this->chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    ui->chartLayout->addWidget(chartView);

    this->chartTimer.start(10); // 10ms delay = 100Hz
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::readData() {
    int values[3] = {ui->SetpointSlider->value(), 0, ui->SetpointSlider->value() - 0};
    chart->addPoint(values);
}

