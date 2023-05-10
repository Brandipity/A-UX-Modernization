#if !defined(__sys_file_h)
#define __sys_file_h

#if !defined(_NO_IDENTS) && defined(_HEAD_IDENTS)
# pragma ident "@(#)head:sys/file.h	1.23 92/12/18 {Apple version 3.0 90/11/29 11:37:56}"
#endif

/*
 * Copyright 1987-91 Apple Computer, Inc.
 * All Rights Reserved.
 *
 * THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF APPLE COMPUTER, INC.
 * The copyright notice above does not evidence any actual or
 * intended publication of such source code.
 */

/* Copyright 1984-85 AT&T */
/* Copyright 1980-87 The Regents of the University of California */

#if !defined(__sys_types_h)
# if defined(__MPW_C__)
#   include "/:usr:include:sys:types.h"
# else
#   include <sys/types.h>
# endif
#endif 

/*
 * Descriptor table entry.
 * One for each kernel object.
 */
struct	file {
	struct	file	*f_freel;	/* free list */
	struct	file	*f_activef;	/* active list forward */
	struct	file	*f_activeb;	/* active list backwards */
	int	f_flag;		/* see below */
	short	f_type;		/* descriptor type */
	short	f_count;	/* reference count */
	short	f_asiocnt;	/* outstanding asio's on this file */
	short	f_msgcount;	/* references from message queue */
	struct	fileops {
		int	(*fo_rw)();
		int	(*fo_ioctl)();
		int	(*fo_select)();
		int	(*fo_close)();
		int	(*fo_stat)();
		int     (*fo_setxinfo)();
	} *f_ops;
	caddr_t	f_data;		/* ptr to file specific struct (vnode/socket) */
	off_t	f_offset;
	struct	ucred *f_cred;	/* credentials of user who opened file */
};

#ifdef KERNEL
  extern int GNOFILE;			/* size of global file table(s) */
  extern struct file *file_freel;	/* file table free list */
  extern struct file *file_activel;	/* file table active list */

  struct file *getf();
  struct file *falloc();
#endif /* KERNEL */

/*
 * flags- also for fcntl call.
 */
#define	FOPEN		(-1)
#define	FREAD		0x00000001	/* descriptor read/receive'able */
#define	FWRITE		0x00000002	/* descriptor write/send'able */
#define	FNDELAY		0x00000004	/* no delay */
#define	FAPPEND		0x00000008	/* append on each write */

#define	FSHLOCK		0x00000010	/* shared lock present */
#define	FEXLOCK		0x00000020	/* exclusive lock present */
#define	FASYNC		0x00000040	/* signal pgrp when data ready */
#define	FSYNC		0x00000080	/* sychronous writes */

/* open only modes */
#define	FCREAT		0x00000100	/* create if nonexistent */
#define	FTRUNC		0x00000200	/* truncate to zero length */
#define	FEXCL		0x00000400	/* error if already created */

#define	FKERNEL		0x00000800	/* kernel is doing open */
#define FMARK	        0x00001000	/* mark during gc */
#define FDEFER	        0x00002000	/* defer for next gc pass */
#define FNONBLOCK       0x00004000	/* no delay POSIX style */
#define	FNOCTTY         0x00008000	/* don't assign controlling tty */
#define	FGETCTTY        0x00010000	/* force controlling tty assignment */
#if SEC_BASE
#define FSTOPIO         0x00020000      /* no further I/O operations */
#endif
#define FASIOSLP	0x00040000	/* waiting for asio in closef() */
#define FNOHUP          0x20000000      /* don't hang up on close */
#define FLOCKOUT        0x40000000      /* exclusive use only */
#define FGLOBAL         0x80000000      /* force allocation from the global table */

/* bits to save after open */
#define FMASK		(FREAD | FWRITE | FNDELAY | FNONBLOCK | FAPPEND | FASYNC | FNOCTTY | FGETCTTY | FSYNC | FGLOBAL)

#if SEC_BASE
#define FCNTLCANT       (FREAD | FWRITE | FSHLOCK | FEXLOCK | FCREAT | FTRUNC | FEXCL | FKERNEL | FMARK | FDEFER | FNOCTTY | FGETCTTY | FSTOPIO)
#else
#define	FCNTLCANT	(FREAD | FWRITE | FSHLOCK | FEXLOCK | FCREAT | FTRUNC | FEXCL | FKERNEL | FMARK | FDEFER | FNOCTTY | FGETCTTY)
#endif

#ifndef	F_DUPFD
/* fcntl(2) requests--from <fcntl.h> */
#define	F_DUPFD		0	/* Duplicate fildes */
#define	F_GETFD		1	/* Get fildes flags */
#define	F_SETFD		2	/* Set fildes flags */
#define	F_GETFL		3	/* Get file flags */
#define	F_SETFL		4	/* Set file flags */
#define	F_GETLK		5	/* Get file lock */
#define	F_SETLK		6	/* Set file lock */
#define	F_SETLKW	7	/* Set file lock and wait */
#define	F_GETOWN	8	/* Get owner */
#define	F_SETOWN	9	/* Set owner */
#endif

/*
 * User definitions.
 */

/*
 * Open call.
 */

#ifndef __fcntl_h	/* most are defined in fcntl.h already */
#define	O_RDONLY	000		/* open for reading */
#define	O_WRONLY	001		/* open for writing */
#define	O_RDWR		002		/* open for read & write */
#define	O_NDELAY	FNDELAY		/* non-blocking open */
#define O_NONBLOCK	FNONBLOCK	/* non-blocking posix style */
#define	O_APPEND	FAPPEND		/* append on each write */
#define	O_CREAT		FCREAT		/* open with file create */
#define	O_TRUNC		FTRUNC		/* open with truncation */
#define	O_EXCL		FEXCL		/* error on create if file exists */
#define	O_NOCTTY	FNOCTTY		/* don't assign controlling tty */
#define	O_GETCTTY	FGETCTTY	/* force controlling tty assignment */
#define O_GLOBAL        FGLOBAL         /* force allocation from the global table */
#define O_SYNC		FSYNC        	/* synchronous writes */
#endif	/* !__fcntl_h */
#define O_NOHUP         FNOHUP   	/* don't hang up on close */
#define O_LOCKOUT	FLOCKOUT        /* exclusive use only */


/*
 * Flock call.
 */
#define	LOCK_SH		1	/* shared lock */
#define	LOCK_EX		2	/* exclusive lock */
#define	LOCK_NB		4	/* don't block when locking */
#define	LOCK_UN		8	/* unlock */

/*
 * Access call.
 */
#define	F_OK		0	/* does file exist */
#define	X_OK		1	/* is it executable by caller */
#define	W_OK		2	/* writable by caller */
#define	R_OK		4	/* readable by caller */

/*
 * Lseek call.
 */
#define	L_SET		0	/* absolute offset */
#define	L_INCR		1	/* relative to current offset */
#define	L_XTND		2	/* relative to end of file */

#ifdef KERNEL
# define DTYPE_VNODE	1	/* file */
# define DTYPE_SOCKET	2	/* communications endpoint */
#endif

#endif /* __sys_file_h */
