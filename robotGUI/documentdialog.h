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

    void on_pageLast_clicked();

    void on_pageLast_pressed();

    void on_pageLast_released();

    void on_pageFirst_clicked();

    void on_pageFirst_pressed();

    void on_pageFirst_released();

private:
    Ui::DocumentDialog *ui;
    int docLength;
    int currentPage = 1;
    std::string path;
};

#endif // DOCUMENTDIALOG_H
