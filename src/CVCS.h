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
    
struct SFileStatus
{
    QFileInfo   m_file_info;
    QString     m_path;

    int         m_status;
};

class CVCS
{
    //methods
    public:
        /**
         * @brief ...
         **/
        CVCS () {};

        /**
         * @brief ...
         * @param inPath repository root path
         * 
         * @return True if repository correctly opened, False if not
         **/
        virtual bool open (const QString& in_path) = 0;
        
        /**
         * @brief ...
         *
         * @return void
         **/
        virtual void close () = 0;

        /**
         * @brief return the status of all files in the repository
         * @see EFileStatus
         *
         * @return list of all file status
         **/
        virtual const QLinkedList<SFileStatus>& get_repository_status () = 0;
        
        virtual ~CVCS() {};

    //members
    protected:

};

}