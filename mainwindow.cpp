#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QTime>
#include "RunnerWidget.h"

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

    ui->tableWidget->setColumnCount( NUM_COLUMNS );
    QStringList headers = {"Name", "Predicted", "Time", "Delta" };
    ui->tableWidget->setHorizontalHeaderLabels( headers );
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

        //runners_.sort();
        ui->tableWidget->setRowCount( runners_.all().size() );

        for( unsigned int row = 0; row < runners_.all().size(); row++ )
        {
            for( int col = 0; col < NUM_COLUMNS; col++ )
            {
                ui->tableWidget->setItem( row, col, new RunnerWidget );
            }
        }

        //int row = 0;
        //auto eric = runners_.notStarted();
        //auto iter = eric.begin();

        //while( iter != eric.end() )
        //{
        //    ui->tableWidget->setItem( row, 0, new QTableWidgetItem( (*iter)->name() ) );
        //    ui->tableWidget->setItem( row, 1, new QTableWidgetItem() );
        //
        //    row++;
        //    iter++;
        //}
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
}

///////////////////////////////////////////////////////////////////////////////
