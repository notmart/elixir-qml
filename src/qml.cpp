 
#include "erl_nif.h"
#include "application.h"

#include <QDebug>

extern "C" {

Application *s_application = nullptr;

ErlNifPid* s_pid;

static ERL_NIF_TERM register_application_server(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
   // qWarning() << "WASSSUUUUUP!";
    ErlNifPid* pid = (ErlNifPid*) enif_alloc(sizeof(ErlNifPid));
    s_pid = enif_self(env, pid);
    

    return argv[0];
}

static ERL_NIF_TERM hello(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    if (!s_application) {
        return argv[0];
    }
    qWarning() << "WASSSUUUUUP!";
    s_application->setProperty("test", "HELLO FROM ELIXIR");
    return enif_make_string(env, "Hello world!", ERL_NIF_LATIN1);
}

static ERL_NIF_TERM exec(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    qWarning() << "CALLING EXEC!";
    if (argc != 1) {
        return enif_make_badarg(env);
    }

    s_application = new Application;

    ErlNifBinary path_bin;

    if (!enif_inspect_iolist_as_binary(env, argv[0], &path_bin)) {
        return enif_make_badarg(env);
    }

    //here because can't touch a qobject before the app is exec'd
    s_application->pid = s_pid;
    char *path = strndup((char*) path_bin.data, path_bin.size);

    const int ret = s_application->exec(path);

    delete s_application;
qWarning() << "Application is dead";
    return enif_make_int(env, ret);
}

static ErlNifFunc nif_funcs[] =
{
    {"hello", 0, hello},
    {"register_application_server", 0, register_application_server},
    {"exec", 1, exec, ERL_NIF_DIRTY_JOB_CPU_BOUND }
};

ERL_NIF_INIT(Elixir.QML.Private,nif_funcs,NULL,NULL,NULL,NULL)

}
