#ifndef DOCUMNETDIALOG_H
#define DOCUMNETDIALOG_H

#include <QDialog>

namespace Ui {
class DocumnetDialog;
}

class DocumnetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DocumnetDialog(QWidget *parent = nullptr);
    ~DocumnetDialog();

private:
    Ui::DocumnetDialog *ui;
};

#endif // DOCUMNETDIALOG_H
