/**
 * msgdata.h - Header for class MsgData.
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
#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>


using namespace std;

// BOT_RUN_LEVEL
#define USE_BASE        	(unsigned char) 001
#define USE_UNKNOW      	(unsigned char) 002
#define USE_ANGRY       	(unsigned char) 004
#define USE_LOG         	(unsigned char) 010
#define USE_AUTO_LEARN		(unsigned char) 020
#define USE_TEACH		(unsigned char) 040
#define USE_USER_DEFINED_USAGES	(unsigned char) 0100

class MsgData
{
	private:
		// functions
		int initFilename();
		int initSpecialMsg(string);
		int initCommonMsg();
		int initUnknowMsgToAsk();
		int initUserDefinedUsages();
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
		VS_map VSM_UserDefinedUsages;
		map<string, bool> M_MsgUnknowToAsk;

		// vars
		string BOT_NAME;
		string CONFIG_PATH;
		string filename_conf;
		string filename_common_msg;
		string filename_unknow_log;
		string filename_user_defined_usages;

		unsigned char BOT_RUN_LEVEL;
		unsigned int LEVEL__ADD_TO_UNKNOW_MSG;
//		unsigned int LEVEL__ASK_UNKNOW_MSG;
		unsigned int LEVEL__RE_LEARNING;
		bool learn_something;
	public:
		MsgData(string bot_name = "default",
				string config_path = "./",
				unsigned char old_level = 0177, 
				int level__re_learning = 5,
				int level__add_to_unknow_msg = 10 );
		~MsgData();
		int ref_counter;
		int getSpecialMsg(int status, string &);
		int getCommonMsg(string &, string &, bool);
		int learning(string &, string &);
		bool getUnknowMsgToAsk(string &);
		bool getUserDefinedUsages(string &key,string &query, string &msg);
		int addUnknowMsgToAsk(string &);
		int addOldMsgToAskAgain(string);
		unsigned char getBotRunLevel();

		bool setLevel__ReLearning(int num)
		{
			if( num >= 0 )
			{
				LEVEL__RE_LEARNING = num;
				return true;
			}
			else return false;
		}

		bool setLevel__AddToUnknowMsg(int num)
		{
			if( num >= 0 )
			{
				LEVEL__ADD_TO_UNKNOW_MSG = num;
				return true;
			}
			else return false;
		}
		

};

#endif
