#include "addressdialog.h"
#include "ui_addressdialog.h"

AddressDialog::AddressDialog(std::string* str, QWidget *parent) :QDialog(parent), ui(new Ui::AddressDialog){
    ipAddress = str;
    *ipAddress = "127.0.0.1";
    ui->setupUi(this);
    ui->robotAddressField->setMaxLength(20);
    this->setFixedSize(this->width(),this->height());
   // this->setAttribute(Qt::WA_DeleteOnClose);
}

AddressDialog::~AddressDialog()
{
    delete ui;
}

bool AddressDialog::isAdressFieldEmpty()
{
    if(ipAddress->empty()){
        return true;
    }
    else{
        return false;
    }
}

void AddressDialog::on_robotAddressField_returnPressed()
{
    *ipAddress = ui->robotAddressField->text().toStdString();
    this->close();
}


void AddressDialog::on_pushButton_clicked()
{
    *ipAddress = ui->robotAddressField->text().toStdString();
    this->close();
}

