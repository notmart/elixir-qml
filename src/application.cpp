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

#include "application.h"
#include "simpledatamodel.h"
#include <string>

#include <QApplication>
#include <QQmlContext>
#include <QDebug>

#include <stdarg.h>

char **makeArgV(int count, ...)
{
    va_list args;
    int i;
    char **argv = (char **)malloc((count+1) * sizeof(char*));
    char *temp;
    va_start(args, count);
    for (i = 0; i < count; i++) {
        temp = va_arg(args, char*);
        argv[i] = (char*)malloc(sizeof(temp));
        argv[i] = temp;
    }
    argv[i] = NULL;
    va_end(args);
    return argv;
}

int num = 1;

//QApplication Application::s_app(num, makeArgV(num, "App"));

Application::Application(ErlNifPid *pid, QObject *parent)
    : QObject(parent),
      m_pid(pid)
{
    qmlRegisterType<ElixirChannel>("qmlelixir", 1, 0, "ElixirChannel");
}

Application::~Application()
{
}

bool Application::registerQmlChannel(const QString &typeId, ErlNifPid *pid)
{
    m_elixirQmlChannels[typeId] = pid;

    if (m_qmlElixirChannels.contains(typeId)) {
        m_qmlElixirChannels[typeId]->setPid(pid);
    }
}

bool Application::registerElixirChannel(const QString &typeId, ElixirChannel *elixirChannel)
{
    if (m_qmlElixirChannels.contains(typeId)) {
        // TODO: is this too much? tough having a duplicate typeid is completely a no go
        qFatal(QStringLiteral("ElixirChannel typeId not unique: %1").arg(typeId).toUtf8());
        return false;
    }

    m_qmlElixirChannels[typeId] = elixirChannel;

    if (m_elixirQmlChannels.contains(typeId)) {
        elixirChannel->setPid(m_elixirQmlChannels[typeId]);
    }

    ErlNifEnv* env = enif_alloc_env();

    enif_send(NULL, m_pid, env, nifpp::make(env,  
        std::make_tuple(nifpp::str_atom("channel_registered"),        
            nifpp::str_atom(typeId.toUtf8()))));

    enif_free_env(env);

    connect(elixirChannel, &QObject::destroyed, this, [this, typeId]() {
        m_qmlElixirChannels.remove(typeId);

        ErlNifEnv* env = enif_alloc_env();

        enif_send(NULL, m_pid, env, nifpp::make(env,  
            std::make_tuple(nifpp::str_atom("channel_removed"), 
                typeId.toUtf8().constData())));

        enif_free_env(env);
    });
}

ElixirChannel *Application::channel(const QString &typeId) const
{
    return m_qmlElixirChannels.value(typeId);
}

void Application::send(const QString &text)
{
    ErlNifEnv* env = enif_alloc_env();

    enif_send(NULL, m_pid, env, nifpp::make(env, text));

    enif_free_env(env);
}

int Application::exec(const QString &path)
{
    int num=1;

    // TODO: App name from Elixir
    QApplication app(num, makeArgV(num, "App"));
    if (!m_engine) {
        m_engine = new QQmlApplicationEngine(this);
    }

    connect(m_engine, &QQmlApplicationEngine::warnings, this, [this](const QList<QQmlError> &warnings) {
        for (const auto &warn : warnings) {
            send(QStringLiteral("QML Engine encountered an error at line %1, column %2: %3").arg(warn.line()).arg(warn.column()).arg(warn.description()));
        }
    });

    connect(m_engine, &QQmlApplicationEngine::objectCreated, this, [this](QObject *object, const QUrl &url) {
        ErlNifEnv* env = enif_alloc_env();
        if (object) {
            enif_send(NULL, m_pid, env, nifpp::make(env,  std::make_tuple(nifpp::str_atom("loaded"), url.toString())));
        } else {
            enif_send(NULL, m_pid, env, nifpp::make(env, std::make_tuple(nifpp::str_atom("error"), url.toString())));
        }
        enif_free_env(env);
    });

    m_engine->rootContext()->setContextProperty("hack", this);
    m_engine->load(path);
    
    const int ret = app.exec();
    delete m_engine;
    return ret;
}

#include "moc_application.cpp"
