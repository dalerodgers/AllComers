#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "Runners.h"
#include <QTableWidgetItem>
#include <QTimer>
#include <QTextToSpeech>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    static constexpr int NUM_COLUMNS = 4;
    static constexpr int msFUDGE = 10000;

    Ui::MainWindow *ui;

    bool isStarted_;
    Runners runners_;
    QTimer timer_;
    QElapsedTimer elapsedTimer_;
    int msSlowest_;
    QTextToSpeech textToSpeech_;

    void redraw(int ms = 0);
    void drawRow( const int row, const QBrush &background, const QString& col1, const QString& col2, const QString& col3="", const QString& col4="" );

private slots:
    void onOpen(bool checked);
    void onSave(bool checked);
    void onExit(bool checked);
    void onAdd(bool checked);

    void onStartStopPressed();

    void onTimer();

    void onCellPressed(int row, int column);
};
#endif // MAINWINDOW_H
