#pragma once

#include <QtMqtt/QMqttClient>

class Mqtt : public QMqttClient
{
    Q_OBJECT

public:
    explicit Mqtt(QObject *parent = nullptr);
    ~Mqtt();
    void connectToHost(const QString &hostname, const uint16_t port, const QString &username, const QString &password);

public slots:

private:
};
