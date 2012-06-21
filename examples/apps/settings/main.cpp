/* This file is part of Maliit framework
 *
 * Copyright (C) 2012 Mattia Barbon <mattia@develer.com>
 *
 * Contact: maliit-discuss@lists.maliit.org
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the licence, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "mainwindow.h"

#include <QApplication>
#if (defined(Q_WS_QPA) || defined(Q_WS_QWS)) && (QT_VERSION < 0x050000)
#include <QInputContextFactory>
#endif


int main(int argc, char **argv)
{
    QApplication kit(argc, argv);
#if (defined(Q_WS_QPA) || defined(Q_WS_QWS)) && (QT_VERSION < 0x050000)
    // Workaround for lighthouse Qt
    kit.setInputContext(QInputContextFactory::create("Maliit", &kit));
#endif
    MainWindow settings;

    settings.show();
    kit.exec();
}
