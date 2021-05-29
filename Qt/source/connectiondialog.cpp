#include "connectiondialog.hpp"
#include "ui_connectiondialog.h"


/**
 * A constructor.
 * @param[in,out] parent QWidget type parent.
 */
ConnectionDialog::ConnectionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectionDialog)
{
    ui->setupUi(this);
    // ui->lineEditTcpPort->setInputMask("99000");
    // ui->lineEditUdpPort->setInputMask("99000");
    // ui->lineEditAdress->setInputMask("900.900.900.900");
}


/**
 * A destructor.
 */
ConnectionDialog::~ConnectionDialog()
{
    delete ui;
}


/**
 * Get adress for connection.
 * @return IP address of the server.
 */
QString ConnectionDialog::getAdress() {
    return ui->addressLineEdit->text();
}


/**
 * Get port
 * @return Server port.
 */
int ConnectionDialog::getPort() {
    return ui->portLineEdit->text().toInt();
}

/**
 * Get username for connection.
 * @return Username.
 */
QString ConnectionDialog::getUsername() {
    return ui->usernameLineEdit->text();
}

/**
 * Get password for connection.
 * @return Password.
 */
QString ConnectionDialog::getPassword() {
    return ui->passwordLineEdit->text();
}