defmodule QMLChannel do
    use GenServer

    alias QML.Private

    def start_link(identifier) do
        GenServer.start_link(__MODULE__, identifier)
    end

    def init(identifier) do
        Private.register_qml_channel identifier
        {:ok, %{}}
    end

    def handle_info({:signal, name, argv}, state) do
        {:noreply, state}
    end

    def handle_info({:property, name, value}, map) do
        IO.inspect name
        IO.inspect value
        newMap = Map.put(map, name, value)
        IO.inspect newMap
        {:noreply, newMap}
    end

    def handle_info(message, state) do
        IO.inspect message
        {:noreply, state}
    end

    def handle_cast({:property, name, value}, map) do
        IO.puts(name, value)
        newMap = Map.put(map, name, value)
        {:noreply, newMap}
    end

end
