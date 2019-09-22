defmodule QML.ChannelServer do
    use GenServer

    alias QML.Private

    def start_link({typeId, operations}) do
        GenServer.start_link(__MODULE__, {typeId, operations})
    end

    def init({typeId, operations}) do
        Private.register_qml_channel typeId
        idAtom = typeId |> to_string |> String.to_atom
        nil = Process.whereis(idAtom)
        Process.register(self(), idAtom)
        {:ok, {%{}, typeId, operations}}
    end

    def handle_info({:signalFromQml, name, argv}, {map, typeId, operations}) do
       # operations.signal(name, argv)
        apply(operations, name |> to_string |> String.to_atom, argv)
        {:noreply, {map, typeId, operations}}
    end

    def handle_info({:changeProperty, name, value}, {map, typeId, operations}) do
        newMap = Map.put(map, name, value)
        operations.propertyChanged(name, value)
       # Private.write_property(typeId, name, value)
        {:noreply, {newMap, typeId, operations}}
    end

    def handle_call({:property, name}, _from, {map, typeId, operations}) do
        {:reply, Map.fetch(map, name), {map, typeId, operations}}
    end

    def handle_cast({:setProperty, name, value}, _from, {map, typeId, operations}) do
        newMap = Map.put(map, name, value)
        Private.write_property(typeId, name, value)
        {:noreply, {newMap, typeId, operations}}
    end
    
#     def handle_call({:propertySet, name, value}, from, {map, operations}) do
#         newMap = Map.put(map, name, value)
#         operations.propertyChanged(name, value)
#         {:reply, value, {newMap, operations}}
#     end
end
