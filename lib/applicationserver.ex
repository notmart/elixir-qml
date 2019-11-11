defmodule QML.ApplicationServer do
    use GenServer

    alias QML.Private

    def start_link({watcherManager, file}) do
        GenServer.start_link(__MODULE__, {watcherManager, file})
    end

    def init({watcherManager, file}) do
        # Only an ApplicationServer can run at once
        nil = Process.whereis(:qmlApplicationServer)
        Private.register_application_server

        Process.register(self(), :qmlApplicationServer)

        guiPid = Process.whereis(:qApplicationProcess)
            || spawn(fn ->
                Private.exec(file)
                :init.stop
            end)

        Process.register(guiPid, :qApplicationProcess)

        children = [
            {DynamicSupervisor, strategy: :one_for_one, name: QML.ChannelSupervisor}
        ]
        Supervisor.start_link(children, strategy: :one_for_one)

        {:ok, {:loading, watcherManager, file, guiPid}}
    end

    def handle_info({:loaded, _}, {_state, watcherManager, file, guiPid}) do
        {:noreply, {:loaded, watcherManager, file, guiPid}}
    end

    def handle_info({:error, file}, state) do
        IO.puts(["Error loading ", file, ", terminating..."])
        :init.stop
        {:stop, :normal, state}
    end

    def handle_info({:channel_registered, typeId}, {:loading, watcherManager, file, guiPid}) do
        qmlChannel = watcherManager.watcherForType typeId
        
        {:ok, channel} = DynamicSupervisor.start_child(QML.ChannelSupervisor, {QML.Channel, {typeId, qmlChannel}})
        
        IO.inspect typeId
        IO.inspect channel
        {:noreply, {:loading, watcherManager, file, guiPid}}
    end

    def handle_info({:channel_unregistered, typeId}, {:loading, watcherManager, file, guiPid}) do
        #DynamicSupervisor.terminate_child(QML.ChannelSupervisor, map pids for typeId)
        {:noreply, {:loading, watcherManager, file, guiPid}}
    end

    # TODO: remove?
    def handle_info(message, state) do
        IO.puts "info"
        IO.inspect message
        {:noreply, state}
    end

    def process do
        Process.whereis(:qmlApplicationServer)
    end
end
