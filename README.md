PCManX
======

How to Build [![Build Status](https://travis-ci.org/pcman-bbs/pcmanx.svg?branch=master)](https://travis-ci.org/pcman-bbs/pcmanx)
------------
PCManX-gtk2 uses autotools (Automake / Autoconf / libtool) to construct
its building process, and provides various options for different profiles.

You can launch configure script with `--help` to dump the supported options, 
and here are the latest options:

```
  --enable-static       Enable static build.
  --disable-shared      Disabled dynamic build. [Default: Dynamic]
  --enable-debug        Enable debugging support or not [Default: Disabled]
                        It will cause PCManX to dump more detail informations.

  --prefix=DIR          Specify the prefix directory to DIR. Usually, we would
                        like to use /usr as the manner defined in LSB (Linux
                        Standard Base).
  --disable-docklet     Disable the docklet / system tray support.
                        [Default: Enabled]
  --disable-notifier    Disable the popup notification support for incoming
                        messages. [Default: Enabled]
  --disable-external    Disable the using of external SSH and Telnet program.
                        [Default: Enabled]
  --disable-nancy       Disable NancyBot. ( Disable auto-reply waterballs )
                        [Default: Enabled]

  --with-mozilla=DIR    Specify DIR as the Mozilla root directory for headers
                        and libraries.
  --enable-libnotify    Enable the use of popup notifier support by libnotify.
                        [Default: Disabled]
  --disable-mouse       Disable mouse click and wheel support
                        [Default: Enabled]
  --enable-iplookup     Enable IP location lookup. Useful to China locations.
                        You also need to download qqwry.dat from the web and
                        place it at ~/.pcmanx/qqwry.dat (case sensitive).
                        [Default: Disabled]
  --enable-proxy        Enable proxy server support
                        [Default: Disabled]
```

Runtime Options
---------------
Passing `--help` argument to the command line would dump several options as
following:

### Help Options:
```
  -?, --help                    Show help options
```

### Application Options:
```
  -m, --multiple-instance=N     Allow multiple instances
```

Keyboard Shortcut List
----------------------
### Connect Shortcuts
```
Site List               Alt+S

New Connection          Alt+Q
New Connection          Ctrl+Shift+T

Reconnection            ALT+R
Reconnection            Ctrl+Ins

Close                   Alt+W
Close                   Ctrl+Del

Next Page               Alt+X
Next Page               Alt+?
Next Page               Ctrl+Shift+PgDn

Previous Page           Alt+Z
Previous Page           Alt+?
Previous Page           Ctrl+Shift+PgUp

First Page              Ctrl+Home
Last Page               Ctrl+End
```

### Edit Shortcuts
```
Copy                    Alt+O
Copy                    Ctrl+Shift+C

Paste                   Alt+P
Paste                   Ctrl+Shift+V

Paste from Clipboard    Shift+Ins

Emotions                Ctrl+Enter
```

### View Shortcuts
```
Full Screen Mode        Alt+Enter
Show Main Window        Alt+M
```
