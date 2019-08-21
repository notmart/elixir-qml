defmodule GuiServer do
    use GenServer

    def start_link() do
        GenServer.start_link(__MODULE__, [])
        QML.exec("minimal.qml")
        spawn(QML, :exec, ["minimal.qml"])
    end

    def init(_) do
        {:ok, 1}
    end

    def handle_call(:print, from, state) do
        IO.puts(from.tag)
        {:reply, state, state}
    end

    def handle_cast(:increment, state) do
        {:noreply, state+1}
    end
end
