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
#include <QString>
#include <QFileInfo>
#include <QLinkedList>

struct git_repository;
struct git_index;

namespace gitoku
{


enum EFileStatus
{
    STATUS_UNKNOWN      = 0,
    STATUS_TRACKED      = 1 << 0,
    STATUS_UNTRACKED    = 1 << 1,
    STATUS_MODIFIED     = 1 << 2,
    STATUS_CONFLICTED   = 1 << 3,
    STATUS_NEW          = 1 << 4,
    STATUS_DELETED      = 1 << 5,
    STATUS_STAGED       = 1 << 6,
    STATUS_UNSTAGED     = 1 << 7,
};
    
class CVcsFile
{
    public:
        QFileInfo   m_file_info;
        QString     m_path;
        int         m_status;
};

    
class CGitRepo
{
    //methods
    public:
        CGitRepo ();
        virtual ~CGitRepo();

        virtual bool open (const QString& in_path);
        virtual void close();
        
        virtual const QLinkedList< CVcsFile >& get_repository_status();
        virtual QString get_repository_path();

    //methods
    private:
        int cvt_git_status (int in_git_status);
        static int get_file_status (const char* in_p_file_path, unsigned int in_status, void* out_p_status_list);
        
    //members
    private:
        QString         m_path;
        git_repository* m_p_repository;
        git_index*      m_p_repository_index;

        QLinkedList<CVcsFile> m_file_status_list;
};

}