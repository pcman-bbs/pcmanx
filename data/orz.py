from string import *
from PCManX import *
from PCManXlib import *
import eliza

bot = eliza.eliza();

def OnNewIncomingMessage(handle, msg):
	rmsg = split(msg, " ", 2)[2]
	reply = bot.respond(rmsg)
	SendString(handle, "" + reply)
	SendReturn(handle)
	SendString(handle, "Y")
	SendReturn(handle)
