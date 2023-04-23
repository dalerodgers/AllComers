#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Runners.h"

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

    Ui::MainWindow *ui;

    Runners runners_;

private slots:
    void onOpen(bool checked);
    void onSave(bool checked);
    void onExit(bool checked);

    void onStartStopPressed();
};
#endif // MAINWINDOW_H
