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
    :CVCS(), m_p_repository(nullptr), 
	 m_p_repository_index(nullptr)
{
}

CLegacyGitVCS::~CLegacyGitVCS()
{
    close();
}

bool CLegacyGitVCS::open(const QString& in_path)
{
    int open_status = GIT_ERROR;
    if (!in_path.isEmpty() && !m_p_repository)
    {
        char repo_found [512];
        int discover_status = git_repository_discover(repo_found, sizeof(repo_found), in_path.toAscii(), true, "");

        if (discover_status == GIT_SUCCESS)
        {
            m_path = repo_found;
            open_status = git_repository_open(&m_p_repository, m_path.toAscii());
            if (open_status == GIT_SUCCESS)
            {
                //get the repository index
                open_status = git_repository_index(&m_p_repository_index, m_p_repository);

                if (open_status == GIT_SUCCESS)
                {
                    debug ("GIT", "Git repository successfully opened");
                }
                else
                {
                    debug ("GIT", "error occured when reading repository index");
                }
            }
            else
            {
                error ("GIT", "error occured when open Git repository => ", m_path.toStdString(), ".\nError [",open_status,"] => ", git_strerror(open_status));
            }
        }
        else
        {
            error ("GIT", "error occured when discover Git repository => ", m_path.toStdString(), ".\nError [",discover_status,"] => ", git_strerror(open_status));
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


void CLegacyGitVCS::close()
{
    if (m_p_repository)
    {
        git_repository_free (m_p_repository);
        m_p_repository = nullptr;
    }

    if (m_p_repository_index)
    {
        git_index_free(m_p_repository_index);
        m_p_repository_index = nullptr;
    }
}


int CLegacyGitVCS::get_file_status (const char* in_p_file_path, unsigned int in_git_status, void* out_p_status_list )
{
    cond_assert(LOG_COND(out_p_status_list), "GIT");
    CLegacyGitVCS* p_this = static_cast<CLegacyGitVCS*>(out_p_status_list);

    CVcsFile file_status;

    file_status.m_file_info= QFileInfo(in_p_file_path);
    file_status.m_path     = QString(in_p_file_path);
    file_status.m_status   = p_this->cvt_git_status(in_git_status);

    p_this->m_file_status_list.push_back(file_status);

    return GIT_SUCCESS;
}

const QLinkedList< CVcsFile >& CLegacyGitVCS::get_repository_status()
{
    m_file_status_list.clear();

    git_status_foreach(m_p_repository, &get_file_status, static_cast<void*>(this));

    return m_file_status_list;
}


QString CLegacyGitVCS::get_repository_path()
{
    QString path;
    if (m_p_repository)
    {
        path = git_repository_path(m_p_repository, GIT_REPO_PATH_WORKDIR);
    }

    return path;
}


int CLegacyGitVCS::cvt_git_status(int in_git_status)
{
    int status = EFileStatus::STATUS_UNKNOWN;

    // tracked file, without any changes
    if (in_git_status == GIT_STATUS_CURRENT)
    {
        status = EFileStatus::STATUS_TRACKED;
    }
    // deleted file
    else if (in_git_status == GIT_STATUS_WT_DELETED)
    {
        status = (EFileStatus::STATUS_TRACKED | EFileStatus::STATUS_DELETED | EFileStatus::STATUS_UNSTAGED);
    }
    // modified file, but not staged
    else if (in_git_status == GIT_STATUS_WT_MODIFIED)
    {
        status = (EFileStatus::STATUS_TRACKED | EFileStatus::STATUS_MODIFIED | EFileStatus::STATUS_UNSTAGED);
    }
    // untracked new file
    else if (in_git_status == GIT_STATUS_WT_NEW)
    {
        status = (EFileStatus::STATUS_UNTRACKED | EFileStatus::STATUS_NEW);
    }
    // modified file, staged
    else if (in_git_status == GIT_STATUS_INDEX_MODIFIED)
    {
        status = (EFileStatus::STATUS_TRACKED | EFileStatus::STATUS_MODIFIED | EFileStatus::STATUS_STAGED);
    }
    // deleted file, staged
    else if (in_git_status == (GIT_STATUS_INDEX_MODIFIED | GIT_STATUS_WT_DELETED))
    {
        status = (EFileStatus::STATUS_TRACKED | EFileStatus::STATUS_DELETED | EFileStatus::STATUS_STAGED | EFileStatus::STATUS_UNSTAGED);
    }
    // modified file, staged
    else if (in_git_status == (GIT_STATUS_INDEX_MODIFIED | GIT_STATUS_WT_MODIFIED))
    {
        status = (EFileStatus::STATUS_TRACKED | EFileStatus::STATUS_MODIFIED | EFileStatus::STATUS_STAGED | EFileStatus::STATUS_UNSTAGED);
    }
    // deleted file staged
    else if (in_git_status == GIT_STATUS_INDEX_DELETED)
    {
        status = (EFileStatus::STATUS_TRACKED | EFileStatus::STATUS_DELETED | EFileStatus::STATUS_STAGED);
    }
    // deleted file staged
    else if (in_git_status == (GIT_STATUS_INDEX_DELETED | GIT_STATUS_WT_NEW))
    {
        status = (EFileStatus::STATUS_TRACKED | EFileStatus::STATUS_DELETED | EFileStatus::STATUS_STAGED );
    }
    // new file staged
    else if (in_git_status == GIT_STATUS_INDEX_NEW)
    {
        status = (EFileStatus::STATUS_TRACKED | EFileStatus::STATUS_NEW | EFileStatus::STATUS_STAGED);
    }
    // new file staged
    else if (in_git_status == (GIT_STATUS_INDEX_NEW | GIT_STATUS_WT_DELETED))
    {
        status = (EFileStatus::STATUS_TRACKED | EFileStatus::STATUS_NEW | EFileStatus::STATUS_STAGED | EFileStatus::STATUS_UNSTAGED);
    }
    // new file staged
    else if (in_git_status == (GIT_STATUS_INDEX_NEW | GIT_STATUS_WT_MODIFIED))
    {
        status = (EFileStatus::STATUS_TRACKED | EFileStatus::STATUS_NEW | EFileStatus::STATUS_STAGED | EFileStatus::STATUS_UNSTAGED);
    }
    else
    {
        error ("GIT", "git uncaught status : ", cvt_git_status_to_str(in_git_status).toStdString());
    }
    
    return status;
}

}
