#ifndef DOCUMENTDIALOG_H
#define DOCUMENTDIALOG_H

#include <QDialog>
#include <iostream>

namespace Ui {
class DocumentDialog;
}

class DocumentDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DocumentDialog(QWidget *parent = nullptr);
    ~DocumentDialog();

private slots:
    void on_pageDown_clicked();

    void on_pageDown_pressed();

    void on_pageUp_clicked();

    void on_pageUp_pressed();

    void on_pageDown_released();

    void on_pageUp_released();

private:
    Ui::DocumentDialog *ui;
};

#endif // DOCUMENTDIALOG_H
