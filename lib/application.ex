defmodule QML.Application do
    use GenServer

    #TODO: this goes into ChannelManager
    #@callback channel_registered(arg :: any) :: any

    alias QML.Private

    def start_link(file) do
        GenServer.start_link(__MODULE__, file)
    end

    def init(file) do
        Private.register_application_server
        guiPid = spawn(fn ->
            Private.exec(file)
            :init.stop
        end)
        {:ok, {:loading, guiPid}}
    end

    def handle_info({:loaded, _}, {_, guiPid}) do
        {:noreply, {:loaded, guiPid}}
    end

    def handle_info({:error, file}, state) do
        IO.puts(["Error loading ", file, ", terminating..."])
        :init.stop
        {:stop, :normal, state}
    end

    def handle_info({:channel_registered, identifier}, state) do
        #__MODULE__.channel_registered identifier
        #ChannelManager.channelForType identifier
        IO.inspect identifier
        {:noreply, state}
    end

    # TODO: remove?
    def handle_info(message, state) do
        IO.puts "info"
        IO.inspect message
        {:noreply, state}
    end
end
