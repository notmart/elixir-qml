defmodule QML.ChannelOperations do
    defmacro __using__(_) do
        quote do
            import QML.ChannelOperations
            @behaviour QML.ChannelOperations
        end
    end

    @callback signal(arg :: String.t(), arg :: any) :: none
    @callback propertyChanged(arg :: String.t(), arg :: any) :: none
end



