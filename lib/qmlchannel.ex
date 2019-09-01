defmodule QML.Channel do
    use GenServer

    alias QML.Private

    def start_link({identifier, operations}) do
        GenServer.start_link(__MODULE__, {identifier, operations})
    end

    def init({identifier, operations}) do
        Private.register_qml_channel identifier
        {:ok, {%{}, operations}}
    end

    def handle_info({:signal, name, argv}, state) do
        {:noreply, state}
    end

    def handle_info({:property, name, value}, {map, operations}) do
        IO.inspect name
        IO.inspect value
        newMap = Map.put(map, name, value)
        IO.inspect newMap
        operations.propertyChanged(name, value)
        {:noreply, {newMap, operations}}
    end

    def handle_info(message, state) do
        IO.inspect message
        {:noreply, state}
    end

end
