/*
 * Copyright (c) 2013-2020 The strace developers.
 * All rights reserved.
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#define BASE_NR 4000
#if defined LINUX_MIPSO32
/* For an O32 strace, decode the o32 syscalls.  */
# define SYS_syscall_subcall	4000
[BASE_NR +   0] = { MA,	TSD,		SEN(syscall),			"syscall"		}, /* start of Linux o32 */
[BASE_NR +   1] = { 1,	TP|SE,		SEN(exit),			"exit"			},
[BASE_NR +   2] = { 0,	TP,		SEN(fork),			"fork"			},
[BASE_NR +   3] = { 3,	TD,		SEN(read),			"read"			},
[BASE_NR +   4] = { 3,	TD,		SEN(write),			"write"			},
[BASE_NR +   5] = { 3,	TD|TF,		SEN(open),			"open"			},
[BASE_NR +   6] = { 1,	TD,		SEN(close),			"close"			},
[BASE_NR +   7] = { 3,	TP,		SEN(waitpid),			"waitpid"		},
[BASE_NR +   8] = { 2,	TD|TF,		SEN(creat),			"creat"			},
[BASE_NR +   9] = { 2,	TF,		SEN(link),			"link"			},
[BASE_NR +  10] = { 1,	TF,		SEN(unlink),			"unlink"		},
[BASE_NR +  11] = { 3,	TF|TP|TSD|SE|SI,	SEN(execve),			"execve"		},
[BASE_NR +  12] = { 1,	TF,		SEN(chdir),			"chdir"			},
[BASE_NR +  13] = { 1,	TCL,		SEN(time),			"time"			},
[BASE_NR +  14] = { 3,	TF,		SEN(mknod),			"mknod"			},
[BASE_NR +  15] = { 2,	TF,		SEN(chmod),			"chmod"			},
[BASE_NR +  16] = { 3,	TF,		SEN(chown),			"lchown"		},
[BASE_NR +  17] = { 0,	TM,		SEN(break),			"break"			},
[BASE_NR +  18] = { 2,	TF|TST|TSTA,	SEN(oldstat),			"oldstat"		},
[BASE_NR +  19] = { 3,	TD,		SEN(lseek),			"lseek"			},
[BASE_NR +  20] = { 0,	PU|NF,		SEN(getpid),			"getpid"		},
[BASE_NR +  21] = { 5,	TF,		SEN(mount),			"mount"			},
[BASE_NR +  22] = { 1,	TF,		SEN(umount),			"umount"		},
[BASE_NR +  23] = { 1,	TC,		SEN(setuid),			"setuid"		},
[BASE_NR +  24] = { 0,	TC|PU|NF,	SEN(getuid),			"getuid"		},
[BASE_NR +  25] = { 1,	0,		SEN(stime),			"stime"			},
[BASE_NR +  26] = { 4,	0,		SEN(ptrace),			"ptrace"		},
[BASE_NR +  27] = { 1,	0,		SEN(alarm),			"alarm"			},
[BASE_NR +  28] = { 2,	TD|TFST|TSTA,	SEN(oldfstat),			"oldfstat"		},
[BASE_NR +  29] = { 0,	TS,		SEN(pause),			"pause"			},
[BASE_NR +  30] = { 2,	TF,		SEN(utime),			"utime"			},
[BASE_NR +  31] = { 0,	0,		SEN(stty),			"stty"			},
[BASE_NR +  32] = { 0,	0,		SEN(gtty),			"gtty"			},
[BASE_NR +  33] = { 2,	TF,		SEN(access),			"access"		},
[BASE_NR +  34] = { 1,	0,		SEN(nice),			"nice"			},
[BASE_NR +  35] = { 1,	0,		SEN(ftime),			"ftime"			},
[BASE_NR +  36] = { 0,	0,		SEN(sync),			"sync"			},
[BASE_NR +  37] = { 2,	TS,		SEN(kill),			"kill"			},
[BASE_NR +  38] = { 2,	TF,		SEN(rename),			"rename"		},
[BASE_NR +  39] = { 2,	TF,		SEN(mkdir),			"mkdir"			},
[BASE_NR +  40] = { 1,	TF,		SEN(rmdir),			"rmdir"			},
[BASE_NR +  41] = { 1,	TD,		SEN(dup),			"dup"			},
[BASE_NR +  42] = { 0,	TD,		SEN(pipe),			"pipe"			},
[BASE_NR +  43] = { 1,	0,		SEN(times),			"times"			},
[BASE_NR +  44] = { 0,	0,		SEN(prof),			"prof"			},
[BASE_NR +  45] = { 1,	TM|SI,		SEN(brk),			"brk"			},
[BASE_NR +  46] = { 1,	TC,		SEN(setgid),			"setgid"		},
[BASE_NR +  47] = { 0,	TC|PU|NF,	SEN(getgid),			"getgid"		},
[BASE_NR +  48] = { 2,	TS,		SEN(signal),			"signal"		},
[BASE_NR +  49] = { 0,	TC|PU|NF,	SEN(geteuid),			"geteuid"		},
[BASE_NR +  50] = { 0,	TC|PU|NF,	SEN(getegid),			"getegid"		},
[BASE_NR +  51] = { 1,	TF,		SEN(acct),			"acct"			},
[BASE_NR +  52] = { 2,	TF,		SEN(umount2),			"umount2"		},
[BASE_NR +  53] = { 0,	0,		SEN(lock),			"lock"			},
[BASE_NR +  54] = { 3,	TD,		SEN(ioctl),			"ioctl"			},
[BASE_NR +  55] = { 3,	TD,		SEN(fcntl),			"fcntl"			},
[BASE_NR +  56] = { 0,	0,		SEN(mpx),			"mpx"			},
[BASE_NR +  57] = { 2,	0,		SEN(setpgid),			"setpgid"		},
[BASE_NR +  58] = { 0,	0,		SEN(ulimit),			"ulimit"		},
[BASE_NR +  59] = { 1,	0,		SEN(oldolduname),		"oldolduname"		},
[BASE_NR +  60] = { 1,	NF,		SEN(umask),			"umask"			},
[BASE_NR +  61] = { 1,	TF,		SEN(chroot),			"chroot"		},
[BASE_NR +  62] = { 2,	TSFA,		SEN(ustat),			"ustat"			},
[BASE_NR +  63] = { 2,	TD,		SEN(dup2),			"dup2"			},
[BASE_NR +  64] = { 0,	PU|NF,		SEN(getppid),			"getppid"		},
[BASE_NR +  65] = { 0,	PU|NF,		SEN(getpgrp),			"getpgrp"		},
[BASE_NR +  66] = { 0,	0,		SEN(setsid),			"setsid"		},
[BASE_NR +  67] = { 3,	TS,		SEN(sigaction),			"sigaction"		},
[BASE_NR +  68] = { 0,	TS,		SEN(sgetmask),			"sgetmask"		},
[BASE_NR +  69] = { 1,	TS,		SEN(ssetmask),			"ssetmask"		},
[BASE_NR +  70] = { 2,	TC,		SEN(setreuid),			"setreuid"		},
[BASE_NR +  71] = { 2,	TC,		SEN(setregid),			"setregid"		},
[BASE_NR +  72] = { 1,	TS,		SEN(sigsuspend),		"sigsuspend"		},
[BASE_NR +  73] = { 1,	TS,		SEN(sigpending),		"sigpending"		},
[BASE_NR +  74] = { 2,	0,		SEN(sethostname),		"sethostname"		},
[BASE_NR +  75] = { 2,	0,		SEN(setrlimit),			"setrlimit"		},
[BASE_NR +  76] = { 2,	0,		SEN(getrlimit),			"getrlimit"		},
[BASE_NR +  77] = { 2,	0,		SEN(getrusage),			"getrusage"		},
[BASE_NR +  78] = { 2,	TCL,		SEN(gettimeofday),		"gettimeofday"		},
[BASE_NR +  79] = { 2,	TCL,		SEN(settimeofday),		"settimeofday"		},
[BASE_NR +  80] = { 2,	TC,		SEN(getgroups),			"getgroups"		},
[BASE_NR +  81] = { 2,	TC,		SEN(setgroups),			"setgroups"		},
[BASE_NR +  82] = { 0,	0,		SEN(printargs),			"reserved82"		},
[BASE_NR +  83] = { 2,	TF,		SEN(symlink),			"symlink"		},
[BASE_NR +  84] = { 2,	TF|TLST|TSTA,	SEN(oldlstat),			"oldlstat"		},
[BASE_NR +  85] = { 3,	TF,		SEN(readlink),			"readlink"		},
[BASE_NR +  86] = { 1,	TF,		SEN(uselib),			"uselib"		},
[BASE_NR +  87] = { 2,	TF,		SEN(swapon),			"swapon"		},
[BASE_NR +  88] = { 4,	0,		SEN(reboot),			"reboot"		},
[BASE_NR +  89] = { 3,	TD,		SEN(readdir),			"readdir"		},
[BASE_NR +  90] = { 6,	TD|TM|SI,	SEN(mmap),			"mmap"			},
[BASE_NR +  91] = { 2,	TM|SI,		SEN(munmap),			"munmap"		},
[BASE_NR +  92] = { 2,	TF,		SEN(truncate),			"truncate"		},
[BASE_NR +  93] = { 2,	TD,		SEN(ftruncate),			"ftruncate"		},
[BASE_NR +  94] = { 2,	TD,		SEN(fchmod),			"fchmod"		},
[BASE_NR +  95] = { 3,	TD,		SEN(fchown),			"fchown"		},
[BASE_NR +  96] = { 2,	0,		SEN(getpriority),		"getpriority"		},
[BASE_NR +  97] = { 3,	0,		SEN(setpriority),		"setpriority"		},
[BASE_NR +  98] = { 0,	0,		SEN(profil),			"profil"		},
[BASE_NR +  99] = { 2,	TF|TSF|TSFA,	SEN(statfs),			"statfs"		},
[BASE_NR + 100] = { 2,	TD|TFSF|TSFA,	SEN(fstatfs),			"fstatfs"		},
[BASE_NR + 101] = { 3,	0,		SEN(ioperm),			"ioperm"		},
[BASE_NR + 102] = { 2,	TD|TSD,		SEN(socketcall),		"socketcall"		},
[BASE_NR + 103] = { 3,	0,		SEN(syslog),			"syslog"		},
[BASE_NR + 104] = { 3,	0,		SEN(setitimer),			"setitimer"		},
[BASE_NR + 105] = { 2,	0,		SEN(getitimer),			"getitimer"		},
[BASE_NR + 106] = { 2,	TF|TST|TSTA,	SEN(stat),			"stat"			},
[BASE_NR + 107] = { 2,	TF|TLST|TSTA,	SEN(lstat),			"lstat"			},
[BASE_NR + 108] = { 2,	TD|TFST|TSTA,	SEN(fstat),			"fstat"			},
[BASE_NR + 109] = { 1,	0,		SEN(olduname),			"olduname"		},
[BASE_NR + 110] = { 1,	0,		SEN(iopl),			"iopl"			},
[BASE_NR + 111] = { 0,	0,		SEN(vhangup),			"vhangup"		},
[BASE_NR + 112] = { 0,	0,		SEN(idle),			"idle"			},
[BASE_NR + 113] = { 5,	0,		SEN(vm86old),			"vm86"			},
[BASE_NR + 114] = { 4,	TP,		SEN(wait4),			"wait4"			},
[BASE_NR + 115] = { 1,	TF,		SEN(swapoff),			"swapoff"		},
[BASE_NR + 116] = { 1,	0,		SEN(sysinfo),			"sysinfo"		},
[BASE_NR + 117] = { 6,	TI|TSD,		SEN(ipc),			"ipc"			},
[BASE_NR + 118] = { 1,	TD,		SEN(fsync),			"fsync"			},
[BASE_NR + 119] = { 0,	TS,		SEN(sigreturn),			"sigreturn"		},
[BASE_NR + 120] = { 5,	TP,		SEN(clone),			"clone"			},
[BASE_NR + 121] = { 2,	0,		SEN(setdomainname),		"setdomainname"		},
[BASE_NR + 122] = { 1,	0,		SEN(uname),			"uname"			},
[BASE_NR + 123] = { 0,	0,		SEN(modify_ldt),		"modify_ldt"		},
[BASE_NR + 124] = { 1,	TCL,		SEN(adjtimex32),		"adjtimex"		},
[BASE_NR + 125] = { 3,	TM|SI,		SEN(mprotect),			"mprotect"		},
[BASE_NR + 126] = { 3,	TS,		SEN(sigprocmask),		"sigprocmask"		},
[BASE_NR + 127] = { 2,	0,		SEN(create_module),		"create_module"		},
[BASE_NR + 128] = { 3,	0,		SEN(init_module),		"init_module"		},
[BASE_NR + 129] = { 2,	0,		SEN(delete_module),		"delete_module"		},
[BASE_NR + 130] = { 1,	0,		SEN(get_kernel_syms),		"get_kernel_syms"	},
[BASE_NR + 131] = { 4,	TF,		SEN(quotactl),			"quotactl"		},
[BASE_NR + 132] = { 1,	0,		SEN(getpgid),			"getpgid"		},
[BASE_NR + 133] = { 1,	TD,		SEN(fchdir),			"fchdir"		},
[BASE_NR + 134] = { 2,	0,		SEN(bdflush),			"bdflush"		},
[BASE_NR + 135] = { 3,	0,		SEN(sysfs),			"sysfs"			},
[BASE_NR + 136] = { 1,	NF,		SEN(personality),		"personality"		},
[BASE_NR + 137] = { 0,	0,		SEN(afs_syscall),		"afs_syscall"		},
[BASE_NR + 138] = { 1,	TC|NF,		SEN(setfsuid),			"setfsuid"		},
[BASE_NR + 139] = { 1,	TC|NF,		SEN(setfsgid),			"setfsgid"		},
[BASE_NR + 140] = { 5,	TD,		SEN(llseek),			"_llseek"		},
[BASE_NR + 141] = { 3,	TD,		SEN(getdents),			"getdents"		},
[BASE_NR + 142] = { 5,	TD,		SEN(select),			"_newselect"		},
[BASE_NR + 143] = { 2,	TD,		SEN(flock),			"flock"			},
[BASE_NR + 144] = { 3,	TM,		SEN(msync),			"msync"			},
[BASE_NR + 145] = { 3,	TD,		SEN(readv),			"readv"			},
[BASE_NR + 146] = { 3,	TD,		SEN(writev),			"writev"		},
[BASE_NR + 147] = { 3,	0,		SEN(printargs),			"cacheflush"		},
[BASE_NR + 148] = { 3,	0,		SEN(printargs),			"cachectl"		},
[BASE_NR + 149] = { 4,	0,		SEN(sysmips),			"sysmips"		},
[BASE_NR + 150] = { 0,	0,		SEN(setup),			"setup"			},
[BASE_NR + 151] = { 1,	0,		SEN(getsid),			"getsid"		},
[BASE_NR + 152] = { 1,	TD,		SEN(fdatasync),			"fdatasync"		},
[BASE_NR + 153] = { 1,	0,		SEN(sysctl),			"_sysctl"		},
[BASE_NR + 154] = { 2,	TM,		SEN(mlock),			"mlock"			},
[BASE_NR + 155] = { 2,	TM,		SEN(munlock),			"munlock"		},
[BASE_NR + 156] = { 1,	TM,		SEN(mlockall),			"mlockall"		},
[BASE_NR + 157] = { 0,	TM,		SEN(munlockall),		"munlockall"		},
[BASE_NR + 158] = { 2,	0,		SEN(sched_setparam),		"sched_setparam"	},
[BASE_NR + 159] = { 2,	0,		SEN(sched_getparam),		"sched_getparam"	},
[BASE_NR + 160] = { 3,	0,		SEN(sched_setscheduler),	"sched_setscheduler"	},
[BASE_NR + 161] = { 1,	0,		SEN(sched_getscheduler),	"sched_getscheduler"	},
[BASE_NR + 162] = { 0,	0,		SEN(sched_yield),		"sched_yield"		},
[BASE_NR + 163] = { 1,	0,		SEN(sched_get_priority_max),	"sched_get_priority_max"},
[BASE_NR + 164] = { 1,	0,		SEN(sched_get_priority_min),	"sched_get_priority_min"},
[BASE_NR + 165] = { 2,	0,		SEN(sched_rr_get_interval_time32),"sched_rr_get_interval"},
[BASE_NR + 166] = { 2,	0,		SEN(nanosleep_time32),		"nanosleep"		},
[BASE_NR + 167] = { 5,	TM|SI,		SEN(mremap),			"mremap"		},
[BASE_NR + 168] = { 3,	TN,		SEN(accept),			"accept"		},
[BASE_NR + 169] = { 3,	TN,		SEN(bind),			"bind"			},
[BASE_NR + 170] = { 3,	TN,		SEN(connect),			"connect"		},
[BASE_NR + 171] = { 3,	TN,		SEN(getpeername),		"getpeername"		},
[BASE_NR + 172] = { 3,	TN,		SEN(getsockname),		"getsockname"		},
[BASE_NR + 173] = { 5,	TN,		SEN(getsockopt),		"getsockopt"		},
[BASE_NR + 174] = { 2,	TN,		SEN(listen),			"listen"		},
[BASE_NR + 175] = { 4,	TN,		SEN(recv),			"recv"			},
[BASE_NR + 176] = { 6,	TN,		SEN(recvfrom),			"recvfrom"		},
[BASE_NR + 177] = { 3,	TN,		SEN(recvmsg),			"recvmsg"		},
[BASE_NR + 178] = { 4,	TN,		SEN(send),			"send"			},
[BASE_NR + 179] = { 3,	TN,		SEN(sendmsg),			"sendmsg"		},
[BASE_NR + 180] = { 6,	TN,		SEN(sendto),			"sendto"		},
[BASE_NR + 181] = { 5,	TN,		SEN(setsockopt),		"setsockopt"		},
[BASE_NR + 182] = { 2,	TN,		SEN(shutdown),			"shutdown"		},
[BASE_NR + 183] = { 3,	TN,		SEN(socket),			"socket"		},
[BASE_NR + 184] = { 4,	TN,		SEN(socketpair),		"socketpair"		},
[BASE_NR + 185] = { 3,	TC,		SEN(setresuid),			"setresuid"		},
[BASE_NR + 186] = { 3,	TC,		SEN(getresuid),			"getresuid"		},
[BASE_NR + 187] = { 5,	0,		SEN(query_module),		"query_module"		},
[BASE_NR + 188] = { 3,	TD,		SEN(poll_time32),		"poll"			},
[BASE_NR + 189] = { 3,	0,		SEN(nfsservctl),		"nfsservctl"		},
[BASE_NR + 190] = { 3,	TC,		SEN(setresgid),			"setresgid"		},
[BASE_NR + 191] = { 3,	TC,		SEN(getresgid),			"getresgid"		},
[BASE_NR + 192] = { 5,	TC,		SEN(prctl),			"prctl"			},
[BASE_NR + 193] = { 0,	TS,		SEN(rt_sigreturn),		"rt_sigreturn"		},
[BASE_NR + 194] = { 4,	TS,		SEN(rt_sigaction),		"rt_sigaction"		},
[BASE_NR + 195] = { 4,	TS,		SEN(rt_sigprocmask),		"rt_sigprocmask"	},
[BASE_NR + 196] = { 2,	TS,		SEN(rt_sigpending),		"rt_sigpending"		},
[BASE_NR + 197] = { 4,	TS,		SEN(rt_sigtimedwait_time32),	"rt_sigtimedwait"	},
[BASE_NR + 198] = { 3,	TS,		SEN(rt_sigqueueinfo),		"rt_sigqueueinfo"	},
[BASE_NR + 199] = { 2,	TS,		SEN(rt_sigsuspend),		"rt_sigsuspend"		},
[BASE_NR + 200] = { 6,	TD,		SEN(pread),			"pread64"		},
[BASE_NR + 201] = { 6,	TD,		SEN(pwrite),			"pwrite64"		},
[BASE_NR + 202] = { 3,	TF,		SEN(chown),			"chown"			},
[BASE_NR + 203] = { 2,	TF,		SEN(getcwd),			"getcwd"		},
[BASE_NR + 204] = { 2,	TC,		SEN(capget),			"capget"		},
[BASE_NR + 205] = { 2,	TC,		SEN(capset),			"capset"		},
[BASE_NR + 206] = { 2,	TS,		SEN(sigaltstack),		"sigaltstack"		},
[BASE_NR + 207] = { 4,	TD|TN,		SEN(sendfile),			"sendfile"		},
[BASE_NR + 208] = { 5,	TN,		SEN(getpmsg),			"getpmsg"		},
[BASE_NR + 209] = { 5,	TN,		SEN(putpmsg),			"putpmsg"		},
[BASE_NR + 210] = { 6,	TD|TM|SI,	SEN(mmap_4koff),		"mmap2"			},
[BASE_NR + 211] = { 4,	TF,		SEN(truncate64),		"truncate64"		},
[BASE_NR + 212] = { 4,	TD,		SEN(ftruncate64),		"ftruncate64"		},
[BASE_NR + 213] = { 2,	TF|TST|TSTA,	SEN(stat64),			"stat64"		},
[BASE_NR + 214] = { 2,	TF|TLST|TSTA,	SEN(lstat64),			"lstat64"		},
[BASE_NR + 215] = { 2,	TD|TFST|TSTA,	SEN(fstat64),			"fstat64"		},
[BASE_NR + 216] = { 2,	TF,		SEN(pivotroot),			"pivot_root"		},
[BASE_NR + 217] = { 3,	TM,		SEN(mincore),			"mincore"		},
[BASE_NR + 218] = { 3,	TM,		SEN(madvise),			"madvise"		},
[BASE_NR + 219] = { 3,	TD,		SEN(getdents64),		"getdents64"		},
[BASE_NR + 220] = { 3,	TD,		SEN(fcntl64),			"fcntl64"		},
[BASE_NR + 221] = { 0,	0,		SEN(printargs),			"reserved221"		},
[BASE_NR + 222] = { 0,	PU|NF,		SEN(gettid),			"gettid"		},
[BASE_NR + 223] = { 5,	TD,		SEN(readahead),			"readahead"		},
[BASE_NR + 224] = { 5,	TF,		SEN(setxattr),			"setxattr"		},
[BASE_NR + 225] = { 5,	TF,		SEN(setxattr),			"lsetxattr"		},
[BASE_NR + 226] = { 5,	TD,		SEN(fsetxattr),			"fsetxattr"		},
[BASE_NR + 227] = { 4,	TF,		SEN(getxattr),			"getxattr"		},
[BASE_NR + 228] = { 4,	TF,		SEN(getxattr),			"lgetxattr"		},
[BASE_NR + 229] = { 4,	TD,		SEN(fgetxattr),			"fgetxattr"		},
[BASE_NR + 230] = { 3,	TF,		SEN(listxattr),			"listxattr"		},
[BASE_NR + 231] = { 3,	TF,		SEN(listxattr),			"llistxattr"		},
[BASE_NR + 232] = { 3,	TD,		SEN(flistxattr),		"flistxattr"		},
[BASE_NR + 233] = { 2,	TF,		SEN(removexattr),		"removexattr"		},
[BASE_NR + 234] = { 2,	TF,		SEN(removexattr),		"lremovexattr"		},
[BASE_NR + 235] = { 2,	TD,		SEN(fremovexattr),		"fremovexattr"		},
[BASE_NR + 236] = { 2,	TS,		SEN(kill),			"tkill"			},
[BASE_NR + 237] = { 4,	TD|TN,		SEN(sendfile64),		"sendfile64"		},
[BASE_NR + 238] = { 6,	0,		SEN(futex_time32),		"futex"			},
[BASE_NR + 239] = { 3,	0,		SEN(sched_setaffinity),		"sched_setaffinity"	},
[BASE_NR + 240] = { 3,	0,		SEN(sched_getaffinity),		"sched_getaffinity"	},
[BASE_NR + 241] = { 2,	TM,		SEN(io_setup),			"io_setup"		},
[BASE_NR + 242] = { 1,	TM,		SEN(io_destroy),		"io_destroy"		},
[BASE_NR + 243] = { 5,	0,		SEN(io_getevents_time32),	"io_getevents"		},
[BASE_NR + 244] = { 3,	0,		SEN(io_submit),			"io_submit"		},
[BASE_NR + 245] = { 3,	0,		SEN(io_cancel),			"io_cancel"		},
[BASE_NR + 246] = { 1,	TP|SE,		SEN(exit),			"exit_group"		},
[BASE_NR + 247] = { 4,	0,		SEN(lookup_dcookie),		"lookup_dcookie"	},
[BASE_NR + 248] = { 1,	TD,		SEN(epoll_create),		"epoll_create"		},
[BASE_NR + 249] = { 4,	TD,		SEN(epoll_ctl),			"epoll_ctl"		},
[BASE_NR + 250] = { 4,	TD,		SEN(epoll_wait),		"epoll_wait"		},
[BASE_NR + 251] = { 5,	TM|SI,		SEN(remap_file_pages),		"remap_file_pages"	},
[BASE_NR + 252] = { 1,	0,		SEN(set_tid_address),		"set_tid_address"	},
[BASE_NR + 253] = { 0,	0,		SEN(restart_syscall),		"restart_syscall"	},
[BASE_NR + 254] = { 7,	TD,		SEN(fadvise64_64),		"fadvise64"		},
[BASE_NR + 255] = { 3,	TF|TSF|TSFA,	SEN(statfs64),			"statfs64"		},
[BASE_NR + 256] = { 3,	TD|TFSF|TSFA,	SEN(fstatfs64),			"fstatfs64"		},
[BASE_NR + 257] = { 3,	0,		SEN(timer_create),		"timer_create"		},
[BASE_NR + 258] = { 4,	0,		SEN(timer_settime32),		"timer_settime"		},
[BASE_NR + 259] = { 2,	0,		SEN(timer_gettime32),		"timer_gettime"		},
[BASE_NR + 260] = { 1,	0,		SEN(timer_getoverrun),		"timer_getoverrun"	},
[BASE_NR + 261] = { 1,	0,		SEN(timer_delete),		"timer_delete"		},
[BASE_NR + 262] = { 2,	TCL,		SEN(clock_settime32),		"clock_settime"		},
[BASE_NR + 263] = { 2,	TCL,		SEN(clock_gettime32),		"clock_gettime"		},
[BASE_NR + 264] = { 2,	TCL,		SEN(clock_getres_time32),	"clock_getres"		},
[BASE_NR + 265] = { 4,	0,		SEN(clock_nanosleep_time32),	"clock_nanosleep"	},
[BASE_NR + 266] = { 3,	TS,		SEN(tgkill),			"tgkill"		},
[BASE_NR + 267] = { 2,	TF,		SEN(utimes),			"utimes"		},
[BASE_NR + 268] = { 6,	TM,		SEN(mbind),			"mbind"			},
[BASE_NR + 269] = { 5,	TM,		SEN(get_mempolicy),		"get_mempolicy"		},
[BASE_NR + 270] = { 3,	TM,		SEN(set_mempolicy),		"set_mempolicy"		},
[BASE_NR + 271] = { 4,	TD,		SEN(mq_open),			"mq_open"		},
[BASE_NR + 272] = { 1,	0,		SEN(mq_unlink),			"mq_unlink"		},
[BASE_NR + 273] = { 5,	TD,		SEN(mq_timedsend_time32),	"mq_timedsend"		},
[BASE_NR + 274] = { 5,	TD,		SEN(mq_timedreceive_time32),	"mq_timedreceive"	},
[BASE_NR + 275] = { 2,	TD,		SEN(mq_notify),			"mq_notify"		},
[BASE_NR + 276] = { 3,	TD,		SEN(mq_getsetattr),		"mq_getsetattr"		},
[BASE_NR + 277] = { 5,	0,		SEN(vserver),			"vserver"		},
[BASE_NR + 278] = { 5,	TP,		SEN(waitid),			"waitid"		},
[BASE_NR + 279] = { },
[BASE_NR + 280] = { 5,	0,		SEN(add_key),			"add_key"		},
[BASE_NR + 281] = { 4,	0,		SEN(request_key),		"request_key"		},
[BASE_NR + 282] = { 5,	0,		SEN(keyctl),			"keyctl"		},
[BASE_NR + 283] = { 1,	0,		SEN(set_thread_area),		"set_thread_area"	},
[BASE_NR + 284] = { 0,	TD,		SEN(inotify_init),		"inotify_init"		},
[BASE_NR + 285] = { 3,	TD|TF,		SEN(inotify_add_watch),		"inotify_add_watch"	},
[BASE_NR + 286] = { 2,	TD,		SEN(inotify_rm_watch),		"inotify_rm_watch"	},
[BASE_NR + 287] = { 4,	TM,		SEN(migrate_pages),		"migrate_pages"		},
[BASE_NR + 288] = { 4,	TD|TF,		SEN(openat),			"openat"		},
[BASE_NR + 289] = { 3,	TD|TF,		SEN(mkdirat),			"mkdirat"		},
[BASE_NR + 290] = { 4,	TD|TF,		SEN(mknodat),			"mknodat"		},
[BASE_NR + 291] = { 5,	TD|TF,		SEN(fchownat),			"fchownat"		},
[BASE_NR + 292] = { 3,	TD|TF,		SEN(futimesat),			"futimesat"		},
[BASE_NR + 293] = { 4,	TD|TF|TFST|TSTA,SEN(fstatat64),			"fstatat64"		},
[BASE_NR + 294] = { 3,	TD|TF,		SEN(unlinkat),			"unlinkat"		},
[BASE_NR + 295] = { 4,	TD|TF,		SEN(renameat),			"renameat"		},
[BASE_NR + 296] = { 5,	TD|TF,		SEN(linkat),			"linkat"		},
[BASE_NR + 297] = { 3,	TD|TF,		SEN(symlinkat),			"symlinkat"		},
[BASE_NR + 298] = { 4,	TD|TF,		SEN(readlinkat),		"readlinkat"		},
[BASE_NR + 299] = { 3,	TD|TF,		SEN(fchmodat),			"fchmodat"		},
[BASE_NR + 300] = { 3,	TD|TF,		SEN(faccessat),			"faccessat"		},
[BASE_NR + 301] = { 6,	TD,		SEN(pselect6_time32),		"pselect6"		},
[BASE_NR + 302] = { 5,	TD,		SEN(ppoll_time32),		"ppoll"			},
[BASE_NR + 303] = { 1,	TP,		SEN(unshare),			"unshare"		},
[BASE_NR + 304] = { 6,	TD,		SEN(splice),			"splice"		},
[BASE_NR + 305] = { 7,	TD,		SEN(sync_file_range),		"sync_file_range"	},
[BASE_NR + 306] = { 4,	TD,		SEN(tee),			"tee"			},
[BASE_NR + 307] = { 4,	TD,		SEN(vmsplice),			"vmsplice"		},
[BASE_NR + 308] = { 6,	TM,		SEN(move_pages),		"move_pages"		},
[BASE_NR + 309] = { 2,	0,		SEN(set_robust_list),		"set_robust_list"	},
[BASE_NR + 310] = { 3,	0,		SEN(get_robust_list),		"get_robust_list"	},
[BASE_NR + 311] = { 4,	0,		SEN(kexec_load),		"kexec_load"		},
[BASE_NR + 312] = { 3,	0,		SEN(getcpu),			"getcpu"		},
[BASE_NR + 313] = { 6,	TD,		SEN(epoll_pwait),		"epoll_pwait"		},
[BASE_NR + 314] = { 3,	0,		SEN(ioprio_set),		"ioprio_set"		},
[BASE_NR + 315] = { 2,	0,		SEN(ioprio_get),		"ioprio_get"		},
[BASE_NR + 316] = { 4,	TD|TF,		SEN(utimensat_time32),		"utimensat"		},
[BASE_NR + 317] = { 3,	TD|TS,		SEN(signalfd),			"signalfd"		},
[BASE_NR + 318] = { 4,	TD,		SEN(timerfd),			"timerfd"		},
[BASE_NR + 319] = { 1,	TD,		SEN(eventfd),			"eventfd"		},
[BASE_NR + 320] = { 6,	TD,		SEN(fallocate),			"fallocate"		},
[BASE_NR + 321] = { 2,	TD,		SEN(timerfd_create),		"timerfd_create"	},
[BASE_NR + 322] = { 2,	TD,		SEN(timerfd_gettime32),		"timerfd_gettime"	},
[BASE_NR + 323] = { 4,	TD,		SEN(timerfd_settime32),		"timerfd_settime"	},
[BASE_NR + 324] = { 4,	TD|TS,		SEN(signalfd4),			"signalfd4"		},
[BASE_NR + 325] = { 2,	TD,		SEN(eventfd2),			"eventfd2"		},
[BASE_NR + 326] = { 1,	TD,		SEN(epoll_create1),		"epoll_create1"		},
[BASE_NR + 327] = { 3,	TD,		SEN(dup3),			"dup3"			},
[BASE_NR + 328] = { 2,	TD,		SEN(pipe2),			"pipe2"			},
[BASE_NR + 329] = { 1,	TD,		SEN(inotify_init1),		"inotify_init1"		},
[BASE_NR + 330] = { 5,	TD,		SEN(preadv),			"preadv"		},
[BASE_NR + 331] = { 5,	TD,		SEN(pwritev),			"pwritev"		},
[BASE_NR + 332] = { 4,	TP|TS,		SEN(rt_tgsigqueueinfo),		"rt_tgsigqueueinfo"	},
[BASE_NR + 333] = { 5,	TD,		SEN(perf_event_open),		"perf_event_open"	},
[BASE_NR + 334] = { 4,	TN,		SEN(accept4),			"accept4"		},
[BASE_NR + 335] = { 5,	TN,		SEN(recvmmsg_time32),		"recvmmsg"		},
[BASE_NR + 336] = { 2,	TD,		SEN(fanotify_init),		"fanotify_init"		},
[BASE_NR + 337] = { 6,	TD|TF,		SEN(fanotify_mark),		"fanotify_mark"		},
[BASE_NR + 338] = { 4,	0,		SEN(prlimit64),			"prlimit64"		},
[BASE_NR + 339] = { 5,	TD|TF,		SEN(name_to_handle_at),		"name_to_handle_at"	},
[BASE_NR + 340] = { 3,	TD,		SEN(open_by_handle_at),		"open_by_handle_at"	},
[BASE_NR + 341] = { 2,	TCL,		SEN(clock_adjtime32),		"clock_adjtime"		},
[BASE_NR + 342] = { 1,	TD,		SEN(syncfs),			"syncfs"		},
[BASE_NR + 343] = { 4,	TN,		SEN(sendmmsg),			"sendmmsg"		},
[BASE_NR + 344] = { 2,	TD,		SEN(setns),			"setns"			},
[BASE_NR + 345] = { 6,	0,		SEN(process_vm_readv),		"process_vm_readv"	},
[BASE_NR + 346] = { 6,	0,		SEN(process_vm_writev),		"process_vm_writev"	},
[BASE_NR + 347] = { 5,	0,		SEN(kcmp),			"kcmp"			},
[BASE_NR + 348] = { 3,	TD,		SEN(finit_module),		"finit_module"		},
[BASE_NR + 349] = { 3,	0,		SEN(sched_setattr),		"sched_setattr"		},
[BASE_NR + 350] = { 4,	0,		SEN(sched_getattr),		"sched_getattr"		},
[BASE_NR + 351] = { 5,	TD|TF,		SEN(renameat2),			"renameat2"		},
[BASE_NR + 352] = { 3,	0,		SEN(seccomp),			"seccomp"		},
[BASE_NR + 353] = { 3,	0,		SEN(getrandom),			"getrandom"		},
[BASE_NR + 354] = { 2,	TD,		SEN(memfd_create),		"memfd_create"		},
[BASE_NR + 355] = { 3,	TD,		SEN(bpf),			"bpf"			},
[BASE_NR + 356] = { 5,	TD|TF|TP|TSD|SE|SI,	SEN(execveat),			"execveat"		},
[BASE_NR + 357] = { 1,	TD,		SEN(userfaultfd),		"userfaultfd"		},
[BASE_NR + 358] = { 2,	0,		SEN(membarrier),		"membarrier"		},
[BASE_NR + 359] = { 3,	TM,		SEN(mlock2),			"mlock2"		},
[BASE_NR + 360] = { 6,	TD,		SEN(copy_file_range),		"copy_file_range"	},
[BASE_NR + 361] = { 6,	TD,		SEN(preadv2),			"preadv2"		},
[BASE_NR + 362] = { 6,	TD,		SEN(pwritev2),			"pwritev2"		},
[BASE_NR + 363] = { 4,	TM|SI,		SEN(pkey_mprotect),		"pkey_mprotect"		},
[BASE_NR + 364] = { 2,	0,		SEN(pkey_alloc),		"pkey_alloc"		},
[BASE_NR + 365] = { 1,	0,		SEN(pkey_free),			"pkey_free"		},
[BASE_NR + 366] = { 5,	TD|TF|TSTA,	SEN(statx),			"statx"			},
[BASE_NR + 367] = { 4,	0,		SEN(rseq),			"rseq"			},
[BASE_NR + 368] = { 6,	0,		SEN(io_pgetevents_time32),	"io_pgetevents"		},
/* room for arch specific calls */
[BASE_NR + 393] = { 3,	TI,		SEN(semget),			"semget"		},
[BASE_NR + 394] = { 4,	TI,		SEN(semctl),			"semctl"		},
[BASE_NR + 395] = { 3,	TI,		SEN(shmget),			"shmget"		},
[BASE_NR + 396] = { 3,	TI,		SEN(shmctl),			"shmctl"		},
[BASE_NR + 397] = { 3,	TI|TM|SI,	SEN(shmat),			"shmat"			},
[BASE_NR + 398] = { 1,	TI|TM|SI,	SEN(shmdt),			"shmdt"			},
[BASE_NR + 399] = { 2,	TI,		SEN(msgget),			"msgget"		},
[BASE_NR + 400] = { 4,	TI,		SEN(msgsnd),			"msgsnd"		},
[BASE_NR + 401] = { 5,	TI,		SEN(msgrcv),			"msgrcv"		},
[BASE_NR + 402] = { 3,	TI,		SEN(msgctl),			"msgctl"		},
# include "syscallent-common-32.h"
# include "syscallent-common.h"

# define SYS_socket_subcall      4500
# include "subcall32.h"

#else

# define SYSCALL_NAME_PREFIX "o32:"
# include "syscallent-o32-stub.h"
# include "syscallent-common-32-stub.h"
# include "syscallent-common-stub.h"
# undef SYSCALL_NAME_PREFIX

#endif
#undef BASE_NR
