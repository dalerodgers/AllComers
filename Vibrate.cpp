#include "Vibrate.h"

#include <QtGlobal>

#ifdef VIBRATE
    #include <QJniObject>
#endif

void Vibrate::start( int ms )
{
#ifdef VIBRATE
    QJniObject::callStaticMethod<void>("org/qtproject/example/Chronometer/Vibrate", "start", "(I)V", ms);
#else
    Q_UNUSED( ms );
#endif
}
