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

#include <QtGui/QtGui>
#include <qlistwidget.h>
#include "src/ui_gitoku.h"
#include "CFileStatusFilterModel.h"

namespace gitoku
{

class CVCS;

class CGitokuWindow : public QMainWindow, private Ui_GitokuWindow
{
    Q_OBJECT

    //methods
    public:
        CGitokuWindow();
        virtual ~CGitokuWindow();

    //methods
    private:
        void populate();

    //slots    
    private slots:
        void on_repository_changed();

    //members
    private:
        CVCS* m_p_vcs;
        QStandardItemModel m_file_status_model;

        CFileStatusFilterModel m_unstaged_model_filter;
        CFileStatusFilterModel m_staged_model_filter;

};

}