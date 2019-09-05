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

#include <QObject>
#include <QQmlParserStatus>

#include "erl_nif.h"
#include "nifpp.h"

class QQuickItem;

class Application;

class ElixirChannel : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_PROPERTY(QString typeId READ typeId WRITE setTypeId NOTIFY typeIdChanged)

    Q_INTERFACES(QQmlParserStatus)

public:

    explicit ElixirChannel(QObject *parent = nullptr);
    ~ElixirChannel() override;

    void setPid(ErlNifPid *pid);
    ErlNifPid *pid() const;

    void setTypeId(const QString &typeId);
    QString typeId() const;

    Q_INVOKABLE void send(const QString &text);

    //TODO: put in an internal object not accessible from QML
    Q_INVOKABLE void sendProperty(const QString &property, const QVariant &value);
    Q_INVOKABLE void sendSignal(const QString &name, const QVariant &params);

protected:
    void classBegin() override;
    void componentComplete() override;

Q_SIGNALS:
    void testChanged();
    void typeIdChanged();

private:
    ErlNifPid *m_pid = nullptr;
    QString m_typeId;
    int m_identifier;
    QObject *m_metaObjectSpy = nullptr;
    static Application *s_spplication;
    friend class Application;

    static int s_maxIdentifier;
};

