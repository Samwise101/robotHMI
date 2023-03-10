#include "addressdialog.h"
#include "ui_addressdialog.h"

AddressDialog::AddressDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddressDialog)
{
    QString ipAddress = "127.0.0.1";
    ui->setupUi(this);
    ui->robotAddressField->setMaxLength(20);
    this->setFixedSize(this->width(),this->height());
}

AddressDialog::~AddressDialog()
{
    delete ui;
}

bool AddressDialog::isAdressFieldEmpty()
{
    if(ui->robotAddressField->text().isEmpty()){
        return true;
    }
    else{
        return false;
    }
}

QString AddressDialog::getAdressFieldIP()
{
    return ipAddress;
}

void AddressDialog::on_robotAddressField_returnPressed()
{
    ipAddress = ui->robotAddressField->text();
    this->close();
}


void AddressDialog::on_pushButton_clicked()
{
    ipAddress = ui->robotAddressField->text();
    this->close();
}

