/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2015 LXQt team
 * Authors:
 *  Balázs Béla <balazsbela[at]gmail.com>
 *  Paulo Lieuthier <paulolieuthier@gmail.com>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include "statusnotifierwidget.h"
#include <QApplication>
#include <QDebug>
#include "../panel/iukuipanelplugin.h"
#include "../panel/customstyle.h"

StatusNotifierWidget::StatusNotifierWidget(IUKUIPanelPlugin *plugin, QWidget *parent) :
    QWidget(parent),
    mPlugin(plugin)
{
    QString dbusName = QString("org.kde.StatusNotifierHost-%1-%2").arg(QApplication::applicationPid()).arg(1);
    if (!QDBusConnection::sessionBus().registerService(dbusName))
        qDebug() << QDBusConnection::sessionBus().lastError().message();

    mWatcher = new StatusNotifierWatcher;
    mWatcher->RegisterStatusNotifierHost(dbusName);

    connect(mWatcher, &StatusNotifierWatcher::StatusNotifierItemRegistered,
            this, &StatusNotifierWidget::itemAdded);
    connect(mWatcher, &StatusNotifierWatcher::StatusNotifierItemUnregistered,
            this, &StatusNotifierWidget::itemRemoved);

    setLayout(new UKUi::GridLayout(this));
    realign();

    qDebug() << mWatcher->RegisteredStatusNotifierItems();

}

StatusNotifierWidget::~StatusNotifierWidget()
{
    delete mWatcher;
}

void StatusNotifierWidget::itemAdded(QString serviceAndPath)
{
    int slash = serviceAndPath.indexOf('/');
    QString serv = serviceAndPath.left(slash);
    QString path = serviceAndPath.mid(slash);
    StatusNotifierButton *button = new StatusNotifierButton(serv, path, mPlugin, this);

    mServices.insert(serviceAndPath, button);
    mStatusNotifierButtons.append(button);
    button->setStyle(new CustomStyle);
    layout()->addWidget(button);
    button->show();
}

void StatusNotifierWidget::itemRemoved(const QString &serviceAndPath)
{
    StatusNotifierButton *button = mServices.value(serviceAndPath, NULL);
    if (button)
    {
        mStatusNotifierButtons.removeOne(button);
        button->deleteLater();
        layout()->removeWidget(button);
    }
}

void StatusNotifierWidget::realign()
{
    UKUi::GridLayout *layout = qobject_cast<UKUi::GridLayout*>(this->layout());
    layout->setEnabled(false);

    IUKUIPanel *panel = mPlugin->panel();
    if (panel->isHorizontal())
    {
        layout->setRowCount(panel->lineCount());
        layout->setColumnCount(0);
    }
    else
    {
        layout->setColumnCount(panel->lineCount());
        layout->setRowCount(0);
    }

    for(int i=0;i<mStatusNotifierButtons.size();i++)
    {
        if(mStatusNotifierButtons.at(i))
        {
            mStatusNotifierButtons.at(i)->setFixedSize(mPlugin->panel()->iconSize(),mPlugin->panel()->panelSize());
            mStatusNotifierButtons.at(i)->setIconSize(QSize(mPlugin->panel()->iconSize()/2,mPlugin->panel()->iconSize()/2));
        }
        else
        {
            qDebug()<<"mTrayIcons add error   :  "<<mStatusNotifierButtons.at(i);
        }
    }
    layout->setEnabled(true);
}
