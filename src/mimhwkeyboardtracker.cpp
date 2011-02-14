#include "mimhwkeyboardtracker.h"

#ifdef HAVE_MEEGOTOUCH
#include <MKeyboardStateTracker>
#endif

MImHwKeyboardTracker::MImHwKeyboardTracker()
    : QObject()
{
#ifdef HAVE_MEEGOTOUCH
    connect(MKeyboardStateTracker::instance(), SIGNAL(stateChanged()),
            this,                              SIGNAL(stateChanged()),
            Qt::UniqueConnection);
#endif
}

MImHwKeyboardTracker::~MImHwKeyboardTracker()
{}

MImHwKeyboardTracker *MImHwKeyboardTracker::instance()
{
    static MImHwKeyboardTracker tracker;
    return &tracker;
}

bool MImHwKeyboardTracker::isPresent() const
{
#ifdef HAVE_MEEGOTOUCH
    return MKeyboardStateTracker::instance()->isPresent();
#else
    return false;
#endif
}

bool MImHwKeyboardTracker::isOpen() const
{
#ifdef HAVE_MEEGOTOUCH
    return MKeyboardStateTracker::instance()->isOpen();
#else
    return false;
#endif
}
