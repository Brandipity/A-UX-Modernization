/*
 * I've implemented a SIGURG wrapper program which ignores SIGURG and
 * then execs any arbitrary program (with its arguments) which is passed
 * in as arguments to the wrapper program. As a special case, if no
 * arguments are provided, the wrapper execs /etc/init. This avoids
 * having to modify the kernel to start a different program as the
 * precursor process; it does require additional configuration, however
 * -- see below.
 * 
 * I've provided the source code to this simple program below so you may
 * modify it to meet any local configuration requirements.
 * 
 * After compiling the program, which I am calling "uwrap", I
 * suggest the following configuration steps, which I tested on an A/UX
 * 3.1.1 system. You must perform these tasks as the super-user, root.
 * 
 * 1) Rename the original /etc/init; be sure to maintain the hard link
 *    to telinit. Make sure that the new name is the same as used by
 *    the 'uwrap' program.
 *     # mv /etc/init /etc/init.orig
 * 
 * 2) Install uwrap in /etc and make it the new /etc/init.
 *     # cc -O uwrap.c -o uwrap
 *     # cp ~/uwrap /etc/uwrap
 *     # chmod 500 /etc/uwrap
 *     # ln /etc/uwrap /etc/init
 * 
 * 3) Edit /etc/inittab to call uwrap to invoke cron, nfsd and bind
 *    as necessary for your site if desired. Since 'init' itself is
 *    ignoring SIGURG, all processes, unless they specifically call
 *    signal(SIGURG, SIG_DFL) _and_ are using SysV signals (ie: not
 *    linked with -lbsd or not linked with -lposix or not calling
 *    set42sig()), will be SIGURG protected; thus no additional
 *    wrapping is really needed. If you do desire to wrap them
 *    anyway, they should look like:
 * 
 *     cr:2:wait: /etc/uwrap /etc/cron </dev/syscon >/dev/syscon 2>&1
 *     nfs4:2:wait: /etc/uwrap /etc/biod 4
 * 
 * 4) Restart the reconfigured system.
 * 
 * The target programs should now be ignoring SIGURG.
 * 
 * Let me know if you have any questions or feedback.
 * 
 * Regards,
 * John Sovereign
 * Server OS Platforms
 * jms@apple.com
 * 
 * Version History:
 * 1.0: Original by John
 *
 * 1.1: Modifications by Jim Jagielski <jim@jaguNET.com>
 *       o if called as 'init', behave as 'init'. ie:
 *         'init s' will behave correctly and not try to
 *         wrap "s", which may not exist
 *       o Description "improved"
 *
 */

#define ORIGINIT		"/etc/init.orig"

#include <signal.h>

extern char **environ;

main(argc, argv)
char **argv;
{
    char *file;
    char *iam;

    (void) signal(SIGURG, SIG_IGN);

    /*
     * As a special case, if no arguments are passed, start init.
     * Init will only cooperate if the pid == 1.
     */
    if (argc == 1)
    {
	argv++;			/* skip over our name */
	argv[0] = "/etc/init";
	argv[1] = 0;
	file = ORIGINIT;
    }
    else
    {
	/*
	 * Hmmm we have arguments. Were we called as 'init' or
	 * as the wrapper? Emulate string functions here to
	 * reduce our size.
	 */
	iam = file = *argv;
	do
	{		/* emulate rindex()... Look for last '/' */
	    if (*file++ == '/')
		iam = file;
	}
	while (*file);
	if (iam[0] == 'i' &&
	    iam[1] == 'n' &&
	    iam[2] == 'i' &&
	    iam[3] == 't' &&
	    iam[4] == '\0')
	{		/* emulate strcmp(iam, "init") */
	    argv[0] = "/etc/init";
	    file = ORIGINIT;
	}
	else
	{
	    argv++;		/* skip over our name */
	    file = argv[0];
	}
    }
#ifdef JUST_TEST_IT
    printf("%s", file);
    do
    {
	printf(" %s", *argv);
    }
    while (*++argv);
    printf("\n");
#else
    (void) execve(file, argv, environ);
#endif
    perror(file);
}
