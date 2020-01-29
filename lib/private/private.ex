defmodule QML.Private do
  
  @on_load :init

  app = Mix.Project.config[:app]

  def init do
    :ok = unquote(app) |> :code.priv_dir |> :filename.join('qml') |> :erlang.load_nif(0) 
  end

  def register_application_server do
    "QML NIF library not loaded"
  end

  def register_qml_channel(_identifier) do
    "QML NIF library not loaded"
  end

  def exec(_path) do
    "QML NIF library not loaded"
  end

  def write_property(_identifier, _property, _value) do
    "QML NIF library not loaded"
  end

  def read_property(_identifier, _property) do
    "QML NIF library not loaded"
  end

  def register_qml_model_channel(_identifier) do
    "QML NIF library not loaded"
  end

  def model_length(_identifier) do
    "QML NIF library not loaded"
  end

  def model_insert_row(_identifier, _row, _data) do
    "QML NIF library not loaded"
  end
end
