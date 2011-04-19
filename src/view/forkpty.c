/*

 forkpty.h - copied from rootsh project from sourceforge.net

rootsh - a logging shell wrapper for root wannabes

rootsh.c contains the main program and it's utility functions

Copyright (C) 2004 Gerhard Lausser

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 3
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include "config.h"
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

#ifndef HAVE_FORKPTY
/* 
//  Emulation of the BSD function forkpty.
*/
#ifndef MASTERPTYDEV
#  error you need to specify a master pty device
#endif
pid_t forkpty(int *amaster,  char  *name,  struct  termios *termp, struct winsize *winp) {
  /*
  //  amaster		A pointer to the master pty's file descriptor which
  //			will be set here.
  //  
  //  name		If name is NULL, the name of the slave pty will be
  //			returned in name.
  //  
  //  termp		If termp is not NULL, the ter minal parameters
  //			of the slave will be set to the values in termp.
  //  
  //  winsize		If winp is not NULL, the window size of the  slave
  //			will be set to the values in winp.
  //  
  //  currentterm	A structure filled with the characteristics of
  //			the current controlling terminal.
  //  
  //  currentwinsize	A structure filled with size characteristics of
  //			the current controlling terminal.
  //  
  //  pid		The process id of the forked child process.
  //  
  //  master		The file descriptor of the master pty.
  //  
  //  slave		The file descriptor of the slave pty.
  //  
  //  slavename		The file name of the slave pty.
  //  
  */
  struct termios currentterm;
  struct winsize currentwinsize;
  pid_t pid;
  int master, slave;
  char *slavename;

  /* 
  //  Get current settings if termp was not provided by the caller.
  */
  if (termp == NULL) {
    tcgetattr(STDIN_FILENO, &currentterm);
    termp = &currentterm;
  }

  /* 
  //  Same for window size.
  */
  if (winp == NULL) {
    ioctl(STDIN_FILENO, TIOCGWINSZ, (char *)&currentwinsize);
    winp->ws_row = currentwinsize.ws_row;
    winp->ws_col = currentwinsize.ws_col;
    winp->ws_xpixel = currentwinsize.ws_xpixel;
    winp->ws_ypixel = currentwinsize.ws_ypixel;
  }

  /*
  //  Get a master pseudo-tty.
  */
  if ((master = open(MASTERPTYDEV, O_RDWR)) < 0) {
    perror(MASTERPTYDEV);
    return(-1);
  }

  /*
  //  Set the permissions on the slave pty.
  */
  if (grantpt(master) < 0) {
    perror("grantpt");
    close(master);
    return(-1);
  }

  /*
  //  Unlock the slave pty.
  */
  if (unlockpt(master) < 0) {
    perror("unlockpt");
    close(master);
    return(-1);
  }

  /*
  //  Start a child process.
  */
  if ((pid = fork()) < 0) {
    perror("fork in forkpty");
    close(master);
    return(-1);
  }

  /*
  //  The child process will open the slave, which will become
  //  its controlling terminal.
  */
  if (pid == 0) {
    /*
    //  Get rid of our current controlling terminal.
    */
    setsid();

    /*
    //  Get the name of the slave pseudo tty.
    */
    if ((slavename = ptsname(master)) == NULL) {
      perror("ptsname");
      close(master);
      return(-1);
    }

    /* 
    //  Open the slave pseudo tty.
    */
    if ((slave = open(slavename, O_RDWR)) < 0) {
      perror(slavename);
      close(master);
      return(-1);
    }

    /*
    //  Copy the caller's terminal modes to the slave pty.
    */
    if (tcsetattr(slave, TCSANOW, termp) < 0) {
      perror("tcsetattr: slave pty");
      close(master);
      close(slave);
      return(-1);
    }

    /*
    //  Set the slave pty window size to the caller's size.
    */
    if (ioctl(slave, TIOCSWINSZ, winp) < 0) {
      perror("ioctl: slave winsz");
      close(master);
      close(slave);
      return(-1);
    }

    /*
    //  Close the logfile and the master pty.
    //  No need for these in the slave process.
    */
    close(master);
    /*
    //  Set the slave to be our standard input, output and error output.
    //  Then get rid of the original file descriptor.
    */
    dup2(slave, 0);
    dup2(slave, 1);
    dup2(slave, 2);
    close(slave);
    /*
    //  If the caller wants it, give him back the slave pty's name.
    */
    if (name != NULL) strcpy(name, slavename);
    return(0); 
  } else {
    /*
    //  Return the slave pty device name if caller wishes so.
    */
    if (name != NULL) {          
      if ((slavename = ptsname(master)) == NULL) {
        perror("ptsname");
        close(master);
        return(-1);
      }
      strcpy(name, slavename);
    }
    /*
    //  Return the file descriptor for communicating with the process
    //  to the caller.
    */
    *amaster = master; 
    return(pid);      
  }
}
#endif

