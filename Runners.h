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

    inline std::list<Runner>& all()
    {
        return all_;
    }

    inline std::list<Runner*>& notStarted()
    {
        return notStarted_;
    }

    inline std::list<Runner*>& started()
    {
        return started_;
    }

    inline std::list<Runner*>& finished()
    {
        return finished_;
    }

    inline std::list<Runner*>& dns()
    {
        return dns_;
    }

    inline std::list<Runner*>& dnf()
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
    static bool cmp_deltas(const Runner* a, const Runner* b);
};

#endif // RUNNERS_H
