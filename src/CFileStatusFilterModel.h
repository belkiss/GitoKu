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


#pragma once
#include <QSortFilterProxyModel>
#include "CVCS.h"

namespace gitoku
{

class CFileStatusFilterModel: public QSortFilterProxyModel
{
    Q_OBJECT
    public:
        CFileStatusFilterModel(QObject* parent = 0);
        void set_status_filter(EFileStatus in_status) {m_status = in_status;}
    
    protected:
        bool filterAcceptsRow(int in_source_row, const QModelIndex &in_source_parent) const;

    private:
        EFileStatus m_status;
    
};

}
