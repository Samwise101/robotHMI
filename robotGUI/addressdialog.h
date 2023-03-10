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
    explicit AddressDialog(QWidget *parent = nullptr);
    ~AddressDialog();
    bool isAdressFieldEmpty();
    QString getAdressFieldIP();

private slots:
    void on_robotAddressField_returnPressed();

    void on_pushButton_clicked();

private:
    Ui::AddressDialog *ui;
    QString ipAddress = "127.0.0.1";
};

#endif // ADDRESSDIALOG_H
