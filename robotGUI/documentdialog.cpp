#include "documentdialog.h"
#include "ui_documentdialog.h"

/**
 * @brief DocumentDialog::DocumentDialog
 * @param parent
 */
DocumentDialog::DocumentDialog(QWidget *parent) :QDialog(parent),ui(new Ui::DocumentDialog)
{
    docLength = 15;
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
    this->setAttribute(Qt::WA_DeleteOnClose);

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
    ui->pageDown->setStyleSheet("background-color: rgb(91, 91, 91);border-style:outset;border-radius: 10px;border-color:white;border-width:4px;padding: 5px;font: 700 12pt Segoe UI;color:white;image:url(:/pages/pageDOWN2.png);");
}


void DocumentDialog::on_pageUp_pressed()
{
    ui->pageUp->setStyleSheet("background-color: rgb(91, 91, 91);border-style:outset;border-radius: 10px;border-color:white;border-width:4px;padding: 5px;font: 700 12pt Segoe UI;color:white;image:url(:/pages/pageUP2.png);");
}


void DocumentDialog::on_pageDown_released()
{
    ui->pageDown->setStyleSheet("background-color: silver;border-style:outset;border-radius: 10px;border-color:black;border-width:4px;padding: 5px;font: 700 12pt Segoe UI;color:black;image:url(:/pages/pageDOWN2.png);");
}


void DocumentDialog::on_pageUp_released()
{
    ui->pageUp->setStyleSheet("background-color: silver;border-style:outset;border-radius: 10px;border-color:black;border-width:4px;padding: 5px;font: 700 12pt Segoe UI;color:black;image:url(:/pages/pageUP2.png);");
}

void DocumentDialog::on_pageLast_clicked()
{
    currentPage = docLength;
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


void DocumentDialog::on_pageLast_pressed()
{
    ui->pageLast->setStyleSheet("background-color: rgb(91, 91, 91);border-style:outset;border-radius: 10px;border-color:white;border-width:4px;padding: 5px;font: 700 12pt Segoe UI;color:white;image:url(:/pages/pageLAST.png);");
}


void DocumentDialog::on_pageLast_released()
{
    ui->pageLast->setStyleSheet("background-color: silver;border-style:outset;border-radius: 10px;border-color:black;border-width:4px;padding: 5px;font: 700 12pt Segoe UI;color:black;image:url(:/pages/pageLAST.png);");
}


void DocumentDialog::on_pageFirst_clicked()
{
    currentPage = 1;
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


void DocumentDialog::on_pageFirst_pressed()
{
     ui->pageFirst->setStyleSheet("background-color: rgb(91, 91, 91);border-style:outset;border-radius: 10px;border-color:white;border-width:4px;padding: 5px;font: 700 12pt Segoe UI;color:white;image:url(:/pages/pageFIRST.png);");
}


void DocumentDialog::on_pageFirst_released()
{
    ui->pageFirst->setStyleSheet("background-color: silver;border-style:outset;border-radius: 10px;border-color:black;border-width:4px;padding: 5px;font: 700 12pt Segoe UI;color:black;image:url(:/pages/pageFIRST.png);");
}

