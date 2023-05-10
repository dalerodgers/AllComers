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

        ui->comboBox_Hours->setCurrentIndex( hours );
        ui->comboBox_Mins->setCurrentIndex( mins );
        ui->comboBox_Secs->setCurrentIndex( secs );

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
    ui->comboBox_Hours->addItem( "0" );
    ui->comboBox_Hours->addItem( "1" );

    for( int i=0; i<60; i++ )
    {
        QString str = QString( "%1" ).arg( i );

        if( i < 10 )
        {
            str = "0" + str;
        }

        ui->comboBox_Mins->addItem( str );
        ui->comboBox_Secs->addItem( str );
    }

    ui->comboBox_Hours->setCurrentIndex( 0 );
    ui->comboBox_Mins->setCurrentIndex( 30 );
    ui->comboBox_Secs->setCurrentIndex( 0 );
}

///////////////////////////////////////////////////////////////////////////////

void RunnerDialog::onCancel()
{
    close();
}

///////////////////////////////////////////////////////////////////////////////

void RunnerDialog::onOk()
{
    QApplication::inputMethod()->commit();

    const int time =  ( ( ui->comboBox_Hours->currentIndex() * 60 * 60 ) + \
                        ( ui->comboBox_Mins->currentIndex() * 60 ) + \
                          ui->comboBox_Secs->currentIndex() ) * 1000;

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

///////////////////////////////////////////////////////////////////////////////
