defmodule Mix.Tasks.Compile.Cmake do
  @moduledoc "Compiles QML NIF"

  def run(_) do
    {result, _} = System.cmd("cmake", ["."], stderr_to_stdout: true)
    Mix.shell.info result
    {result, _} = System.cmd("make", ["all"], stderr_to_stdout: true)
    Mix.shell.info result
    :ok
  end
end

defmodule QML.Mixfile do
  use Mix.Project

  def project do
    [app: :ringbuffer,
     version: "0.1.0",
     elixir: "~> 1.4",
     build_embedded: Mix.env == :prod,
     start_permanent: Mix.env == :prod,
     compilers: [:cmake] ++ Mix.compilers,
     source_url: "https://github.com/notmart/elixir-qml",
     homepage_url: "https://github.com/notmart/elixir-qml",
     docs: [extras: ["README.md"]],
     deps: deps()]
  end

  def application do
    [extra_applications: [:logger]]
  end

  def package do
    [name: :ringbuffer,
     maintainers: ["Marco Martin"],
     licenses: ["LGPL2"],
     links: %{"GitHub" => "https://github.com/notmart/elixir-qml",
              "Docs" => ""}]
  end

  defp deps do
    [{:ex_doc, "~> 0.14", only: :dev, runtime: false}]
  end
end
