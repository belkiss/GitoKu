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


#include "CGitokuWindow.h"
#include "CVCS.h"
#include "log.h"
#include "CLegacyGitVCS.h"

namespace gitoku
{
    
CGitokuWindow::CGitokuWindow()
    :m_p_vcs(nullptr)
{
    setupUi(this);

    ui_repository_line_edit->setText(QDir::currentPath());

    m_file_status_model.setColumnCount(2);

    //filter unstaged files
    m_unstaged_model_filter.set_status_filter(EFileStatus::STATUS_UNSTAGED);
    m_unstaged_model_filter.setSourceModel(&m_file_status_model);
    ui_unstaged_list_view->setModel(&m_unstaged_model_filter);

    //filter staged files
    m_staged_model_filter.set_status_filter(EFileStatus::STATUS_STAGED);
    m_staged_model_filter.setSourceModel(&m_file_status_model);
    ui_staged_list_view->setModel(&m_staged_model_filter);

}

CGitokuWindow::~CGitokuWindow()
{

}

void CGitokuWindow::populate()
{
    cond_assert(LOG_COND(m_p_vcs), "MAIN");

    //query repository files status
    QLinkedList<SFileStatus> repo_status = m_p_vcs->get_repository_status();

    //clear previously loaded repository
    m_file_status_model.clear();

    foreach (SFileStatus status, repo_status)
    {
        QList<QStandardItem*> columns;

        //column 0 = file path
        QStandardItem *p_item;
        p_item = new QStandardItem();
        p_item->setData(status.m_path, Qt::DisplayRole);
        p_item->setEditable(false);
        columns << p_item;

        //column 1 = file status
        p_item = new QStandardItem();
        p_item->setData(status.m_status, Qt::DisplayRole);
        p_item->setEditable(false);
        columns << p_item;

        //append row to the model
        m_file_status_model.appendRow(columns);
    }
}


void CGitokuWindow::on_repository_changed()
{
    //try to open the repository specified in GUI
    CVCS* p_vcs = new CLegacyGitVCS();
    bool repository_opened = p_vcs->open(ui_repository_line_edit->text());

    if (repository_opened)
    {
        // if a previous vcs was used, delete it
        if(m_p_vcs)
        {
            delete m_p_vcs;
            m_p_vcs = 0;
        }

        //associate and load new vcs repository
        m_p_vcs = p_vcs;
        populate();
    }
    else
    {
        error ("MAIN", "failed to open git repository");
    }
}



}