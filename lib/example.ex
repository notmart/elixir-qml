defmodule ExampleApp do
  use Application
 
  def start(_type, _args) do
    children = [
        {QML.Application, "minimal.qml"}
    ]
 
    opts = [strategy: :one_for_one, name: Sample.Supervisor]
    Supervisor.start_link(children, opts)
  end
end
