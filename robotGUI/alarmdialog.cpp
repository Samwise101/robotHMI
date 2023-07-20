#include "alarmdialog.h"
#include "ui_alarmdialog.h"

/**
 * @brief AlarmDialog::AlarmDialog
 * @param parent
 */
AlarmDialog::AlarmDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AlarmDialog)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
}

AlarmDialog::~AlarmDialog()
{
    delete ui;
}
