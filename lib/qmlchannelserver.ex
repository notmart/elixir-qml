defmodule QML.ChannelServer do
    use GenServer

    alias QML.Private

    def start_link({identifier, operations}) do
        GenServer.start_link(__MODULE__, {identifier, operations})
    end

    def init({identifier, operations}) do
        Private.register_qml_channel identifier
        {:ok, {%{}, operations}}
    end

    def handle_info({:signalFromQml, name, argv}, {map, operations}) do
        operations.signal(name, argv)
        {:noreply, {map, operations}}
    end

    def handle_info({:propertyFromQml, name, value}, {map, operations}) do
        newMap = Map.put(map, name, value)
        operations.propertyChanged(name, value)
        {:noreply, {newMap, operations}}
    end

#     def handle_call({:propertySet, name, value}, from, {map, operations}) do
#         newMap = Map.put(map, name, value)
#         operations.propertyChanged(name, value)
#         {:reply, value, {newMap, operations}}
#     end
end
