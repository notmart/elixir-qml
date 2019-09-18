defmodule TestChannelManager do
    @behaviour QML.ChannelManager

    def channelForType 'elixirTestChannel' do
        IO.puts "doing an elixirtestchannel"
        ElixirTestChannel
    end
end
#guardare Phoenix liveview
defmodule ElixirTestChannel do
    use QML.Channel

    #phoenix liveview come idea

    def testSignal(param1, param2) do
        IO.puts "On ElixirTestChannel testSignal"
        IO.puts param1
        IO.puts param2
    end
    #pub/subscribe
    #qinvokable->chimate
    def propertyChanged(name, value) do
        IO.puts "On ElixirTestChannel propertyChanged"
        IO.puts name
        IO.puts value
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
    opts = [strategy: :one_for_one, name: Sample.Supervisor, restart: :temporary, max_restarts: 0]
    Supervisor.start_link(children, opts)
  end
end
