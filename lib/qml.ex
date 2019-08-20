defmodule Qml do
  
  @on_load :load_nifs

  def load_nif do
    :ok = :erlang.load_nif('./qml', 0)
  end

  def dello do
    "QML NIF library not loaded"
  end
end
