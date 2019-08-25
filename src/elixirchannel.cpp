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
#include <string>

#include <QDebug>


ElixirChannel::ElixirChannel(QObject *parent)
    : QObject(parent)
{
    connect(this, &ElixirChannel::testChanged, this, [this]() {
        ErlNifEnv* env = enif_alloc_env();
    
        enif_send(NULL, m_pid, env, nifpp::make(env,  std::make_tuple(nifpp::str_atom("$gen_cast"), std::make_tuple(nifpp::str_atom("property"), std::string("test"), std::string(m_test.toUtf8().constData())))));
    
        enif_free_env(env);
    });
}

ElixirChannel::~ElixirChannel()
{
}

void ElixirChannel::setPid(ErlNifPid *pid)
{
    if (pid == m_pid) {
        return;
    }

    m_pid = pid;
}

ErlNifPid *ElixirChannel::pid() const
{
    return m_pid;
}

void ElixirChannel::send(const QString &text)
{
    ErlNifEnv* env = enif_alloc_env();

    enif_send(NULL, m_pid, env, nifpp::make(env, std::string(text.toUtf8().constData())));

    enif_free_env(env);
}


#include "moc_elixirchannel.cpp"
