#ifndef RUNNERWIDGET_H
#define RUNNERWIDGET_H

#include <QTableWidgetItem>

class Runner;

class RunnerWidget : public QTableWidgetItem
{
public:
    RunnerWidget();

    const Runner* runnner() const;
    void setRunner( const Runner* const runner );

private:
    const Runner* runner_;
};

#endif // RUNNERWIDGET_H
