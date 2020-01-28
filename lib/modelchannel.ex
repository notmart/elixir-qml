defmodule QML.ModelChannel do
    use QML.Channel

    alias QML.Private

    def data(pid, rowData) do
        GenServer.call(pid, {:data, rowData})
    end

    def prependRow(pid, row) do
        GenServer.cast(pid, {:prependRow, row})
    end

    def appendRow(pid, row) do
        GenServer.cast(pid, {:appendRow, row})
    end

    def insertRow(pid, row) do
        GenServer.cast(pid, {:insertRow, row})
    end

    def moveRow(pid, from, to) do
        GenServer.call(pid, {:moveRow, from, to})
    end

    def removeRow(pid, row) do
        GenServer.call(pid, {:removeRow, row})
    end

    # Server handling
    def start_link({typeId, watcher}) do
        QML.Channel.start_link({typeId, watcher})
    end

    def init({typeId, watcher}) do
        #channels must be unique per typeId
        nil = Process.whereis(typeId)

        Private.register_qml_model_channel typeId

        Process.register(self(), typeId)
        {:ok, {typeId, watcher}}
    end

    def handle_call({:data, row}, _from, {typeId, watcher}) do
        {:reply, Private.model_data(typeId, row), {typeId, watcher}}
    end

    def handle_cast({:prependRow, rowData}, {typeId, watcher}) do
         Private.model_insert_row(typeId, 0, rowData)
         {:noreply, {typeId, watcher}}
    end

    def handle_cast({:appendRow, rowData}, {typeId, watcher}) do
         Private.model_insert_row(typeId, Private.model_length(typeId), rowData)
         {:noreply, {typeId, watcher}}
    end

    def handle_cast({:insertRow, row, rowData}, {typeId, watcher}) do
         Private.model_insert_row(typeId, row, rowData)
         {:noreply, {typeId, watcher}}
    end

    def handle_cast({:moveRow, from, to}, {typeId, watcher}) do
         Private.model_move_row(typeId, from, to)
         {:noreply, {typeId, watcher}}
    end

    def handle_cast({:removeRow, row}, {typeId, watcher}) do
         Private.model_remove_row(typeId, row)
         {:noreply, {typeId, watcher}}
    end
end

