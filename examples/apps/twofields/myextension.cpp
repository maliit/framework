#include "myextension.h"

namespace
{
    const char * const actionKeyString("/keys/actionKey/label");
}

MyExtension::MyExtension(const QString &label)
: QObject(),
  m_extension(new Maliit::AttributeExtension),
  m_label(label)
{}

int MyExtension::id() const
{
    return m_extension->id();
}

void MyExtension::overrideLabel(bool override)
{
    if (override) {
        m_extension->setAttribute(actionKeyString, m_label);
    } else {
        m_extension->setAttribute(actionKeyString, QString());
    }
}
