defmodule QML.Application do
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
        {:ok, {:loading, channelManager, file, guiPid}}
    end

    def handle_info({:loaded, _}, {_, _, _, guiPid}) do
        {:noreply, {:loaded, guiPid}}
    end

    def handle_info({:error, file}, state) do
        IO.puts(["Error loading ", file, ", terminating..."])
        :init.stop
        {:stop, :normal, state}
    end

    def handle_info({:channel_registered, identifier}, {:loading, channelManager, file, guiPid}) do
        #__MODULE__.channel_registered identifier
        operations = channelManager.channelForType identifier
        QML.Channel.start_link({identifier, operations})
        IO.inspect identifier
        {:noreply, {:loading, channelManager, file, guiPid}}
    end

    # TODO: remove?
    def handle_info(message, state) do
        IO.puts "info"
        IO.inspect message
        {:noreply, state}
    end
end
