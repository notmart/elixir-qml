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

#include "erl_nif.h"
#include "application.h"

#include <QDebug>

extern "C" {

Application *s_application = nullptr;

ErlNifPid* s_pid;

Application *Application::self()
{
    return s_application;
}

static ERL_NIF_TERM register_application_server(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ErlNifPid* pid = (ErlNifPid*) enif_alloc(sizeof(ErlNifPid));
    s_pid = enif_self(env, pid);
    
    return argv[0];
}

static ERL_NIF_TERM register_qml_channel(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    if (!s_application) {
        return argv[0];
    }

    if (argc != 1) {
        return enif_make_badarg(env);
    }

    int identifier;
    enif_get_int(env, argv[0], &identifier);

    ErlNifPid* pid = (ErlNifPid*) enif_alloc(sizeof(ErlNifPid));
    pid = enif_self(env, pid);
    s_application->registerQmlChannel(identifier, pid);
    return argv[0];
}

static ERL_NIF_TERM write_property(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    if (!s_application) {
        return argv[0];
    }

    if (argc != 3) {
        return enif_make_badarg(env);
    }

    int channelId;
    enif_get_int(env, argv[0], &channelId);

    std::string property = nifpp::get<std::string>(env, argv[1]);
    //TODO: support multiple types
    QString value = QString::fromUtf8(nifpp::get<std::string>(env, argv[2]).c_str());

    auto *channel = s_application->channel(channelId);
    if (channel) {
        channel->bridge()->receiveProperty(property.c_str(), value);
    }
    return argv[0];
}

static ERL_NIF_TERM exec(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    if (s_application) {
        qWarning() << "QApplication already running";
        return argv[0];
    }
    qWarning() << "CALLING EXEC!";
    if (argc != 1) {
        return enif_make_badarg(env);
    }

    s_application = new Application(s_pid);

    ErlNifBinary path_bin;

    if (!enif_inspect_iolist_as_binary(env, argv[0], &path_bin)) {
        return enif_make_badarg(env);
    }

    char *path = strndup((char*) path_bin.data, path_bin.size);

    const int ret = s_application->exec(path);

    delete s_application;

    return enif_make_int(env, ret);
}

static ErlNifFunc nif_funcs[] =
{
    {"register_application_server", 0, register_application_server},
    {"register_qml_channel", 1, register_qml_channel},
    {"write_property", 3, write_property},
    {"exec", 1, exec, ERL_NIF_DIRTY_JOB_CPU_BOUND }
};

ERL_NIF_INIT(Elixir.QML.Private,nif_funcs,NULL,NULL,NULL,NULL)

}
