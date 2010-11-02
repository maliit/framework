#ifndef CONTEXTPROPERTYSTUB_H
#define CONTEXTPROPERTYSTUB_H

#include <contextproperty.h>

/*!
 * Stub ContextProperty to avoid the crash which is caused by ContextProperty trying to
 * use real QDBus.
 *
 * Further information: ut_minputcontext has dependency to libmeegotouch, and libmeegotouch
 * has depenency to contextsubscriber.
 */
ContextProperty::ContextProperty(const QString &key, QObject *parent)
{
    Q_UNUSED(key);
    Q_UNUSED(parent);
}

ContextProperty::~ContextProperty()
{
}

QString ContextProperty::key() const
{
    return QString();
}

QVariant ContextProperty::value(const QVariant &def) const
{
    return def;
}

QVariant ContextProperty::value() const
{
    return QVariant();
}

void ContextProperty::subscribe() const
{
}

void ContextProperty::unsubscribe() const
{
}

void ContextProperty::waitForSubscription() const
{
}

void ContextProperty::ignoreCommander()
{
}

void ContextProperty::setTypeCheck(bool typeCheck)
{
    Q_UNUSED(typeCheck);
}

#endif
