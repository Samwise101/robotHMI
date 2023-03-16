#ifndef ADDRESSDIALOG_H
#define ADDRESSDIALOG_H

#include <QDialog>

namespace Ui {
class AddressDialog;
}

class AddressDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddressDialog(std::string* str, QWidget *parent = nullptr);
    ~AddressDialog();
    bool isAdressFieldEmpty();
    QString getAdressFieldIP();
    void setFunction(std::function<void()> func);

private slots:
    void on_robotAddressField_returnPressed();

    void on_pushButton_clicked();


    void on_simulatorButton_clicked();

private:
    Ui::AddressDialog *ui;
    std::string* ipAddress;
    std::function<void()> func;
};

#endif // ADDRESSDIALOG_H
