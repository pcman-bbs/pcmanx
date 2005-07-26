from PCManX import *
from PCManXlib import *

def OnNewIncomingMessage(handle, msg):
	SendString(handle, "XD")
	SendReturn(handle)
	SendString(handle, "Y")
	SendReturn(handle)
