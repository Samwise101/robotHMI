#include "documentdialog.h"
#include "ui_documentdialog.h"

DocumentDialog::DocumentDialog(QWidget *parent) :QDialog(parent),ui(new Ui::DocumentDialog)
{
    docLength = 11;
    currentPage = 1;

    ui->setupUi(this);
    ui->documentation->setStyleSheet("background-color: white; "
                                     "border-style:outset; "
                                     "border-radius: 10px;"
                                     "border-color:black;"
                                     "border-width:4px;"
                                     "min-width: 10em;"
                                     "padding: 5px;"
                                     "image:url(:/pages/page1.png)"
                                     );
    this->setFixedSize(this->width(),this->height());

    ui->pages->setText(QString::number(currentPage) + "/" + QString::number(docLength));
}

DocumentDialog::~DocumentDialog()
{
    delete ui;
}

void DocumentDialog::on_pageDown_clicked()
{
    currentPage--;

    if(currentPage < 1){
        currentPage = docLength;
    }

    ui->pages->setText(QString::number(currentPage) + "/" + QString::number(docLength));

    ui->documentation->setStyleSheet("background-color: white; "
                                     "border-style:outset; "
                                     "border-radius: 10px;"
                                     "border-color:black;"
                                     "border-width:4px;"
                                     "min-width: 10em;"
                                     "padding: 5px;"
                                     "image:url(:/pages/page" + QString::number(currentPage) + ".png)"
                                     );
}

void DocumentDialog::on_pageUp_clicked()
{
    currentPage++;

    if(currentPage > docLength){
        currentPage = 1;
    }

    ui->pages->setText(QString::number(currentPage) + "/" + QString::number(docLength));

    ui->documentation->setStyleSheet("background-color: white; "
                                     "border-style:outset; "
                                     "border-radius: 10px;"
                                     "border-color:black;"
                                     "border-width:4px;"
                                     "min-width: 10em;"
                                     "padding: 5px;"
                                     "image:url(:/pages/page" + QString::number(currentPage) + ".png)"
                                     );
}


void DocumentDialog::on_pageDown_pressed()
{
    ui->pageDown->setStyleSheet("background-color: rgb(91, 91, 91);border-style:outset;border-radius: 10px;border-color:white;border-width:4px;padding: 5px;font: 700 12pt Segoe UI;color:#fffff;image:url(:/pages/pageDOWN.png);");
}


void DocumentDialog::on_pageUp_pressed()
{
    ui->pageUp->setStyleSheet("background-color: rgb(91, 91, 91);border-style:outset;border-radius: 10px;border-color:white;border-width:4px;padding: 5px;font: 700 12pt Segoe UI;color:#ffffff;image:url(:/pages/pageUP.png);");
}


void DocumentDialog::on_pageDown_released()
{
    ui->pageDown->setStyleSheet("background-color: silver;border-style:outset;border-radius: 10px;border-color:black;border-width:4px;padding: 5px;font: 700 12pt Segoe UI;color:black;image:url(:/pages/pageDOWN.png);");
}


void DocumentDialog::on_pageUp_released()
{
    ui->pageUp->setStyleSheet("background-color: silver;border-style:outset;border-radius: 10px;border-color:black;border-width:4px;padding: 5px;font: 700 12pt Segoe UI;color:black;image:url(:/pages/pageUP.png);");
}
