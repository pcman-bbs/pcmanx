
// Name:        nancybot.h
// Purpose:     Header of NancyBot.
// Author:      Youchen Lee ( utcr.org ) 
// Licence:     GPL : http://www.gnu.org/licenses/gpl.html

#ifndef __INCLUDE_NANCYBOT_H__
#define __INCLUDE_NANCYBOT_H__

#include <cstring>
#include <cstdlib>
#include <iostream>
#include <cstdio>
#include <ctime>
#include <vector>
#include <map>
#include <sstream>
using namespace std;

#include "nancy_bot/msgdata.h"

class NancyBot
{
  private:
    // ************************** functions
    int initFilename();
    int checkMsgRepeat(string);
    bool writeLog(string &, string &);
    bool replaceFirstString(string & modify_me, string &find_me, string &replace_with_me);
    bool askHowToAnser(string &, string &);
    
    //******************************* vars
    // base
    string filename_conf;
    string CONFIG_PATH;
    string BOT_NAME;
    string BOT_INDEX;
    unsigned char BOT_LEVEL;
    MsgData *pMyMsgData;
    int BOT_STATUS;

    // angry
    string Array_MsgRemember[5];
    int memory_index;  // index for Array_MsgRemember

    // learning
    bool just_asking;
    string just_asking_unknow_msg;
    unsigned int HARD_WORKING;
    string ask_flag;

    // log
    FILE *fp_log;
    
    
  public:
    NancyBot (const char * bot_name = "default", const char * config_path = "./",
		    char old_bot_level = 037);
    ~NancyBot ();
    int flag;
    string NANCY_VERSION;
    string askNancy (string);
    static map<string, MsgData *> BOTS_LIST;
    void setHardWorking(int num)
    {
	    HARD_WORKING = num;
    }
#ifdef ADV_BOT_DEBUG
    void debug (void);
#endif

};

#endif  // nancybot.h
