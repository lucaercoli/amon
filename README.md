# amon
Hijacking libc6 for fun and profit https://lucaercoli.it/

# What's amon?

amon.so is a library that integrates with the php interpreter and intercepts and manipulates the system calls provided by libc6. Useful to prevent that logical errors in the web-based applications (script php, perl, etc..), could allow an attacker to execute arbitrary code on the system.


# How to install it?

You can install amon compiling the source code or by downloading the library precompiled for your distribution and architecture.

Proof of concept code can be downloaded here

The build process is very easy, just run the command "gcc -fPIC -shared -ldl -o amon.so amon.c" and copy the library into /lib directory. 

You can load amon.so within apache in different ways, such of using the default environment variables files, mod_fcgid and also with the suEXEC feature of the Apache HTTP server. 

1) Apache's DSO module 

If PHP interpreter runs in the Apache process, then you can store this directive into the environment variables file: 

/etc/apache2/envvars (Debian GNU/Linux)
/etc/sysconfig/httpd (CentOS GNU/Linux) 

The instruction is "export LD_PRELOAD=amon.so" 

2) PHP with mod_fcgid 

If you're using mod_fcgid, then you need to export amon.so directly from the wrapper that calls the php (again with "export LD_PRELOAD = amon.so):

#!/bin/sh
export PHPRC=/etc/php5/cgi
export PHP_FCGI_MAX_REQUESTS=800
export PHP_FCGI_CHILDREN=2
export LD_PRELOAD = amon.so
exec /usr/lib/cgi-bin/php

3) Apache suEXEC support

If you have enabled the suEXEC feature you can enable amon creating a wrapper with the Action directive in the configuration of the site / server:

Action php-script wrapper-del-php

In the file "wrapper-of-php" you have to store it (and make it executable with "chmod 755 "):

#!/bin/sh
export LD_PRELOAD=amon.so
exec /path/del/vero/php5-cgi "$@"


Configuration

These are a pre-set of commands that the web user can run, such as:

"/usr/sbin/sendmail"
"/usr/lib/sendmail"
"/etc/alternatives/lib.sendmail"
"/usr/lib/sm.bin/sendmail"
"/usr/bin/mail"
..............
..............

These applications allow the user to send email from the web and perform operations that are not "dangerous" for the system. In order to add or delete commands, edit the variable "char * cmds []" in the source code.


Beware of perl and python interpreters

If you're using Perl or Python web applications, then you must uncomment the '#define WITH_PERL_AND_PYTHON' in the source code of the amon.so (line 15).
In this way, even allowing users to script in perl/python, its interpreters can be called directly from php.
So even if an attacker can execute Python/Perl scripts, he can't access to /bin/bash or other commands that will be used to take control of the machine.
If the system will block the command perl or python, you may be stored in /var/log/messages this error message:

sh[27910]: segfault at 0 ip 00007f248e0b6e34 sp 00007fff7fed2350 error 4 in libc-2.11.1.so[7f248e02b000+17a000]

There are no security problems it otherwise.


# Logging

When an user tries to execute a command that is not granted, into apache's error log will be saved this entry:

sh: command_name: command not found

When a user executes a command suid root (such as sendmail) in the error file of the web server will be saved this message:

ERROR: ld.so: object 'amon.so' from LD_PRELOAD cannot be preloaded: ignored.

For security reasons the dynamic linker will ignore requests to preload user specified libraries for setuid/setgid programs, so the error can be safely ignored.
