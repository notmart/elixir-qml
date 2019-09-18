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

#include "elixirchannel.h"
#include "application.h"

#include <string>

#include <QQmlEngine>
#include <QQmlContext>
#include <QQmlComponent>
#include <QDebug>

int ElixirChannel::s_maxIdentifier = 0;

PropertyBridge::PropertyBridge(ElixirChannel *parent)
    : QObject(parent),
      m_channel(parent)
{
    connect(this, &PropertyBridge::receiveProperty, this, &PropertyBridge::propertyReceived);

    // Ugly generated QML wrapper for instrading properties
    QString qml = QStringLiteral("import QtQuick 2.10\n"
                                 "QtObject {"
                                      "property QtObject parent;"
                                      "property QtObject channel;");
    QSet<QByteArray> propertySignals;
    for (int i = 0; i < m_channel->metaObject()->propertyCount(); ++i) {
        const QMetaProperty p = m_channel->metaObject()->property(i);

        if (p.name() != "typeId" && p.hasNotifySignal()) {
            QString capNotifySignal = p.notifySignal().name();
            capNotifySignal[0] = capNotifySignal[0].toUpper();

            qml += QStringLiteral("property var ") + p.name() + ": channel." + p.name()+"; ";
            qml += QStringLiteral("on") + capNotifySignal + ": parent.sendProperty(\"" + p.name() + "\", channel." + p.name() + "); ";

            propertySignals.insert(p.notifySignal().name());
        }
        sendProperty(p.name(), p.read(m_channel));
    }

    QSet<QByteArray> methods;
    qml += QStringLiteral("Component.onCompleted: {");
    for (int i = 0; i < m_channel->metaObject()->methodCount(); ++i) {
        const QMetaMethod m = m_channel->metaObject()->method(i);
        if (m.methodType() == QMetaMethod::Signal &&
            m.name() != "typeId" &&
            m.name() != "destroyed" &&
            !propertySignals.contains(m.name()) &&
            !methods.contains(m.name())) {

            qml += QStringLiteral("channel.") + m.name() + ".connect(function(";
            bool first = true;
            for (const auto &p : m.parameterNames()) {
                if (!first) {
                    qml += QStringLiteral(",");
                }
                first = false;
                qml += QString(p);
            }
            qml += "){parent.sendSignal(\"" + m.name() + "\", [";
            first = true;
            for (const auto &p : m.parameterNames()) {
                if (!first) {
                    qml += QStringLiteral(",");
                }
                first = false;
                qml += QString(p);
            }
            qml += "]);});";

            methods.insert(m.name());
        }
    }

    qml += QStringLiteral("}}");

    qWarning() << qml;
    QQmlEngine *engine = qmlEngine(m_channel);
    QQmlContext *context = qmlContext(m_channel);
    Q_ASSERT(engine);
    Q_ASSERT(context);

    QQmlComponent spyComponent(engine);
    spyComponent.setData(qml.toUtf8(), QUrl());
    m_metaObjectSpy = spyComponent.beginCreate(context);
    Q_ASSERT(m_metaObjectSpy);
    m_metaObjectSpy->setProperty("parent", QVariant::fromValue(this));
    m_metaObjectSpy->setProperty("channel", QVariant::fromValue(m_channel));
    spyComponent.completeCreate();
    qWarning()<<"AAAAAAA";
}

PropertyBridge::~PropertyBridge()
{
}

void PropertyBridge::propertyReceived(const QString &property, const QVariant &value)
{
    m_channel->setProperty(property.toUtf8(), value);
}

void PropertyBridge::sendProperty(const QString &property, const QVariant &value)
{
    if (!m_channel->pid()) {
        return;
    }

    ErlNifEnv* env = enif_alloc_env();
/*
    enif_send(NULL, m_channel->pid(), env,
        nifpp::make(env,
            std::make_tuple(nifpp::str_atom("$gen_call"), 
                std::make_tuple(
                    nifpp::str_atom("property"),
                    QStringLiteral("test"),
                    value))));
*/
    enif_send(NULL, m_channel->pid(), env, nifpp::make(env,  std::make_tuple(nifpp::str_atom("changeProperty"),
        property,
        value)));

    enif_free_env(env);
}

void PropertyBridge::sendSignal(const QString &name, const QVariantList &params)
{
    if (!m_channel->pid()) {
        return;
    }

    ErlNifEnv* env = enif_alloc_env();

    enif_send(NULL, m_channel->pid(), env, nifpp::make(env,  std::make_tuple(nifpp::str_atom("signalFromQml"),
        name,
        params.toStdList())));

    enif_free_env(env);
}



///////////////////////////////////////////////

ElixirChannel::ElixirChannel(QObject *parent)
    : QObject(parent)
{
    m_identifier = s_maxIdentifier++;
}

ElixirChannel::~ElixirChannel()
{
}

void ElixirChannel::classBegin()
{
}

void ElixirChannel::componentComplete()
{
    m_propertyBridge = new PropertyBridge(this);  
}

void ElixirChannel::setPid(ErlNifPid *pid)
{
    if (pid == m_pid) {
        return;
    }

    m_pid = pid;

    if (m_propertyBridge) {
        for (int i = 0; i < metaObject()->propertyCount(); ++i) {
            const QMetaProperty p = metaObject()->property(i);
            m_propertyBridge->sendProperty(p.name(), p.read(this));
        }
    }
}

ErlNifPid *ElixirChannel::pid() const
{
    return m_pid;
}

void ElixirChannel::setTypeId(const QString &typeId)
{
    if (m_typeId == typeId) {
        return;
    }
    if (!m_typeId.isEmpty()) {
        qWarning() << "cannot rename an ElixirChannel type";
        return;
    }

    //ability to change it on the fly?
    m_typeId = typeId;
    Application::self()->registerElixirChannel(m_identifier, typeId, this);

    emit typeIdChanged();
}

QString ElixirChannel::typeId() const
{
    return m_typeId;
}

PropertyBridge *ElixirChannel::bridge() const
{
    return m_propertyBridge;
}

#include "moc_elixirchannel.cpp"
