defmodule QML do
  
  @on_load :init

  app = Mix.Project.config[:app]

  def init do
    :ok = unquote(app) |> :code.priv_dir |> :filename.join('qml') |> :erlang.load_nif(0) 
  end

  def hello do
    "QML NIF library not loaded"
  end

  def exec(_path) do
    "QML NIF library not loaded"
  end

  def process_events do
    "QML NIF library not loaded"
  end
  
end
