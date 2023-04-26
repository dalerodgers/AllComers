#include "Runner.h"

///////////////////////////////////////////////////////////////////////////////

Runner::Runner( const QString& name, const int msPredicted ) :
    name_( name ),
    msPredicted_( msPredicted )
{
    state_ = State_e::WAITIING_TO_START;
    msFinished_ = 0;
    msStart_ = 0;
    id_ = -1;
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

int Runner::msPredicted() const
{
    return msPredicted_;
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
            state_ = State_e::DNS;
            break;

        case State_e::RUNNING:
            if( msElapsed >= 0 )
            {
                state_ = State_e::FINISHED;
                msFinished_ = msElapsed;
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
