%define ver 0.1.5
%define rel 1

Summary:   user-friendly telnet client designed for BBS browsing.
Name:      pcmanx-pure-gtk2
Version:   %{ver}
Release:   %{rel}
Copyright: GPL
Vendor:    PCMan X pure GTK+ 2 project
Group:     Applications/Internet
Source:    pcmanx-pure-gtk2-%{ver}.tar.gz
Url:       http://pcmanx.csie.net/
Packager:  Hong Jen Yee (PCMan) <hzysoft@sina.com.tw>

%description
An easy-to-use telnet client mainly targets BBS users.
PCMan X is a newly developed GPL'd version of PCMan, a full-featured famous BBS client formerly designed for MS Windows only.
It aimed to be an easy-to-use yet full-featured telnet client facilitating BBS browsing with the ability to process double-byte characters.

%prep

%setup
	./configure --prefix=/usr
%build
	make
%install
	make install-strip
%files
	/usr/bin/pcmanx
	/usr/share/pcmanx/sitelist
	/usr/share/pcmanx/emoticons
	/usr/share/pixmaps/pcmanx.png
	/usr/share/applications/pcmanx.desktop
	/usr/share/locale/zh_TW/LC_MESSAGES/pcmanx.mo
