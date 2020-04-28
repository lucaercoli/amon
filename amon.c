/*
 *
 * Info:        amon.so
 * Contact:     mailto: <luca [at] lucaercoli.it> https://lucaercoli.it
 * Version:     1.1
 * Author:      Luca Ercoli
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *
 */

#define _GNU_SOURCE
#include <dlfcn.h>
#define _FCNTL_H
//#include <bits/fcntl.h>

#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define WITH_PERL_AND_PYTHON 1


/** Allowed commands **/

char *cmds[]={
	"/usr/sbin/sendmail","/usr/lib/sendmail","/etc/alternatives/lib.sendmail","/usr/lib/sm.bin/sendmail",
	"/usr/bin/mail",
	"/bin/mv",
	"/bin/rm",
	"/usr/bin/unlink",
	"/bin/rmdir",
	"/bin/cp",
	"/bin/date",
	"/bin/bzip2",
	"/bin/gunzip",
	"/bin/gzip",
	"/usr/bin/unzip",
	"/bin/tar",
	"/usr/bin/host",
	"/usr/bin/file",
	"/usr/bin/uptime",
	"/bin/grep",
	"/usr/bin/diff3",
	"/bin/pwd"
};


/** function prototype **/
int (*_execve)(const char *filename, char *const argv[],char *const envp[]); 



/** execve **/
int execve(const char *filename, char *const argv[],char *const envp[])
{

	char *script_name, *redirect_handle;
	register short cx;
	char *chi;

	_execve = (int (*)(const char *filename, char *const argv[],char *const envp[]))  dlsym(RTLD_NEXT, "execve");

#ifdef WITH_PERL_AND_PYTHON

       //deny perl and python execution from PHP interpreter
       
	if ( strcmp(filename,"/usr/bin/perl") == 0 || strcmp(filename,"/usr/bin/python") == 0 || strcmp(filename,"/usr/bin/python2.4") == 0 ) 
	{

		script_name = getenv("SCRIPT_NAME");
		redirect_handle = getenv("REDIRECT_HANDLER");

		if ( (strstr(script_name,".php") != NULL) || (strlen(redirect_handle)>2) )
		{
			return (666);
		}
		else { return _execve(filename,argv,envp); }
	}

#endif


	for (cx=0; cx<(sizeof(cmds)/sizeof(cmds[0])); cx++){
		if ( strcmp(filename,cmds[cx]) == 0 ) { return _execve(filename,argv,envp); }
	}

	return(666);
}
