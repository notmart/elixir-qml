defmodule TestChannelManager do
    @behaviour QML.ChannelManager

    def channelForType :elixirTestChannel do
        IO.puts "doing an elixirtestchannel"
        ElixirTestChannelWatcher
    end
end
#guardare Phoenix liveview
defmodule ElixirTestChannelWatcher do
    use QML.ChannelWatcher

    #phoenix liveview come idea

    def testSignal(param1, param2) do
        IO.puts "On ElixirTestChannel testSignal invoked from QML signal, param1 is #{param1}, param2 is #{param2}"
    end
    #pub/subscribe
    #qinvokable->chimate
    def propertyChanged(name, value) do
        IO.puts "On ElixirTestChannel propertyChanged. name: #{name}; value: #{value}"
    end
end

defmodule ExampleApp do
  use Application
 
  def start(_type, _args) do
    children = [
        {QML.ApplicationServer, {TestChannelManager, "minimal.qml"}}
    ]
 
 #TODO: not a supervisor, but when it dies, kill the whole VM?
 #or rebuild everything, but recycling the same QApplication instance if possible
    opts = [strategy: :one_for_one, name: Sample.Supervisor]
    Supervisor.start_link(children, opts)
  end
end
