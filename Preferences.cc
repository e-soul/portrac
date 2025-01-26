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

#include <unistd.h>

#include <string>
#include <map>
#include <iostream>
#include <fstream>

#include "Preferences.hh"

using namespace std;

Preferences::Preferences()
{
}

Preferences::~Preferences()
{
}

void Preferences::load()
{
	char* home = getenv("HOME");
	string config(home);
	config += "/.portrac";
	ifstream configFile(config.c_str());
	if(!configFile.fail())
	{
		// file exists
		string line;
		while(!configFile.eof())
		{
			getline(configFile, line);
			processInputLine(line);
		}
		configFile.close();
	}
	else
	{
		// initialise default values
		preferencesMap[INDEX_PREF] = "http://www.freebsd.org/ports/INDEX-10.bz2";
		preferencesMap[CHECK_INTERVAL_PREF] = "3";
	}
}

void Preferences::persist()
{
	char* home = getenv("HOME");
	string config(home);
	config += "/.portrac";
	ofstream configFile(config.c_str(), ios::out | ios::trunc);
	map<string, string>::iterator i = preferencesMap.begin();
	while(i != preferencesMap.end())
	{
		configFile << i->first << '=' << i->second << endl;
		i++;
	}
	configFile.close();
}

string Preferences::getPreference(string preference)
{
	return preferencesMap[preference];
}

void Preferences::setPreference(string preference, string value)
{
	preferencesMap[preference] = value;
}

void Preferences::processInputLine(string line)
{
	string::size_type loc = line.find('=');
	if(loc != string::npos)
	{
		string pref = line.substr(0, loc);
		string val = line.substr(loc + 1);
		if((pref.compare(INDEX_PREF) == 0)
			|| (pref.compare(CHECK_INTERVAL_PREF) == 0))
		{
			setPreference(pref, val);
		}
	}
}
