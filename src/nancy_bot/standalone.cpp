#include "nancy_bot/api.h"
#include <stdio.h>     /* standard I/O functions                         */
#include <unistd.h>    /* standard unix functions, like getpid()         */
#include <sys/types.h> /* various type definitions, like pid_t           */
#include <signal.h>    /* signal name macros, and the signal() prototype */

NancyBot *b;
/* first, here is the signal handler */
void catch_int(int sig_num)
{
	delete b;
	exit(0);
}

#ifdef CONSOLE_BOT
int
main (int argc, char **argv)
{
	signal(SIGINT, catch_int);
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
		if(strstr(input, "reload") != 0) {
			delete b;
			b = new NancyBot();
			continue;
		}
    }
	while ((cout << "Nancy: " << b->askNancy (msg_in) << endl));
    // ask_nancy return "PCManX-NancyBot" when initial error ( no config file found )
    delete b;
    return 0;
}

#endif // define CONSOLE_BOT
