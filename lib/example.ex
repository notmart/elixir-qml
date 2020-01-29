defmodule TestWatcherManager do
    @behaviour QML.ChannelWatcherManager

    def watcherForType :elixirTestChannel do
        IO.puts "doing an ElixirTestChannelWatcher"
        ElixirTestChannelWatcher
    end

    def watcherForType :testModel do
        IO.puts "doing an ElixirTestModelChannelWatcher"
        ElixirTestModelChannelWatcher
    end
end
#guardare Phoenix liveview
defmodule ElixirTestChannelWatcher do
    use QML.ChannelWatcher

    #phoenix liveview come idea

    def init(pid) do
        IO.puts "ElixirTestChannel process created with pid"
        inspect pid
        :ok
    end

    def testSignal(param1, param2) do
        IO.puts "On ElixirTestChannel testSignal invoked from QML signal, param1 is #{param1}, param2 is #{param2}"

        QML.ModelChannel.appendRows(Process.whereis(:testModel), %{"title" => "Test row ##{param1}", "description" => "Descrition: #{param2}"})
    end
    #pub/subscribe
    #qinvokable->chimate
    def propertyChanged(name, value) do
        IO.puts "On ElixirTestChannel propertyChanged. name: #{name}; value: #{value}"
    end
end

defmodule ElixirTestModelChannelWatcher do
    use QML.ModelChannelWatcher

    def init(pid) do
        QML.ModelChannel.appendRows(pid, [%{"title" => "Test row #1", "description" => "Descrition of the first row"}, %{"title" => "Test row #2", "description" => "Descrition of the second row"},
        %{"title" => "Test row #3", "description" => "Descrition of the third row"}])
        :ok
    end

    def rowsInserted(row, data) do
        IO.puts "On ElixirTestModelChannelWatcher rowInserted. row: #{row}; data: #{data}"
    end
end

defmodule ExampleApp do
  use Application
 
  def start(_type, _args) do
    children = [
        {QML.ApplicationServer, {TestWatcherManager, "minimal.qml"}}
    ]
 
 #TODO: not a supervisor, but when it dies, kill the whole VM?
 #or rebuild everything, but recycling the same QApplication instance if possible
    opts = [strategy: :one_for_one, name: Sample.Supervisor]
    Supervisor.start_link(children, opts)
  end
end
