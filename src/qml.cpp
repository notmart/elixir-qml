 
#include "erl_nif.h"
#include "application.h"

#include <QDebug>

extern "C" {

Q_GLOBAL_STATIC(Application, s_application)

static ERL_NIF_TERM hello(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    qWarning() << "WASSSUUUUUP!";
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

    char *path = strndup((char*) path_bin.data, path_bin.size);
    qWarning()<<"BOOOH"<<path;
    return enif_make_int(env, s_application->exec(path));
}

static ErlNifFunc nif_funcs[] =
{
    {"hello", 0, hello},
    {"exec", 1, exec}
};

ERL_NIF_INIT(Elixir.QML,nif_funcs,NULL,NULL,NULL,NULL)

}
