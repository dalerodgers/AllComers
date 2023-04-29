#ifndef RUNNER_H
#define RUNNER_H

#include <QString>

class QTextToSpeech;

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

    Runner( const QString& name, const int msPredicted, QTextToSpeech& textToSpech  );

    void newTime( const int msTime );

    void Start( const int msStart );
    void Stop( const int msElapsed );

    State_e state() const;
    const QString& name() const;

    void set_msPredicted( const int msPredicted );
    int msPredicted() const;
    int msStart() const;
    int msFinished() const;
    int msDelta() const;

    int id() const;
    void setId( const int id );

    void reset();

private:
    static constexpr int WARNING_DISABLED = 0x7FFFFFFF;

    const QString name_;
    int msPredicted_;
    QTextToSpeech& textToSpeech_;
    int msStart_;
    int msFinished_;

    State_e state_;
    int id_;

    struct Warning
    {
        QString pre;
        QString mid;
        QString post;
        int time;
    };

    Warning warnings_[10];
};

#endif // RUNNER_H
