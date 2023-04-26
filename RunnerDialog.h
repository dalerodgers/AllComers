#ifndef RUNNERDIALOG_H
#define RUNNERDIALOG_H

#include <QDialog>

namespace Ui {
class RunnerDialog;
}

class RunnerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RunnerDialog(QWidget *parent = nullptr);
    ~RunnerDialog();

private:
    Ui::RunnerDialog *ui;
};

#endif // RUNNERDIALOG_H
