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
        {:ok, {typeId, watcher}}
    end

    def handle_info({:signalFromQml, name, argv}, {typeId, watcher}) do
       # watcher.signal(name, argv)
        apply(watcher, name |> to_string |> String.to_atom, argv)
        {:noreply, {typeId, watcher}}
    end

    def handle_call({:property, name}, _from, {typeId, watcher}) do
        {:reply, Private.read_property(typeId, name), {typeId, watcher}}
    end

    def handle_call({:setProperty, name, value}, {fromPid, _}, {typeId, watcher}) do
        if fromPid != 0 do
            Private.write_property(typeId, name, value)
        end

        watcher.propertyChanged(name, value)
        {:reply, nil, {typeId, watcher}}
    end

    def handle_cast({:setProperty, name, value}, {typeId, watcher}) do
         Private.write_property(typeId, name, value)
         {:noreply, {typeId, watcher}}
    end

end
