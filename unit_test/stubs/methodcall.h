#ifndef STUBMETHOD_H
#define STUBMETHOD_H

class MethodCall
{
public:
    MethodCall(QString name, QList<ParameterBase *> params, ParameterBase *returnValue)
        : _name(name),
          _params(params),
          _returnValue(returnValue) {
    }

    virtual ~MethodCall() {
        foreach(ParameterBase * p, _params) {
            delete p;
        }
    }

    QString name() {
        return _name;
    }

    QList<ParameterBase *> params() {
        return _params;
    }

    template <typename T>
    T parameter(int number) {
        if (number >= _params.count()) {
            qDebug() << "MethodCall::" << __func__ << ": method " << _name << " does not have parameter #" << number
                     << ". Check your test code.";
        }
        Parameter<T>* param = dynamic_cast<Parameter<T>* >(_params[number]);
        if (!param) {
            qDebug() << "MethodCall::" << __func__ << ": failed dynamic_cast, check that parameter type matches parameter number";
        }
        return param->data;
    }

    template <typename T>
    T returnValue() {
        Parameter<T>* value = dynamic_cast<Parameter<T>*>(_returnValue);

        if (!value) {
            qDebug() << "MethodCall::" << __func__ << ": failed dynamic_cast, check that type matches return value";
        }
        return value->data;
    }

    bool returnValueExists() {
        return (_returnValue != NULL);
    }

private:
    QString _name;
    QList<ParameterBase *> _params;
    ParameterBase *_returnValue;

};

#endif
