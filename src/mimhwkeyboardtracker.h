#ifndef MIMKEYBOARDSTATETRACKER_H
#define MIMKEYBOARDSTATETRACKER_H

#include <QObject>

//! \internal
/*!
 * Class responsible for tracking the hardware keyboard properties and signaling events.
 * It is designed as singleton. Using isPresent() can query whether the device supports
 * hardware keyboard or not. If hardware keyboard is supported, using isOpen() to query
 * its current state. Signal stateChanged will be emitted when hardware keyboard state is changed.
 */

// TODO: Check (build) depedencies for context provider, and whether I should simply move MKeyboardStateTracker here
class MImHwKeyboardTracker
    : public QObject
{
    Q_OBJECT

public:
    //! \brief Returns the singleton instance.
    static MImHwKeyboardTracker *instance();

    //! \brief Returns whether device has a hardware keyboard.
    bool isPresent() const;

    //! \brief Returns whether hardware keyboard is opened.
    bool isOpen() const;

Q_SIGNALS:
    //! \brief Emitted whenever the hardware keyboard state changed.
    void stateChanged();

private:
    MImHwKeyboardTracker();
    virtual ~MImHwKeyboardTracker();
};
//! \internal_end

#endif // MIMKEYBOARDSTATETRACKER_H
