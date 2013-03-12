/*
 *   Copyright 2011 Marco Martin <mart@kde.org>
 *   Copyright 2013 Sebastian Kügler <sebas@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation; either version 2.1 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .
 */

#ifndef TOOLTIP_P_H
#define TOOLTIP_P_H

#include <QObject>
#include <QQuickItem>
#include <QQuickWindow>
#include <QPoint>
#include <QPointer>


class Tooltip : public QQuickWindow
{
    Q_OBJECT

    /**
     * The QML item on which this tooltip will be positioned relative to
     */
    Q_PROPERTY(QQuickItem *visualParent READ visualParent WRITE setVisualParent NOTIFY visualParentChanged)

    /**
     * Visibility of the Tooltip window.
     */
    Q_PROPERTY(bool visible READ isVisible WRITE setVisible NOTIFY visibleChanged)

    /**
     * Alignment of the tooltip relative to visualParent
     */
    Q_PROPERTY(int alignment READ alignment WRITE setAlignment NOTIFY alignmentChanged)

public:


    Tooltip(QQuickItem *parent = 0);
    ~Tooltip();

    QQuickItem *visualParent() const;
    void setVisualParent(QQuickItem *visualParent);

    bool isVisible() const;
    void setVisible(const bool visible);

    int alignment() const;
    void setAlignment(int alignment);

protected:
    /**
     * @returns The suggested screen position for the popup
     * @arg item the item the popup has to be positioned relatively to. if null, the popup will be positioned in the center of the window
     * @arg alignment alignment of the popup compared to the item
     */
    //FIXME: alignment should be Qt::AlignmentFlag
    QPoint popupPosition(QQuickItem *item, Qt::AlignmentFlag alignment=Qt::AlignCenter);
    void syncNullInputMask();

Q_SIGNALS:
    void visibleChanged();
    void alignmentChanged();
    void visualParentChanged();

private:
    Qt::AlignmentFlag m_alignment;
    QPointer<QQuickItem> m_visualParent;
};

#endif
