#include "mqtt.hpp"


Mqtt::Mqtt(QObject *parent) :
QMqttClient(parent)
{
    setAutoKeepAlive(true);
}

Mqtt::~Mqtt() {}


void Mqtt::connectToHost(const QString &hostname, const uint16_t port, const QString &username, const QString &password) {
    this->setHostname(hostname);
    this->setPort(port);
    this->setUsername(username);
    this->setPassword(password);
    this->QMqttClient::connectToHost();
}
