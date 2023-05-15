#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QTime>
#include <QScroller>
#include <QScrollBar>

#include "RunnerDialog.h"
#include "Vibrate.h"
#include "xlsxdocument.h"

#ifdef Q_OS_ANDROID
    #include <QJniObject>
#endif

///////////////////////////////////////////////////////////////////////////////

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect( ui->button_Open, SIGNAL(pressed()), this, SLOT(onOpen()) );

    connect( ui->button_Save, SIGNAL(pressed()), this, SLOT(onSave()) );
    ui->button_Save->setEnabled( false );

    connect( ui->button_Add, SIGNAL(pressed()), this, SLOT(onAdd()) );
    ui->button_Add->setEnabled( false );

    connect( ui->startStopButton, SIGNAL(pressed()), this, SLOT(onStartStopPressed()) );
    ui->startStopButton->setEnabled( false );

    connect( ui->tableWidget, SIGNAL(cellPressed(int,int)), this, SLOT(onCellPressed(int,int)) );

    connect( &timer_, SIGNAL(timeout()), this, SLOT(onTimer()) );
    timer_.stop();
    timer_.setSingleShot( false );

    setupTable();

    ui->downText->setReadOnly( true );
    ui->upText->setReadOnly( true );
#ifdef Q_OS_ANDROID
    QFont font = ui->downText->font();
    font.setPointSize(12);
    font.setBold( true );
    ui->downText->setFont( font );
    ui->upText->setFont( font );

    QJniObject activity = QNativeInterface::QAndroidApplication::context();

    if (activity.isValid())
    {
        QJniObject window = activity.callObjectMethod("getWindow", "()Landroid/view/Window;");

        if( window.isValid() )
        {
            const int FLAG_KEEP_SCREEN_ON = 128;
            //window.callObjectMethod("addFlags", "(I)V", FLAG_KEEP_SCREEN_ON);
            window.callMethod<void>("addFlags", "(I)V", FLAG_KEEP_SCREEN_ON);
        }
    }
#endif

    isStarted_ = false;
}

///////////////////////////////////////////////////////////////////////////////

MainWindow::~MainWindow()
{
    delete ui;
}

///////////////////////////////////////////////////////////////////////////////

void MainWindow::onOpen()
{
    const QString fileName = QFileDialog::getOpenFileName( this,
                                                           tr("Open File"),
                                                           "",
                                                           tr("XLSX (*.xlsx)") );

    QXlsx::Document xlsxR( fileName );

    int row = 1;

    if (xlsxR.load()) // load excel file
    {
        setupTable();

        QXlsx::Cell* cell1;
        QXlsx::Cell* cell2;

        do
        {
            cell1 = xlsxR.cellAt(row, 1); // get cell pointer.
            cell2 = xlsxR.cellAt(row, 2); // get cell pointer.

            if ( ( cell1 != nullptr ) && (cell2 != nullptr) )
            {
                int ms;
                QVariant name = cell1->readValue();   // read cell value (number(double), QDateTime, QString ...)
                QVariant time = cell2->readValue();   // read cell value (number(double), QDateTime, QString ...)

                if( time.canConvert<QTime>() )
                {
                    const QTime eric = time.toTime();
                    ms = abs( eric.msecsTo( QTime(0, 0, 0) ) );
                }
                else
                {
                    ms = static_cast<int>( 1000.0 * time.toDouble() * 60.0 * 60.0 * 24.0 );
                }

                if( !name.toString().isEmpty() )
                {
                    runners_.add( name.toString(), ms );
                }
            }

            row++;
        } while( nullptr != cell1 );

        // ....................................................................

        addRows();
        redraw();

        ui->button_Add->setEnabled( true );
        ui->startStopButton->setEnabled( true );
    }

    ui->button_Open->setDown( false );
}

///////////////////////////////////////////////////////////////////////////////

void MainWindow::onSave()
{
    const QString fileName = QFileDialog::getSaveFileName( this,
                                                           tr("Open File"),
                                                           "",
                                                           tr("XLSX (*.xlsx)") );

    if( fileName.size() > 0 )
    {
        QXlsx::Document xlsxW( fileName );

        for( int row = 0; row < ui->tableWidget->rowCount(); row++ )
        {
            for( int col = 0; col < ui->tableWidget->columnCount(); col++ )
            {
                xlsxW.write( row+1, col+1, QVariant( ui->tableWidget->item(row, col)->text() ) );
            }
        }

        xlsxW.save();
    }

    ui->button_Save->setDown( false );
}

///////////////////////////////////////////////////////////////////////////////

void MainWindow::onExit(bool checked)
{
    Q_UNUSED(checked);

    close();
}

///////////////////////////////////////////////////////////////////////////////

void MainWindow::onAdd()
{
    qDebug() << "onAdd";

    RunnerDialog rd( &runners_, nullptr, this );
    rd.exec();

    addRows();
    redraw();

    ui->button_Add->setDown( false );
}

///////////////////////////////////////////////////////////////////////////////

void MainWindow::onStartStopPressed()
{
    qDebug() << "onStartStopPressed";
    Vibrate::start( 500 );

    if( runners_.all().size() == 0 )
    {
        ; // do nothing
    }
    else if( !isStarted_ )
    {
        msSlowest_ = runners_.notStarted().front()->msPredicted();

        isStarted_ = true;
        ui->startStopButton->setText( "Stop" );

        ui->button_Open->setEnabled( false );
        ui->button_Save->setEnabled( false );
        ui->button_Add->setEnabled( false );

        msStart_ = QDateTime::currentMSecsSinceEpoch();
        timer_.start(100);
    }
    else
    {
        timer_.stop();
        isStarted_ = false;

        std::list<Runner>& all = runners_.all();
        auto iter = all.begin();

        while( iter != all.end() )
        {
            iter->Stop( -1 );
            iter++;
        }

        ui->button_Open->setEnabled( true );
        ui->button_Save->setEnabled( true );

        redraw( -1 );

        ui->startStopButton->setText( "Start" );
        ui->startStopButton->setEnabled( false );
    }
}

///////////////////////////////////////////////////////////////////////////////

void MainWindow::onTimer()
{
    msElapsed_ = QDateTime::currentMSecsSinceEpoch() - msStart_;

    const unsigned int sortOfFinished = runners_.finished().size() + \
                                        runners_.dns().size() + \
                                        runners_.dnf().size();

    if( !isStarted_ )
    {
        onStartStopPressed();
    }
    else if( sortOfFinished >= runners_.all().size() )
    {
        onStartStopPressed();
    }
    else
    {
        int t = msSlowest_ - msElapsed_;
        QString s;

        if( msSlowest_ < 0 )
        {
            s = "-";
            t = -t;
        }

        s += QTime( QTime(0 ,0, 0).addMSecs(t + msFUDGE) ).toString("hh:mm:ss");
        ui->downText->setText( s );

        t = msElapsed_ - msFUDGE;
        s.clear();

        if( t >= 0 )
        {
            s = QTime( QTime(0 ,0, 0).addMSecs(t) ).toString("hh:mm:ss");
        }

        ui->upText->setText( s );

        redraw( t );
    }
}

///////////////////////////////////////////////////////////////////////////////

void MainWindow::onCellPressed(int row, int column)
{
    if( column == 0 )
    {
        std::list<Runner>& all = runners_.all();
        auto iter = all.begin();

        while( iter != all.end() )
        {
            if( row == iter->id() )
            {
                qDebug() << iter->name();

                if( isStarted_ )
                {
                    if( iter->state() == Runner::State_e::DNS )
                    {
                        iter->reset();
                    }
                    else
                    {
                        iter->Stop( msElapsed_ - msFUDGE );
                    }
                }
                else
                {
                    RunnerDialog rd( nullptr, &(*iter), this );
                    rd.exec();

                    addRows();
                    redraw();
                }

                iter = all.end();
            }
            else
            {
                iter++;
            }
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

    const int newWidth = ui->tableWidget->width();

    if( width_ != newWidth )
    {
        width_ = newWidth;
        const int colWidth = width_ / NUM_COLUMNS;

        ui->tableWidget->horizontalHeader()->resizeSection(0, colWidth );
        ui->tableWidget->horizontalHeader()->resizeSection(1, colWidth );
        ui->tableWidget->horizontalHeader()->resizeSection(2, colWidth );
        ui->tableWidget->horizontalHeader()->resizeSection(3, colWidth );
    }

    // ........................................................................

    if( !runners_.all().empty() )
    {
        ui->startStopButton->setEnabled( true );

        const int newHeight = ui->tableWidget->height();

        if( height_ != newHeight )
        {
            height_ = newHeight;
            int rowHeight = newHeight / runners_.all().size();

            if( rowHeight < 40 )
            {
                rowHeight = 40;
            }

            for( int i = 0; i<ui->tableWidget->rowCount(); i++)
            {
                ui->tableWidget->verticalHeader()->resizeSection( i, rowHeight );
            }
        }
    }

    // ........................................................................

    while( ui->tableWidget->rowCount() != (int)runners_.all().size() )
    {
        ui->tableWidget->insertRow( ui->tableWidget->rowCount() - 1 );
    }

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
            int delta = ms - ( msSlowest_ - (*notStartedI)->msPredicted() );
            (*notStartedI)->newTime( delta );

            if( delta >= 0 )
            {
                (*notStartedI)->Start( ms );
            }
            else
            {
                delta = -delta;
                col3 = "-";
            }

            time = QTime(0, 0, 0).addMSecs( delta + 500 );
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

        int delta = ms - (*startedI)->msStart();
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

        time = QTime(0, 0, 0).addMSecs( (*finishedI)->msFinished() - (*finishedI)->msStart() );
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

        delta += 500;

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

void MainWindow::addRows()
{
    int rowCount = ui->tableWidget->rowCount();

    while( rowCount != static_cast<int>( runners_.all().size() ) )
    {
        ui->tableWidget->setRowCount( rowCount + 1 );

        for( int col = 0; col < NUM_COLUMNS; col++ )
        {
            QTableWidgetItem* pTWI = new QTableWidgetItem;
            QFont font = pTWI->font();

 #ifndef Q_OS_ANDROID
            font.setPointSize( 24 );
 #else
            font.setPointSize( 18 );
 #endif
            font.setFamily( "Courier" );
            pTWI->setFont( font );

            if( col > 0 )
            {
                pTWI->setTextAlignment( Qt::AlignVCenter | Qt::AlignRight);
                pTWI->setFlags( Qt::NoItemFlags );
            }
            else
            {
                pTWI->setTextAlignment( Qt::AlignVCenter | Qt::AlignLeft );
                pTWI->setFlags( Qt::ItemIsEnabled );
            }

            ui->tableWidget->setItem( rowCount, col, pTWI );

            int fontPixelSize = font.pixelSize();
            ui->tableWidget->verticalHeader()->resizeSection( rowCount, fontPixelSize + 40 );
        }

        height_ = -1;
        rowCount = ui->tableWidget->rowCount();
    }
}

///////////////////////////////////////////////////////////////////////////////

void MainWindow::setupTable()
{
    runners_.clear( true );

    // ........................................................................

    for( int row = 0; row < ui->tableWidget->rowCount(); row++ )
    {
        for( int col = 0; col < ui->tableWidget->columnCount(); col++ )
        {
            QTableWidgetItem* pTWI = ui->tableWidget->item( row, col );

            if( nullptr != pTWI )
            {
                delete pTWI;
            }
        }
    }

    ui->tableWidget->clear();
    ui->tableWidget->setRowCount( 0 );
    ui->tableWidget->setColumnCount( 0 );

    // ........................................................................

    ui->tableWidget->setColumnCount( NUM_COLUMNS );
    QStringList headers = {"Name", "Predicted", "Time", "Delta" };
    ui->tableWidget->setHorizontalHeaderLabels( headers );
    ui->tableWidget->verticalHeader()->hide();

    // ........................................................................

    ui->tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tableWidget->horizontalScrollBar()->setDisabled(true);
    ui->tableWidget->verticalHeader()->setHidden(true);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setAutoScroll(true);
    ui->tableWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    // add a kinetic scroller
    QScroller::grabGesture(ui->tableWidget, QScroller::LeftMouseButtonGesture);

    // ........................................................................

    width_ = -1;
    height_ = -1;
}

///////////////////////////////////////////////////////////////////////////////
