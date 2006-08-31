from string import *
from PCManX import *
from PCManXlib import *
import eliza

bot = eliza.eliza();

def OnNewIncomingMessage(handle, msg):
	rmsg = split(msg, " ", 2)[2]
	reply = bot.respond(rmsg)
	SendUnEscapedString(handle, "^R" + reply)
	SendReturn(handle)
	SendString(handle, "Y")
	SendReturn(handle)
