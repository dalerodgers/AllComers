#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>

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

            QVariant name;
            QVariant time;

            if ( cell1 != nullptr )
            {
                name = cell1->readValue();   // read cell value (number(double), QDateTime, QString ...)
            }

            if( cell2 != nullptr )
            {
                time = cell2->readValue(); // read cell value (number(double), QDateTime, QString ...)
            }

            qDebug() << name << " " << time;

            row++;
        } while( nullptr != cell1 );
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
    qDebug() << "onExit";
}

///////////////////////////////////////////////////////////////////////////////

void MainWindow::onStartStopPressed()
{
    qDebug() << "onStartStopPressed";
}

///////////////////////////////////////////////////////////////////////////////
