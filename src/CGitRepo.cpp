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


#include "CGitRepo.h"
#include "log.h"
#include <git2.h>


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


CGitRepo::CGitRepo ()
    :m_p_repository(nullptr),
	 m_p_repository_index(nullptr)
{
}

CGitRepo::~CGitRepo()
{
    close();
}

bool CGitRepo::open(const QString& in_path)
{
    int open_status = GIT_ERROR;
    if (!in_path.isEmpty() && !m_p_repository)
    {
        char repo_found [512];
        int discover_status = git_repository_discover(repo_found, sizeof(repo_found), in_path.toAscii(), true, "");

        if (discover_status == GIT_OK)
        {
            m_path = repo_found;
            open_status = git_repository_open(&m_p_repository, m_path.toAscii());
            if (open_status == GIT_OK)
            {
                //get the repository index
                open_status = git_repository_index(&m_p_repository_index, m_p_repository);

                update_repo_status();
                
                if (open_status == GIT_OK)
                {
                    debug ("GIT", "Git repository successfully opened");
                }
                else
                {
                    error ("GIT", "error occured when reading repository index");
                }
            }
            else
            {
                error ("GIT", "error occured when open Git repository => ", m_path.toStdString(), ".\nError [",open_status,"] => ", giterr_last()->message);
            }
        }
        else
        {
            error ("GIT", "error occured when discover Git repository => ", m_path.toStdString(), ".\nError [",discover_status,"] => ", giterr_last()->message);
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
    return (open_status == GIT_OK);
}

void CGitRepo::close()
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


int CGitRepo::get_file_status (const char* in_p_file_path, unsigned int in_git_status, void* out_p_this)
{
    cond_assert(LOG_COND(out_p_this), "GIT");
    CGitRepo* p_this = static_cast<CGitRepo*>(out_p_this);

    CVcsFile file_status;

    file_status.m_file_info= QFileInfo(in_p_file_path);
    file_status.m_path     = QString(in_p_file_path);
    file_status.m_status   = p_this->cvt_git_status(in_git_status);

    p_this->m_file_status_list.push_back(file_status);

    return GIT_OK;
}

void CGitRepo::update_repo_status ()
{
    //update file status list
    m_file_status_list.clear();
    git_status_foreach(m_p_repository, &get_file_status, static_cast<void*>(this));
}

static int printer( void *data, const git_diff_delta *delta, const git_diff_range *range, char usage, const char *line, size_t line_len)
{
    std::cout << line << std::endl;
    return 0;
}

QString CGitRepo::diff_to_head(const QString& in_file)
{
    git_diff_options opts;
    memset(&opts,0, sizeof(git_diff_options));
    
    std::string file = in_file.toStdString();
    
    char* p_file = const_cast<char*>(file.c_str());
    opts.pathspec.strings = & p_file;
    opts.pathspec.count = 1;

    debug ("GIT", *opts.pathspec.strings);
    
    git_diff_list* p_diff_list = 0;
    
    git_tree *p_head = nullptr;
    bool ok = resolve_to_tree("HEAD", &p_head);
    
    debug("GIT", ok);
    int status = git_diff_workdir_to_tree(&p_diff_list, m_p_repository, p_head, &opts);
    if (status == GIT_OK)
    {
        git_diff_print_patch(p_diff_list, 0, printer);
    }
    else
    {
        error ("GIT", "error occured when diff file to index => ", giterr_last()->message);
    }
}


QString CGitRepo::get_repository_path()
{
    QString path;
    if (m_p_repository)
    {
        path = git_repository_workdir(m_p_repository);
    }

    return path;
}


bool CGitRepo::resolve_to_tree(const std::string& in_identifier, git_tree **out_p_tree)
{
    bool ok = false;
    git_oid oid;
    git_object *p_obj = nullptr;

    /* try to resolve as OID */
    if (git_oid_fromstrn(&oid, in_identifier.c_str(), in_identifier.size()) == GIT_OK)
    {
        git_object_lookup_prefix(&p_obj, m_p_repository, &oid, in_identifier.size(), GIT_OBJ_ANY);
    }
  
    /* try to resolve as reference */
    if (!p_obj) 
    {
        git_reference *ref, *resolved;
        if (git_reference_lookup(&ref, m_p_repository, in_identifier.c_str()) == GIT_OK) 
        {
            git_reference_resolve(&resolved, ref);
            git_reference_free(ref);
            if (resolved) 
            {
                git_object_lookup(&p_obj, m_p_repository, git_reference_oid(resolved), GIT_OBJ_ANY);
                git_reference_free(resolved);
            }
        }
    }

    if (p_obj != nullptr)
    {
        switch (git_object_type(p_obj)) 
        {
            case GIT_OBJ_TREE:
                *out_p_tree = (git_tree *)p_obj;
                ok = true;
                break;
            case GIT_OBJ_COMMIT:
                ok = GIT_OK == git_commit_tree(out_p_tree, (git_commit *)p_obj);
                git_object_free(p_obj);
                break;
            default:
                ok = false;
        }
    }

    return ok;
}

int CGitRepo::cvt_git_status(int in_git_status)
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
