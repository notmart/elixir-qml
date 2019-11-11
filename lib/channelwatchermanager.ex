defmodule QML.ChannelWatcherManager do
    @callback watcherForType(arg :: String.t()) :: any
end
