#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "Runners.h"
#include <QElapsedTimer>
#include <QTableWidgetItem>
#include <QTimer>

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
    int width_;
    int height_;

    void redraw(int ms = 0);
    void drawRow( const int row, const QBrush &background, const QString& col1, const QString& col2, const QString& col3="", const QString& col4="" );
    void addRows();

private slots:
    void onOpen();
    void onSave();
    void onExit(bool checked);
    void onAdd();

    void onStartStopPressed();

    void onTimer();

    void onCellPressed(int row, int column);
};
#endif // MAINWINDOW_H
