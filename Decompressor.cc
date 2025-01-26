/* Copyright 2009-2014 e-soul.org. All rights reserved.
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

#include <string>
#include <bzlib.h>

#include "Decompressor.hh"
#include "PortracException.hh"
#include "Logger.hh"

static const int BUFF_LEN = 1024 * 1024;

void Decompressor::decompress(std::string source, std::string destination)
{
    //TODO use try-finally to clean up resources
    int bzerror = 0;
    FILE *sourceFile = openFile(source, std::string("rb"));
    FILE *destinationFile = openFile(destination, std::string("wb"));
    BZFILE *bzfile = BZ2_bzReadOpen(&bzerror, sourceFile, 0, 0, NULL, 0);
    if(BZ_OK != bzerror)
    {
        std::string message = Logger::libraryCallError(std::string("bz2"), std::string("BZ2_bzReadOpen"), source, std::to_string(bzerror));
        throw PortracException(message);
    }
    signed char buf[BUFF_LEN] = {0};
    int bytesRead = 0;
    int bytesWritten = 0;
    while(true)
    {
        bytesRead = BZ2_bzRead(&bzerror, bzfile, buf, BUFF_LEN);
        if((BZ_OK == bzerror) && 0 < bytesRead)
        {
            bytesWritten = fwrite(buf, sizeof(signed char), bytesRead, destinationFile);
            if((bytesWritten != bytesRead) || ferror(destinationFile))
            {
                std::string message = std::string("Cannot write to ");
                message += destination;
                throw PortracException(message);
            }
        }
        else if((BZ_STREAM_END == bzerror))
        {
            //TODO remove code duplication
            bytesWritten = fwrite(buf, sizeof(signed char), bytesRead, destinationFile);
            if((bytesWritten != bytesRead) || ferror(destinationFile))
            {
                std::string message = std::string("Cannot write to ");
                message += destination;
                throw PortracException(message);
            }
            break;
        }
        else
        {
            std::string message = std::string("Cannot read from ");
            message += source;
            throw PortracException(message);
        }
    }
    BZ2_bzReadClose(&bzerror, bzfile);
    if(BZ_OK != bzerror)
    {
        //TODO log warning
    }
    fclose(destinationFile);
    fclose(sourceFile);
}

FILE *Decompressor::openFile(std::string path, std::string mode)
{
    FILE *file = fopen(path.c_str(), mode.c_str());
    if(!file || ferror(file))
    {
        std::string message("Cannot open ");
        message += path;
        throw PortracException(message);
    }
    return file;
}
