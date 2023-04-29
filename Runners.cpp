#include "Runners.h"

///////////////////////////////////////////////////////////////////////////////

Runners::Runners()
{
    textToSpeech_.setRate( 0.0 );
}

///////////////////////////////////////////////////////////////////////////////

void Runners::add( const QString& name, const int msPredicted )
{
    all_.push_back( Runner(name, msPredicted, textToSpeech_ ) );
}

///////////////////////////////////////////////////////////////////////////////

void Runners::sort()
{
    notStarted_.clear();
    started_.clear();
    finished_.clear();
    dns_.clear();
    dnf_.clear();

    auto iter = all_.begin();
    while( iter != all_.end() )
    {
        const Runner::State_e state = iter->state();

        switch( state )
        {
            case Runner::State_e::WAITIING_TO_START:
                notStarted_.push_back( &(*iter) );
                break;

            case Runner::State_e::RUNNING:
                started_.push_back( &(*iter) );
                break;

            case Runner::State_e::FINISHED:
                finished_.push_back( &(*iter) );
                break;

            case Runner::State_e::DNS:
                dns_.push_back( &(*iter) );
                break;

            case Runner::State_e::DNF:
            default:
                dnf_.push_back( &(*iter) );
                break;
        }

        iter++;
    }

    notStarted_.sort( &cmp_notStarted );
    started_.sort( &cmp_names );
    finished_.sort( &cmp_deltas );
    dns_.sort( &cmp_names );
    dnf_.sort( &cmp_names );
}

///////////////////////////////////////////////////////////////////////////////

bool Runners::cmp_notStarted(const Runner* a, const Runner* b)
{
    return static_cast<bool>( a->msPredicted() > b->msPredicted() );
}

///////////////////////////////////////////////////////////////////////////////

bool Runners::cmp_names(const Runner* a, const Runner* b)
{
    return static_cast<bool>( a->name() < b->name() );
}

///////////////////////////////////////////////////////////////////////////////

bool Runners::cmp_deltas(const Runner* a, const Runner* b)
{
    const int delta_a = abs( a->msDelta() );
    const int delta_b = abs( b->msDelta() );

    return static_cast<bool>( delta_a < delta_b );
}

///////////////////////////////////////////////////////////////////////////////
