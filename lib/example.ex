defmodule TestChannelManager do
    @behaviour QML.ChannelManager

    def channelForType 'elixirTestChannel' do
        IO.puts "doing an elixirtestchannel"
        ElixirTestChannel
    end
end

defmodule ElixirTestChannel do
    use QML.ChannelOperations

    def signal(name, params) do
        IO.puts "On ElixirTestChannel signal"
        IO.puts name
        IO.puts params
    end

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
        {QML.Application, {TestChannelManager, "minimal.qml"}}
    ]
 
    opts = [strategy: :one_for_one, name: Sample.Supervisor]
    Supervisor.start_link(children, opts)
  end
end
