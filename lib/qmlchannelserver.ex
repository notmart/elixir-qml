defmodule QML.ChannelServer do
    use GenServer

    alias QML.Private

    def start_link({identifier, operations}) do
        GenServer.start_link(__MODULE__, {identifier, operations})
    end

    def init({identifier, operations}) do
        Private.register_qml_channel identifier
        {:ok, {%{}, identifier, operations}}
    end

    def handle_info({:signalFromQml, name, argv}, {map, identifier, operations}) do
       # operations.signal(name, argv)
        apply(operations, name |> to_string |> String.to_atom, argv)
        {:noreply, {map, identifier, operations}}
    end

    def handle_info({:changeProperty, name, value}, {map, identifier, operations}) do
        newMap = Map.put(map, name, value)
        operations.propertyChanged(name, value)
       # Private.write_property(identifier, name, value)
        {:noreply, {newMap, identifier, operations}}
    end

#     def handle_call({:propertySet, name, value}, from, {map, operations}) do
#         newMap = Map.put(map, name, value)
#         operations.propertyChanged(name, value)
#         {:reply, value, {newMap, operations}}
#     end
end
