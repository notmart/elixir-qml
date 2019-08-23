defmodule QMLChannel do
    use GenServer

    alias QML.Private

    def start_link(identifier) do
        GenServer.start_link(__MODULE__, identifier)
    end

    def init(identifier) do
        {:ok, {identifier, %{}}}
    end

    def handle_info({:signal, name, argv}, state) do
        {:noreply, state}
    end

    def handle_info(message, state) do
        {:noreply, state}
    end

    def handle_cast({:property, name, value}, {identifier, map}) do
        newMap = Map.put(map, name, value)
        {:noreply, newMap}
    end

end
