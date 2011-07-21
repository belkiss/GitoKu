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


#include "CLegacyGitVCS.h"
#include "log.h"
#include <git2/errors.h>
#include <git2/repository.h>
#include <git2/index.h>
#include <git2/status.h>


QString cvt_git_status_to_str(int in_git_status)
{
    QString ret;
    if (in_git_status == GIT_STATUS_CURRENT)
    {
        ret += "GIT_STATUS_CURRENT ";
    }
    if (in_git_status & GIT_STATUS_INDEX_DELETED)
    {
        ret += "GIT_STATUS_INDEX_DELETED ";
    }
    if (in_git_status & GIT_STATUS_WT_DELETED)
    {
        ret += "GIT_STATUS_WT_DELETED ";
    }
    if( in_git_status & GIT_STATUS_INDEX_MODIFIED)
    {
        ret += "GIT_STATUS_INDEX_MODIFIED ";
    }
    if ( in_git_status & GIT_STATUS_WT_MODIFIED)
    {
        ret += "GIT_STATUS_WT_MODIFIED ";
    }
    if ( in_git_status & GIT_STATUS_INDEX_NEW)
    {
        ret += "GIT_STATUS_INDEX_NEW ";
    }
    if (in_git_status & GIT_STATUS_WT_NEW)
    {
        ret += "GIT_STATUS_WT_NEW ";
    }
    if (in_git_status & GIT_STATUS_IGNORED)
    {
        ret += "GIT_STATUS_IGNORED ";
    }
    if (ret.isEmpty())
    {
        ret = ret.number(in_git_status);
    }

    return ret;
}



namespace gitoku
{


CLegacyGitVCS::CLegacyGitVCS ()
    :CVCS(), m_p_repository(nullptr)
{
}

CLegacyGitVCS::~CLegacyGitVCS()
{
    if (m_p_repository)
    {
        git_repository_free (m_p_repository);
        m_p_repository = nullptr;
    }
}

bool CLegacyGitVCS::open(const QString& in_path)
{
    int open_status = GIT_ERROR;
    if (!in_path.isEmpty() && !m_p_repository)
    {
        m_path = in_path;
        open_status = git_repository_open(&m_p_repository, m_path.toAscii());
        if (open_status != GIT_SUCCESS)
        {
            error ("GIT", "error occured when open Git repository => ", m_path.toStdString(), ".\nError [",open_status,"] => ", git_strerror(open_status));
        }
        else
        {
            debug ("GIT", "Git repository successfully opened");
        }
    }
    else
    {
        if (m_path.isEmpty())
        {
            error ("GIT", "empty path provided to open Git repository");
        }
        else
        {
            error ("GIT", "Git repository already opened");
        }
    }
    return (open_status == GIT_SUCCESS);
}


int CLegacyGitVCS::get_file_status (const char* in_p_file_path, unsigned int in_git_status, void* out_p_status_list )
{
    int status = GIT_SUCCESS;

    cond_assert(LOG_COND(out_p_status_list), "GIT");
    QLinkedList <SFileStatus>* p_status_list = static_cast<QLinkedList<SFileStatus>*>(out_p_status_list);

    SFileStatus file_status;

    file_status.m_file_info= QFileInfo(in_p_file_path);
    file_status.m_path     = QString(in_p_file_path);
    file_status.m_status   = cvt_git_status(in_git_status);

    p_status_list->push_back(file_status);

    return status;
}

QLinkedList< SFileStatus > CLegacyGitVCS::get_repository_status()
{
    QLinkedList<SFileStatus> file_status_list;

    git_status_foreach(m_p_repository, &get_file_status, static_cast<void*>(&file_status_list));

    return file_status_list;
}


void CLegacyGitVCS::print()
{
    cond_assert(LOG_COND(m_p_repository), "GIT");
    git_index *index;

    git_repository_index(&index, m_p_repository);

    git_index_read(index);
    unsigned int ecount = git_index_entrycount(index);


    debug("GIT", "=======  index =========");
    
    for (unsigned int i = 0; i < ecount; ++i)
    {
        git_index_entry *e = git_index_get(index, i);

        debug("GIT", "path: ", e->path);
    }

    debug("GIT", "========== unmerged ==============");
    
    ecount = git_index_entrycount_unmerged(index);
    for (unsigned int i = 0; i < ecount; ++i)
    {
        const git_index_entry_unmerged *e = git_index_get_unmerged_byindex(index, i);

        debug("GIT", "path: ", e->path);
    }

    debug("GIT", "==============================");


    git_index_free(index);
}


int CLegacyGitVCS::cvt_git_status(int in_git_status)
{
    int status = EFileStatus::STATUS_UNKNOWN;

    // untracked file
    if (in_git_status == GIT_STATUS_WT_NEW)
    {
        status = (EFileStatus::STATUS_UNTRACKED | EFileStatus::STATUS_NEW);
    }
    // tracked file, without any changes
    else if (in_git_status == GIT_STATUS_CURRENT)
    {
        status = EFileStatus::STATUS_TRACKED;
    }
    // new file staged
    else if (in_git_status == (GIT_STATUS_WT_MODIFIED | GIT_STATUS_INDEX_NEW))
    {
        status = (EFileStatus::STATUS_STAGED | EFileStatus::STATUS_NEW);
    }
    // modified file, but not staged
    else if (in_git_status == GIT_STATUS_WT_MODIFIED)
    {
        status = (EFileStatus::STATUS_MODIFIED | EFileStatus::STATUS_TRACKED | EFileStatus::STATUS_UNSTAGED);
    }
    // modified file, staged
    else if (in_git_status == GIT_STATUS_INDEX_MODIFIED)
    {
        status = (EFileStatus::STATUS_STAGED | EFileStatus::STATUS_MODIFIED | EFileStatus::STATUS_TRACKED);
    }
    // nzw file, staged
    else if (in_git_status == GIT_STATUS_INDEX_NEW)
    {
        status = (EFileStatus::STATUS_STAGED | EFileStatus::STATUS_NEW | EFileStatus::STATUS_UNTRACKED);
    }
    else
    {
        error ("GIT", "git uncaught status : ", cvt_git_status_to_str(in_git_status).toStdString());
    }
    
    return status;
}

}
