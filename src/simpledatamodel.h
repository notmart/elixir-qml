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

#pragma once

#include <QAbstractListModel>

class AbstractDelegate;
class DelegatesModel;

class SimpleDataModel : public QAbstractListModel
{
    Q_OBJECT

public:

    explicit SimpleDataModel(QObject *parent = nullptr);
    virtual ~SimpleDataModel();

    /**
     * Insert new data in the model, at a given position
     */
    void insertData(int position, const QList<QVariantMap> &dataList);

    /**
     * update the value of items from position to dataList.count()
     * for each key contained in dataList.
     * keys not present will not be updated
     */
    void updateData(int position, const QList<QVariantMap> &dataList);

    /**
     * clears the whole model
     */
    void clear();

//REIMPLEMENTED
    bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild) override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::UserRole + 1) const override;
    QHash<int, QByteArray> roleNames() const override;

private:
    QHash<int, QByteArray> m_roles;
    QList<QVariantMap> m_data;
};


