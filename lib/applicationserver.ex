defmodule QML.ApplicationServer do
    use GenServer

    alias QML.Private

    def start_link({channelManager, file}) do
        GenServer.start_link(__MODULE__, {channelManager, file})
    end

    def init({channelManager, file}) do
        Private.register_application_server
        guiPid = spawn(fn ->
            Private.exec(file)
            :init.stop
        end)

        children = [
            {DynamicSupervisor, strategy: :one_for_one, name: QML.ChannelSupervisor}
        ]
        Supervisor.start_link(children, strategy: :one_for_one)

        {:ok, {:loading, channelManager, file, guiPid}}
    end

    def handle_info({:loaded, _}, {_state, channelManager, file, guiPid}) do
        {:noreply, {:loaded, channelManager, file, guiPid}}
    end

    def handle_info({:error, file}, state) do
        IO.puts(["Error loading ", file, ", terminating..."])
        :init.stop
        {:stop, :normal, state}
    end

    def handle_info({:channel_registered, identifier, typeId}, {:loading, channelManager, file, guiPid}) do
        qmlChannel = channelManager.channelForType typeId
        #QML.ChannelServer.start_link({identifier, qmlChannel})
        
        DynamicSupervisor.start_child(QML.ChannelSupervisor, {QML.ChannelServer, {identifier, qmlChannel}})
        
        IO.inspect identifier
        {:noreply, {:loading, channelManager, file, guiPid}}
    end

    def handle_info({:channel_unregistered, identifier}, {:loading, channelManager, file, guiPid}) do
        #DynamicSupervisor.terminate_child(QML.ChannelSupervisor, map pids for identifier)
        {:noreply, {:loading, channelManager, file, guiPid}}
    end

    # TODO: remove?
    def handle_info(message, state) do
        IO.puts "info"
        IO.inspect message
        {:noreply, state}
    end
end
