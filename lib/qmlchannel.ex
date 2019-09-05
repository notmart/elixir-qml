defmodule QML.Channel do
    defmacro __using__(_) do
        quote do
            import QML.Channel
            @behaviour QML.Channel
        end
    end

    
    @callback signal(arg :: String.t(), arg :: any) :: none
    @callback propertyChanged(arg :: String.t(), arg :: any) :: none
end



