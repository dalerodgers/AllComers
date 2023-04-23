#ifndef RUNNERS_H
#define RUNNERS_H

#include "Runner.h"
#include <list>

class Runners
{
public:
    Runners();

    void add( const QString& name, const int msPredicted );
    void sort();

    inline const std::list<Runner> all() const
    {
        return all_;
    }

    inline const std::list<Runner*> notStarted() const
    {
        return notStarted_;
    }

    inline const std::list<Runner*> started() const
    {
        return started_;
    }

    inline const std::list<Runner*> finished() const
    {
        return finished_;
    }

    inline const std::list<Runner*> dns() const
    {
        return dns_;
    }

    inline const std::list<Runner*> dnf() const
    {
        return dnf_;
    }

private:
    std::list<Runner> all_;
    std::list<Runner*> notStarted_;
    std::list<Runner*> started_;
    std::list<Runner*> finished_;
    std::list<Runner*> dns_;
    std::list<Runner*> dnf_;

    static bool cmp_notStarted(const Runner* a, const Runner* b);
    static bool cmp_names(const Runner* a, const Runner* b);
};

#endif // RUNNERS_H
