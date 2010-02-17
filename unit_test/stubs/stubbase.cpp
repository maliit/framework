#include <assert.h>
#include "stubbase.h"
#include "methodcall.h"

QList<MethodCall *> StubBase::stubCallHistory() const
{
    return _stubCallHistory;
}

void StubBase::stubReset() const
{
    foreach(ParameterBase * p, _stubReturnValues) {
        delete p;
    }

    foreach(MethodCall * p, _stubCallHistory) {
        delete p;
    }

    _stubReturnValues.clear();
    _stubCallCounts.clear();
    _stubCallHistory.clear();
}

int StubBase::stubCallCount(QString method) const
{
    return _stubCallCounts[method];
}

void StubBase::stubMethodEntered(QString methodName, QList<ParameterBase *> params) const
{
    MethodCall *method = new MethodCall(methodName, params, stubReturnValue(methodName));
    _stubCallCounts[methodName] = _stubCallCounts[methodName] + 1;
    _stubCallHistory.append(method);
}

void StubBase::stubMethodEntered(QString methodName) const
{
    MethodCall *method = new MethodCall(methodName, QList<ParameterBase *>(), stubReturnValue(methodName));
    _stubCallCounts[methodName] = _stubCallCounts[methodName] + 1;
    _stubCallHistory.append(method);
}

ParameterBase *StubBase::stubReturnValue(QString methodName) const
{
    ParameterBase *retVal = NULL;

    if (_stubReturnValues.contains(methodName))
        retVal = _stubReturnValues[methodName];

    return retVal;
}

StubBase::~StubBase()
{
    stubReset();
}

MethodCall &StubBase::stubLastCall() const
{
    return *(_stubCallHistory.last());
}

MethodCall &StubBase::stubLastCallTo(QString method) const
{
    for (int i = _stubCallHistory.count() - 1; i >= 0; i--) {
        if (_stubCallHistory.at(i)->name() == method) {
            return *(_stubCallHistory.at(i));
        }
    }
    qDebug() << "StubBase::lastCallTo: call not found to:" << method;
    return *((MethodCall *)0);
}

