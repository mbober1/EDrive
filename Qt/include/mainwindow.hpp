#pragma once
#include <QMainWindow>
#include "chart.hpp"
#include "mqtt.hpp"
#include "secrets.hpp"
#include "connectiondialog.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void actionConnect();
    void actionDisconnect();

private:
    Ui::MainWindow *ui;
    Chart *chart;
    Mqtt *mqtt;
    QTimer chartTimer;

    void readData();
};