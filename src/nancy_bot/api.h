
// Name:        nancybot.h
// Purpose:     Header of NancyBot.
// Author:      Youchen Lee ( utcr.org ) 
// Licence:     GPL : http://www.gnu.org/licenses/gpl.html

/* ## NancyBot Settings HOWTO ##
 * 
 * # Constructor:
 * 	NancyBot(const char *BOT_NAME, const char *CONFIG_PATH, unsigned char BOT_RUN_LEVEL );
 *	
 *	
 * # BOT_RUN_LEVEL: ( set in constructor, and auto-reset when initialing config files )
 * <LEVEL>        <OCT>  <Definitions>
 * USE_BASE        001   Use basic messages in ${BOT_NAME}_msg.data
 * USE_UNKNOW      002   Use unknow messages in ${BOT_NAME}.conf [UNKNOW]
 * USE_ANGRY       004   Use angry messages in ${BOT_NAME}.conf [ANGRY]
 * USE_LOG         010   Use chat log, writting in ${BOT_NAME}._${BOT_NUN}.log
 * USE_AUTO_LEARN  020   Use auto learning, read below for details.
 *
 * 
 * # AUTO_LEARN SETTINGS: (available to reset in runtime)
 * 0 <= num <= 100
 * NancyBot::setLevel__AddToUnknowMsg(num)    The probability that Nancy add unknow messages 
 *                                              to unknow messages MAP.
 * NancyBot::setLevel__AskUnknowMsg(num)      The probability that Nancy ask the messages form unknow
 *                                              messages MAP.
 * NancyBot::setLevel__ReLearning(num)        The probability that Nancy ask the messages she had already
 *                                              known.
 *
 */

#ifndef __INCLUDE_NANCYBOT_H__
#define __INCLUDE_NANCYBOT_H__

#include "pcmanx_utils.h"

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
#define TEACH_BOT
class NancyBot
{
  private:
    // ************************** functions
    int initFilename();
    int checkMsgRepeat(string);
    bool writeLog(string &, string &);
    bool askHowToAnser(string &, string &);
    //******************************* vars
    // base
    string filename_conf;
    string CONFIG_PATH;
    string BOT_NAME;
    string BOT_INDEX;
    unsigned char BOT_RUN_LEVEL;
    MsgData *pMyMsgData;
    int BOT_STATUS;

    // angry
    string Array_MsgRemember[5];
    int memory_index;  // index for Array_MsgRemember

    // learning
    bool just_asked;
    bool add_to_unknow;
    string just_asked_unknow_msg;
    unsigned int LEVEL__ASK_UNKNOW_MSG;
    unsigned int LEVEL__ADD_TO_UNKNOW_MSG;
    unsigned int LEVEL__RE_LEARNING;
    string ask_flag;
    bool level__ask_unknow_msg_changed;
    bool level__add_to_unknow_msg_changed;
    bool level__re_learning_changed;

    // log
    FILE *fp_log;
    
    
  public:
    NancyBot (const char * bot_name = "default", const char * config_path = "./",
		    unsigned char old_bot_run_level = 0177);
    ~NancyBot ();
    int flag;
    string askNancy (string);
    static map<string, MsgData *> BOTS_LIST;
    void setLevel__AskUnknowMsg(int num)
    {
	    LEVEL__ASK_UNKNOW_MSG = num;
	    level__ask_unknow_msg_changed = true;
    }
    void setLevel__AddToUnknowMsg(int num)
    {
	    LEVEL__ADD_TO_UNKNOW_MSG = num;
	    level__add_to_unknow_msg_changed = true;
    }

    void setLevel__ReLearning(int num)
    {
	    LEVEL__RE_LEARNING = num;
	    level__re_learning_changed = true;
    }

#ifdef ADV_BOT_DEBUG
    void debug (void);
#endif

};

#endif  // nancybot.h
