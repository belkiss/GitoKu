/*
 * Copyright (C) 2011  Luc Bertrand <grumpy@cacou.net>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 *
 */

#include "CFileStatusFilterModel.h"
#include "CVCS.h"
#include "log.h"

namespace gitoku
{

CFileStatusFilterModel::CFileStatusFilterModel(QObject* parent)
    :QSortFilterProxyModel(parent), m_status(EFileStatus::STATUS_UNKNOWN)
{

}


bool CFileStatusFilterModel::filterAcceptsRow(int in_source_row, const QModelIndex& in_source_parent) const
{
    QModelIndex idx = sourceModel()->index(in_source_row, 1, in_source_parent);

    return sourceModel()->data(idx).toInt() & m_status;
}


}
