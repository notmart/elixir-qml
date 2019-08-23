/*
*   Copyright (C) 2019 by Marco Martin <mart@kde.org>
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU Library General Public License as
*   published by the Free Software Foundation; either version 2, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU Library General Public License for more details
*
*   You should have received a copy of the GNU Library General Public
*   License along with this program; if not, write to the
*   Free Software Foundation, Inc.,
*   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#pragma once

#include <QQmlApplicationEngine>
#include <QApplication>
#include <QPointer>
#include <QObject>

#include "erl_nif.h"
#include "nifpp.h"

class QQuickItem;


class Application : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString test MEMBER m_test NOTIFY testChanged)

public:

    explicit Application(QObject *parent = nullptr);
    ~Application() override;

    int exec(const QString &path);

    Q_INVOKABLE void send(const QString &text);

    ErlNifPid *pid;

Q_SIGNALS:
    void testChanged();

private:
  //  static QApplication s_app;
    QString m_test;
    QPointer<QQmlApplicationEngine> m_engine;
};

