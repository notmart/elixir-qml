defmodule QML.ModelChannelWatcher do
    defmacro __using__(_) do
        quote do
            import QML.ModelChannelWatcher
            @behaviour QML.ModelChannelWatcher

            @doc false
            def init(_pid) do
                :ok
            end

            defoverridable [init: 1]
        end
    end

    @callback init(arg :: pid()) :: none

    @callback rowInserted(arg :: integer(), arg :: any) :: none
end



