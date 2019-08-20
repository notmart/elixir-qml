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
#include <QDebug>

char **new_argv(int count, ...)
{
    va_list args;
    int i;
    char **argv = malloc((count+1) * sizeof(char*));
    char *temp;
    va_start(args, count);
    for (i = 0; i < count; i++) {
        temp = va_arg(args, char*);
        argv[i] = malloc(sizeof(temp));
        argv[i] = temp;
    }
    argv[i] = NULL;
    va_end(args);
    return argv;
}

Application::Application(QObject *parent)
    : QObject(parent)
{
    int num = 1;
    m_app = new QApplication(num, new_argv(num, "App"));
}

Application::~Application()
{
}

int Application::exec(const QString &path)
{
    if (!m_engine) {
        m_engine = new QQmlApplicationEngine(this);
    }

    m_engine->load(path);
    return m_app->exec();
}

#include "moc_application.cpp"
