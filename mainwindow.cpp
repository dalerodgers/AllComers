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
    ui->actionSave->setEnabled( false );

    connect( ui->actionExit, SIGNAL(triggered(bool)), this, SLOT(onExit(bool)) );

    connect( ui->startStopButton, SIGNAL(pressed()), this, SLOT(onStartStopPressed()) );
    ui->startStopButton->setEnabled( false );

    connect( ui->tableWidget, SIGNAL(cellPressed(int,int)), this, SLOT(onCellPressed(int,int)) );

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
                QTableWidgetItem* pTWI = new QTableWidgetItem;
                QFont font = pTWI->font();
                font.setPointSize( 24 );
                pTWI->setFont( font );

                if( col > 0 )
                {
                    pTWI->setTextAlignment( Qt::AlignVCenter | Qt::AlignRight);
                }
                else
                {
                    pTWI->setTextAlignment( Qt::AlignVCenter | Qt::AlignLeft );
                }

                ui->tableWidget->setItem( row, col, pTWI );
            }

            row++;
            iter++;
        }

        ui->tableWidget->verticalHeader()->hide();
        ui->tableWidget->resizeRowsToContents();

        redraw();

        ui->startStopButton->setEnabled( true );
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
        msSlowest_ = runners_.notStarted().front()->msPredicted();

        isStarted_ = true;
        ui->startStopButton->setText( "Stop" );

        ui->actionOpen->setEnabled( false );
        ui->actionSave->setEnabled( false );
        ui->actionExit->setEnabled( false );

        elapsedTimer_.start();
        timer_.start(50);
    }
    else
    {
        isStarted_ = false;
        ui->startStopButton->setText( "Start" );

        std::list<Runner>& all = runners_.all();
        auto iter = all.begin();

        while( iter != all.end() )
        {
            iter->Stop( -1 );
            iter++;
        }

        ui->actionOpen->setEnabled( true );
        ui->actionSave->setEnabled( true );
        ui->actionExit->setEnabled( true );
    }
}

///////////////////////////////////////////////////////////////////////////////

void MainWindow::onTimer()
{
    if( !isStarted_ )
    {
        timer_.stop();
    }

    int t = msSlowest_ - elapsedTimer_.elapsed();
    QString s;

    if( msSlowest_ < 0 )
    {
        s = "-";
        t = -t;
    }

    s += QTime( QTime(0 ,0, 0).addMSecs(t + msFUDGE) ).toString("hh:mm:ss");
    ui->downText->setText( s );

    t = elapsedTimer_.elapsed() - msFUDGE;
    s.clear();

    if( t >= 0 )
    {
        s = QTime( QTime(0 ,0, 0).addMSecs(t) ).toString("hh:mm:ss");
    }

    ui->upText->setText( s );

    redraw( t );
}

///////////////////////////////////////////////////////////////////////////////

void MainWindow::onCellPressed(int row, int column)
{
    Q_UNUSED( column );

    std::list<Runner>& all = runners_.all();
    auto iter = all.begin();

    while( iter != all.end() )
    {
        if( row == iter->id() )
        {
            qDebug() << iter->name();

            if( isStarted_ )
            {
                iter->Stop( elapsedTimer_.elapsed() - msFUDGE );
            }

            iter = all.end();
        }
        else
        {
            iter++;
        }
    }

    ui->tableWidget->clearSelection();
}

////////////////////////// /////////////////////////////////////////////////////

void MainWindow::redraw( int ms )
{
    int row = 0;
    runners_.sort();
    QString col1, col2, col3, col4;
    QTime time;

    // ........................................................................

    std::list<Runner*>& notStarted = runners_.notStarted();
    auto notStartedI = notStarted.begin();

    while( notStartedI != notStarted.end() )
    {      
        col1 = (*notStartedI)->name();

        time = QTime(0, 0, 0).addMSecs( (*notStartedI)->msPredicted() );
        col2 = time.toString("hh:mm:ss");

        if( isStarted_ )
        {
            int delta = (*notStartedI)->msPredicted() - msSlowest_ + ms;

            if( delta >= 0 )
            {
                (*notStartedI)->Start();
                textToSpeech_.say( (*notStartedI)->name() );
            }
            else
            {
                delta = -delta;
                col3 = "-";
            }

            time = QTime(0, 0, 0).addMSecs( delta );
            col3 += time.toString("hh:mm:ss");
        }

        drawRow( row, Qt::yellow, col1, col2, col3 );
        (*notStartedI)->setId( row );

        row++;
        notStartedI++;
    }

    // ........................................................................

    std::list<Runner*>& started = runners_.started();
    auto startedI = started.begin();

    while( startedI != started.end() )
    {
        col1 = (*startedI)->name();

        time = QTime(0, 0, 0).addMSecs( (*startedI)->msPredicted() );
        col2 = time.toString("hh:mm:ss");

        int delta = (*startedI)->msPredicted() - ms;
        time = QTime(0, 0, 0).addMSecs( delta );
        col3 = time.toString("hh:mm:ss");

        drawRow( row, Qt::green, col1, col2, col3 );
        (*startedI)->setId( row );

        row++;
        startedI++;
    }

    // ........................................................................

    std::list<Runner*>& finished = runners_.finished();
    auto finishedI = finished.begin();

    while( finishedI != finished.end() )
    {
        col1 = (*finishedI)->name();

        time = QTime(0, 0, 0).addMSecs( (*finishedI)->msPredicted() );
        col2 = time.toString("hh:mm:ss");

        time = QTime(0, 0, 0).addMSecs( (*finishedI)->msFinished() );
        col3 = time.toString("hh:mm:ss");

        int delta = (*finishedI)->msDelta();

        if( delta >= 0 )
        {
            col4.clear();
        }
        else
        {
            delta = -delta;
            col4 = "-";
        }

        time = QTime(0, 0, 0).addMSecs( delta );
        col4 += time.toString("hh:mm:ss");

        drawRow( row, Qt::red, col1, col2, col3, col4 );
        (*finishedI)->setId( row );

        row++;
        finishedI++;
    }

    // ........................................................................

    std::list<Runner*>& dns = runners_.dns();
    auto dnsI = dns.begin();

    while( dnsI != dns.end() )
    {
        col1 = (*dnsI)->name();

        time = QTime(0, 0, 0).addMSecs( (*dnsI)->msPredicted() );
        col2 = time.toString("hh:mm:ss");

        col3 = "DNS";

        drawRow( row, Qt::gray, col1, col2, col3 );
        (*dnsI)->setId( row );

        row++;
        dnsI++;
    }

    // ........................................................................

    std::list<Runner*>& dnf = runners_.dnf();
    auto dnfI = dnf.begin();

    while( dnfI != dnf.end() )
    {
        col1 = (*dnfI)->name();

        time = QTime(0, 0, 0).addMSecs( (*dnfI)->msPredicted() );
        col2 = time.toString("hh:mm:ss");

        col3 = "DNF";

        drawRow( row, Qt::lightGray, col1, col2, col3 );
        (*dnfI)->setId( row );

        row++;
        dnfI++;
    }
}

///////////////////////////////////////////////////////////////////////////////

void MainWindow::drawRow( const int row, const QBrush &background, const QString& col1, const QString& col2, const QString& col3, const QString& col4 )
{
    if( row < static_cast<int>(runners_.all().size()) )
    {
        QTableWidgetItem* pTWI;

        pTWI = ui->tableWidget->item( row, 0 );
        pTWI->setText( col1 );
        pTWI->setBackground( background );

        pTWI = ui->tableWidget->item( row, 1 );
        pTWI->setText( col2 );
        pTWI->setBackground( background );

        pTWI = ui->tableWidget->item( row, 2 );
        pTWI->setText( col3 );
        pTWI->setBackground( background );

        pTWI = ui->tableWidget->item( row, 3 );
        pTWI->setText( col4 );
        pTWI->setBackground( background );
    }
}

///////////////////////////////////////////////////////////////////////////////
