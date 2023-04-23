#ifndef RUNNER_H
#define RUNNER_H

#include <QString>
#include <QElapsedTimer>

class Runner
{
public:
    enum class State_e
    {
        WAITIING_TO_START,
        RUNNING,
        FINISHED,
        DNS,
        DNF
    };

    Runner( const QString& name, const int msPredicted );

    State_e state() const;
    const QString& name() const;
    int msPredicted() const;
    int msElapsed() const;

private:
    const QString name_;
    const int msPredicted_;
    QElapsedTimer elapsed_;
    int msFinished_;

    State_e state_;
};

#endif // RUNNER_H
