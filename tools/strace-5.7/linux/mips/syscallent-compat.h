/*
 * Copyright (c) 2013-2020 The strace developers.
 * All rights reserved.
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

[   0] = { 0,	0,	SEN(printargs),		"svr4_syscall"	},
[   1] = { 0,	0,	SEN(printargs),		"svr4_exit"	},
[   2] = { 0,	0,	SEN(printargs),		"svr4_fork"	},
[   3] = { 0,	0,	SEN(printargs),		"svr4_read"	},
[   4] = { 0,	0,	SEN(printargs),		"svr4_write"	},
[   5] = { 0,	0,	SEN(printargs),		"svr4_open"	},
[   6] = { 0,	0,	SEN(printargs),		"svr4_close"	},
[   7] = { 0,	0,	SEN(printargs),		"svr4_wait"	},
[   8] = { 0,	0,	SEN(printargs),		"svr4_creat"	},
[   9] = { 0,	0,	SEN(printargs),		"svr4_link"	},
[  10] = { 0,	0,	SEN(printargs),		"svr4_unlink"	},
[  11] = { 0,	0,	SEN(printargs),		"svr4_exec"	},
[  12] = { 0,	0,	SEN(printargs),		"svr4_chdir"	},
[  13] = { 0,	0,	SEN(printargs),		"svr4_gtime"	},
[  14] = { 0,	0,	SEN(printargs),		"svr4_mknod"	},
[  15] = { 0,	0,	SEN(printargs),		"svr4_chmod"	},
[  16] = { 0,	0,	SEN(printargs),		"svr4_chown"	},
[  17] = { 0,	TM,	SEN(printargs),		"svr4_sbreak"	},
[  18] = { 0,	TF|TST|TSTA,SEN(printargs),		"svr4_stat"	},
[  19] = { 0,	0,	SEN(printargs),		"svr4_lseek"	},
[  20] = { 0,	0,	SEN(printargs),		"svr4_getpid"	},
[  21] = { 0,	0,	SEN(printargs),		"svr4_mount"	},
[  22] = { 0,	0,	SEN(printargs),		"svr4_umount"	},
[  23] = { 0,	TC,	SEN(printargs),		"svr4_setuid"	},
[  24] = { 0,	TC,	SEN(printargs),		"svr4_getuid"	},
[  25] = { 0,	0,	SEN(printargs),		"svr4_stime"	},
[  26] = { 0,	0,	SEN(printargs),		"svr4_ptrace"	},
[  27] = { 0,	0,	SEN(printargs),		"svr4_alarm"	},
[  28] = { 0,	TD|TFST|TSTA,SEN(printargs),		"svr4_fstat"	},
[  29] = { 0,	0,	SEN(printargs),		"svr4_pause"	},
[  30] = { 0,	0,	SEN(printargs),		"svr4_utime"	},
[  31] = { 0,	0,	SEN(printargs),		"svr4_stty"	},
[  32] = { 0,	0,	SEN(printargs),		"svr4_gtty"	},
[  33] = { 0,	0,	SEN(printargs),		"svr4_access"	},
[  34] = { 0,	0,	SEN(printargs),		"svr4_nice"	},
[  35] = { 0,	TF|TSF|TSFA,SEN(printargs),		"svr4_statfs"	},
[  36] = { 0,	0,	SEN(printargs),		"svr4_sync"	},
[  37] = { 0,	0,	SEN(printargs),		"svr4_kill"	},
[  38] = { 0,	TD|TFSF|TSFA,SEN(printargs),		"svr4_fstatfs"	},
[  39] = { 0,	0,	SEN(printargs),		"svr4_setpgrp"	},
[  40] = { 0,	0,	SEN(printargs),		"svr4_cxenix"	},
[  41] = { 0,	0,	SEN(printargs),		"svr4_dup"	},
[  42] = { 0,	0,	SEN(printargs),		"svr4_pipe"	},
[  43] = { 0,	0,	SEN(printargs),		"svr4_times"	},
[  44] = { 0,	0,	SEN(printargs),		"svr4_profil"	},
[  45] = { 0,	0,	SEN(printargs),		"svr4_plock"	},
[  46] = { 0,	TC,	SEN(printargs),		"svr4_setgid"	},
[  47] = { 0,	TC,	SEN(printargs),		"svr4_getgid"	},
[  48] = { 0,	0,	SEN(printargs),		"svr4_sig"	},
[  49] = { 0,	0,	SEN(printargs),		"svr4_msgsys"	},
[  50] = { 0,	0,	SEN(printargs),		"svr4_sysmips"	},
[  51] = { 0,	0,	SEN(printargs),		"svr4_sysacct"	},
[  52] = { 0,	0,	SEN(printargs),		"svr4_shmsys"	},
[  53] = { 0,	0,	SEN(printargs),		"svr4_semsys"	},
[  54] = { 0,	0,	SEN(printargs),		"svr4_ioctl"	},
[  55] = { 0,	0,	SEN(printargs),		"svr4_uadmin"	},
[  56] = { 0,	0,	SEN(printargs),		"svr4_exch"	},
[  57] = { 0,	0,	SEN(printargs),		"svr4_utssys"	},
[  58] = { 0,	0,	SEN(printargs),		"svr4_fsync"	},
[  59] = { 0,	0,	SEN(printargs),		"svr4_exece"	},
[  60] = { 0,	0,	SEN(printargs),		"svr4_umask"	},
[  61] = { 0,	0,	SEN(printargs),		"svr4_chroot"	},
[  62] = { 0,	0,	SEN(printargs),		"svr4_fcntl"	},
[  63] = { 0,	0,	SEN(printargs),		"svr4_ulimit"	},
[64 ... 69] = { },
[  70] = { 0,	0,	SEN(printargs),		"svr4_advfs"	},
[  71] = { 0,	0,	SEN(printargs),		"svr4_unadvfs"	},
[  72] = { },
[  73] = { },
[  74] = { 0,	0,	SEN(printargs),		"svr4_rfstart"	},
[  75] = { },
[  76] = { 0,	0,	SEN(printargs),		"svr4_rdebug"	},
[  77] = { 0,	0,	SEN(printargs),		"svr4_rfstop"	},
[  78] = { 0,	0,	SEN(printargs),		"svr4_rfsys"	},
[  79] = { 0,	0,	SEN(printargs),		"svr4_rmdir"	},
[  80] = { 0,	0,	SEN(printargs),		"svr4_mkdir"	},
[  81] = { 0,	0,	SEN(printargs),		"svr4_getdents"	},
[  82] = { 0,	0,	SEN(printargs),		"svr4_libattach"	},
[  83] = { 0,	0,	SEN(printargs),		"svr4_libdetach"	},
[  84] = { 0,	0,	SEN(printargs),		"svr4_sysfs"	},
[  85] = { 0,	0,	SEN(printargs),		"svr4_getmsg"	},
[  86] = { 0,	0,	SEN(printargs),		"svr4_putmsg"	},
[  87] = { 0,	0,	SEN(printargs),		"svr4_poll"	},
[  88] = { 0,	TF|TLST|TSTA,SEN(printargs),		"svr4_lstat"	},
[  89] = { 0,	0,	SEN(printargs),		"svr4_symlink"	},
[  90] = { 0,	0,	SEN(printargs),		"svr4_readlink"	},
[  91] = { 0,	TC,	SEN(printargs),		"svr4_setgroups"	},
[  92] = { 0,	TC,	SEN(printargs),		"svr4_getgroups"	},
[  93] = { 0,	0,	SEN(printargs),		"svr4_fchmod"	},
[  94] = { 0,	0,	SEN(printargs),		"svr4_fchown"	},
[  95] = { 0,	0,	SEN(printargs),		"svr4_sigprocmask"	},
[  96] = { 0,	0,	SEN(printargs),		"svr4_sigsuspend"	},
[  97] = { 0,	0,	SEN(printargs),		"svr4_sigaltstack"	},
[  98] = { 0,	0,	SEN(printargs),		"svr4_sigaction"	},
[  99] = { 0,	0,	SEN(printargs),		"svr4_sigpending"	},
[ 100] = { 0,	0,	SEN(printargs),		"svr4_setcontext"	},
[ 101] = { 0,	0,	SEN(printargs),		"svr4_evsys"	},
[ 102] = { 0,	0,	SEN(printargs),		"svr4_evtrapret"	},
[ 103] = { 0,	TF|TSF|TSFA,SEN(printargs),		"svr4_statvfs"	},
[ 104] = { 0,	TD|TFSF|TSFA,SEN(printargs),		"svr4_fstatvfs"	},
[ 105] = { },
[ 106] = { 0,	0,	SEN(printargs),		"svr4_nfssys"	},
[ 107] = { 0,	0,	SEN(printargs),		"svr4_waitid"	},
[ 108] = { 0,	0,	SEN(printargs),		"svr4_sigsendset"	},
[ 109] = { 0,	0,	SEN(printargs),		"svr4_hrtsys"	},
[ 110] = { 0,	0,	SEN(printargs),		"svr4_acancel"	},
[ 111] = { 0,	0,	SEN(printargs),		"svr4_async"	},
[ 112] = { 0,	0,	SEN(printargs),		"svr4_priocntlset"	},
[ 113] = { 0,	0,	SEN(printargs),		"svr4_pathconf"	},
[ 114] = { 0,	TM,	SEN(printargs),		"svr4_mincore"	},
[ 115] = { 0,	TD|TM|SI,	SEN(printargs),		"svr4_mmap"	},
[ 116] = { 0,	TM|SI,	SEN(printargs),		"svr4_mprotect"	},
[ 117] = { 0,	TM|SI,	SEN(printargs),		"svr4_munmap"	},
[ 118] = { 0,	0,	SEN(printargs),		"svr4_fpathconf"	},
[ 119] = { 0,	0,	SEN(printargs),		"svr4_vfork"	},
[ 120] = { 0,	0,	SEN(printargs),		"svr4_fchdir"	},
[ 121] = { 0,	0,	SEN(printargs),		"svr4_readv"	},
[ 122] = { 0,	0,	SEN(printargs),		"svr4_writev"	},
[ 123] = { 0,	TF|TST|TSTA,SEN(printargs),		"svr4_xstat"	},
[ 124] = { 0,	TF|TLST|TSTA,SEN(printargs),		"svr4_lxstat"	},
[ 125] = { 0,	TD|TFST|TSTA,SEN(printargs),		"svr4_fxstat"	},
[ 126] = { 0,	0,	SEN(printargs),		"svr4_xmknod"	},
[ 127] = { 0,	0,	SEN(printargs),		"svr4_clocal"	},
[ 128] = { 0,	0,	SEN(printargs),		"svr4_setrlimit"	},
[ 129] = { 0,	0,	SEN(printargs),		"svr4_getrlimit"	},
[ 130] = { 0,	0,	SEN(printargs),		"svr4_lchown"	},
[ 131] = { 0,	0,	SEN(printargs),		"svr4_memcntl"	},
[ 132] = { 0,	0,	SEN(printargs),		"svr4_getpmsg"	},
[ 133] = { 0,	0,	SEN(printargs),		"svr4_putpmsg"	},
[ 134] = { 0,	0,	SEN(printargs),		"svr4_rename"	},
[ 135] = { 0,	0,	SEN(printargs),		"svr4_nuname"	},
[ 136] = { 0,	TC,	SEN(printargs),		"svr4_setegid"	},
[ 137] = { 0,	0,	SEN(printargs),		"svr4_sysconf"	},
[ 138] = { 0,	TCL,	SEN(printargs),		"svr4_adjtime"	},
[ 139] = { 0,	0,	SEN(printargs),		"svr4_sysinfo"	},
[ 140] = { },
[ 141] = { 0,	TC,	SEN(printargs),		"svr4_seteuid"	},
[ 142] = { 0,	0,	SEN(printargs),		"svr4_PYRAMID_statis"	},
[ 143] = { 0,	0,	SEN(printargs),		"svr4_PYRAMID_tuning"	},
[ 144] = { 0,	0,	SEN(printargs),		"svr4_PYRAMID_forcerr"	},
[ 145] = { 0,	0,	SEN(printargs),		"svr4_PYRAMID_mpcntl"	},
[146 ... 200] = { },
[ 201] = { 0,	0,	SEN(printargs),		"svr4_aread"	},
[ 202] = { 0,	0,	SEN(printargs),		"svr4_awrite"	},
[ 203] = { 0,	0,	SEN(printargs),		"svr4_listio"	},
[ 204] = { 0,	0,	SEN(printargs),		"svr4_mips_acancel"	},
[ 205] = { 0,	0,	SEN(printargs),		"svr4_astatus"	},
[ 206] = { 0,	0,	SEN(printargs),		"svr4_await"	},
[ 207] = { 0,	0,	SEN(printargs),		"svr4_areadv"	},
[ 208] = { 0,	0,	SEN(printargs),		"svr4_awritev"	},

[1000] = { 0,	0,	SEN(printargs),		"sysv_syscall"	}, /* start of SYSV */
[1001] = { 0,	0,	SEN(printargs),		"sysv_exit"	},
[1002] = { 0,	0,	SEN(printargs),		"sysv_fork"	},
[1003] = { 0,	0,	SEN(printargs),		"sysv_read"	},
[1004] = { 0,	0,	SEN(printargs),		"sysv_write"	},
[1005] = { 0,	0,	SEN(printargs),		"sysv_open"	},
[1006] = { 0,	0,	SEN(printargs),		"sysv_close"	},
[1007] = { 0,	0,	SEN(printargs),		"sysv_wait"	},
[1008] = { 0,	0,	SEN(printargs),		"sysv_creat"	},
[1009] = { 0,	0,	SEN(printargs),		"sysv_link"	},
[1010] = { 0,	0,	SEN(printargs),		"sysv_unlink"	},
[1011] = { 0,	0,	SEN(printargs),		"sysv_execv"	},
[1012] = { 0,	0,	SEN(printargs),		"sysv_chdir"	},
[1013] = { 0,	0,	SEN(printargs),		"sysv_time"	},
[1014] = { 0,	0,	SEN(printargs),		"sysv_mknod"	},
[1015] = { 0,	0,	SEN(printargs),		"sysv_chmod"	},
[1016] = { 0,	0,	SEN(printargs),		"sysv_chown"	},
[1017] = { 0,	TM|SI,	SEN(printargs),		"sysv_brk"	},
[1018] = { 0,	TF|TST|TSTA,SEN(printargs),		"sysv_stat"	},
[1019] = { 0,	0,	SEN(printargs),		"sysv_lseek"	},
[1020] = { 0,	0,	SEN(printargs),		"sysv_getpid"	},
[1021] = { 0,	0,	SEN(printargs),		"sysv_mount"	},
[1022] = { 0,	0,	SEN(printargs),		"sysv_umount"	},
[1023] = { 0,	TC,	SEN(printargs),		"sysv_setuid"	},
[1024] = { 0,	TC,	SEN(printargs),		"sysv_getuid"	},
[1025] = { 0,	0,	SEN(printargs),		"sysv_stime"	},
[1026] = { 0,	0,	SEN(printargs),		"sysv_ptrace"	},
[1027] = { 0,	0,	SEN(printargs),		"sysv_alarm"	},
[1028] = { 0,	TD|TFST|TSTA,SEN(printargs),		"sysv_fstat"	},
[1029] = { 0,	0,	SEN(printargs),		"sysv_pause"	},
[1030] = { 0,	0,	SEN(printargs),		"sysv_utime"	},
[1031] = { 0,	0,	SEN(printargs),		"sysv_stty"	},
[1032] = { 0,	0,	SEN(printargs),		"sysv_gtty"	},
[1033] = { 0,	0,	SEN(printargs),		"sysv_access"	},
[1034] = { 0,	0,	SEN(printargs),		"sysv_nice"	},
[1035] = { 0,	TF|TSF|TSFA,SEN(printargs),		"sysv_statfs"	},
[1036] = { 0,	0,	SEN(printargs),		"sysv_sync"	},
[1037] = { 0,	0,	SEN(printargs),		"sysv_kill"	},
[1038] = { 0,	TD|TFSF|TSFA,SEN(printargs),		"sysv_fstatfs"	},
[1039] = { 0,	0,	SEN(printargs),		"sysv_setpgrp"	},
[1040] = { 0,	0,	SEN(printargs),		"sysv_syssgi"	},
[1041] = { 0,	0,	SEN(printargs),		"sysv_dup"	},
[1042] = { 0,	0,	SEN(printargs),		"sysv_pipe"	},
[1043] = { 0,	0,	SEN(printargs),		"sysv_times"	},
[1044] = { 0,	0,	SEN(printargs),		"sysv_profil"	},
[1045] = { 0,	0,	SEN(printargs),		"sysv_plock"	},
[1046] = { 0,	TC,	SEN(printargs),		"sysv_setgid"	},
[1047] = { 0,	TC,	SEN(printargs),		"sysv_getgid"	},
[1048] = { 0,	0,	SEN(printargs),		"sysv_sig"	},
[1049] = { 0,	0,	SEN(printargs),		"sysv_msgsys"	},
[1050] = { 0,	0,	SEN(printargs),		"sysv_sysmips"	},
[1051] = { 0,	0,	SEN(printargs),		"sysv_acct"	},
[1052] = { 0,	0,	SEN(printargs),		"sysv_shmsys"	},
[1053] = { 0,	0,	SEN(printargs),		"sysv_semsys"	},
[1054] = { 0,	0,	SEN(printargs),		"sysv_ioctl"	},
[1055] = { 0,	0,	SEN(printargs),		"sysv_uadmin"	},
[1056] = { 0,	0,	SEN(printargs),		"sysv_sysmp"	},
[1057] = { 0,	0,	SEN(printargs),		"sysv_utssys"	},
[1058] = { },
[1059] = { 0,	0,	SEN(printargs),		"sysv_execve"	},
[1060] = { 0,	0,	SEN(printargs),		"sysv_umask"	},
[1061] = { 0,	0,	SEN(printargs),		"sysv_chroot"	},
[1062] = { 0,	0,	SEN(printargs),		"sysv_fcntl"	},
[1063] = { 0,	0,	SEN(printargs),		"sysv_ulimit"	},
[1064 ... 1069] = { },
[1070] = { 0,	0,	SEN(printargs),		"sysv_advfs"	},
[1071] = { 0,	0,	SEN(printargs),		"sysv_unadvfs"	},
[1072] = { 0,	0,	SEN(printargs),		"sysv_rmount"	},
[1073] = { 0,	0,	SEN(printargs),		"sysv_rumount"	},
[1074] = { 0,	0,	SEN(printargs),		"sysv_rfstart"	},
[1075] = { 0,	0,	SEN(printargs),		"sysv_getrlimit64"	},
[1076] = { 0,	0,	SEN(printargs),		"sysv_setrlimit64"	},
[1077] = { 0,	0,	SEN(printargs),		"sysv_nanosleep"	},
[1078] = { 0,	0,	SEN(printargs),		"sysv_lseek64"	},
[1079] = { 0,	0,	SEN(printargs),		"sysv_rmdir"	},
[1080] = { 0,	0,	SEN(printargs),		"sysv_mkdir"	},
[1081] = { 0,	0,	SEN(printargs),		"sysv_getdents"	},
[1082] = { 0,	0,	SEN(printargs),		"sysv_sginap"	},
[1083] = { 0,	0,	SEN(printargs),		"sysv_sgikopt"	},
[1084] = { 0,	0,	SEN(printargs),		"sysv_sysfs"	},
[1085] = { 0,	0,	SEN(printargs),		"sysv_getmsg"	},
[1086] = { 0,	0,	SEN(printargs),		"sysv_putmsg"	},
[1087] = { 0,	0,	SEN(printargs),		"sysv_poll"	},
[1088] = { 0,	0,	SEN(printargs),		"sysv_sigreturn"	},
[1089] = { 0,	0,	SEN(printargs),		"sysv_accept"	},
[1090] = { 0,	0,	SEN(printargs),		"sysv_bind"	},
[1091] = { 0,	0,	SEN(printargs),		"sysv_connect"	},
[1092] = { 0,	0,	SEN(printargs),		"sysv_gethostid"	},
[1093] = { 0,	0,	SEN(printargs),		"sysv_getpeername"	},
[1094] = { 0,	0,	SEN(printargs),		"sysv_getsockname"	},
[1095] = { 0,	0,	SEN(printargs),		"sysv_getsockopt"	},
[1096] = { 0,	0,	SEN(printargs),		"sysv_listen"	},
[1097] = { 0,	0,	SEN(printargs),		"sysv_recv"	},
[1098] = { 0,	0,	SEN(printargs),		"sysv_recvfrom"	},
[1099] = { 0,	0,	SEN(printargs),		"sysv_recvmsg"	},
[1100] = { 0,	0,	SEN(printargs),		"sysv_select"	},
[1101] = { 0,	0,	SEN(printargs),		"sysv_send"	},
[1102] = { 0,	0,	SEN(printargs),		"sysv_sendmsg"	},
[1103] = { 0,	0,	SEN(printargs),		"sysv_sendto"	},
[1104] = { 0,	0,	SEN(printargs),		"sysv_sethostid"	},
[1105] = { 0,	0,	SEN(printargs),		"sysv_setsockopt"	},
[1106] = { 0,	0,	SEN(printargs),		"sysv_shutdown"	},
[1107] = { 0,	0,	SEN(printargs),		"sysv_socket"	},
[1108] = { 0,	0,	SEN(printargs),		"sysv_gethostname"	},
[1109] = { 0,	0,	SEN(printargs),		"sysv_sethostname"	},
[1110] = { 0,	0,	SEN(printargs),		"sysv_getdomainname"	},
[1111] = { 0,	0,	SEN(printargs),		"sysv_setdomainname"	},
[1112] = { 0,	0,	SEN(printargs),		"sysv_truncate"	},
[1113] = { 0,	0,	SEN(printargs),		"sysv_ftruncate"	},
[1114] = { 0,	0,	SEN(printargs),		"sysv_rename"	},
[1115] = { 0,	0,	SEN(printargs),		"sysv_symlink"	},
[1116] = { 0,	0,	SEN(printargs),		"sysv_readlink"	},
[1117] = { 0,	TF|TLST|TSTA,SEN(printargs),		"sysv_lstat"	},
[1118] = { 0,	0,	SEN(printargs),		"sysv_nfsmount"	},
[1119] = { 0,	0,	SEN(printargs),		"sysv_nfssvc"	},
[1120] = { 0,	0,	SEN(printargs),		"sysv_getfh"	},
[1121] = { 0,	0,	SEN(printargs),		"sysv_async_daemon"	},
[1122] = { 0,	0,	SEN(printargs),		"sysv_exportfs"	},
[1123] = { 0,	TC,	SEN(printargs),		"sysv_setregid"	},
[1124] = { 0,	TC,	SEN(printargs),		"sysv_setreuid"	},
[1125] = { 0,	0,	SEN(printargs),		"sysv_getitimer"	},
[1126] = { 0,	0,	SEN(printargs),		"sysv_setitimer"	},
[1127] = { 0,	TCL,	SEN(printargs),		"sysv_adjtime"	},
[1128] = { 0,	0,	SEN(printargs),		"sysv_BSD_getime"	},
[1129] = { 0,	0,	SEN(printargs),		"sysv_sproc"	},
[1130] = { 0,	0,	SEN(printargs),		"sysv_prctl"	},
[1131] = { 0,	0,	SEN(printargs),		"sysv_procblk"	},
[1132] = { 0,	0,	SEN(printargs),		"sysv_sprocsp"	},
[1133] = { 0,	0,	SEN(printargs),		"sysv_sgigsc"	},
[1134] = { 0,	TD|TM|SI,	SEN(printargs),		"sysv_mmap"	},
[1135] = { 0,	TM|SI,	SEN(printargs),		"sysv_munmap"	},
[1136] = { 0,	TM|SI,	SEN(printargs),		"sysv_mprotect"	},
[1137] = { 0,	TM,	SEN(printargs),		"sysv_msync"	},
[1138] = { 0,	TM,	SEN(printargs),		"sysv_madvise"	},
[1139] = { 0,	0,	SEN(printargs),		"sysv_pagelock"	},
[1140] = { 0,	0,	SEN(printargs),		"sysv_getpagesize"	},
[1141] = { 0,	TF,	SEN(printargs),		"sysv_quotactl"	},
[1142] = { 0,	0,	SEN(printargs),		"sysv_libdetach"	},
[1143] = { 0,	0,	SEN(printargs),		"sysv_BSDgetpgrp"	},
[1144] = { 0,	0,	SEN(printargs),		"sysv_BSDsetpgrp"	},
[1145] = { 0,	0,	SEN(printargs),		"sysv_vhangup"	},
[1146] = { 0,	0,	SEN(printargs),		"sysv_fsync"	},
[1147] = { 0,	0,	SEN(printargs),		"sysv_fchdir"	},
[1148] = { 0,	0,	SEN(printargs),		"sysv_getrlimit"	},
[1149] = { 0,	0,	SEN(printargs),		"sysv_setrlimit"	},
[1150] = { 0,	0,	SEN(printargs),		"sysv_cacheflush"	},
[1151] = { 0,	0,	SEN(printargs),		"sysv_cachectl"	},
[1152] = { 0,	0,	SEN(printargs),		"sysv_fchown"	},
[1153] = { 0,	0,	SEN(printargs),		"sysv_fchmod"	},
[1154] = { 0,	0,	SEN(printargs),		"sysv_wait3"	},
[1155] = { 0,	0,	SEN(printargs),		"sysv_socketpair"	},
[1156] = { 0,	0,	SEN(printargs),		"sysv_sysinfo"	},
[1157] = { 0,	0,	SEN(printargs),		"sysv_nuname"	},
[1158] = { 0,	TF|TST|TSTA,SEN(printargs),		"sysv_xstat"	},
[1159] = { 0,	TF|TLST|TSTA,SEN(printargs),		"sysv_lxstat"	},
[1160] = { 0,	TD|TFST|TSTA,SEN(printargs),		"sysv_fxstat"	},
[1161] = { 0,	0,	SEN(printargs),		"sysv_xmknod"	},
[1162] = { 0,	0,	SEN(printargs),		"sysv_ksigaction"	},
[1163] = { 0,	0,	SEN(printargs),		"sysv_sigpending"	},
[1164] = { 0,	0,	SEN(printargs),		"sysv_sigprocmask"	},
[1165] = { 0,	0,	SEN(printargs),		"sysv_sigsuspend"	},
[1166] = { 0,	0,	SEN(printargs),		"sysv_sigpoll"	},
[1167] = { 0,	0,	SEN(printargs),		"sysv_swapctl"	},
[1168] = { 0,	0,	SEN(printargs),		"sysv_getcontext"	},
[1169] = { 0,	0,	SEN(printargs),		"sysv_setcontext"	},
[1170] = { 0,	0,	SEN(printargs),		"sysv_waitsys"	},
[1171] = { 0,	0,	SEN(printargs),		"sysv_sigstack"	},
[1172] = { 0,	0,	SEN(printargs),		"sysv_sigaltstack"	},
[1173] = { 0,	0,	SEN(printargs),		"sysv_sigsendset"	},
[1174] = { 0,	TF|TSF|TSFA,SEN(printargs),		"sysv_statvfs"	},
[1175] = { 0,	TD|TFSF|TSFA,SEN(printargs),		"sysv_fstatvfs"	},
[1176] = { 0,	0,	SEN(printargs),		"sysv_getpmsg"	},
[1177] = { 0,	0,	SEN(printargs),		"sysv_putpmsg"	},
[1178] = { 0,	0,	SEN(printargs),		"sysv_lchown"	},
[1179] = { 0,	0,	SEN(printargs),		"sysv_priocntl"	},
[1180] = { 0,	0,	SEN(printargs),		"sysv_ksigqueue"	},
[1181] = { 0,	0,	SEN(printargs),		"sysv_readv"	},
[1182] = { 0,	0,	SEN(printargs),		"sysv_writev"	},
[1183] = { 0,	0,	SEN(printargs),		"sysv_truncate64"	},
[1184] = { 0,	0,	SEN(printargs),		"sysv_ftruncate64"	},
[1185] = { 0,	TD|TM|SI,	SEN(printargs),		"sysv_mmap64"	},
[1186] = { 0,	0,	SEN(printargs),		"sysv_dmi"	},
[1187] = { 0,	0,	SEN(printargs),		"sysv_pread"	},
[1188] = { 0,	0,	SEN(printargs),		"sysv_pwrite"	},

[2000] = { 0,	0,	SEN(printargs),		"bsd43_syscall"	}, /* start of BSD 4.3 */
[2001] = { 0,	0,	SEN(printargs),		"bsd43_exit"	},
[2002] = { 0,	0,	SEN(printargs),		"bsd43_fork"	},
[2003] = { 0,	0,	SEN(printargs),		"bsd43_read"	},
[2004] = { 0,	0,	SEN(printargs),		"bsd43_write"	},
[2005] = { 0,	0,	SEN(printargs),		"bsd43_open"	},
[2006] = { 0,	0,	SEN(printargs),		"bsd43_close"	},
[2007] = { 0,	0,	SEN(printargs),		"bsd43_wait"	},
[2008] = { 0,	0,	SEN(printargs),		"bsd43_creat"	},
[2009] = { 0,	0,	SEN(printargs),		"bsd43_link"	},
[2010] = { 0,	0,	SEN(printargs),		"bsd43_unlink"	},
[2011] = { 0,	0,	SEN(printargs),		"bsd43_exec"	},
[2012] = { 0,	0,	SEN(printargs),		"bsd43_chdir"	},
[2013] = { 0,	0,	SEN(printargs),		"bsd43_time"	},
[2014] = { 0,	0,	SEN(printargs),		"bsd43_mknod"	},
[2015] = { 0,	0,	SEN(printargs),		"bsd43_chmod"	},
[2016] = { 0,	0,	SEN(printargs),		"bsd43_chown"	},
[2017] = { 0,	TM,	SEN(printargs),		"bsd43_sbreak"	},
[2018] = { 0,	TF|TST|TSTA,SEN(printargs),		"bsd43_oldstat"	},
[2019] = { 0,	0,	SEN(printargs),		"bsd43_lseek"	},
[2020] = { 0,	0,	SEN(printargs),		"bsd43_getpid"	},
[2021] = { 0,	0,	SEN(printargs),		"bsd43_oldmount"	},
[2022] = { 0,	0,	SEN(printargs),		"bsd43_umount"	},
[2023] = { 0,	TC,	SEN(printargs),		"bsd43_setuid"	},
[2024] = { 0,	TC,	SEN(printargs),		"bsd43_getuid"	},
[2025] = { 0,	0,	SEN(printargs),		"bsd43_stime"	},
[2026] = { 0,	0,	SEN(printargs),		"bsd43_ptrace"	},
[2027] = { 0,	0,	SEN(printargs),		"bsd43_alarm"	},
[2028] = { 0,	TD|TFST|TSTA,SEN(printargs),		"bsd43_oldfstat"	},
[2029] = { 0,	0,	SEN(printargs),		"bsd43_pause"	},
[2030] = { 0,	0,	SEN(printargs),		"bsd43_utime"	},
[2031] = { 0,	0,	SEN(printargs),		"bsd43_stty"	},
[2032] = { 0,	0,	SEN(printargs),		"bsd43_gtty"	},
[2033] = { 0,	0,	SEN(printargs),		"bsd43_access"	},
[2034] = { 0,	0,	SEN(printargs),		"bsd43_nice"	},
[2035] = { 0,	0,	SEN(printargs),		"bsd43_ftime"	},
[2036] = { 0,	0,	SEN(printargs),		"bsd43_sync"	},
[2037] = { 0,	0,	SEN(printargs),		"bsd43_kill"	},
[2038] = { 0,	TF|TST|TSTA,SEN(printargs),		"bsd43_stat"	},
[2039] = { 0,	0,	SEN(printargs),		"bsd43_oldsetpgrp"	},
[2040] = { 0,	TF|TLST|TSTA,SEN(printargs),		"bsd43_lstat"	},
[2041] = { 0,	0,	SEN(printargs),		"bsd43_dup"	},
[2042] = { 0,	0,	SEN(printargs),		"bsd43_pipe"	},
[2043] = { 0,	0,	SEN(printargs),		"bsd43_times"	},
[2044] = { 0,	0,	SEN(printargs),		"bsd43_profil"	},
[2045] = { 0,	0,	SEN(printargs),		"bsd43_msgsys"	},
[2046] = { 0,	TC,	SEN(printargs),		"bsd43_setgid"	},
[2047] = { 0,	TC,	SEN(printargs),		"bsd43_getgid"	},
[2048] = { 0,	0,	SEN(printargs),		"bsd43_ssig"	},
[2049] = { },
[2050] = { },
[2051] = { 0,	0,	SEN(printargs),		"bsd43_sysacct"	},
[2052] = { 0,	0,	SEN(printargs),		"bsd43_phys"	},
[2053] = { 0,	0,	SEN(printargs),		"bsd43_lock"	},
[2054] = { 0,	0,	SEN(printargs),		"bsd43_ioctl"	},
[2055] = { 0,	0,	SEN(printargs),		"bsd43_reboot"	},
[2056] = { 0,	0,	SEN(printargs),		"bsd43_mpxchan"	},
[2057] = { 0,	0,	SEN(printargs),		"bsd43_symlink"	},
[2058] = { 0,	0,	SEN(printargs),		"bsd43_readlink"	},
[2059] = { 0,	0,	SEN(printargs),		"bsd43_execve"	},
[2060] = { 0,	0,	SEN(printargs),		"bsd43_umask"	},
[2061] = { 0,	0,	SEN(printargs),		"bsd43_chroot"	},
[2062] = { 0,	TD|TFST|TSTA,SEN(printargs),		"bsd43_fstat"	},
[2063] = { },
[2064] = { 0,	0,	SEN(printargs),		"bsd43_getpagesize"	},
[2065] = { 0,	TM|SI,	SEN(printargs),		"bsd43_mremap"	},
[2066] = { 0,	0,	SEN(printargs),		"bsd43_vfork"	},
[2067] = { 0,	0,	SEN(printargs),		"bsd43_vread"	},
[2068] = { 0,	0,	SEN(printargs),		"bsd43_vwrite"	},
[2069] = { 0,	TM|SI,	SEN(printargs),		"bsd43_sbrk"	},
[2070] = { 0,	0,	SEN(printargs),		"bsd43_sstk"	},
[2071] = { 0,	TD|TM|SI,	SEN(printargs),		"bsd43_mmap"	},
[2072] = { 0,	0,	SEN(printargs),		"bsd43_vadvise"	},
[2073] = { 0,	TM|SI,	SEN(printargs),		"bsd43_munmap"	},
[2074] = { 0,	TM|SI,	SEN(printargs),		"bsd43_mprotect"	},
[2075] = { 0,	TM,	SEN(printargs),		"bsd43_madvise"	},
[2076] = { 0,	0,	SEN(printargs),		"bsd43_vhangup"	},
[2077] = { 0,	0,	SEN(printargs),		"bsd43_vlimit"	},
[2078] = { 0,	TM,	SEN(printargs),		"bsd43_mincore"	},
[2079] = { 0,	TC,	SEN(printargs),		"bsd43_getgroups"	},
[2080] = { 0,	TC,	SEN(printargs),		"bsd43_setgroups"	},
[2081] = { 0,	0,	SEN(printargs),		"bsd43_getpgrp"	},
[2082] = { 0,	0,	SEN(printargs),		"bsd43_setpgrp"	},
[2083] = { 0,	0,	SEN(printargs),		"bsd43_setitimer"	},
[2084] = { 0,	0,	SEN(printargs),		"bsd43_wait3"	},
[2085] = { 0,	0,	SEN(printargs),		"bsd43_swapon"	},
[2086] = { 0,	0,	SEN(printargs),		"bsd43_getitimer"	},
[2087] = { 0,	0,	SEN(printargs),		"bsd43_gethostname"	},
[2088] = { 0,	0,	SEN(printargs),		"bsd43_sethostname"	},
[2089] = { 0,	0,	SEN(printargs),		"bsd43_getdtablesize"	},
[2090] = { 0,	0,	SEN(printargs),		"bsd43_dup2"	},
[2091] = { 0,	0,	SEN(printargs),		"bsd43_getdopt"	},
[2092] = { 0,	0,	SEN(printargs),		"bsd43_fcntl"	},
[2093] = { 0,	0,	SEN(printargs),		"bsd43_select"	},
[2094] = { 0,	0,	SEN(printargs),		"bsd43_setdopt"	},
[2095] = { 0,	0,	SEN(printargs),		"bsd43_fsync"	},
[2096] = { 0,	0,	SEN(printargs),		"bsd43_setpriority"	},
[2097] = { 0,	0,	SEN(printargs),		"bsd43_socket"	},
[2098] = { 0,	0,	SEN(printargs),		"bsd43_connect"	},
[2099] = { 0,	0,	SEN(printargs),		"bsd43_oldaccept"	},
[2100] = { 0,	0,	SEN(printargs),		"bsd43_getpriority"	},
[2101] = { 0,	0,	SEN(printargs),		"bsd43_send"	},
[2102] = { 0,	0,	SEN(printargs),		"bsd43_recv"	},
[2103] = { 0,	0,	SEN(printargs),		"bsd43_sigreturn"	},
[2104] = { 0,	0,	SEN(printargs),		"bsd43_bind"	},
[2105] = { 0,	0,	SEN(printargs),		"bsd43_setsockopt"	},
[2106] = { 0,	0,	SEN(printargs),		"bsd43_listen"	},
[2107] = { 0,	0,	SEN(printargs),		"bsd43_vtimes"	},
[2108] = { 0,	0,	SEN(printargs),		"bsd43_sigvec"	},
[2109] = { 0,	0,	SEN(printargs),		"bsd43_sigblock"	},
[2110] = { 0,	0,	SEN(printargs),		"bsd43_sigsetmask"	},
[2111] = { 0,	0,	SEN(printargs),		"bsd43_sigpause"	},
[2112] = { 0,	0,	SEN(printargs),		"bsd43_sigstack"	},
[2113] = { 0,	0,	SEN(printargs),		"bsd43_oldrecvmsg"	},
[2114] = { 0,	0,	SEN(printargs),		"bsd43_oldsendmsg"	},
[2115] = { 0,	0,	SEN(printargs),		"bsd43_vtrace"	},
[2116] = { 0,	TCL,	SEN(printargs),		"bsd43_gettimeofday"	},
[2117] = { 0,	0,	SEN(printargs),		"bsd43_getrusage"	},
[2118] = { 0,	0,	SEN(printargs),		"bsd43_getsockopt"	},
[2119] = { },
[2120] = { 0,	0,	SEN(printargs),		"bsd43_readv"	},
[2121] = { 0,	0,	SEN(printargs),		"bsd43_writev"	},
[2122] = { 0,	TCL,	SEN(printargs),		"bsd43_settimeofday"	},
[2123] = { 0,	0,	SEN(printargs),		"bsd43_fchown"	},
[2124] = { 0,	0,	SEN(printargs),		"bsd43_fchmod"	},
[2125] = { 0,	0,	SEN(printargs),		"bsd43_oldrecvfrom"	},
[2126] = { 0,	TC,	SEN(printargs),		"bsd43_setreuid"	},
[2127] = { 0,	TC,	SEN(printargs),		"bsd43_setregid"	},
[2128] = { 0,	0,	SEN(printargs),		"bsd43_rename"	},
[2129] = { 0,	0,	SEN(printargs),		"bsd43_truncate"	},
[2130] = { 0,	0,	SEN(printargs),		"bsd43_ftruncate"	},
[2131] = { 0,	0,	SEN(printargs),		"bsd43_flock"	},
[2132] = { 0,	0,	SEN(printargs),		"bsd43_semsys"	},
[2133] = { 0,	0,	SEN(printargs),		"bsd43_sendto"	},
[2134] = { 0,	0,	SEN(printargs),		"bsd43_shutdown"	},
[2135] = { 0,	0,	SEN(printargs),		"bsd43_socketpair"	},
[2136] = { 0,	0,	SEN(printargs),		"bsd43_mkdir"	},
[2137] = { 0,	0,	SEN(printargs),		"bsd43_rmdir"	},
[2138] = { 0,	0,	SEN(printargs),		"bsd43_utimes"	},
[2139] = { 0,	0,	SEN(printargs),		"bsd43_sigcleanup"	},
[2140] = { 0,	TCL,	SEN(printargs),		"bsd43_adjtime"	},
[2141] = { 0,	0,	SEN(printargs),		"bsd43_oldgetpeername"	},
[2142] = { 0,	0,	SEN(printargs),		"bsd43_gethostid"	},
[2143] = { 0,	0,	SEN(printargs),		"bsd43_sethostid"	},
[2144] = { 0,	0,	SEN(printargs),		"bsd43_getrlimit"	},
[2145] = { 0,	0,	SEN(printargs),		"bsd43_setrlimit"	},
[2146] = { 0,	0,	SEN(printargs),		"bsd43_killpg"	},
[2147] = { 0,	0,	SEN(printargs),		"bsd43_shmsys"	},
[2148] = { 0,	0,	SEN(printargs),		"bsd43_quota"	},
[2149] = { 0,	0,	SEN(printargs),		"bsd43_qquota"	},
[2150] = { 0,	0,	SEN(printargs),		"bsd43_oldgetsockname"	},
[2151] = { 0,	0,	SEN(printargs),		"bsd43_sysmips"	},
[2152] = { 0,	0,	SEN(printargs),		"bsd43_cacheflush"	},
[2153] = { 0,	0,	SEN(printargs),		"bsd43_cachectl"	},
[2154] = { 0,	0,	SEN(printargs),		"bsd43_debug"	},
[2155] = { },
[2156] = { },
[2157] = { 0,	0,	SEN(printargs),		"bsd43_nfs_mount"	},
[2158] = { 0,	0,	SEN(printargs),		"bsd43_nfs_svc"	},
[2159] = { 0,	0,	SEN(printargs),		"bsd43_getdirentries"	},
[2160] = { 0,	TF|TSF|TSFA,SEN(printargs),		"bsd43_statfs"	},
[2161] = { 0,	TD|TFSF|TSFA,SEN(printargs),		"bsd43_fstatfs"	},
[2162] = { 0,	0,	SEN(printargs),		"bsd43_unmount"	},
[2163] = { 0,	0,	SEN(printargs),		"bsd43_async_daemon"	},
[2164] = { 0,	0,	SEN(printargs),		"bsd43_nfs_getfh"	},
[2165] = { 0,	0,	SEN(printargs),		"bsd43_getdomainname"	},
[2166] = { 0,	0,	SEN(printargs),		"bsd43_setdomainname"	},
[2167] = { 0,	0,	SEN(printargs),		"bsd43_pcfs_mount"	},
[2168] = { 0,	TF,	SEN(printargs),		"bsd43_quotactl"	},
[2169] = { 0,	0,	SEN(printargs),		"bsd43_oldexportfs"	},
[2170] = { 0,	0,	SEN(printargs),		"bsd43_smount"	},
[2171] = { 0,	0,	SEN(printargs),		"bsd43_mipshwconf"	},
[2172] = { 0,	0,	SEN(printargs),		"bsd43_exportfs"	},
[2173] = { 0,	0,	SEN(printargs),		"bsd43_nfsfh_open"	},
[2174] = { 0,	0,	SEN(printargs),		"bsd43_libattach"	},
[2175] = { 0,	0,	SEN(printargs),		"bsd43_libdetach"	},
[2176] = { 0,	0,	SEN(printargs),		"bsd43_accept"	},
[2177] = { },
[2178] = { },
[2179] = { 0,	0,	SEN(printargs),		"bsd43_recvmsg"	},
[2180] = { 0,	0,	SEN(printargs),		"bsd43_recvfrom"	},
[2181] = { 0,	0,	SEN(printargs),		"bsd43_sendmsg"	},
[2182] = { 0,	0,	SEN(printargs),		"bsd43_getpeername"	},
[2183] = { 0,	0,	SEN(printargs),		"bsd43_getsockname"	},
[2184] = { 0,	0,	SEN(printargs),		"bsd43_aread"	},
[2185] = { 0,	0,	SEN(printargs),		"bsd43_awrite"	},
[2186] = { 0,	0,	SEN(printargs),		"bsd43_listio"	},
[2187] = { 0,	0,	SEN(printargs),		"bsd43_acancel"	},
[2188] = { 0,	0,	SEN(printargs),		"bsd43_astatus"	},
[2189] = { 0,	0,	SEN(printargs),		"bsd43_await"	},
[2190] = { 0,	0,	SEN(printargs),		"bsd43_areadv"	},
[2191] = { 0,	0,	SEN(printargs),		"bsd43_awritev"	},

[3000] = { 0,	0,	SEN(printargs),		"posix_syscall"	}, /* start of POSIX */
[3001] = { 0,	0,	SEN(printargs),		"posix_exit"	},
[3002] = { 0,	0,	SEN(printargs),		"posix_fork"	},
[3003] = { 0,	0,	SEN(printargs),		"posix_read"	},
[3004] = { 0,	0,	SEN(printargs),		"posix_write"	},
[3005] = { 0,	0,	SEN(printargs),		"posix_open"	},
[3006] = { 0,	0,	SEN(printargs),		"posix_close"	},
[3007] = { 0,	0,	SEN(printargs),		"posix_wait"	},
[3008] = { 0,	0,	SEN(printargs),		"posix_creat"	},
[3009] = { 0,	0,	SEN(printargs),		"posix_link"	},
[3010] = { 0,	0,	SEN(printargs),		"posix_unlink"	},
[3011] = { 0,	0,	SEN(printargs),		"posix_exec"	},
[3012] = { 0,	0,	SEN(printargs),		"posix_chdir"	},
[3013] = { 0,	0,	SEN(printargs),		"posix_gtime"	},
[3014] = { 0,	0,	SEN(printargs),		"posix_mknod"	},
[3015] = { 0,	0,	SEN(printargs),		"posix_chmod"	},
[3016] = { 0,	0,	SEN(printargs),		"posix_chown"	},
[3017] = { 0,	TM,	SEN(printargs),		"posix_sbreak"	},
[3018] = { 0,	TF|TST|TSTA,SEN(printargs),		"posix_stat"	},
[3019] = { 0,	0,	SEN(printargs),		"posix_lseek"	},
[3020] = { 0,	0,	SEN(printargs),		"posix_getpid"	},
[3021] = { 0,	0,	SEN(printargs),		"posix_mount"	},
[3022] = { 0,	0,	SEN(printargs),		"posix_umount"	},
[3023] = { 0,	TC,	SEN(printargs),		"posix_setuid"	},
[3024] = { 0,	TC,	SEN(printargs),		"posix_getuid"	},
[3025] = { 0,	0,	SEN(printargs),		"posix_stime"	},
[3026] = { 0,	0,	SEN(printargs),		"posix_ptrace"	},
[3027] = { 0,	0,	SEN(printargs),		"posix_alarm"	},
[3028] = { 0,	TD|TFST|TSTA,SEN(printargs),		"posix_fstat"	},
[3029] = { 0,	0,	SEN(printargs),		"posix_pause"	},
[3030] = { 0,	0,	SEN(printargs),		"posix_utime"	},
[3031] = { 0,	0,	SEN(printargs),		"posix_stty"	},
[3032] = { 0,	0,	SEN(printargs),		"posix_gtty"	},
[3033] = { 0,	0,	SEN(printargs),		"posix_access"	},
[3034] = { 0,	0,	SEN(printargs),		"posix_nice"	},
[3035] = { 0,	TF|TSF|TSFA,SEN(printargs),		"posix_statfs"	},
[3036] = { 0,	0,	SEN(printargs),		"posix_sync"	},
[3037] = { 0,	0,	SEN(printargs),		"posix_kill"	},
[3038] = { 0,	TD|TFSF|TSFA,SEN(printargs),		"posix_fstatfs"	},
[3039] = { 0,	0,	SEN(printargs),		"posix_getpgrp"	},
[3040] = { 0,	0,	SEN(printargs),		"posix_syssgi"	},
[3041] = { 0,	0,	SEN(printargs),		"posix_dup"	},
[3042] = { 0,	0,	SEN(printargs),		"posix_pipe"	},
[3043] = { 0,	0,	SEN(printargs),		"posix_times"	},
[3044] = { 0,	0,	SEN(printargs),		"posix_profil"	},
[3045] = { 0,	0,	SEN(printargs),		"posix_lock"	},
[3046] = { 0,	TC,	SEN(printargs),		"posix_setgid"	},
[3047] = { 0,	TC,	SEN(printargs),		"posix_getgid"	},
[3048] = { 0,	0,	SEN(printargs),		"posix_sig"	},
[3049] = { 0,	0,	SEN(printargs),		"posix_msgsys"	},
[3050] = { 0,	0,	SEN(printargs),		"posix_sysmips"	},
[3051] = { 0,	0,	SEN(printargs),		"posix_sysacct"	},
[3052] = { 0,	0,	SEN(printargs),		"posix_shmsys"	},
[3053] = { 0,	0,	SEN(printargs),		"posix_semsys"	},
[3054] = { 0,	0,	SEN(printargs),		"posix_ioctl"	},
[3055] = { 0,	0,	SEN(printargs),		"posix_uadmin"	},
[3056] = { 0,	0,	SEN(printargs),		"posix_exch"	},
[3057] = { 0,	0,	SEN(printargs),		"posix_utssys"	},
[3058] = { },
[3059] = { 0,	0,	SEN(printargs),		"posix_exece"	},
[3060] = { 0,	0,	SEN(printargs),		"posix_umask"	},
[3061] = { 0,	0,	SEN(printargs),		"posix_chroot"	},
[3062] = { 0,	0,	SEN(printargs),		"posix_fcntl"	},
[3063] = { 0,	0,	SEN(printargs),		"posix_ulimit"	},
[3064 ... 3069] = { },
[3070] = { 0,	0,	SEN(printargs),		"posix_advfs"	},
[3071] = { 0,	0,	SEN(printargs),		"posix_unadvfs"	},
[3072] = { 0,	0,	SEN(printargs),		"posix_rmount"	},
[3073] = { 0,	0,	SEN(printargs),		"posix_rumount"	},
[3074] = { 0,	0,	SEN(printargs),		"posix_rfstart"	},
[3075] = { },
[3076] = { 0,	0,	SEN(printargs),		"posix_rdebug"	},
[3077] = { 0,	0,	SEN(printargs),		"posix_rfstop"	},
[3078] = { 0,	0,	SEN(printargs),		"posix_rfsys"	},
[3079] = { 0,	0,	SEN(printargs),		"posix_rmdir"	},
[3080] = { 0,	0,	SEN(printargs),		"posix_mkdir"	},
[3081] = { 0,	0,	SEN(printargs),		"posix_getdents"	},
[3082] = { 0,	0,	SEN(printargs),		"posix_sginap"	},
[3083] = { 0,	0,	SEN(printargs),		"posix_sgikopt"	},
[3084] = { 0,	0,	SEN(printargs),		"posix_sysfs"	},
[3085] = { 0,	0,	SEN(printargs),		"posix_getmsg"	},
[3086] = { 0,	0,	SEN(printargs),		"posix_putmsg"	},
[3087] = { 0,	0,	SEN(printargs),		"posix_poll"	},
[3088] = { 0,	0,	SEN(printargs),		"posix_sigreturn"	},
[3089] = { 0,	0,	SEN(printargs),		"posix_accept"	},
[3090] = { 0,	0,	SEN(printargs),		"posix_bind"	},
[3091] = { 0,	0,	SEN(printargs),		"posix_connect"	},
[3092] = { 0,	0,	SEN(printargs),		"posix_gethostid"	},
[3093] = { 0,	0,	SEN(printargs),		"posix_getpeername"	},
[3094] = { 0,	0,	SEN(printargs),		"posix_getsockname"	},
[3095] = { 0,	0,	SEN(printargs),		"posix_getsockopt"	},
[3096] = { 0,	0,	SEN(printargs),		"posix_listen"	},
[3097] = { 0,	0,	SEN(printargs),		"posix_recv"	},
[3098] = { 0,	0,	SEN(printargs),		"posix_recvfrom"	},
[3099] = { 0,	0,	SEN(printargs),		"posix_recvmsg"	},
[3100] = { 0,	0,	SEN(printargs),		"posix_select"	},
[3101] = { 0,	0,	SEN(printargs),		"posix_send"	},
[3102] = { 0,	0,	SEN(printargs),		"posix_sendmsg"	},
[3103] = { 0,	0,	SEN(printargs),		"posix_sendto"	},
[3104] = { 0,	0,	SEN(printargs),		"posix_sethostid"	},
[3105] = { 0,	0,	SEN(printargs),		"posix_setsockopt"	},
[3106] = { 0,	0,	SEN(printargs),		"posix_shutdown"	},
[3107] = { 0,	0,	SEN(printargs),		"posix_socket"	},
[3108] = { 0,	0,	SEN(printargs),		"posix_gethostname"	},
[3109] = { 0,	0,	SEN(printargs),		"posix_sethostname"	},
[3110] = { 0,	0,	SEN(printargs),		"posix_getdomainname"	},
[3111] = { 0,	0,	SEN(printargs),		"posix_setdomainname"	},
[3112] = { 0,	0,	SEN(printargs),		"posix_truncate"	},
[3113] = { 0,	0,	SEN(printargs),		"posix_ftruncate"	},
[3114] = { 0,	0,	SEN(printargs),		"posix_rename"	},
[3115] = { 0,	0,	SEN(printargs),		"posix_symlink"	},
[3116] = { 0,	0,	SEN(printargs),		"posix_readlink"	},
[3117] = { 0,	TF|TLST|TSTA,SEN(printargs),		"posix_lstat"	},
[3118] = { 0,	0,	SEN(printargs),		"posix_nfs_mount"	},
[3119] = { 0,	0,	SEN(printargs),		"posix_nfs_svc"	},
[3120] = { 0,	0,	SEN(printargs),		"posix_nfs_getfh"	},
[3121] = { 0,	0,	SEN(printargs),		"posix_async_daemon"	},
[3122] = { 0,	0,	SEN(printargs),		"posix_exportfs"	},
[3123] = { 0,	TC,	SEN(printargs),		"posix_SGI_setregid"	},
[3124] = { 0,	TC,	SEN(printargs),		"posix_SGI_setreuid"	},
[3125] = { 0,	0,	SEN(printargs),		"posix_getitimer"	},
[3126] = { 0,	0,	SEN(printargs),		"posix_setitimer"	},
[3127] = { 0,	TCL,	SEN(printargs),		"posix_adjtime"	},
[3128] = { 0,	0,	SEN(printargs),		"posix_SGI_bsdgettime"	},
[3129] = { 0,	0,	SEN(printargs),		"posix_SGI_sproc"	},
[3130] = { 0,	0,	SEN(printargs),		"posix_SGI_prctl"	},
[3131] = { 0,	0,	SEN(printargs),		"posix_SGI_blkproc"	},
[3132] = { },
[3133] = { 0,	0,	SEN(printargs),		"posix_SGI_sgigsc"	},
[3134] = { 0,	TD|TM|SI,	SEN(printargs),		"posix_SGI_mmap"	},
[3135] = { 0,	TM|SI,	SEN(printargs),		"posix_SGI_munmap"	},
[3136] = { 0,	TM|SI,	SEN(printargs),		"posix_SGI_mprotect"	},
[3137] = { 0,	TM,	SEN(printargs),		"posix_SGI_msync"	},
[3138] = { 0,	TM,	SEN(printargs),		"posix_SGI_madvise"	},
[3139] = { 0,	0,	SEN(printargs),		"posix_SGI_mpin"	},
[3140] = { 0,	0,	SEN(printargs),		"posix_SGI_getpagesize"	},
[3141] = { 0,	0,	SEN(printargs),		"posix_SGI_libattach"	},
[3142] = { 0,	0,	SEN(printargs),		"posix_SGI_libdetach"	},
[3143] = { 0,	0,	SEN(printargs),		"posix_SGI_getpgrp"	},
[3144] = { 0,	0,	SEN(printargs),		"posix_SGI_setpgrp"	},
[3145 ... 3149] = { },
[3150] = { 0,	0,	SEN(printargs),		"posix_cacheflush"	},
[3151] = { 0,	0,	SEN(printargs),		"posix_cachectl"	},
[3152] = { 0,	0,	SEN(printargs),		"posix_fchown"	},
[3153] = { 0,	0,	SEN(printargs),		"posix_fchmod"	},
[3154] = { 0,	0,	SEN(printargs),		"posix_wait3"	},
[3155] = { 0,	TD|TM|SI,	SEN(printargs),		"posix_mmap"	},
[3156] = { 0,	TM|SI,	SEN(printargs),		"posix_munmap"	},
[3157] = { 0,	TM,	SEN(printargs),		"posix_madvise"	},
[3158] = { 0,	0,	SEN(printargs),		"posix_BSD_getpagesize"	},
[3159] = { 0,	TC,	SEN(printargs),		"posix_setreuid"	},
[3160] = { 0,	TC,	SEN(printargs),		"posix_setregid"	},
[3161] = { 0,	0,	SEN(printargs),		"posix_setpgid"	},
[3162] = { 0,	TC,	SEN(printargs),		"posix_getgroups"	},
[3163] = { 0,	TC,	SEN(printargs),		"posix_setgroups"	},
[3164] = { 0,	TCL,	SEN(printargs),		"posix_gettimeofday"	},
[3165] = { 0,	0,	SEN(printargs),		"posix_getrusage"	},
[3166] = { 0,	0,	SEN(printargs),		"posix_getrlimit"	},
[3167] = { 0,	0,	SEN(printargs),		"posix_setrlimit"	},
[3168] = { 0,	0,	SEN(printargs),		"posix_waitpid"	},
[3169] = { 0,	0,	SEN(printargs),		"posix_dup2"	},
[3170 ... 3259] = { },
[3260] = { 0,	0,	SEN(printargs),		"posix_netboot"	},
[3261] = { 0,	0,	SEN(printargs),		"posix_netunboot"	},
[3262] = { 0,	0,	SEN(printargs),		"posix_rdump"	},
[3263] = { 0,	0,	SEN(printargs),		"posix_setsid"	},
[3264] = { 0,	0,	SEN(printargs),		"posix_getmaxsig"	},
[3265] = { 0,	0,	SEN(printargs),		"posix_sigpending"	},
[3266] = { 0,	0,	SEN(printargs),		"posix_sigprocmask"	},
[3267] = { 0,	0,	SEN(printargs),		"posix_sigsuspend"	},
[3268] = { 0,	0,	SEN(printargs),		"posix_sigaction"	},
