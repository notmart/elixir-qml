defmodule QML.ModelChannel do
    use GenServer

    alias QML.Private

    def data(pid, rowData) do
        GenServer.call(pid, {:data, rowData})
    end

    def prependRows(pid, row) do
        GenServer.cast(pid, {:prependRows, row})
    end

    def appendRows(pid, row) do
        GenServer.cast(pid, {:appendRows, row})
    end

    def insertRows(pid, row) do
        GenServer.cast(pid, {:insertRows, row})
    end

    def moveRows(pid, from, count, to) do
        GenServer.cast(pid, {:moveRows, from, count, to})
    end

    def removeRows(pid, row, count) do
        GenServer.cast(pid, {:removeRows, row, count})
    end

    # Server handling
    def start_link({typeId, watcher}) do
        GenServer.start_link(__MODULE__, {typeId, watcher})
    end

    def init({typeId, watcher}) do
        #channels must be unique per typeId
        nil = Process.whereis(typeId)

        Private.register_qml_model_channel typeId

        Process.register(self(), typeId)

        :ok = watcher.init(self())

        {:ok, {typeId, watcher}}
    end

    def handle_call({:data, row}, _from, {typeId, watcher}) do
        {:reply, Private.model_data(typeId, row), {typeId, watcher}}
    end

    def handle_cast({:prependRows, rowData}, {typeId, watcher}) do
         Private.model_insert_rows(typeId, 0, rowData)
         {:noreply, {typeId, watcher}}
    end

    def handle_cast({:appendRows, rowData}, {typeId, watcher}) do
         Private.model_insert_rows(typeId, Private.model_length(typeId), rowData)
         {:noreply, {typeId, watcher}}
    end

    def handle_cast({:insertRows, row, rowData}, {typeId, watcher}) do
         Private.model_insert_rows(typeId, row, rowData)
         watcher.rowInserted(row, rowData)
         {:noreply, {typeId, watcher}}
    end

    def handle_cast({:moveRows, from, count, to}, {typeId, watcher}) do
         Private.model_move_rows(typeId, from, count, to)
         {:noreply, {typeId, watcher}}
    end

    def handle_cast({:removeRows, row, count}, {typeId, watcher}) do
         Private.model_remove_rows(typeId, row, count)
         {:noreply, {typeId, watcher}}
    end
end

