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


Application::Application(QObject *parent)
    : QObject(parent)
{
}

Application::~Application()
{
}

int Application::exec(const QString &path)
{
    std::string name = "App";
    std::vector<char*> argv;
    argv.push_back((char*)name.data());
    argv.push_back(nullptr);

    int num = 1;
    QApplication app(num, argv.data());

    Q_ASSERT(!m_engine);
    m_engine = new QQmlApplicationEngine(this);

    m_engine->load(path);
    const int ret = app.exec();
    delete m_engine;
    return ret;
}

#include "moc_application.cpp"
