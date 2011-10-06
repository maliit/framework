#ifndef MY_EXTENSION_H
#define MY_EXTENSION_H

#include <QObject>
#include <QScopedPointer>
#include <QString>

#include <maliit/attributeextension.h>

class MyExtension : public QObject
{
    Q_OBJECT

public:
    MyExtension(const QString &label);
    int id() const;

public Q_SLOTS:
    void overrideLabel(bool override);

private:
    QScopedPointer<Maliit::AttributeExtension> m_extension;
    QString m_label;
};

#endif // MY_EXTENSION_H
