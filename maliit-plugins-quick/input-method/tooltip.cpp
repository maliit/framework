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

#include "tooltip_p.h"

#include <QGuiApplication>
#include <QTimer>
#include <QScreen>

#include <QDebug>

#include <qpa/qplatformnativeinterface.h>
#include <xcb/xcb.h>
#include <xcb/xfixes.h>

Tooltip::Tooltip(QQuickItem *parent)
    : QQuickWindow(),
      m_alignment(Qt::AlignCenter)
{
    Q_UNUSED(parent);

    QSurfaceFormat format;
    format.setAlphaBufferSize(8);
    setFormat(format);
    setClearBeforeRendering(true);
    setColor(QColor(Qt::transparent));
    setFlags(Qt::ToolTip);
}

Tooltip::~Tooltip()
{
}

QQuickItem *Tooltip::visualParent() const
{
    return m_visualParent.data();
}

void Tooltip::setVisualParent(QQuickItem *visualParent)
{
    if (m_visualParent.data() == visualParent) {
        return;
    }

    if (visualParent) {
        setPosition(popupPosition(visualParent, m_alignment));
    }

    m_visualParent = visualParent;
    Q_EMIT visualParentChanged();
}

bool Tooltip::isVisible() const
{
    return QQuickWindow::isVisible();
}

void Tooltip::setVisible(const bool visible)
{
    //qDebug() << visible;
    QRect avail = screen()->availableGeometry();

    if (visible) {
        if (!m_visualParent.isNull()) {
            setPosition(popupPosition(m_visualParent.data(), m_alignment));
        } else {
            // no visual parent -> center on screen
            setPosition((avail.width() - width()) / 2, (avail.height() - height()) / 2);
        }
        raise();
    }
    QQuickWindow::setVisible(visible);
    syncNullInputMask();
}

QPoint Tooltip::popupPosition(QQuickItem *item, Qt::AlignmentFlag alignment)
{
    if (!item) {
        //If no item was specified try to align at the center of the parent view
        QQuickItem *parentItem = qobject_cast<QQuickItem *>(parent());
        if (parentItem && parentItem->window()) {
            //qDebug() << "NO visual parent ... Centering at " << (parentItem->window()->geometry().center() - QPoint(width()/2, height()/2));
            return parentItem->window()->geometry().center() - QPoint(width()/2, height()/2);
        } else {
            //qDebug() << "No QQuickItem as parent found";
            return QPoint();
        }
    }
    QRectF boundingSceneRect = item->mapRectToScene(item->boundingRect());
    QPointF pos = boundingSceneRect.center();
    //qDebug() << "I've an Item at " << pos;
    if (item->window() && item->window()->screen()) {
        pos = item->window()->mapToGlobal(pos.toPoint());
    } else {
        return QPoint();
    }

    QPoint topPoint(0, -height() - boundingSceneRect.height()/2);
    QPoint bottomPoint(0, boundingSceneRect.height()/2);
    QPoint leftPoint(-width() - boundingSceneRect.width()/2, 0);
    QPoint rightPoint(boundingSceneRect.width()/2, 0);

    QPoint offset(-width()/2, -height()/2);
    if (alignment & Qt::AlignBottom) {
        offset.setY(bottomPoint.y());
    } else if (alignment & Qt::AlignLeft) {
        offset.setX(leftPoint.x());
    } else if (alignment & Qt::AlignRight) {
        offset.setX(rightPoint.x());
    } else if (alignment & Qt::AlignTop) {
        offset.setY(topPoint.y());
    }

    const QRect avail = item->window()->screen()->availableGeometry();
    QPoint menuPos = pos.toPoint() + offset;

    if (menuPos.x() < 0) {
        // popup hits lhs
        if (alignment & Qt::AlignTop || alignment & Qt::AlignBottom) {
            // move it
            menuPos.setX(0);
        } else {
            // swap edge
            menuPos.setX(pos.x() + rightPoint.x());
        }
    }
    if (menuPos.x() + width() > avail.width()) {
        // popup hits rhs
        if (alignment & Qt::AlignTop || alignment & Qt::AlignBottom) {
            menuPos.setX(avail.width() - boundingSceneRect.width());
        } else {
            menuPos.setX(pos.x() + leftPoint.x());
        }
    }
    if (menuPos.y() < 0) {
        // hitting top
        if (alignment & Qt::AlignLeft || alignment & Qt::AlignRight) {
            menuPos.setY(0);
        } else {
            menuPos.setY(pos.y() + bottomPoint.y());
        }
    }
    if (menuPos.y() + height() > avail.height()) {
        // hitting bottom
        if (alignment & Qt::AlignTop || alignment & Qt::AlignBottom) {
            menuPos.setY(pos.y() + topPoint.y());
        } else {
            menuPos.setY(avail.height() - boundingSceneRect.height());
        }
        //qDebug() << menuPos;
    }

    //qDebug() << "Popup position" << menuPos;
    return menuPos;
}


int Tooltip::alignment() const
{
    return m_alignment;
}

void Tooltip::setAlignment(int align)
{
    Qt::AlignmentFlag alignment = (Qt::AlignmentFlag)align;
    if (m_alignment == alignment) {
        return;
    }

    m_alignment = alignment;
    Q_EMIT alignmentChanged();
}

//we need the tooltip window to be completely transparent to events, set a mask
void Tooltip::syncNullInputMask()
{
    if (QGuiApplication::platformName() != "xcb")
        return;

    QPlatformNativeInterface *nativeInterface = QGuiApplication::platformNativeInterface();
    xcb_connection_t *connection = static_cast<xcb_connection_t *>(nativeInterface->nativeResourceForWindow("connection", this));

    xcb_rectangle_t rect;
    rect.x = 0;
    rect.y = 0;
    rect.width = 0;
    rect.height = 0;

    xcb_xfixes_region_t region = xcb_generate_id(connection);
    xcb_xfixes_create_region(connection, region, 1, &rect);

    xcb_window_t window  = winId();
    xcb_xfixes_set_window_shape_region(connection, window, XCB_SHAPE_SK_BOUNDING, 0, 0, 0);
    xcb_xfixes_set_window_shape_region(connection, window, XCB_SHAPE_SK_INPUT, 0, 0, region);

    xcb_xfixes_destroy_region(connection, region);
}

#include "moc_tooltip_p.cpp"

