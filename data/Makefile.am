if USE_SCRIPT
SCRIPT_FILES = \
	script/orz.py \
	script/eliza.py \
	script/PCManXlib.py
endif

if USE_NANCY
NANCY_FILES = \
	nancy_bot/default.conf \
	nancy_bot/default_msg.data \
	nancy_bot/example.conf \
	nancy_bot/example_msg.data
endif

pcmanx_DATA = \
	sitelist \
	emoticons

pcmanxdir = $(datadir)/pcmanx

pcmanx_script_DATA = $(SCRIPT_FILES)
pcmanx_scriptdir = $(datadir)/pcmanx/script

pcmanx_nancy_DATA = $(NANCY_FILES)
pcmanx_nancydir = $(datadir)/pcmanx/nancy_bot

EXTRA_DIST = \
	$(pcmanx_DATA) \
	$(pcmanx_script_DATA) \
	$(pcmanx_nancy_DATA) \
	pcmanx.png
