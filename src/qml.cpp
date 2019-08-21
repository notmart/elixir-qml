 
#include "erl_nif.h"
#include "application.h"

#include <QDebug>

extern "C" {

Q_GLOBAL_STATIC(Application, s_application)

ErlNifPid* s_pid;

static ERL_NIF_TERM hello(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
   // qWarning() << "WASSSUUUUUP!";
    ErlNifPid* pid = (ErlNifPid*) enif_alloc(sizeof(ErlNifPid));
    s_pid = enif_self(env, pid);
    return enif_make_string(env, "Hello world!", ERL_NIF_LATIN1);
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

 //   ErlNifPid* pid = (ErlNifPid*) enif_alloc(sizeof(ErlNifPid));
  //  s_application->pid = enif_self(env, pid);
  s_application->pid = s_pid;
    char *path = strndup((char*) path_bin.data, path_bin.size);

    return enif_make_int(env, s_application->exec(path));
}

static ERL_NIF_TERM process_events(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    if (argc != 0) {
        return enif_make_badarg(env);
    }

    s_application->processEvents();
    return argv[0];
}

static ErlNifFunc nif_funcs[] =
{
    {"hello", 0, hello},
    {"exec", 1, exec, ERL_NIF_DIRTY_JOB_CPU_BOUND },
    {"process_events", 0, process_events}
};

ERL_NIF_INIT(Elixir.QML,nif_funcs,NULL,NULL,NULL,NULL)

}
