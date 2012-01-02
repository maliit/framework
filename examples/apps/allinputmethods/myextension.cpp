#include "myextension.h"

namespace
{
    const char * const loadAll("/inputMethod/loadAll");
}

MyExtension::MyExtension(const QString &label)
    : QObject()
    , m_extension(new Maliit::AttributeExtension)
    , m_label(label)
{}

int MyExtension::id() const
{
    return m_extension->id();
}

void MyExtension::enableAllSubviews(bool enable)
{
    m_extension->setAttribute(loadAll, enable);
}
