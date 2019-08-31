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


ElixirChannel::ElixirChannel(QObject *parent)
    : QObject(parent)
{
  /*  connect(this, &ElixirChannel::testChanged, this, [this]() {
        ErlNifEnv* env = enif_alloc_env();

       // enif_send(NULL, m_pid, env, nifpp::make(env,  std::make_tuple(nifpp::str_atom("$gen_call"), std::make_tuple(nifpp::str_atom("property"), std::string("test"), std::string(m_test.toUtf8().constData())))));

        enif_send(NULL, m_pid, env, nifpp::make(env,  std::make_tuple(nifpp::str_atom("property"), std::string("test"), std::string(m_test.toUtf8().constData()))));
    
        enif_free_env(env);
    });*/
}

ElixirChannel::~ElixirChannel()
{
}

void ElixirChannel::classBegin()
{
}

void ElixirChannel::componentComplete()
{
    QString qml = QStringLiteral("import QtQuick 2.6\n"
                                 "QtObject {"
                                      "property QtObject parent;");
    for (int i = 0; i < metaObject()->propertyCount(); ++i) {
        const QMetaProperty p = metaObject()->property(i);
        if (p.hasNotifySignal()) {
            QString capNotifySignal = p.notifySignal().name();
            capNotifySignal[0] = capNotifySignal[0].toUpper();

            qml += QStringLiteral("property var ") + p.name() + ": parent." + p.name()+"; ";
            qml += QStringLiteral("on") + capNotifySignal + ": parent.sendProperty(\"" + p.name() + "\", parent." + p.name() + "); ";
        }
        sendProperty(p.name(), p.read(this));
    }
    qml += QStringLiteral("}");

    qWarning() << qml;
    QQmlEngine *engine = qmlEngine(this);
    QQmlContext *context = qmlContext(this);
    qWarning()<<engine<<context;
    Q_ASSERT(engine);
    Q_ASSERT(context);

    QQmlComponent spyComponent(engine);
    spyComponent.setData(qml.toUtf8(), QUrl());
    m_metaObjectSpy = spyComponent.beginCreate(context);
    qWarning()<<m_metaObjectSpy<<spyComponent.errors();
    Q_ASSERT(m_metaObjectSpy);
    m_metaObjectSpy->setProperty("parent", QVariant::fromValue(this));
    spyComponent.completeCreate();    
}

void ElixirChannel::setPid(ErlNifPid *pid)
{
    if (pid == m_pid) {
        return;
    }

    m_pid = pid;

    for (int i = 0; i < metaObject()->propertyCount(); ++i) {
        const QMetaProperty p = metaObject()->property(i);
        sendProperty(p.name(), p.read(this));
    }
}

ErlNifPid *ElixirChannel::pid() const
{
    return m_pid;
}

void ElixirChannel::setIdentifier(const QString &identifier)
{
    if (m_identifier == identifier) {
        return;
    }

    //ability to change it on the fly?
    m_identifier = identifier;
    Application::self()->registerElixirChannel(identifier, this);

    emit identifierChanged();
}

QString ElixirChannel::identifier() const
{
    return m_identifier;
}


void ElixirChannel::sendProperty(const QString &property, const QVariant &value)
{
    if (!m_pid) {
        return;
    }

    ErlNifEnv* env = enif_alloc_env();

    // enif_send(NULL, m_pid, env, nifpp::make(env,  std::make_tuple(nifpp::str_atom("$gen_call"), std::make_tuple(nifpp::str_atom("property"), std::string("test"), std::string(m_test.toUtf8().constData())))));

    enif_send(NULL, m_pid, env, nifpp::make(env,  std::make_tuple(nifpp::str_atom("property"), std::string(property.toUtf8().constData()), std::string(value.toString().toUtf8().constData()))));

    enif_free_env(env);
}

void ElixirChannel::send(const QString &text)
{
    ErlNifEnv* env = enif_alloc_env();

    enif_send(NULL, m_pid, env, nifpp::make(env, std::string(text.toUtf8().constData())));

    enif_free_env(env);
}


#include "moc_elixirchannel.cpp"
