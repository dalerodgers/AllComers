#include "RunnerDialog.h"
#include "ui_RunnerDialog.h"

#include "Runner.h"
#include "Runners.h"

///////////////////////////////////////////////////////////////////////////////

RunnerDialog::RunnerDialog(Runners* runners, Runner* runner, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RunnerDialog),
    runners_( runners ),
    runner_( runner )
{
    ui->setupUi(this);
    setRanges();

    if( nullptr != runners_ )
    {
        ui->button_DNS->hide();
        ui->lineEdit_name->setEnabled( true );
    }
    else if( nullptr != runner_ )
    {
        ui->button_DNS->show();
        ui->lineEdit_name->setEnabled( false );
        ui->lineEdit_name->setText( runner_->name() );

        const int totalSecs = runner_->msPredicted() / 1000;
        const int hours = totalSecs / 3600;
        const int mins  = ( totalSecs - (hours * 3600) ) / 60;
        const int secs  = totalSecs - (hours * 3600) - (mins*60);

        ui->spinBox_hours->setValue( hours );
        ui->spinBox_mins->setValue( mins );
        ui->spinBox_secs->setValue( secs );

        if( runner_->state() == Runner::State_e::DNS )
        {
            ui->button_DNS->setText( "Mark as Here" );
        }
        else
        {
            ui->button_DNS->setText( "Mark as DNS" );
        }

        connect( ui->button_DNS, SIGNAL(pressed()), this, SLOT(onMarkAsDNS()) );
    }
    else
    {
        ; // do nothing
    }

    connect( ui->button_OK, SIGNAL(pressed()), this, SLOT(onOk()) );
    connect( ui->button_Cancel, SIGNAL(pressed()), this, SLOT(onCancel()) );
}

///////////////////////////////////////////////////////////////////////////////

RunnerDialog::~RunnerDialog()
{
    delete ui;
}

///////////////////////////////////////////////////////////////////////////////

void RunnerDialog::setRanges()
{
    ui->spinBox_hours->setRange( 0, 2 );
    ui->spinBox_hours->setValue( 0 );

    ui->spinBox_mins->setRange( 0, 59 );
    ui->spinBox_mins->setValue( 30 );

    ui->spinBox_secs->setRange( 0, 59 );
    ui->spinBox_secs->setValue( 0 );
}

///////////////////////////////////////////////////////////////////////////////

void RunnerDialog::onCancel()
{
    close();
}

///////////////////////////////////////////////////////////////////////////////

void RunnerDialog::onOk()
{
    const int time =  ( ( ui->spinBox_hours->value() * 60 * 60 ) + \
                        ( ui->spinBox_mins->value() * 60 ) + \
                          ui->spinBox_secs->value() ) * 1000;

    if( nullptr != runners_ )
    {
        if( !ui->lineEdit_name->text().isEmpty() )
        {
            runners_->add( ui->lineEdit_name->text(), time );
        }
    }
    else if( nullptr != runner_ )
    {
        runner_->set_msPredicted( time );
    }
    else
    {
        ;   // do nothing
    }

    close();
}

///////////////////////////////////////////////////////////////////////////////

void RunnerDialog::onMarkAsDNS()
{
    if( nullptr != runner_ )
    {
        if( runner_->state() == Runner::State_e::DNS )
        {
            runner_->reset();
        }
        else
        {
            runner_->Stop( -1 );
        }

        close();
    }
}
