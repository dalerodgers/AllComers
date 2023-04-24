#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QTime>

#include "xlsxdocument.h"

///////////////////////////////////////////////////////////////////////////////

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect( ui->actionOpen, SIGNAL(triggered(bool)), this, SLOT(onOpen(bool)) );
    connect( ui->actionSave, SIGNAL(triggered(bool)), this, SLOT(onSave(bool)) );
    connect( ui->actionExit, SIGNAL(triggered(bool)), this, SLOT(onExit(bool)) );

    connect( ui->startStopButton, SIGNAL(pressed()), this, SLOT(onStartStopPressed()) );

    connect( ui->tableWidget, SIGNAL(cellPressed(int, int)), this, SLOT(onCellPressed(int, int)) );

    connect( &timer_, SIGNAL(timeout()), this, SLOT(onTimer()) );
    timer_.stop();
    timer_.setSingleShot( false );

    ui->tableWidget->setColumnCount( NUM_COLUMNS );
    QStringList headers = {"Name", "Predicted", "Time", "Delta" };
    ui->tableWidget->setHorizontalHeaderLabels( headers );

    isStarted_ = false;
}

///////////////////////////////////////////////////////////////////////////////

MainWindow::~MainWindow()
{
    delete ui;
}

///////////////////////////////////////////////////////////////////////////////

void MainWindow::onOpen(bool checked)
{
    Q_UNUSED(checked);

    const QString fileName = QFileDialog::getOpenFileName( this,
                                                     tr("Open File"),
                                                     "",
                                                     tr("XLSX (*.xlsx)") );

    QXlsx::Document xlsxR( fileName );

    int row = 1;

    if (xlsxR.load()) // load excel file
    {
        QXlsx::Cell* cell1;
        QXlsx::Cell* cell2;

        do
        {
            cell1 = xlsxR.cellAt(row, 1); // get cell pointer.
            cell2 = xlsxR.cellAt(row, 2); // get cell pointer.

            if ( ( cell1 != nullptr ) && (cell2 != nullptr) )
            {
                QVariant name = cell1->readValue();   // read cell value (number(double), QDateTime, QString ...)
                QVariant time = cell2->readValue();   // read cell value (number(double), QDateTime, QString ...)

                const int ms = static_cast<int>( 1000.0 * time.toDouble() * 60.0 * 60.0 * 24.0 );

                runners_.add( name.toString(), ms );
            }

            row++;
        } while( nullptr != cell1 );

        // ....................................................................

        ui->tableWidget->setRowCount( runners_.all().size() );
        row = 0;

        auto all = runners_.all();
        auto iter = all.begin();

        while( iter != all.end() )
        {
            for( int col = 0; col < NUM_COLUMNS; col++ )
            {
                ui->tableWidget->setItem( row, col, new QTableWidgetItem );

                if( col > 0 )
                {
                    ui->tableWidget->item( row, col )->setTextAlignment(Qt::AlignRight);
                }
            }

            row++;
            iter++;
        }

        ui->tableWidget->verticalHeader()->hide();

        redraw();
    }
}

///////////////////////////////////////////////////////////////////////////////

void MainWindow::onSave(bool checked)
{
    Q_UNUSED(checked);
    qDebug() << "onSave";
}

///////////////////////////////////////////////////////////////////////////////

void MainWindow::onExit(bool checked)
{
    Q_UNUSED(checked);

    close();
}

///////////////////////////////////////////////////////////////////////////////

void MainWindow::onStartStopPressed()
{
    qDebug() << "onStartStopPressed";

    if( runners_.all().size() == 0 )
    {
        ; // do nothing
    }
    else if( !isStarted_ )
    {
        msSlowest_ = msFUDGE + runners_.notStarted().front()->msPredicted();

        isStarted_ = true;
        ui->startStopButton->setText( "Stop" );

        elapsedTimer_.start();
        timer_.start(50);
    }
    else
    {
        isStarted_ = false;
        ui->startStopButton->setText( "Start" );
        timer_.stop();
    }
}

///////////////////////////////////////////////////////////////////////////////

void MainWindow::onTimer()
{
    int t = msSlowest_ - elapsedTimer_.elapsed();
    QString s = QTime( QTime(0 ,0, 0).addMSecs(t) ).toString("hh:mm:ss");

    if( msSlowest_ < 0 )
    {
        s = "-" + s;
    }

    ui->stopwatchText->setText( s );

    redraw( t );
}

///////////////////////////////////////////////////////////////////////////////

void MainWindow::onCellPressed(int row, int column)
{
    Q_UNUSED( column );

    const std::list<Runner*> notStarted = runners_.notStarted();
    const std::list<Runner*> started = runners_.started();
    const std::list<Runner*> finished = runners_.finished();
    const std::list<Runner*> dns = runners_.dns();
    const std::list<Runner*> dnf = runners_.dnf();

    std::list< const std::list<Runner*>* > lists;
    lists.push_back( &notStarted );
    lists.push_back( &started );
    lists.push_back( &finished );
    lists.push_back( &dns );
    lists.push_back( &dnf );

    int r = 0;
    std::list< const std::list<Runner*>* >::iterator iterO = lists.begin();
    std::list<Runner*>::const_iterator iterI = (*iterO)->begin();

    while( ( iterO != lists.end() ) && ( iterI != (*iterO)->end() ) )
    {
        if( r == row )
        {
            if( isStarted_ )
            {
                (*iterI)->Stop( elapsedTimer_.elapsed() );
            }

            iterI = (*iterO)->end();
            iterO = lists.end();
        }
        else
        {
            r++;
            iterI++;
        }

        if( ( iterO != lists.end() ) && ( iterI == (*iterO)->end() ) )
        {
            iterO++;
            iterI = (*iterO)->begin();
        }
    }
}

////////////////////////// /////////////////////////////////////////////////////

void MainWindow::redraw(int ms)
{
    int row = 0;
    runners_.sort();
    QTableWidgetItem* pTWI;

    // ........................................................................

    auto notStarted = runners_.notStarted();
    auto iter = notStarted.begin();

    while( iter != notStarted.end() )
    {      
        pTWI = ui->tableWidget->item( row, 0 );
        pTWI->setText( (*iter)->name() );
        pTWI->setBackground( Qt::yellow );

        pTWI = ui->tableWidget->item( row, 1 );
        QTime time = QTime(0, 0, 0).addMSecs( (*iter)->msPredicted() );
        pTWI->setText( time.toString("hh:mm:ss") );
        pTWI->setBackground( Qt::yellow );

        pTWI = ui->tableWidget->item( row, 2 );
        if( isStarted_ )
        {
            QString s;
            int delta = (*iter)->msPredicted() - ms;

            if( delta >= 0 )
            {
                (*iter)->Start();
                textToSpeech_.say( (*iter)->name() );
            }
            else
            {
                delta = -delta;
                s = "-";
            }

            time = QTime(0, 0, 0).addMSecs( delta );
            s += time.toString("hh:mm:ss");

            pTWI->setText( s );
        }

        pTWI->setBackground( Qt::yellow );

        pTWI = ui->tableWidget->item( row, 3 );
        pTWI->setBackground( Qt::yellow );

        row++;
        iter++;
    }

    // ........................................................................

    auto started = runners_.started();
    auto startedI = started.begin();

    while( startedI != started.end() )
    {
        pTWI = ui->tableWidget->item( row, 0 );
        pTWI->setText( (*startedI)->name() );
        pTWI->setBackground( Qt::green );

        pTWI = ui->tableWidget->item( row, 1 );
        QTime time = QTime(0, 0, 0).addMSecs( (*startedI)->msPredicted() );
        pTWI->setText( time.toString("hh:mm:ss") );
        pTWI->setBackground( Qt::green );

        pTWI = ui->tableWidget->item( row, 2 );
        QString s;
        int delta = (*startedI)->msPredicted() - ms;

        time = QTime(0, 0, 0).addMSecs( delta );
        s = time.toString("hh:mm:ss");

        pTWI->setText( s );
        pTWI->setBackground( Qt::green );

        pTWI = ui->tableWidget->item( row, 3 );
        pTWI->setBackground( Qt::green );

        row++;
        startedI++;
    }

    // ........................................................................

    auto finished = runners_.finished();
    auto finishedI = finished.begin();

    while( finishedI != finished.end() )
    {
        pTWI = ui->tableWidget->item( row, 0 );
        pTWI->setText( (*startedI)->name() );
        pTWI->setBackground( Qt::red );

        pTWI = ui->tableWidget->item( row, 1 );
        QTime time = QTime(0, 0, 0).addMSecs( (*startedI)->msPredicted() );
        pTWI->setText( time.toString("hh:mm:ss") );
        pTWI->setBackground( Qt::red );

        pTWI = ui->tableWidget->item( row, 2 );
        QString s;
        int delta = (*startedI)->msPredicted() - ms;

        time = QTime(0, 0, 0).addMSecs( delta );
        s = time.toString("hh:mm:ss");

        pTWI->setText( s );
        pTWI->setBackground( Qt::red );

        pTWI = ui->tableWidget->item( row, 3 );
        pTWI->setBackground( Qt::red );

        row++;
        startedI++;
    }
}

///////////////////////////////////////////////////////////////////////////////
