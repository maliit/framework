#ifndef MY_EXTENSION_H
#define MY_EXTENSION_H

#include <QObject>
#include <QScopedPointer>
#include <QString>

#include <maliit/attributeextension.h>

class MyExtension
    : public QObject
{
    Q_OBJECT

public:
    explicit MyExtension();
    int id() const;

    Q_SLOT void enableAllSubviews(bool enable);

private:
    QScopedPointer<Maliit::AttributeExtension> m_extension;
};

#endif // MY_EXTENSION_H
