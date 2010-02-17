#ifndef STUBBASE_H
#define STUBBASE_H

#include <QList>
#include <QVariant>
#include <QMap>
#include <QtDebug>
#include "parameter.h"
#include "methodcall.h"


class StubBase
{
public:
    // Return a list of method calls
    QList<MethodCall *> stubCallHistory() const;

    // Return a reference to the last method call
    MethodCall &stubLastCall() const;

    // Access parameters of the last method call
    template <typename T>
    T stubLastParameters(int number) const;

    // Access parameters of the last method call
    MethodCall &stubLastCallTo(QString method) const;

    // Return the number of times a method has been called
    int stubCallCount(QString method) const;

    // Set the return value for next call of methodName
    template <typename T>
    void stubSetReturnValue(QString methodName, T value) const;

    // Return the return value set for methodName
    template <typename T>
    T &stubReturnValue(QString methodName) const;



    // For use by stubs
    virtual ~StubBase();
    void stubReset() const;
    ParameterBase *stubReturnValue(QString methodName) const;
    void stubMethodEntered(QString methodName, QList<ParameterBase *> params) const;
    void stubMethodEntered(QString methodName) const;

private:
    mutable QMap<QString, ParameterBase *> _stubReturnValues;
    mutable QMap<QString, int> _stubCallCounts;
    mutable QList<MethodCall *> _stubCallHistory;

};

template <typename T>
void StubBase::stubSetReturnValue(QString methodName, T value) const
{
    Parameter<T>* param = new Parameter<T>(value);
    _stubReturnValues[methodName] = param;
}

template <typename T>
T &StubBase::stubReturnValue(QString methodName) const
{
    if (! _stubReturnValues.contains(methodName)) {
        stubSetReturnValue<T>(methodName, T());
    }

    ParameterBase *base = _stubReturnValues[methodName];
    Parameter<T>* param = dynamic_cast<Parameter<T>*>(base);
    if (!param) {
        qDebug() << "StubBase::" << __func__ << ": failed dynamic_cast, check that return value type matches the method; check also that you have used stubSetReturnValue(" << methodName << ")";
    }
    return param->data;

}

template <typename T>
T StubBase::stubLastParameters(int number) const
{
    MethodCall &call = stubLastCall();
    return call.parameter<T>(number);
}

#endif
