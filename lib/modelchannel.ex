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
IO.puts("SWSW")
        Private.register_qml_model_channel typeId

        Process.register(self(), typeId)
        {:ok, {[], typeId, watcher}}
    end

    def handle_call({:data, row}, _from, {listData, typeId, watcher}) do
        {:reply, Enum.fetch(listData, row), {listData, typeId, watcher}}
    end

    def handle_cast({:prependRow, rowData}, {listData, typeId, watcher}) do
         Private.model_insert_row(typeId, 0, rowData)
         {:noreply, {[rowData | listData], typeId, watcher}}
    end

    def handle_cast({:appendRow, rowData}, {listData, typeId, watcher}) do
         Private.model_insert_row(typeId, length(listData), rowData)
         {:noreply, {listData ++ [rowData], typeId, watcher}}
    end

    def handle_cast({:insertRow, row, rowData}, {listData, typeId, watcher}) do
         Private.model_insert_row(typeId, row, rowData)
         newList = List.insert_at(listData, rowData, rowData)
         {:noreply, {newList, typeId, watcher}}
    end

    def handle_cast({:moveRow, from, to}, {listData, typeId, watcher}) do
         Private.model_move_row(typeId, from, to)
         {rowData, newList} = List.pop_at(listData, from)
         newList = List.insert_at(newList, to, rowData)
         {:noreply, {newList, typeId, watcher}}
    end

    def handle_cast({:removeRow, row}, {listData, typeId, watcher}) do
         Private.model_remove_row(typeId, row)
         newList = List.delete_at(listData, row)
         {:noreply, {newList, typeId, watcher}}
    end
end

