defmodule GuiServer do
    use GenServer

    alias QML.Private

    def start_link(file) do
        GenServer.start_link(__MODULE__, file)
    end

    def init(file) do
        Private.register_application_server
        guiPid = spawn(fn->Private.exec(file) end)
        {:ok, guiPid}
    end

    def handle_info(message, state) do
        IO.puts(message)
        {:noreply, state}
    end

    def handle_call(:print, from, state) do
        IO.puts(from.tag)
        {:reply, state, state}
    end

    def handle_cast(:increment, state) do
        {:noreply, state+1}
    end
end
