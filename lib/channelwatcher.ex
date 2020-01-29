defmodule QML.ChannelWatcher do
    defmacro __using__(_) do
        quote do
            import QML.ChannelWatcher
            @behaviour QML.ChannelWatcher

            @doc false
            def init(_pid) do
                :ok
            end

            defoverridable [init: 1]
        end
    end

    
    #@callback signal(arg :: String.t(), arg :: any) :: none
    @callback propertyChanged(arg :: String.t(), arg :: any) :: none
end



