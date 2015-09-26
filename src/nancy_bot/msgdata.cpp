/**
 * msgdata.cpp - Class of messages MAPs for Bot.
 *
 * Copyright (c) 2005 Youchen Lee ( utcr.org )
 *                                                                              
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <cstdlib>
#include <unistd.h>
#include <stdio.h>

#include "nancy_bot/msgdata.h"
#include "nancy_bot/botutil.h"
#include "fileutil.h"

int MsgData::errorHandler(int level, const string &flag)
{
	if( level <= 0 ){
		perror(flag.c_str());
		return 1; // error
	}
	else return 0;
}

int MsgData::errorHandler(int level, const string &flag, const string &msg)
{
	if( level <= 0 ){
		fprintf(stderr, "%s: %s", flag.c_str(), msg.c_str());
		return 1; // error
	}
	else return 0;
}


MsgData::MsgData(string bot_name, string config_path, unsigned char old_run_level, int level__re_learning, int level__add_to_unknow_msg)
{
	learn_something = false;
	srand(time(NULL));
	BOT_NAME = bot_name;
	BOT_RUN_LEVEL = old_run_level;
	LEVEL__ADD_TO_UNKNOW_MSG = level__add_to_unknow_msg; // Default: 100
	LEVEL__RE_LEARNING = level__re_learning; // Default: 5
	CONFIG_PATH = config_path;
	if( *(CONFIG_PATH.end() -1) != '/' )
		CONFIG_PATH += '/';
	ref_counter = 1;
	initFilename();

#ifndef CONSOLE_BOT
#ifdef DATADIR
	if(access(CONFIG_PATH.c_str(), F_OK))
		mkdir( CONFIG_PATH.c_str(), 0755 );

	if(BOT_NAME == "default")
	{
		string srcdir = DATADIR;
		if( '/' != srcdir[srcdir.length()-1] )
			srcdir += '/';
		srcdir += "pcmanx/nancy_bot/";
		copyfile( (srcdir + "default_msg.data" ).c_str(), (CONFIG_PATH + "default_msg.data").c_str(), false);
		copyfile( (srcdir + "default.conf" ).c_str(), (CONFIG_PATH + "default.conf").c_str(), false);
		copyfile( (srcdir + "default_usages.data" ).c_str(), (CONFIG_PATH + "default_usages.data").c_str(), false);
	}
#endif
#endif
	if( (BOT_RUN_LEVEL & USE_TEACH) && !(BOT_RUN_LEVEL & USE_AUTO_LEARN) )
	{
		BOT_RUN_LEVEL ^= USE_TEACH;
	}
	if(BOT_RUN_LEVEL & USE_ANGRY)
	{
		if( errorHandler( initSpecialMsg("[ANGRY]"), "ANGRY_MSG" ))
			BOT_RUN_LEVEL ^= USE_ANGRY;
	}
	if(BOT_RUN_LEVEL & USE_UNKNOW)
	{
		if( errorHandler ( initSpecialMsg("[UNKNOW]"), "UNKNOW_MSG"))
			BOT_RUN_LEVEL ^= USE_UNKNOW;
	}

	
	if(BOT_RUN_LEVEL & USE_AUTO_LEARN)
	{
		errorHandler ( initUnknowMsgToAsk() ,"INITIAL UNKNOW MSG TO ASK" );
		if( (errorHandler ( initSpecialMsg("[ASK]"), "ASK_MSG")) )
		{
			BOT_RUN_LEVEL ^= USE_AUTO_LEARN;
		}
	}
		

	if(BOT_RUN_LEVEL & USE_BASE)
	{
		if (errorHandler( initCommonMsg(), "BASE_MSG" ) < 0 )
			BOT_RUN_LEVEL ^= USE_BASE;
	}
	if(BOT_RUN_LEVEL & USE_USER_DEFINED_USAGES)
	{
		if (errorHandler( initUserDefinedUsages(), "USER_DEFINED_USAGES" ) < 0 )
		{
			BOT_RUN_LEVEL ^= USE_USER_DEFINED_USAGES;
		}
	}
}

int
MsgData::writeToMsgData() // FIXME
{
	FILE *fptr = fopen(filename_common_msg.c_str(), "a+t");
	if (!fptr)
		return -1;
	VS_map::iterator cur = VSM_MsgLearnToSave.begin();
	string buf;
	buf = "# New written:\n";
	for(; cur != VSM_MsgLearnToSave.end(); cur++)
	{
		buf += ">" + cur->first + "\n";
		for(unsigned int i = 0; i < (cur->second).size()  ; i++)
		{
			buf += (cur->second)[i] + "\n";
		}
	}
	fputs(buf.c_str(), fptr);
	fclose(fptr);
	return 1;
}

bool
MsgData::writeUnknowLog()
{
	FILE *fptr = fopen( filename_unknow_log.c_str(), "w+t");
	string buf = "";
	if(!fptr)
		return false;
	else
	{
		map<string, bool>::iterator cur = M_MsgUnknowToAsk.begin();
	
		for(; cur !=  M_MsgUnknowToAsk.end(); cur++ )
		{
			if(!cur->second) // not new messages, already recognized
				buf = buf + ">" + cur->first + "\n";
		}
		fputs(buf.c_str(), fptr);
		fclose(fptr);
		return true;
	}
}

MsgData::~MsgData()
{
	if(BOT_RUN_LEVEL & USE_AUTO_LEARN)
	{
		if(learn_something)
		{
			writeToMsgData();
		}
		writeUnknowLog();
	}
}

int MsgData::initFilename()
{	
	if( BOT_NAME.length() ){
		filename_conf = CONFIG_PATH + BOT_NAME + ".conf";
		filename_common_msg = CONFIG_PATH + BOT_NAME + "_msg.data";
		filename_unknow_log = CONFIG_PATH + BOT_NAME + "_unknow.log";
		filename_user_defined_usages = CONFIG_PATH + BOT_NAME + "_usages.data";
		return 0;
	}
	else return -1;
}

int
MsgData::initSpecialMsg(string flag)
{
	FILE *fptr = fopen(filename_conf.c_str(), "r");
	vector<string> *pVMsg;
	if(!fptr){
		return 0;
	
	}
	else // start initial special msg
		// logic original written by pcmanx
	{
		if(flag == "[ANGRY]")
			pVMsg = &V_MsgAngry;
		else if(flag == "[UNKNOW]")
			pVMsg = &V_MsgUnknow;
		else if(flag == "[ASK]")
			pVMsg = &V_MsgAsk;
		else
		{
			fprintf(stderr, "%s: No such flag.\n", flag.c_str());
		}
	
		char buf[4096];
		int flag_len = flag.length();
		bool reading = false;
		while (fgets (buf, sizeof (buf), fptr) )
		{
		        char *line = strtok (buf, "\n\r\t");
		        if (!line || !*line)
			{
				continue;
			}
			if (*line == '#')
				continue;
			if (reading)
			{
				if(*line == '[')
				{
					if ( 0 == strncmp (line, "[ANGRY]", strlen("[ANGRY]"))
				       		|| 0 == strncmp (line, "[UNKNOW]", strlen("[UNKNOW]"))
				       		|| 0 == strncmp (line, "[ASK]", strlen("[ASK]")))
					{
						reading = false;
						break;
					}
				}
				pVMsg->push_back((string)line);
			}
			else if (*line == '[' && 0 == strncmp (line, flag.c_str(), flag_len))
			{
				reading = true;
				continue;
			}
		}
		fclose(fptr);
	}
	return (int)pVMsg->size(); 
}

int MsgData::initUserDefinedUsages()
{
	string filename = filename_user_defined_usages;
	FILE *fptr = fopen( filename.c_str(), "rt");
	string index;
	
	
	if(!fptr)
	{
		return 0;
	}
	else
	{
		char buf[4096];
		while (fgets (buf, sizeof (buf), fptr) )
		{
			bool just_got_key = false;
		        char *line = strtok (buf, "\n\r\t");
		        if (!line || !*line)
			{
				continue;
			}
			if (*line == '#')
				continue;
			if(!just_got_key && *line == '>')
			{
					goto GOT_ME;
			}
			else
			{
				VSM_UserDefinedUsages[index].push_back( (string)line );
				continue;
			}
			
			if (*line == '>')
			{
GOT_ME:
				just_got_key = true;
				line++;
				index = (string)line;
				vector<string> V_s;
				VSM_UserDefinedUsages[index] = V_s;
				continue;
			}
		}
		fclose(fptr);
	}
	return (int)VSM_UserDefinedUsages.size(); 
}

int
MsgData::initCommonMsg()
{
	string filename = filename_common_msg;
	FILE *fptr = fopen( filename.c_str(), "rt");
	int len;
	string index;
	
	
	if(!fptr)
	{
		return 0;
	}
	else
	{
		char buf[4096];
		while (fgets (buf, sizeof (buf), fptr) )
		{
			bool just_got_key = false;
		        char *line = strtok (buf, "\n\r\t");
		        if (!line || !*line)
			{
				continue;
			}
			if (*line == '#')
				continue;
			if(!just_got_key && *line == '>')
			{
					goto GOT_VALUE;
			}
			else
			{
				MT[len][index].push_back( (string)line );
				continue;
			}
			
			if (*line == '>')
			{
GOT_VALUE:
				just_got_key = true;
				line++;
				len = strlen(line);
				index = (string)line;
				
				if( MT.find(len) == MT.end() )
				{
					VS_map *vsm = new VS_map;
					MT[len] = *vsm;
				}
				continue;
			}
		}
		fclose(fptr);
	}
	return (int)MT.size(); 
}

int MsgData::initUnknowMsgToAsk()
{
	FILE *fptr = fopen( filename_unknow_log.c_str(), "rt");
	string index;
	if(!fptr)
	{
		return 0;
	}
	else
	{
		char buf[4096];
		while (fgets (buf, sizeof (buf), fptr) )
		{
		        char *line = strtok (buf, "\n\r");
		        if (!line || !*line)
			{
				continue;
			}
			if (*line == '#')
				continue;
			if (*line == '>')
			{
				line++;
				M_MsgUnknowToAsk[line] = false;
				continue;
			}
		}
		fclose(fptr);
	}
	return 1;
//	return (int)M_MsgUnknowToAsk.size();
}

int
MsgData::addUnknowMsgToAsk(string & unknow_msg)
{
	if(M_MsgUnknowToAsk.find(unknow_msg) == M_MsgUnknowToAsk.end()) // not found
	{
		M_MsgUnknowToAsk[unknow_msg] = false;
		return 1;
	}
	else return 0;
}

int
MsgData::addOldMsgToAskAgain(string old_msg)
{
        if(M_MsgUnknowToAsk.find(old_msg) == M_MsgUnknowToAsk.end()) // not found
	{
		M_MsgUnknowToAsk[old_msg] = false;
		return 1;
	}
	else return 0;	
}

//FIXME
int
MsgData::learning(string &key, string &msg_to_remember)
{
	//M_MsgUnknowToAsk[key] = true;
	M_MsgUnknowToAsk.erase(key);          // TODO , don't erase but set to true, Learn more.
	int len = key.length();
	// save to memory
	if( MT.find(len) == MT.end())
	{
		VS_map *vsm = new VS_map;
		(*vsm)[key].push_back(msg_to_remember);
		MT[len] = *vsm;
	}
	else
	{
		if( MT[len].find(key) == MT[len].end() )
		{
			vector<string> V;
			V.push_back(msg_to_remember);
			MT[len][key] = V;
		}
		else
		{
			MT[len][key].push_back(msg_to_remember);
		}
	}

	// FIXME
	// save to Map ( for write to file )
	if(VSM_MsgLearnToSave.find(key) == VSM_MsgLearnToSave.end()) // not found
	{
		vector<string> V2;
		V2.push_back(msg_to_remember);
		VSM_MsgLearnToSave[key] = V2;
	}
	else
	{
		VSM_MsgLearnToSave[key].push_back(msg_to_remember);
	}
	
	learn_something = true;
	return 1;
}


int
MsgData::getSpecialMsg(int status, string &msg )
{
	int random = 100;
	if(status == 1) // angry
	{
		if((int)V_MsgAngry.size() == 0 )
			return 0;
		random = rand() % (int) V_MsgAngry.size();
		msg = V_MsgAngry[random];
	}
	else if(status == 2) // unknow
	{
		if((int)V_MsgUnknow.size() == 0 )
			return 0;
		random = rand() % (int) V_MsgUnknow.size();
		msg = V_MsgUnknow[random];
	}
	else if(status == 3) // ask
	{
		if( (int)V_MsgAsk.size() == 0)
			return 0;
		random = rand() % (int) V_MsgAsk.size();
		msg = V_MsgAsk[random];
	}
	return 1;
}



// FIXME: What a massup!!  ugly ugly ugly..
int MsgData::getCommonMsg(string &input, string &msg, bool add_to_unknow)
{
	int len = input.length();
	int random;
	int random2 = rand(); // learn this
	bool re_learn = false;
	bool ask = false;
	int is_this = 0;
	int learn_this;
	VS_map::iterator cur;
	
	if(BOT_RUN_LEVEL & USE_AUTO_LEARN)
	{
		if( (unsigned int) (rand() % 100) < LEVEL__RE_LEARNING ) // old msg to learn again
			re_learn = true;
		
		// FIXME: Are the two rand()s different?
		if( add_to_unknow && ((unsigned int) (rand() % 100 ) < LEVEL__ADD_TO_UNKNOW_MSG) ) // ask ( new Msg to leran )
			ask = true;
	}

	while(len > 0)
	{
		if(MT.find(len) != MT.end()) // found this len key in MAP
		{
			learn_this = random2 % (MT[len].size()+1);
			for(cur = MT[len].begin(); cur != MT[len].end(); cur++)
			{
				// choose which one to learn by a random num -- learn_this
				if(re_learn && (is_this == learn_this) )
				{
					addOldMsgToAskAgain(cur->first);
					re_learn = false;
				}
				is_this++;
				
				// not learn, just search other maching messages
				if(input.find((string)cur->first) != string::npos ) // found 
				{
					random = rand() % (cur->second).size();
					msg = (cur->second)[random];
					return 1; // got it!
				}
			}
		}
		len--; // search form other length
		
		if(ask)
		{
			addUnknowMsgToAsk(input);
			ask = false; // already asked;
		}
	}
	return 0;
}

bool
MsgData::getUserDefinedUsages(string &key,string &query, string &msg)
{
	VS_map::iterator cur;
	if( (cur = VSM_UserDefinedUsages.find(key)) == VSM_UserDefinedUsages.end() ) // not found
	{
		return 0;
	}
	else
	{
		msg = (cur->second)[rand() % (cur->second).size()];
		key.insert( 0,"{" );
		key.append( "}" );
		replaceString(msg, key, query);
		return 1;
	}
}

unsigned char
MsgData::getBotRunLevel()
{
	return BOT_RUN_LEVEL;
}


// get a unknow msg to ask by random from map --- M_MsgUnknowToAsk
bool
MsgData::getUnknowMsgToAsk(string &unknow_msg)
{
	if( M_MsgUnknowToAsk.size() == 0 )
		return false;
	else
	{
		int random = rand() % M_MsgUnknowToAsk.size();
		map<string, bool>::iterator cur = M_MsgUnknowToAsk.begin();
		for( int i = 0; i < random; i++ ) //FIXME <= ?
			cur++;
		unknow_msg = cur->first;
		return true;
	}
}

