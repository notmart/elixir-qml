/*
*   Copyright (C) 2020 by Marco Martin <mart@kde.org>
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU Library General Public License as
*   published by the Free Software Foundation; either version 2, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU Library General Public License for more details
*
*   You should have received a copy of the GNU Library General Public
*   License along with this program; if not, write to the
*   Free Software Foundation, Inc.,
*   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "simpledatamodel.h"

#include <QDebug>

SimpleDataModel::SimpleDataModel(QObject *parent)
    : ElixirModelChannel(parent)
{
}

SimpleDataModel::~SimpleDataModel()
{
}

void SimpleDataModel::insertData(int position, const QList<QVariantMap> &dataList)
{
    if (position < 0 || position > m_data.count()) {
        return;
    }
    if (dataList.isEmpty()) {
        return;
    }

    beginInsertRows(QModelIndex(), position, position + dataList.count() - 1);
    int i = 0;
    for (const auto &item : dataList) {
        m_data.insert(position + i, item);
        ++i;
    }

    endInsertRows();
}

void SimpleDataModel::updateData(int position, const QList<QVariantMap> &dataList)
{
    if (dataList.isEmpty()) {
        return;
    }
    //too much rows to update, we don't have enough
    if (m_data.count() - position < dataList.count()) {
        return;
    }

    QSet<int> roles;

    int i = 0;
    for (auto it = m_data.begin() + position; it < m_data.begin() + position + dataList.count(); ++it) {
        const QVariantMap newValues = dataList[i];
        for (auto newIt = newValues.begin(); newIt != newValues.end(); ++newIt) {
            (*it)[newIt.key()] = newIt.value();
            roles.insert(roleNames().key(newIt.key().toUtf8()));
        }
        ++i;
    }
    emit dataChanged(index(position, 0), index(position + dataList.length() - 1, 0), roles.values().toVector());
}

void SimpleDataModel::clear()
{
    beginResetModel();
    m_data.clear();
    endResetModel();
}

bool SimpleDataModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild)
{
    if (sourceParent.isValid() || destinationParent.isValid()) {
        return false;
    }

    if (count <= 0 || sourceRow == destinationChild || sourceRow < 0 || sourceRow >= m_data.count() ||
        destinationChild < 0 || destinationChild > m_data.count() || count - destinationChild > m_data.count() - sourceRow) {
        return false;
    }
    const int sourceLast = sourceRow + count - 1;

    //beginMoveRows wants indexes before the source rows are removed from the old order
    if (!beginMoveRows(sourceParent, sourceRow, sourceLast, destinationParent, destinationChild)) {
        return false;
    }

    if (sourceRow < destinationChild) {
        for (int i = count - 1; i >= 0; --i) {
            m_data.move(sourceRow + i, destinationChild - count + i);
        }
    } else {
        for (int i = 0; i < count; ++i) {
            m_data.move(sourceRow + i, destinationChild + i);
        }
    }

    endMoveRows();
    return true;
}

bool SimpleDataModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (row < 0 || count <= 0 || row + count > m_data.count() || parent.isValid()) {
        return false;
    }

    beginRemoveRows(parent, row, row + count - 1);

    m_data.erase(m_data.begin() + row, m_data.begin() + row + count);
    endRemoveRows();
    return true;
}


int SimpleDataModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return m_data.count();
}

QVariant SimpleDataModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    const int row = index.row();

    if (row < 0 || row >= m_data.count() || !roleNames().contains(role)) {
        return QVariant();
    }

    return m_data[row][QString::fromUtf8(roleNames()[role])];
}

#include "moc_simpledatamodel.cpp"
