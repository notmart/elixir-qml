 
#include "erl_nif.h"
#include "application.h"

#include <QDebug>

extern "C" {

Q_GLOBAL_STATIC(Application, s_application)

ErlNifPid* s_pid;

static ERL_NIF_TERM register_application_server(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
   // qWarning() << "WASSSUUUUUP!";
    ErlNifPid* pid = (ErlNifPid*) enif_alloc(sizeof(ErlNifPid));
    s_pid = enif_self(env, pid);
    

    return argv[0];
}


static ERL_NIF_TERM exec(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    qWarning() << "CALLING EXEC!";
    if (argc != 1) {
        return enif_make_badarg(env);
    }

    ErlNifBinary path_bin;

    if (!enif_inspect_iolist_as_binary(env, argv[0], &path_bin)) {
        return enif_make_badarg(env);
    }

    //here because can't touch a qobject before the app is exec'd
    s_application->pid = s_pid;
    char *path = strndup((char*) path_bin.data, path_bin.size);

    return enif_make_int(env, s_application->exec(path));
}

static ErlNifFunc nif_funcs[] =
{
    {"register_application_server", 0, register_application_server},
    {"exec", 1, exec, ERL_NIF_DIRTY_JOB_CPU_BOUND }
};

ERL_NIF_INIT(Elixir.QML.Private,nif_funcs,NULL,NULL,NULL,NULL)

}
