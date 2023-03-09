#include "documnetdialog.h"
#include "ui_documnetdialog.h"

DocumnetDialog::DocumnetDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DocumnetDialog)
{
    ui->setupUi(this);
}

DocumnetDialog::~DocumnetDialog()
{
    delete ui;
}
