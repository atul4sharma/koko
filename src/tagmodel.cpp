/*
 * Copyright (C) 2014  Vishesh Handa <me@vhanda.in>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "tagmodel.h"

static QStringList s_tags = {"green", "blue", "red", "black", "orange", "purple", "yellow", "cyan", "brown"};

TagModel::TagModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

QHash<int, QByteArray> TagModel::roleNames() const
{
    auto hash = QAbstractItemModel::roleNames();
    hash.insert(ColorRole, "color");

    return hash;
}

QVariant TagModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= m_tags.size()) {
        return QVariant();
    }

    switch (role) {
    case Qt::DisplayRole: {
        QString tag = m_tags.at(index.row());
        return tag;
    }

    case ColorRole:
        if (index.row() < s_tags.size()) {
            return s_tags.at(index.row());
        } else {
            return QStringLiteral("maroon");
        }
    }

    return QVariant();
}

int TagModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return m_tags.size();
}

void TagModel::setTags(const QStringList& tags)
{
    beginResetModel();
    m_tags = tags;
    endResetModel();

    emit tagsChanged();
    emit colorsChanged();
}

QStringList TagModel::tags() const
{
    return m_tags;
}

void TagModel::addTag(const QString& tag)
{
    if (m_tags.contains(tag)) {
        return;
    }

    beginInsertRows(QModelIndex(), m_tags.size(), m_tags.size());
    m_tags << tag;
    endInsertRows();

    emit tagsChanged();
    emit colorsChanged();
}

bool TagModel::removeRows(int row, int count, const QModelIndex& parent)
{
    if (parent.isValid()) {
        return false;
    }

    beginRemoveRows(parent, row, row + count - 1);
    while (count) {
        m_tags.removeAt(row);
        count--;
    }
    endRemoveRows();

    emit tagsChanged();
    emit colorsChanged();

    return QAbstractItemModel::removeRows(row, count, parent);
}

QStringList TagModel::colors() const
{
    return s_tags.mid(0, m_tags.size());
}
