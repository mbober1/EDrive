#pragma once
#include <QMainWindow>
#include "chart.hpp"
#include "mqtt.hpp"
#include "secrets.hpp"
#include "connectiondialog.hpp"
#include "engine.hpp"
#include "./ui_mainwindow.h"

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
    void enableUi(bool state);
    void changeConnectionStatus(QMqttClient::ClientState state);
    void connectionError(QMqttClient::ClientError error);

private:
    Ui::MainWindow* ui;
    Chart* chart;
    Mqtt* mqtt;
    QTimer chartTimer;
    Engine* engine;

    void drawData();
    void subscribe();
    void unsubscribe();
    void createTopics();

    void setSetpoint(int value);
    void setVoltage(float value);
    void setValue(int value);
    void setPwmDuty(int value);
    void setTorque(float value);
    void stop();
};