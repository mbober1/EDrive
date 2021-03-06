#include "mainwindow.hpp"


/**
 * A constructor
 * @param[in,out] parent QWidget type parent.
 */
MainWindow::MainWindow(QWidget *parent) : 
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    chart(new Chart(80)), // 80 points
    mqtt(new Mqtt(this)),
    engine(new Engine)
{
    ui->setupUi(this);
    this->enableUi(false);

    connect(ui->actionConnect, &QAction::triggered, this, &MainWindow::actionConnect);
    connect(ui->actionDisconnect, &QAction::triggered, this, &MainWindow::actionDisconnect);
    connect(&chartTimer, &QTimer::timeout, this, &MainWindow::drawData);
    connect(mqtt, &QMqttClient::stateChanged, this, &MainWindow::changeConnectionStatus);
    connect(mqtt, &QMqttClient::errorChanged, this, &MainWindow::connectionError);
    connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
    connect(ui->stopButton, &QPushButton::clicked, this, &MainWindow::stop);

    connect(ui->SetpointCheckBox, &QCheckBox::stateChanged, [this](int state) {
        if(state == Qt::CheckState::Unchecked) this->chart->setSeriesVisible(0, false);
        else if(state == Qt::CheckState::Checked) this->chart->setSeriesVisible(0, true);
    });

    connect(ui->ValueCheckBox, &QCheckBox::stateChanged, [this](int state) {
        if(state == Qt::CheckState::Unchecked) this->chart->setSeriesVisible(1, false);
        else if(state == Qt::CheckState::Checked) this->chart->setSeriesVisible(1, true);
    });

    connect(ui->DutyCheckBox, &QCheckBox::stateChanged, [this](int state) {
        if(state == Qt::CheckState::Unchecked) this->chart->setSeriesVisible(2, false);
        else if(state == Qt::CheckState::Checked) this->chart->setSeriesVisible(2, true);
    });




    // setup chart
    QChartView *chartView = new QChartView(this->chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    ui->chartLayout->addWidget(chartView);
}


/**
 * A destructor
 */
MainWindow::~MainWindow()
{
    delete ui;
}


/**
 * Add data to chart
 */
void MainWindow::drawData() {
    int values[3] = {engine->getSetpoint(), engine->getValue(), engine->getPwmDuty()};
    chart->addPoint(values);
}


/**
 * Open connecting dialog and connect to server
 */
void MainWindow::actionConnect() {
    ConnectionDialog dialog;
    dialog.setModal(true);
    if(dialog.exec() == QDialog::Accepted) {
        mqtt->connectToHost(mqttHostname, mqttPort, mqttUsername, mqttPassword);
        // mqtt->connectToHost(dialog.getAdress(), dialog.getPort(), dialog.getUsername(), dialog.getPassword());
    }
}


/**
 * Disconect from server
 */
void MainWindow::actionDisconnect() {
    this->stop();
    chart->clear();
    mqtt->disconnectFromHost();
}


/**
 * Enable UI
 */
void MainWindow::enableUi(bool state) {
    ui->centralwidget->setEnabled(state);
    if(state) this->chartTimer.start(17); // 17ms delay = 60Hz
    else this->chartTimer.stop();
}


/**
 * Change connection status
 */
void MainWindow::changeConnectionStatus(QMqttClient::ClientState state) {
    switch (state)
    {
    case QMqttClient::Connected: {
        enableUi(true);
        ui->actionConnect->setVisible(false);
        ui->actionDisconnect->setVisible(true);
        ui->actionDisconnect->setEnabled(true);
        // this->createTopics();
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


/**
 * Connection error handler
 */
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


/**
 * Subscribe topics and publish data
 */
void MainWindow::subscribe() {

    // read subscriptions
    auto valueSubscription = mqtt->subscribe(QMqttTopicFilter("edrive/value"), 0);
    connect(valueSubscription, &QMqttSubscription::messageReceived, [this](QMqttMessage msg) {
        this->setValue(msg.payload().toInt());
    });

    auto voltageSubscription = mqtt->subscribe(QMqttTopicFilter("edrive/voltage"), 0);
    connect(voltageSubscription, &QMqttSubscription::messageReceived, [this](QMqttMessage msg) {
        this->setVoltage(msg.payload().toFloat());
    });

    auto pwmDutySubscription = mqtt->subscribe(QMqttTopicFilter("edrive/pwm_duty"), 0);
    connect(pwmDutySubscription, &QMqttSubscription::messageReceived, [this](QMqttMessage msg) {
        this->setPwmDuty(msg.payload().toInt());
    });

    auto currentSubscription = mqtt->subscribe(QMqttTopicFilter("edrive/current"), 0);
    connect(currentSubscription, &QMqttSubscription::messageReceived, [this](QMqttMessage msg) {
        this->setTorque(msg.payload().toFloat());
    });


    // write subscription
    connect(ui->SetpointSlider, &QAbstractSlider::valueChanged, [this](int value) {
        QByteArray array;
        array.setNum(value);
        mqtt->publish(QMqttTopicName("edrive/setpoint"), array);
        this->setSetpoint(value);
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

    this->createTopics();
}



/**
 * Unsubscribe topics
 */
void MainWindow::unsubscribe() {}


/**
 * Set motor revolutions value
 * @param[in] value Value to set
 */
void MainWindow::setValue(int value) {
    engine->setValue(value);
    QString text(QString::number(tickToRPM(value)));
    text += " RPM";
    ui->valueLabelValue->setText(text);
}


/**
 * Set PWM duty
 * @param[in] value PWM duty
 */
void MainWindow::setPwmDuty(int value) {
    engine->setPwmDuty(value);
    QString text(QString::number(value));
    text += "%";
    ui->dutyLabelValue->setText(text);
}


/**
 * Set PID setpoint
 * @param[in] value Setpoint
 */
void MainWindow::setSetpoint(int value) {
    engine->setSetpoint(value);
    QString text(QString::number(tickToRPM(value)));
    text += " RPM";
    ui->setpointLabelValue->setText(text);
}


/**
 * Set motor voltage
 * @param[in] value Motor voltage
 */
void MainWindow::setVoltage(float value) {
    engine->setVoltage((int)value);
    QString text(QString::number(value, 'f', 2));
    text += " V";
    ui->voltageLabelValue->setText(text);
}


/**
 * Set motor torque
 * @param[in] current Motor current
 */
void MainWindow::setTorque(float current) {
    float value = mAToKgCm(current);
    engine->setTorque(value);
    QString text(QString::number(value, 'f', 2));
    text += " Kg*cm";
    // ui->torqueLabelValue->setText(text);
}


/**
 * Set setpoint to zero
 */
void MainWindow::stop() {
    ui->SetpointSlider->setValue(0);
}


/**
 * Initialize topics
 */
void MainWindow::createTopics() {
    ui->SetpointSlider->setValue(0);
    ui->KpSpinBox->setValue(3);
    ui->KiSpinbox->setValue(2);
    ui->KdSpinbox->setValue(1);
}