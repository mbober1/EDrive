#include "mqtt.hpp"


/**
 * A constructor.
 * @param[in,out] parent QWidget type parent.
 */
Mqtt::Mqtt(QObject *parent) :
QMqttClient(parent)
{
    setAutoKeepAlive(true);
}


/**
 * A deconstructor.
 */
Mqtt::~Mqtt() {}


/**
 * Connect to mqtt server
 * @param[in] hostname Server address
 * @param[in] port Server port
 * @param[in] username Username
 * @param[in] password Psername
 */
void Mqtt::connectToHost(const QString &hostname, const uint16_t port, const QString &username, const QString &password) {
    this->setHostname(hostname);
    this->setPort(port);
    this->setUsername(username);
    this->setPassword(password);
    this->QMqttClient::connectToHost();
}
