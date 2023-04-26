#include "RunnerDialog.h"
#include "ui_RunnerDialog.h"

RunnerDialog::RunnerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RunnerDialog)
{
    ui->setupUi(this);

    ui->spinBox_hours->setRange( 0, 2 );
    ui->spinBox_hours->setValue( 0 );

    ui->spinBox_mins->setRange( 0, 59 );
    ui->spinBox_mins->setValue( 30 );

    ui->spinBox_secs->setRange( 0, 59 );
    ui->spinBox_secs->setValue( 0 );
}

RunnerDialog::~RunnerDialog()
{
    delete ui;
}
