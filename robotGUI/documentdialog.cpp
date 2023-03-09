#include "documentdialog.h"
#include "ui_documentdialog.h"

DocumentDialog::DocumentDialog(QWidget *parent) :QDialog(parent),ui(new Ui::DocumentDialog)
{
    ui->setupUi(this);
    ui->documentation->setStyleSheet("background-color: silver; "
                                     "border-style:outset; "
                                     "border-radius: 10px;"
                                     "border-color:black;"
                                     "border-width:4px;"
                                     "min-width: 10em;"
                                     "padding: 5px;"
                                     "image:url(:/resource/Baterka/battery0.png)"
                                     );
    this->setFixedSize(this->width(),this->height());
}

DocumentDialog::~DocumentDialog()
{
    delete ui;
}

void DocumentDialog::on_pageDown_clicked()
{

}

void DocumentDialog::on_pageUp_clicked()
{

}


void DocumentDialog::on_pageDown_pressed()
{
    ui->pageDown->setStyleSheet("background-color: rgb(91, 91, 91);border-style:outset;border-radius: 10px;border-color:#ffffff;border-width:4px;padding: 5px;font: 700 12pt Segoe UI;color:#ffffff");
}


void DocumentDialog::on_pageUp_pressed()
{
    ui->pageUp->setStyleSheet("background-color: rgb(91, 91, 91);border-style:outset;border-radius: 10px;border-color:#ffffff;border-width:4px;padding: 5px;font: 700 12pt Segoe UI;color:#ffffff");
}


void DocumentDialog::on_pageDown_released()
{
    ui->pageDown->setStyleSheet("background-color: silver;border-style:outset;border-radius: 10px;border-color:black;border-width:4px;padding: 5px;font: 700 12pt Segoe UI;color:black");
}


void DocumentDialog::on_pageUp_released()
{
    ui->pageUp->setStyleSheet("background-color: silver;border-style:outset;border-radius: 10px;border-color:black;border-width:4px;padding: 5px;font: 700 12pt Segoe UI;color:black");
}

