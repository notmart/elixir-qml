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
#include <string>

#include <QApplication>
#include <QQmlContext>
#include <QDebug>

#include "nifpp.h"

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

QApplication Application::s_app(num, makeArgV(num, "App"));

Application::Application(QObject *parent)
    : QObject(parent)
{
    
}

Application::~Application()
{
}

void Application::send(const QString &text)
{
    ErlNifEnv* env = enif_alloc_env();

    //pid is an HACK

    enif_send(NULL, pid, env, nifpp::make(env, std::string(text.toUtf8().constData())));

    enif_free_env(env);
}

void Application::processEvents()
{
    s_app.exec();
    s_app.processEvents();
}

int Application::exec(const QString &path)
{
    if (!m_engine) {
        m_engine = new QQmlApplicationEngine(this);
    }

    m_engine->rootContext()->setContextProperty("hack", this);
    m_engine->load(path);
    //s_app.exec();
    /*
    const int ret = s_app.exec();
    delete m_engine;
    return ret;*/
}

#include "moc_application.cpp"
