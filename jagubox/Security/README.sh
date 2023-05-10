This is a "fixed" version of /bin/sh, which adds extra security by resetting
IFS before a script is executed. This closes a few holes with SUID programs
that use 'system()' or 'open()' to execute commands.

To install:

    o Make a copy of your current Bourne shell:
	$ cp /bin/sh /bin/Osh
    
    o Now copy this version to '/bin/sh':
	$ cp sh /bin/sh

A very special thanks to John Coolidge, et. al, at Apple for providing this
special fix so quickly!!!

--
#include <std/disclaimer.h>
    Jim Jagielski               |
    jim@jagubox.gsfc.nasa.gov   |  "Dead or alive, you're coming with me."
    NASA/GSFC, Code 734.4       |                  RoboCop
    Greenbelt, MD 20771         |
