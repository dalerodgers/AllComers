#ifndef RUNNERDIALOG_H
#define RUNNERDIALOG_H

#include <QDialog>

namespace Ui {
class RunnerDialog;
}

class Runner;
class Runners;

class RunnerDialog : public QDialog
{
    Q_OBJECT

public:
    RunnerDialog(Runners* runners, Runner* runner, QWidget *parent = nullptr);
    ~RunnerDialog();

private:
    Ui::RunnerDialog *ui;
    Runners* runners_;
    Runner* runner_;

    void setRanges();

private slots:
    void onCancel();
    void onOk();
    void onMarkAsDNS();
};

#endif // RUNNERDIALOG_H
