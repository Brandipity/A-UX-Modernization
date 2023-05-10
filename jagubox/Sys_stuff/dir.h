#if !defined(__sys_dir_h)
#define __sys_dir_h

#if !defined(_NO_IDENTS) && defined(_HEAD_IDENTS)
# pragma ident "@(#)head:sys/dir.h	1.10 91/07/10 {Apple version 3.0 90/11/29 11:37:56}"
#endif

/*
 * Copyright 1987-91 Apple Computer, Inc.
 * All Rights Reserved.
 *
 * THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF APPLE COMPUTER, INC.
 * The copyright notice above does not evidence any actual or
 * intended publication of such source code.
 */

/* Copyright 1983-87 Sun Microsystems, Inc. */
/* Copyright 1980-87 The Regents of the University of California */

/*
 * [ANSI C] ANSI X3.159-1989 (3.5.4.3) Function Declarators
 * [POSIX]  IEEE Std 1003.1-1988 (5.1) Directories
 */

#if !defined(__sys_types_h)
# if defined(__MPW_C__)
#   include "/:usr:include:sys:types.h"
# else
#   include <sys/types.h>
# endif
#endif

#if defined(_BSD_SOURCE)
 /*
  * A directory consists of some number of blocks each of which is
  * less than or equal to the filesystem block size number of
  * bytes.
  *
  * Each block contains some number of directory entry structures,
  * which are of variable length.  Each directory entry has
  * a struct direct at the front of it, containing its file number,
  * the length of the entry, and the length of the name contained in
  * the entry.  These are followed by the name padded to a 4 byte boundary
  * with null bytes.  All names are guaranteed null terminated.
  * The maximum length of a name in a directory is MAXNAMLEN, plus
  * a null byte.
  *
  * The macro DIRSIZ(dp) gives the amount of space required to represent
  * a directory entry.  Free space in a directory is represented by
  * entries which have dp->d_reclen > DIRSIZ(dp).
  *
  * All the bytes in a directory block are claimed by the directory entries.
  * This usually results in the last entry in a directory having a large
  * dp->d_reclen.  Free entries have their dp->d_fileno set to 0.
  */

#define DIRBLKSIZ	512
#define MAXNAMLEN	255

struct	direct {
	u_long	d_fileno;		/* file number of entry */
	u_short	d_reclen;		/* length of this record */
	u_short	d_namlen;		/* length of string in d_name */
	char	d_name[MAXNAMLEN + 1];	/* name (up to MAXNAMLEN + 1) */
};

#if !defined(KERNEL)
/*
 * The DIRSIZ macro gives the minimum record length which will hold
 * the directory entry.  This requires the amount of space in 'struct
 * direct' without the d_name field, plus enough space for the name with
 * a terminating null byte (dp->d_namlen+1), rounded up to a 4 byte
 * boundary.
 */
#define DIRSIZ(dp) ((sizeof (struct direct) - \
      (MAXNAMLEN+1)) + (((dp)->d_namlen+1 + 3) &~ 3))

#if !defined(NULL)
#define NULL 0
#endif

#endif /* !KERNEL */
#endif /* _BSD_SOURCE */

#if defined(_SYSV_SOURCE) || defined(_POSIX_SOURCE)
/*
 * System-wide file name maximum (MAXNAMLEN isn't ANSC-compliant).
 * Individual file systems (directories?) may impose smaller limits,
 * e.g., System V uses 14 single-byte characters, but _SYS_NAME_MAX
 * is the largest value that can be handled by the directory routines.
 */
#define _SYS_NAME_MAX	255

struct  dirent {
	unsigned long	d_ino;			/* file number of entry */
	unsigned short	d_reclen;               /* length of this record */
	unsigned short	d_namlen;               /* length of string in d_name */
	char	d_name[_SYS_NAME_MAX + 1];	/* name with terminating NULL */
};
#endif /* _SYSV_SOURCE || _POSIX_SOURCE */

/*
 * Definitions for library routines operating on directories.
 */
typedef struct __dirdesc {
	int	dd_fd;
	long	dd_loc;
	long	dd_size;
	long	dd_bbase;
	long	dd_entno;
	long	dd_bsize;
	char	*dd_buf;
} DIR;

#if defined(__STDC__)
  long telldir(DIR *dirp);
  long seekdir(DIR *dirp, long loc);
  DIR *opendir(const char *dirname);
#else
  long telldir();
  void seekdir();
  DIR *opendir();
#endif

/*
 * Place conflicting definitions here.
 */
#if defined(__dirent_h)
/*
 * Came through the <dirent.h> entry point, assume POSIX/SVR3 interface.
 */
#if defined(_POSIX_SOURCE) || defined(_SYSV_SOURCE)

#if defined(__STDC__)
  struct dirent *readdir(DIR *dirp);
  int closedir(DIR *dirp);
#else
  struct dirent *readdir();
  int closedir();
#endif

#endif /* _POSIX_SOURCE || _SYSV_SOURCE */

#if defined(_POSIX_SOURCE)
#if defined(__STDC__)
  void rewinddir(DIR *dirp);
#else
  void rewinddir();
#endif
#endif /* _POSIX_SOURCE */

#else /* !__dirent_h */

/*
 * Else use the BSD interface (the original implementation).
 */
#if defined(_BSD_SOURCE)
#define d_ino d_fileno		/* compatablity */
#if !defined(KERNEL)

#if defined(__STDC__)
  void closedir(DIR *dirp);
  struct direct *readdir(DIR *dirp);
#else
  void closedir();
  struct direct *readdir();
#endif

#endif /* !KERNEL */
#endif /* _BSD_SOURCE */
#endif /* __dirent_h */

#if defined(_BSD_SOURCE) || defined (_SYSV_SOURCE)
#if !defined(KERNEL)
#define rewinddir(dirp) seekdir((dirp), (long)0)
#endif /* !KERNEL */
#endif

#endif /* __sys_dir_h */
