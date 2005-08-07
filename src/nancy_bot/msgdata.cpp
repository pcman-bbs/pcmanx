
// Name:        msgdata.cpp
// Purpose:     Class of messages MAPs for Bot.
// Author:      Youchen Lee ( utcr.org ) 
// Licence:     GPL : http://www.gnu.org/licenses/gpl.html

#include "nancy_bot/msgdata.h"

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


MsgData::MsgData(string bot_name, string config_path, char old_level, int re_learning_level, int hard_working_level)
{
	learn_something = false;
	srand(time(NULL));
	BOT_NAME = bot_name;
	BOT_LEVEL = old_level;
	HARD_WORKING_LEVEL = hard_working_level; // Default: 20
	RE_LEARNING_LEVEL = re_learning_level; // Default: 10
	CONFIG_PATH = config_path;
//	cout << *(CONFIG_PATH.end() -1) << endl;
	if( *(CONFIG_PATH.end() -1) != '/' )
		CONFIG_PATH += "/";
	ref_counter = 1;
	initFilename();
	if(BOT_LEVEL & USE_ANGRY)
	{
		if( errorHandler( initSpecialMsg("[ANGRY]"), "ANGRY_MSG" ))
			BOT_LEVEL ^= USE_ANGRY;
	}
	if(BOT_LEVEL & USE_UNKNOW)
	{
		if( errorHandler ( initSpecialMsg("[UNKNOW]"), "UNKNOW_MSG"))
			BOT_LEVEL ^= USE_UNKNOW;
	}

	
	if(BOT_LEVEL & USE_AUTO_LEARN)
	{
		errorHandler ( initUnknowMsgToAsk() ,"INITIAL UNKNOW MSG TO ASK" );
		if( (errorHandler ( initSpecialMsg("[ASK]"), "ASK_MSG")) )
		{
			BOT_LEVEL ^= USE_AUTO_LEARN;
		}
	}
		

	if(BOT_LEVEL & USE_BASE)
	{
		if (errorHandler( initCommonMsg(), "BASE_MSG" ) < 0 )
			BOT_LEVEL ^= USE_BASE;
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
		for(int i = 0; i < (cur->second).size()  ; i++)
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
	if(BOT_LEVEL & USE_AUTO_LEARN)
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
	return (int)M_MsgUnknowToAsk.size();
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
int MsgData::getCommonMsg(string &input, string &msg)
{
	int len = input.length();
	int random;
	int random2 = rand(); // learn this
	bool re_learn = false;
	bool ask = false;
	int is_this = 0;
	int learn_this;
	VS_map::iterator cur;
	
	if(BOT_LEVEL & USE_AUTO_LEARN)
	{
		if( (rand() % 100) < RE_LEARNING_LEVEL ) // old msg to learn again
			re_learn = true;
		
		// FIXME: Are the two rand()s different?
		if( (rand() % 100 ) < HARD_WORKING_LEVEL ) // ask ( new Msg to leran )
			ask = true;
	}

	while(len > 0)
	{
		if(MT.find(len) != MT.end()) // found this len key in MAP
		{
			learn_this = random2 % MT[len].size();
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
				if(input.find((string)cur->first) != -1 ) // found 
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
		
char
MsgData::getBotLevel()
{
	return BOT_LEVEL;
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

