#include "Runner.h"
#include <QTextToSpeech>

///////////////////////////////////////////////////////////////////////////////

Runner::Runner( const QString& name, const int msPredicted, QTextToSpeech& textToSpech ) :
    name_( name ),
    msPredicted_( msPredicted ),
    textToSpeech_( textToSpech )
{
    reset();
}

///////////////////////////////////////////////////////////////////////////////

Runner::State_e Runner::state() const
{
    return state_;
}

///////////////////////////////////////////////////////////////////////////////

const QString& Runner::name() const
{
    return name_;
}

///////////////////////////////////////////////////////////////////////////////

void Runner::set_msPredicted( const int msPredicted )
{
    msPredicted_ = msPredicted;
}

///////////////////////////////////////////////////////////////////////////////

int Runner::msPredicted() const
{
    return msPredicted_;
}

///////////////////////////////////////////////////////////////////////////////

void Runner::newTime( const int msTime )
{
    Warning* p = &warnings_[0];

    while( p->time != WARNING_DISABLED )
    {
        if( msTime >= p->time )
        {
            if( msTime < ( p->time + 1500 ) )
            {
                textToSpeech_.say( p->pre + p->mid + p->post );
            }

            p->time = WARNING_DISABLED;
        }
        else
        {
            p++;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

int Runner::msStart() const
{
    return msStart_;
}

///////////////////////////////////////////////////////////////////////////////

int Runner::msFinished() const
{
    return msFinished_;
}

///////////////////////////////////////////////////////////////////////////////

int Runner::msDelta() const
{
    return msFinished_ - msStart_ - msPredicted_;
}

///////////////////////////////////////////////////////////////////////////////

void Runner::Start( const int msStart )
{
    if( ( state_ == State_e::WAITIING_TO_START ) || \
        ( state_ == State_e::FINISHED ) )
    {
        msStart_ = msStart;
        state_ = State_e::RUNNING;
    }
}

///////////////////////////////////////////////////////////////////////////////

void Runner::Stop( const int msElapsed )
{
    switch( state_ )
    {
        case State_e::WAITIING_TO_START:
            textToSpeech_.say( name_ );
            state_ = State_e::DNS;
            break;

        case State_e::RUNNING:
            if( msElapsed >= 0 )
            {
                state_ = State_e::FINISHED;
                msFinished_ = msElapsed;
                textToSpeech_.say( name_ );
            }
            else
            {
                state_ = State_e::DNF;
            }
            break;

        case State_e::FINISHED:
            ( msElapsed >= 0 ? state_ = State_e::RUNNING : state_ = State_e::FINISHED );
            break;

        case State_e::DNS:
            state_ = State_e::WAITIING_TO_START;
            break;

        case State_e::DNF:
        default:
            ; // do nothing
            break;
    }
}

///////////////////////////////////////////////////////////////////////////////

int Runner::id() const
{
    return id_;
}

///////////////////////////////////////////////////////////////////////////////

void Runner::setId( const int id )
{
    id_ = id;
}

///////////////////////////////////////////////////////////////////////////////

void Runner::reset()
{
    state_ = State_e::WAITIING_TO_START;
    msFinished_ = 0;
    msStart_ = 0;
    id_ = -1;

    warnings_[0].pre =  "Go ";
    warnings_[0].mid =  name_;
    warnings_[0].post = " Go";
    warnings_[0].time = 0;

    warnings_[1].pre =  "1";
    warnings_[1].mid =  "";
    warnings_[1].post = "";
    warnings_[1].time = -1000;

    warnings_[2].pre =  "2";
    warnings_[2].mid =  "";
    warnings_[2].post = "";
    warnings_[2].time = -2000;

    warnings_[3].pre =  "3";
    warnings_[3].mid =  "";
    warnings_[3].post = "";
    warnings_[3].time = -3000;

    warnings_[4].pre =  "4";
    warnings_[4].mid =  "";
    warnings_[4].post = "";
    warnings_[4].time = -4000;

    warnings_[5].pre =  name_;
    warnings_[5].mid =  " 5";
    warnings_[5].post = "";
    warnings_[5].time = -5000;

    warnings_[6].pre =  name_;
    warnings_[6].mid =  " 10 seconds";
    warnings_[6].post = "";
    warnings_[6].time = -10000;

    warnings_[7].pre =  name_;
    warnings_[7].mid =  " 30 seconds";
    warnings_[7].post = "";
    warnings_[7].time = -30000;

    warnings_[8].pre =  name_;
    warnings_[8].mid =  " one minute";
    warnings_[8].post = "";
    warnings_[8].time = -60000;

    warnings_[9].time = WARNING_DISABLED;

}

///////////////////////////////////////////////////////////////////////////////
