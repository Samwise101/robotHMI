#include "alarmdialog.h"
#include "ui_alarmdialog.h"

AlarmDialog::AlarmDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AlarmDialog)
{
    ui->setupUi(this);

    QVector<double> x(101), y(101);
    x[0] = y[0] = 1;
    ui->plot->addGraph();
    ui->plot->graph(0)->addData(1,1);
    // give the axes some labels:
    ui->plot->xAxis->setLabel("x");
    ui->plot->yAxis->setLabel("y");
    // set axes ranges, so we see all data:
    ui->plot->xAxis->setRange(-1, 1);
    ui->plot->yAxis->setRange(0, 1);
    ui->plot->replot();
}

AlarmDialog::~AlarmDialog()
{
    delete ui;
}
