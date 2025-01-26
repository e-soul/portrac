/* Copyright 2009-2025 e-soul.org. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided
 * that the following conditions are met:
 *
 *   1. Redistributions of source code must retain the above copyright notice, this list of conditions and
 *      the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions
 *      and the following disclaimer in the documentation and/or other materials provided with the
 *      distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY E-SOUL.ORG ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL E-SOUL.ORG
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those of the authors and
 * should not be interpreted as representing official policies, either expressed or implied, of e-soul.org.
 */

#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sstream>
#include <string>
#include <algorithm>
#include <iostream>

#include <QStringList>

#include "PortChecker.hh"
#include "Preferences.hh"
#include "Environment.hh"
#include "Downloader.hh"
#include "Logger.hh"
#include "Decompressor.hh"

using namespace std;

PortChecker::PortChecker(Preferences* preferences)
{
	this->preferences = preferences;
	pipeFileDescr[0] = -1;
	pipeFileDescr[1] = -1;
}

PortChecker::~PortChecker()
{
}

QStringList* PortChecker::check()
{
	int status = pipe(pipeFileDescr);
	if(status != 0)
	{
		perror("pipe");
		return NULL;
	}
	QStringList* list = new QStringList;
	switch(fork())
	{
		case -1: // fork failed
			return list;
		case 0: // child
			close(pipeFileDescr[0]);
			dup2(pipeFileDescr[1], 1);
			executePkgVersion();
			break; // should not happen because there's a call to exec()
		default: // parent
			close(pipeFileDescr[1]);
			catchPkgVersionOutput(list);
			close(pipeFileDescr[0]);
			// make sure *any* child processes have terminated
			waitpid(-1, NULL, 0);
			waitpid(0, NULL, 0);
	}
	return list;
}

void PortChecker::executePkgVersion()
{
	const char* index;
	if(isCompressedIndex())
	{
		try
        {
            index = handleCompressedIndex();
        }
        catch(PortracException e)
        {
            cerr << e.what() << endl;
            return;
        }
	}
	else
	{
		index = (preferences->getPreference(INDEX_PREF).c_str());
	}
	execl("/usr/sbin/pkg", "pkg", "version", "-I", index, (char *) 0);
	perror("execl");
}

void PortChecker::catchPkgVersionOutput(QStringList* list)
{
	int n, i;
	char buff[BUFF_LEN] = {'\0'};
	string s;

	while((n = read(pipeFileDescr[0], buff, BUFF_LEN)) > 0)
	{
		for(i = 0; i < n; i++)
		{
			if(buff[i] == '\n')
			{
				if(s.find("<") != string::npos)
				{
					size_t endpos = s.find_last_not_of(" \t");
					s.erase(endpos);
					list->append(s.c_str());
				}
				s.clear();
			}
			else
			{
				s.append(1, buff[i]);
			}
		}
	}
}

bool PortChecker::isCompressedIndex()
{
	string index = preferences->getPreference(INDEX_PREF);
	if(index.find(".bz2", index.find_last_of("/")) != string::npos)
	{
		return true;
	}
	return false;
}

const char* PortChecker::handleCompressedIndex()
{
	string file = downloadIndex();
	Decompressor::decompress(file, file.substr(0, file.find_last_of('.')));
    return file.substr(0, file.find_last_of('.')).c_str();
}

string PortChecker::downloadIndex() const
{
    string dataDir = Environment::getDataDir();
    //TODO use system calls
    string mkdirStr("mkdir -p ");
    mkdirStr += dataDir;
    system(mkdirStr.c_str());
    struct stat statStruct;
    if(stat(dataDir.c_str(), &statStruct))
    {
        string logEntry = Logger::systemCallError(string("stat"), dataDir, string(strerror(errno)));
        throw PortracException(logEntry);
    }
    int modeMask = S_IFDIR | S_IRWXU;
    if((statStruct.st_mode & modeMask) == modeMask)
    {
        Downloader downloader;
        string url = preferences->getPreference(INDEX_PREF);
        size_t fileNameIndex = url.find_last_of('/');
        size_t fileNameLength = url.length() - fileNameIndex;
        ostringstream file(dataDir, ios_base::app);
        file << '/' << url.substr(fileNameIndex, fileNameLength);
        downloader.download(url, file.str());
        return file.str();
    }
    string message(dataDir);
    message += ": invalid directory!";
    throw PortracException(message);
}

void PortChecker::run()
{
	list = check();
}

QStringList* PortChecker::getList()
{
	return list;
}
