#include "addressdialog.h"
#include "ui_addressdialog.h"

AddressDialog::AddressDialog(std::string* str, QWidget *parent) :QDialog(parent), ui(new Ui::AddressDialog){
    ipAddress = str;
    ui->setupUi(this);
    ui->robotAddressField->setMaxLength(20);
    this->setFixedSize(this->width(),this->height());
    this->setAttribute(Qt::WA_DeleteOnClose);
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

void AddressDialog::setFunction(std::function<void()> func)
{
    this->func = func;
}

void AddressDialog::on_robotAddressField_returnPressed()
{
    *ipAddress = ui->robotAddressField->text().toStdString();
    func();
    this->close();
}


void AddressDialog::on_pushButton_clicked()
{
    *ipAddress = ui->robotAddressField->text().toStdString();
    func();
    this->close();
}

