
// Name:        nancybot.cpp
// Purpose:     A chatting bot -- Nancy.
// Author:      Youchen Lee ( http://utcr.org ) 
// Licence:     GPL : http://www.gnu.org/licenses/gpl.html

// 
// TODO:
// 1. User defined vars
// 2. More and better auto-learn

#include "nancy_bot/api.h"

map<string, MsgData* > NancyBot::BOTS_LIST;


NancyBot::NancyBot(const char *bot_name , const char *config_path, char old_bot_run_level)
{
	BOT_STATUS = 0;
	BOT_RUN_LEVEL = old_bot_run_level;
	LEVEL__ASK_UNKNOW_MSG = 10; // default 10
	LEVEL__RE_LEARNING = 5; // default 5
	LEVEL__ADD_TO_UNKNOW_MSG = 100; // default 100, always add

	just_asked = false;
//	level__ask_unknow_msg_changed = false;
	level__re_learning_changed = false;
	level__add_to_unknow_msg_changed = false;
	ask_flag = "{ask}";
	fp_log = NULL;
	
	srand(time(NULL));
	memory_index = 0;
	filename_conf = "";
	CONFIG_PATH = config_path;
	if( *(CONFIG_PATH.end() -1) != '/' )
		CONFIG_PATH += '/';
		
	BOT_NAME = bot_name;
	BOT_INDEX = CONFIG_PATH + BOT_NAME;


	if( BOTS_LIST.find(BOT_INDEX) != BOTS_LIST.end() )
	{
		pMyMsgData = BOTS_LIST[BOT_INDEX];
		BOTS_LIST[BOT_INDEX]->ref_counter++;
	}
	else
	{
		BOTS_LIST[BOT_INDEX] = new MsgData(BOT_NAME, CONFIG_PATH, BOT_RUN_LEVEL, LEVEL__ADD_TO_UNKNOW_MSG);
		pMyMsgData = BOTS_LIST[BOT_INDEX];
		pMyMsgData->ref_counter = 1;
	}
	BOT_RUN_LEVEL = pMyMsgData->getBotRunLevel(); // get bot level ( after initialed,
						      // BOT_RUN_LEVLE may changed )
	
	//printf("%x\n",BOT_RUN_LEVEL);  // DEBUG
	string filename_log;
	std::ostringstream os_num;
	os_num << pMyMsgData->ref_counter;
		      
	filename_log = BOT_NAME + "_" + os_num.str();
		
	fp_log = fopen((CONFIG_PATH + filename_log + ".log").c_str() , "a+t");
	if(!fp_log){
		perror(filename_log.c_str());
		BOT_RUN_LEVEL ^= USE_LOG;
	}
	
}


int
NancyBot::checkMsgRepeat(string msg_input)
{
	int repeat_counter = 0;
	Array_MsgRemember[memory_index] = msg_input;
	if(++memory_index > 4)
		memory_index = 0;
	for(int i = 0; i < 5; i++)
	{
		if(Array_MsgRemember[i] == msg_input)
			repeat_counter++;
	}
	return repeat_counter;
}



bool NancyBot::writeLog(string &msg_in, string &msg_out)
{
	time_t current_time;
	time (&current_time);
	if (fprintf(fp_log, "Time: %sUser Say: %s\nBot Say: %s\n\n",
		ctime (&current_time), msg_in.c_str(), msg_out.c_str()) == -1)
	{
		fprintf (stderr, "Writing logs to \"chat.log\" error!\n");
		return false;
	}
	else
		return true;
		
}

NancyBot::~NancyBot()
{
	pMyMsgData->ref_counter--;
	if(pMyMsgData->ref_counter <= 0)
	{
		delete BOTS_LIST[BOT_INDEX]; // FIXME
		BOTS_LIST.erase(BOT_INDEX);
	}
	if(fp_log)
		fclose(fp_log);
}


int
NancyBot::replaceString(string &modify_me, string &find_me , string &replace_with_me)
{
	int search_here = 0;
	int num_replaced = 0;
	if( find_me == replace_with_me )
		return 0;
	while(1)
	{
        	search_here = modify_me.find( find_me, search_here );
		if( search_here != string::npos && search_here < modify_me.length() ) // found
		{
			modify_me.replace( search_here, find_me.length(), replace_with_me );
			num_replaced++;
			search_here++;
		}
		else
			break;
	}
	return num_replaced;
}

string
NancyBot::askNancy(string msg_input)
{
	BOT_STATUS = 0;
	int random;
	string unknow_msg;
	int len = msg_input.length();
	string msg_out = "PCManX-NancyBot";  // init msg_out

	if(level__add_to_unknow_msg_changed)
	{
		pMyMsgData->setLevel__AddToUnknowMsg(LEVEL__ADD_TO_UNKNOW_MSG);
		level__add_to_unknow_msg_changed = false;
	}
	if(level__re_learning_changed)
	{
		pMyMsgData->setLevel__ReLearning(LEVEL__RE_LEARNING);
		level__re_learning_changed = false;
	}
	
	if(just_asked) // AUTO_LEARN
	{
		pMyMsgData->learning(just_asked_unknow_msg, msg_input);
		just_asked = false;
	}
	
	if( BOT_RUN_LEVEL & USE_AUTO_LEARN)
	{
		if( (rand()%100 ) < LEVEL__ASK_UNKNOW_MSG )
			BOT_STATUS = 3; // Auto learn;
		if(BOT_STATUS == 3)
		{
			if(pMyMsgData->getSpecialMsg(BOT_STATUS, msg_out ) == 0){
				// should not here
			}
			else
			{
				// get a msg that bot unknow to ask
				
				if(pMyMsgData->getUnknowMsgToAsk(unknow_msg)) // got it
				{
					replaceString(msg_out, ask_flag, unknow_msg);
					just_asked = true;
					just_asked_unknow_msg = unknow_msg;
					
					if( BOT_RUN_LEVEL & USE_LOG )
					{
						writeLog(msg_input, msg_out);
					}
					return msg_out;	
				}
			}
		} // end (BOT_STATUS = 3)
	} // end (USE_AUTO_LEARN)
	
	

	if( BOT_RUN_LEVEL & USE_ANGRY )
	{
		if( checkMsgRepeat(msg_input) > 3)
		{
			BOT_STATUS = 1;   // anacy is angry
			if(pMyMsgData->getSpecialMsg(BOT_STATUS, msg_out ) == 0){
				//should not run here
			}
		}
	}
		
	if( (BOT_RUN_LEVEL & USE_BASE ) && BOT_STATUS != 1 ) // use_base and nancy not angry
	{
		if(pMyMsgData->getCommonMsg(msg_input, msg_out) == 0) // not found
		{
			if(BOT_RUN_LEVEL & USE_UNKNOW)
			{
				BOT_STATUS = 2; // BOT UNKNOW
			}
		}
		if(BOT_STATUS == 2) // UNKNOW MSG
		{
			pMyMsgData->getSpecialMsg(BOT_STATUS, msg_out );
			//if( BOT_RUN_LEVEL & USE_AUTO_LEARN)
			//	pMyMsgData->addUnknowMsgToAsk(msg_input);
		}
	}
	
	if( BOT_RUN_LEVEL & USE_LOG )
	{
		writeLog(msg_input, msg_out);
	}
	
	return msg_out;
}

#ifdef ADV_BOT_DEBUG
void NancyBot::debug()
{
	for(int i = 0; i < V_MsgAngry.size(); i++)
	{
		cout << i << ": " << V_MsgAngry[i] << endl;
	}
	for(int i = 0; i < V_MsgUnknow.size(); i++)
	{
		cout << i << ": " << V_MsgUnknow[i] << endl;
	}
	for(int i = 0; i < MT[2]["hi"].size(); i++)
	{
		cout << i << ": " << MT[2]["hi"][i] << endl;
	}
}
#endif
