#include "mimsubviewdescription.h"

//! \internal
class MImSubViewDescriptionPrivate
{
public:
    //! Construct new instance.
    MImSubViewDescriptionPrivate(const QString &pluginId,
                                 const QString &subViewId,
                                 const QString &subViewTitle);

    friend bool operator==(const MImSubViewDescriptionPrivate &left,
                           const MImSubViewDescriptionPrivate &right);

public:
    //! Plugin ID.
    QString pluginId;

    //! Subview ID
    QString id;

    //! Subview title
    QString title;
};
//! \internal_end


MImSubViewDescriptionPrivate::MImSubViewDescriptionPrivate(const QString &pluginId,
                                                           const QString &subViewId,
                                                           const QString &subViewTitle)
    : pluginId(pluginId),
    id(subViewId),
    title(subViewTitle)
{
}

bool operator==(const MImSubViewDescriptionPrivate &left,
                const MImSubViewDescriptionPrivate &right)
{
    return (left.pluginId == right.pluginId && left.id == right.id && left.title == right.title);
}

MImSubViewDescription::MImSubViewDescription(const QString &pluginId,
                                             const QString &subViewId,
                                             const QString &subViewTitle)
    : d_ptr(new MImSubViewDescriptionPrivate(pluginId, subViewId, subViewTitle))
{
}

MImSubViewDescription::MImSubViewDescription(const MImSubViewDescription &other)
    : d_ptr(new MImSubViewDescriptionPrivate(*other.d_ptr))
{
}

MImSubViewDescription::~MImSubViewDescription()
{
    delete d_ptr;
}

void MImSubViewDescription::operator=(const MImSubViewDescription &other)
{
    *d_ptr = *other.d_ptr;
}

bool operator==(const MImSubViewDescription &left, const MImSubViewDescription &right)
{
    return *left.d_ptr == *right.d_ptr;
}

QString MImSubViewDescription::pluginId() const
{
    Q_D(const MImSubViewDescription);

    return d->pluginId;
}

QString MImSubViewDescription::id() const
{
    Q_D(const MImSubViewDescription);

    return d->id;
}

QString MImSubViewDescription::title() const
{
    Q_D(const MImSubViewDescription);

    return d->title;
}
