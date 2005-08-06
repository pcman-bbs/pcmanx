#include "nancybot.h"
#ifdef CONSOLE_BOT
int
main (int argc, char **argv)
{
    NancyBot *b;
    if(argv[1] != NULL)
    {
    	b = new NancyBot(argv[1]);
    }
    else
	b = new NancyBot();
#ifdef ADV_BOT_DEBUG
    b->debug();
#endif
    char *input = new char[80];
    string msg_in;
    do
    {
	    cin.getline(input,80);
	    cin.clear();
	    msg_in = input;
    }
    while (strncmp (input, "exit", 4) != 0 &&
	   (cout << "Nancy: " << b->askNancy (msg_in) << endl));
    // ask_nancy return NULL when initial error ( no config file found )
    delete b;
    return 0;
}

#endif // define CONSOLE_BOT
