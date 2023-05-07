#include "Vibrate.h"

#include <QtGlobal>

#ifdef Q_OS_ANDROID
    #include <QJniObject>
#endif

void Vibrate::start( int ms )
{
#ifdef Q_OS_ANDROID
    QJniObject::callStaticMethod<void>("org/qtproject/example/Chronometer/Vibrate", "start", "(I)V", ms);
#else
    Q_UNUSED( ms );
#endif
}
