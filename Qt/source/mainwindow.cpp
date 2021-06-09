#include "mainwindow.hpp"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : 
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    chart(new Chart(100)), // 100 points
    mqtt(new Mqtt(this)),
    engine(new Engine)
{
    ui->setupUi(this);
    this->enableUi(false);

    connect(ui->actionConnect, &QAction::triggered, this, &MainWindow::actionConnect);
    connect(ui->actionDisconnect, &QAction::triggered, this, &MainWindow::actionDisconnect);
    connect(&chartTimer, &QTimer::timeout, this, &MainWindow::readData);
    connect(mqtt, &QMqttClient::stateChanged, this, &MainWindow::changeConnectionStatus);
    connect(mqtt, &QMqttClient::errorChanged, this, &MainWindow::connectionError);
    connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::close);

    // setup chart
    QChartView *chartView = new QChartView(this->chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    ui->chartLayout->addWidget(chartView);
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::readData() {
    int values[3] = {ui->SetpointSlider->value(), engine->getValue(), ui->SetpointSlider->value() - engine->getValue()};
    chart->addPoint(values);
}


void MainWindow::actionConnect() {
    ConnectionDialog dialog;
    dialog.setModal(true);
    if(dialog.exec() == QDialog::Accepted) {
        mqtt->connectToHost(mqttHostname, mqttPort, mqttUsername, mqttPassword);
        // mqtt->connectToHost(dialog.getAdress(), dialog.getPort(), dialog.getUsername(), dialog.getPassword());
    }
}


void MainWindow::actionDisconnect() { 
    mqtt->disconnectFromHost();
}


void MainWindow::enableUi(bool state) {
    ui->centralwidget->setEnabled(state);
    if(state) this->chartTimer.start(10); // 10ms delay = 100Hz
    else this->chartTimer.stop();
}


void MainWindow::changeConnectionStatus(QMqttClient::ClientState state) {
    switch (state)
    {
    case QMqttClient::Connected: {
        enableUi(true);
        ui->actionConnect->setVisible(false);
        ui->actionDisconnect->setVisible(true);
        ui->actionDisconnect->setEnabled(true);
        this->createTopics();
        this->subscribe();
        break;
    }

    case QMqttClient::Connecting:
    case QMqttClient::Disconnected:
        enableUi(false);
        ui->actionConnect->setVisible(true);
        ui->actionConnect->setEnabled(true);
        ui->actionDisconnect->setVisible(false);
        this->unsubscribe();
        break;
    }
}


void MainWindow::connectionError(QMqttClient::ClientError error) {
    QString mess("Unable connect to the server! ");

    switch (error)
    {
    case QMqttClient::InvalidProtocolVersion:
        mess += "The broker does not accept a connection using the specified protocol version.";
        break;
    
    case QMqttClient::IdRejected:
        mess += "The client ID is malformed. This might be related to its length.";
        break;
    
    case QMqttClient::ServerUnavailable:
        mess += "The network connection has been established, but the service is unavailable on the broker side.";
        break;
    
    case QMqttClient::BadUsernameOrPassword:
        mess += "The data in the username or password is malformed.";
        break;
    
    case QMqttClient::NotAuthorized:
        mess += "The client is not authorized to connect.";
        break;
    
    case QMqttClient::TransportInvalid:
        mess += "The underlying transport caused an error. For example, the connection might have been interrupted unexpectedly.";
        break;
    
    case QMqttClient::ProtocolViolation:
        mess += "The client encountered a protocol violation, and therefore closed the connection.";
        break;
    
    case QMqttClient::UnknownError:
        mess += "An unknown error occurred.";
        break;
    
    case QMqttClient::Mqtt5SpecificError:
        mess += "The error is related to MQTT protocol level 5. A reason code might provide more details.";
        break;
    
    default:
        return;
    }

    QMessageBox::critical(this, "Connection error", mess);
}


void MainWindow::subscribe() {
    auto valueSubscription = mqtt->subscribe(QMqttTopicFilter("edrive/value"), 1);
    connect(valueSubscription, &QMqttSubscription::messageReceived, [this](QMqttMessage msg) {
        this->setValue(msg.payload().toInt());
    });

    auto voltageSubscription = mqtt->subscribe(QMqttTopicFilter("edrive/voltage"), 1);
    connect(voltageSubscription, &QMqttSubscription::messageReceived, [this](QMqttMessage msg) {
        this->setVoltage(msg.payload().toInt());
    });

    connect(ui->SetpointSlider, &QAbstractSlider::valueChanged, [this](int value) {
        QByteArray array;
        array.setNum(value);
        mqtt->publish(QMqttTopicName("edrive/setpoint"), array);
    });

    connect(ui->KpSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [this](int i) {
        QByteArray array;
        array.setNum(i);
        mqtt->publish(QMqttTopicName("edrive/kp"), array);
    });

    connect(ui->KiSpinbox, QOverload<int>::of(&QSpinBox::valueChanged), [this](int i) {
        QByteArray array;
        array.setNum(i);
        mqtt->publish(QMqttTopicName("edrive/ki"), array);
    });

    connect(ui->KdSpinbox, QOverload<int>::of(&QSpinBox::valueChanged), [this](int i) {
        QByteArray array;
        array.setNum(i);
        mqtt->publish(QMqttTopicName("edrive/kd"), array);
    });

    mqtt->publish(QMqttTopicName("edrive/kp"), "0");
    mqtt->publish(QMqttTopicName("edrive/ki"), "0");
    mqtt->publish(QMqttTopicName("edrive/kd"), "0");
    mqtt->publish(QMqttTopicName("edrive/setpoint"), "0");
}




void MainWindow::unsubscribe() {}


void MainWindow::setValue(int value) {
    engine->setValue(value);
    ui->valueLabelValue->setText(QString::number(value));
}

void MainWindow::setSetpoint(int value) {
    ui->setpointLabelValue->setText(QString::number(value));
}

void MainWindow::setVoltage(int value) {
    QString text(QString::number(value));
    text += " mV";
    engine->setVoltage(value);
    ui->voltageLabelValue->setText(text);
}

void MainWindow::createTopics() {
    mqtt->publish(QMqttTopicName("edrive/setpoint"), "0");
    mqtt->publish(QMqttTopicName("edrive/value"), "0");
    mqtt->publish(QMqttTopicName("edrive/voltage"), "0");
    mqtt->publish(QMqttTopicName("edrive/kp"), "0");
    mqtt->publish(QMqttTopicName("edrive/ki"), "0");
    mqtt->publish(QMqttTopicName("edrive/kd"), "0");
}