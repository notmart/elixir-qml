defmodule QML.Channel do
    use GenServer

    alias QML.Private

    defmacro __using__(opts) do
        quote do
            use GenServer
        end
    end

    def setProperty(pid, property, value) do
        GenServer.cast(pid, {:setProperty, property, value})
    end

    def property(pid, property) do
        GenServer.call(pid, {:property, property})
    end

    # Server handling
    def start_link({typeId, watcher}) do
        GenServer.start_link(__MODULE__, {typeId, watcher})
    end

    def init({typeId, watcher}) do
        #channels must be unique per typeId
        nil = Process.whereis(typeId)

        Private.register_qml_channel typeId

        Process.register(self(), typeId)
        {:ok, {%{}, typeId, watcher}}
    end

    def handle_info({:signalFromQml, name, argv}, {map, typeId, watcher}) do
       # watcher.signal(name, argv)
        apply(watcher, name |> to_string |> String.to_atom, argv)
        {:noreply, {map, typeId, watcher}}
    end

    def handle_call({:property, name}, _from, {map, typeId, watcher}) do
        {:reply, Map.fetch(map, name), {map, typeId, watcher}}
    end

    def handle_call({:setProperty, name, value}, {fromPid, _}, {map, typeId, watcher}) do
        newMap = Map.put(map, name, value)

        if fromPid != 0 do
            Private.write_property(typeId, name, value)
        end

        watcher.propertyChanged(name, value)
        {:reply, nil, {newMap, typeId, watcher}}
    end

    def handle_cast({:setProperty, name, value}, {map, typeId, watcher}) do
         newMap = Map.put(map, name, value)
         Private.write_property(typeId, name, value)
         {:noreply, {newMap, typeId, watcher}}
    end

end
