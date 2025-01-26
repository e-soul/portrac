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

#include "Downloader.hh"

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <sstream>

#include <sys/param.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <fetch.h>

static const int BUFFER_SIZE = 256;
//TODO throw exceptions if any of fetchGetURL(), open() or write() fial.
void Downloader::download(std::string url, std::string file)
{
    FILE *stream = fetchGetURL(url.c_str(), NULL);
    int outputFd = createOutput(file);
    if(-1 == outputFd)
    {
        std::ostringstream msg(file, std::ios_base::app);
        msg << ' ' << strerror(errno);
        throw PortracException(msg.str().c_str());
    }
    copy(stream, outputFd);
    close(outputFd);
}

int Downloader::createOutput(std::string name)
{
    mode_t openMode = O_CREAT | O_WRONLY | O_TRUNC;
    mode_t accessMode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    return open(name.c_str(), openMode, accessMode);
}

void Downloader::copy(FILE *stream, int outputFd)
{
    char buffer[BUFFER_SIZE];
    while(!feof(stream))
    {
        int charsRead = fread(&buffer, sizeof(char), BUFFER_SIZE, stream);
        write(outputFd, &buffer, charsRead);
    }
}
