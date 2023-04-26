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

    void Start( const int msStart );
    void Stop( const int msElpased );

    State_e state() const;
    const QString& name() const;
    int msPredicted() const;
    int msStart() const;
    int msFinished() const;
    int msDelta() const;

    int id() const;
    void setId( const int id );

private:
    const QString name_;
    const int msPredicted_;
    int msStart_;
    int msFinished_;

    State_e state_;
    int id_;
};

#endif // RUNNER_H
