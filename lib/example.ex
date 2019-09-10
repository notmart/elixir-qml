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
    #apply per invocare segnali
    def signal(name, params) do
        IO.puts "On ElixirTestChannel signal"
        IO.puts name
        IO.puts params
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
