defmodule QML.ChannelServer do
    use GenServer

    alias QML.Private

    def setProperty(pid, property, value) do
        GenServer.cast(pid, {:setProperty, property, value})
    end

    def property(pid, property) do
        GenServer.call(pid, {:property, property})
    end

    # Server handling
    def start_link({typeId, operations}) do
        GenServer.start_link(__MODULE__, {typeId, operations})
    end

    def init({typeId, operations}) do
        #channels must be unique per typeId
        nil = Process.whereis(typeId)

        Private.register_qml_channel typeId

        Process.register(self(), typeId)
        {:ok, {%{}, typeId, operations}}
    end

    def handle_info({:signalFromQml, name, argv}, {map, typeId, operations}) do
       # operations.signal(name, argv)
        apply(operations, name |> to_string |> String.to_atom, argv)
        {:noreply, {map, typeId, operations}}
    end

    def handle_call({:property, name}, _from, {map, typeId, operations}) do
        {:reply, Map.fetch(map, name), {map, typeId, operations}}
    end

    def handle_call({:setProperty, name, value}, {fromPid, _}, {map, typeId, operations}) do
        IO.puts "setProperty"
        IO.puts value
        IO.inspect fromPid

        newMap = Map.put(map, name, value)

        if fromPid != 0 do
            Private.write_property(typeId, name, value)
        end

        operations.propertyChanged(name, value)
        {:reply, nil, {newMap, typeId, operations}}
    end

    def handle_cast({:setProperty, name, value}, {map, typeId, operations}) do
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
