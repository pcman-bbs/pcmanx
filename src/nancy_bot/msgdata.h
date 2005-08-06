
//// Name:        msgdata.h
//// Purpose:     Header for class MsgData.
//// Author:      Youchen Lee ( utcr.org )
//// Licence:     GPL : http://www.gnu.org/licenses/gpl.html

#ifndef __INCLUDE_MSGDATA_H__
#define __INCLUDE_MSGDATA_H__

#include <iostream>
#include <map>
#include <ctime>
#include <vector>
#include <string>

using namespace std;

// RUN_LEVEL
#define USE_BASE        001
#define USE_UNKNOW      002
#define USE_ANGRY       004
#define USE_LOG         010
#define USE_AUTO_LEARN	020

class MsgData
{
	private:
		// functions
		int initFilename();
		int initSpecialMsg(string);
		int initCommonMsg();
		int initUnknowMsgToAsk();
		int writeToMsgData();
		bool writeUnknowLog();
		int errorHandler(int, const string &);
		int errorHandler(int, const string &, const string &);
			
		
		// STL
		vector<string> V_MsgAngry;
	        vector<string> V_MsgUnknow;
		vector<string> V_MsgAsk;
		typedef map<string, vector<string> > VS_map;
		map<int, VS_map> MT; // map table

		VS_map VSM_MsgLearnToSave;
		map<string, bool> M_MsgUnknowToAsk;

		// vars
		string BOT_NAME;
		string CONFIG_PATH;
		string filename_conf;
		string filename_common_msg;
		string filename_unknow_log;

		char BOT_LEVEL;
		unsigned int HARD_WORKING_LEVEL;
		unsigned int RE_LEARNING_LEVEL;
		bool learn_something;
	public:
		MsgData(string bot_name = "default",
				string config_path = "./",
				char old_level = 037, 
				int re_learning_level = 10,
				int hard_working_level = 20 );
		~MsgData();
		int ref_counter;
		int getSpecialMsg(int status, string &);
		int getCommonMsg(string &, string &);
		int learning(string &, string &);
		bool getUnknowMsgToAsk(string &);
		int addUnknowMsgToAsk(string &);
		int addOldMsgToAskAgain(string);
		bool setReLearningLevel(int num)
		{
			if( num >= 0 )
			{
				RE_LEARNING_LEVEL = num;
				return true;
			}
			else return false;
		}
		
		char getBotLevel();

};

#endif
