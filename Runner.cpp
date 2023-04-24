#include "Runner.h"

///////////////////////////////////////////////////////////////////////////////

Runner::Runner( const QString& name, const int msPredicted ) :
    name_( name ),
    msPredicted_( msPredicted )
{
    state_ = State_e::WAITIING_TO_START;
    msFinished_ = 0;
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

void Runner::Start()
{
    if( ( state_ == State_e::WAITIING_TO_START ) || \
        ( state_ == State_e::FINISHED ) )
    {
        state_ = State_e::RUNNING;
    }
}

///////////////////////////////////////////////////////////////////////////////

void Runner::Stop( int msElapsed )
{
    switch( state_ )
    {
        case State_e::WAITIING_TO_START:
            state_ = State_e::DNS;
            break;

        case State_e::RUNNING:
            state_ = State_e::FINISHED;
            break;

        case State_e::FINISHED:
            state_ = State_e::RUNNING;
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
