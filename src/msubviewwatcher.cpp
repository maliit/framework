#include "msubviewwatcher.h"

MSubViewWatcher::MSubViewWatcher(MInputContextConnection *newConnection,
                                 MImOnScreenPlugins * newOnScreenPlugins,
                                 QObject *parent) :
    QObject(parent)
    , connection(newConnection)
    , onScreenPlugins(newOnScreenPlugins)
    , connected(false)
{
    Q_FOREACH(const MImOnScreenPlugins::SubView & subView, newOnScreenPlugins->enabledSubViews()) {
        recordedSubViews.insert(subView);
    }

    connect(newOnScreenPlugins, SIGNAL(activeSubViewChanged()),
            this, SLOT(startWatching()));
}

MSubViewWatcher::~MSubViewWatcher()
{
    enableRecordedSubViews();
}

void MSubViewWatcher::startWatching()
{
    MInputContextConnection * const icConnection = connection.data();

    if (!icConnection || connected) {
        return;
    }
    connect(icConnection, SIGNAL(keyEventSent()),
            this, SLOT(recordActiveSubView()));

    connect(icConnection, SIGNAL(stringCommited()),
            this, SLOT(recordActiveSubView()));

    connected = true;
}

void MSubViewWatcher::recordActiveSubView()
{
    MImOnScreenPlugins * const onScreen = onScreenPlugins.data();

    if (!onScreen) {
        return;
    }

    MImOnScreenPlugins::SubView subView = onScreen->activeSubView();
    recordedSubViews.insert(subView);

    MInputContextConnection * const icConnection = connection.data();

    if (!icConnection) {
        return;
    }

    disconnect(icConnection, 0, this, 0);
    connected = false;
}

void MSubViewWatcher::enableRecordedSubViews()
{
    MImOnScreenPlugins * const onScreen = onScreenPlugins.data();

    if (!onScreen) {
        return;
    }

    QList<MImOnScreenPlugins::SubView> list;
    std::copy(recordedSubViews.begin(), recordedSubViews.end(),
              std::back_inserter(list));
    qSort(list);
    onScreen->setEnabledSubViews(list);
}
