defmodule QMLChannel do
    use GenServer

    alias QML.Private

    def start_link(identifier) do
        GenServer.start_link(__MODULE__)
        Private.register_qml_channel identifier
    end

    def init(_) do
        {:ok, %{}}
    end

    def handle_info({:signal, name, argv}, state) do
        {:noreply, state}
    end

    def handle_info(message, state) do
        IO.inspect message
        {:noreply, state}
    end

    def handle_cast({:property, name, value}, map) do
        newMap = Map.put(map, name, value)
        {:noreply, newMap}
    end

end
