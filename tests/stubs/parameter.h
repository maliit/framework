#ifndef PARAMETER_H
#define PARAMETER_H

class ParameterBase
{
public:
    virtual ~ParameterBase() {
        // Class needs to have at least one virtual function to be polymorphic
        // (and thus enable dynamic_cast)
    }
protected:
    ParameterBase() {
    }
};

template <typename T>
class Parameter : public ParameterBase
{
public:
    Parameter(T value) : ParameterBase(), data(value) {
    }
    T data;
};
#endif
