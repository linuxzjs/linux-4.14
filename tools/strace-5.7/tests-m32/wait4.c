/*
 * Check decoding of wait4 syscall.
 *
 * Copyright (c) 2015-2016 Dmitry V. Levin <ldv@altlinux.org>
 * Copyright (c) 2016-2020 The strace developers.
 * All rights reserved.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "tests.h"
#include "scno.h"

#ifdef __NR_wait4

# include <assert.h>
# include <signal.h>
# include <stdio.h>
# include <unistd.h>
# include <sys/wait.h>
# include <sys/resource.h>

static const char *
sprint_rusage(const struct rusage *const ru)
{
	static char buf[1024];
	snprintf(buf, sizeof(buf),
		 "{ru_utime={tv_sec=%lld, tv_usec=%llu}"
		 ", ru_stime={tv_sec=%lld, tv_usec=%llu}"
# if VERBOSE
		 ", ru_maxrss=%lu"
		 ", ru_ixrss=%lu"
		 ", ru_idrss=%lu"
		 ", ru_isrss=%lu"
		 ", ru_minflt=%lu"
		 ", ru_majflt=%lu"
		 ", ru_nswap=%lu"
		 ", ru_inblock=%lu"
		 ", ru_oublock=%lu"
		 ", ru_msgsnd=%lu"
		 ", ru_msgrcv=%lu"
		 ", ru_nsignals=%lu"
		 ", ru_nvcsw=%lu"
		 ", ru_nivcsw=%lu}"
# else
		 ", ...}"
# endif
		 , (long long) ru->ru_utime.tv_sec
		 , zero_extend_signed_to_ull(ru->ru_utime.tv_usec)
		 , (long long) ru->ru_stime.tv_sec
		 , zero_extend_signed_to_ull(ru->ru_stime.tv_usec)
# if VERBOSE
		 , (long) ru->ru_maxrss
		 , (long) ru->ru_ixrss
		 , (long) ru->ru_idrss
		 , (long) ru->ru_isrss
		 , (long) ru->ru_minflt
		 , (long) ru->ru_majflt
		 , (long) ru->ru_nswap
		 , (long) ru->ru_inblock
		 , (long) ru->ru_oublock
		 , (long) ru->ru_msgsnd
		 , (long) ru->ru_msgrcv
		 , (long) ru->ru_nsignals
		 , (long) ru->ru_nvcsw
		 , (long) ru->ru_nivcsw
# endif
		 );
	return buf;
}

static pid_t
do_wait4(pid_t pid, int *wstatus, int options, struct rusage *ru)
{
	sigset_t mask = {};
	sigaddset(&mask, SIGCHLD);

	assert(sigprocmask(SIG_BLOCK, &mask, NULL) == 0);
	pid_t rc = wait4(pid, wstatus, options, ru);
	assert(sigprocmask(SIG_UNBLOCK, &mask, NULL) == 0);
	return rc;
}

int
main(void)
{
	tprintf("%s", "");

	int fds[2];
	if (pipe(fds))
		perror_msg_and_fail("pipe");

	pid_t pid;
	pid = fork();
	if (pid < 0)
		perror_msg_and_fail("fork");

	if (!pid) {
		char c;
		(void) close(1);
		assert(read(0, &c, sizeof(c)) == 1);
		return 42;
	}

	(void) close(0);

	TAIL_ALLOC_OBJECT_CONST_PTR(int, s);
	if (wait4(pid, s, WNOHANG|__WALL, NULL))
		perror_msg_and_fail("wait4 #1");
	tprintf("wait4(%d, %p, WNOHANG|__WALL, NULL) = 0\n", pid, s);

	TAIL_ALLOC_OBJECT_CONST_PTR(struct rusage, rusage);
	if (wait4(pid, s, WNOHANG|__WALL, rusage))
		perror_msg_and_fail("wait4 #2");
	tprintf("wait4(%d, %p, WNOHANG|__WALL, %p) = 0\n", pid, s, rusage);

	assert(write(1, "", 1) == 1);
	(void) close(1);

	assert(do_wait4(pid, s, 0, rusage) == pid);
	assert(WIFEXITED(*s) && WEXITSTATUS(*s) == 42);
	tprintf("wait4(%d, [{WIFEXITED(s) && WEXITSTATUS(s) == 42}], 0, %s)"
		" = %d\n", pid, sprint_rusage(rusage), pid);

	pid = fork();
	if (pid < 0)
		perror_msg_and_fail("fork");

	if (!pid) {
		(void) raise(SIGUSR1);
		return 1;
	}

	assert(do_wait4(pid, s, __WALL, rusage) == pid);
	assert(WIFSIGNALED(*s) && WTERMSIG(*s) == SIGUSR1);
	tprintf("wait4(%d, [{WIFSIGNALED(s) && WTERMSIG(s) == SIGUSR1}]"
		", __WALL, %s) = %d\n", pid, sprint_rusage(rusage), pid);

	if (pipe(fds))
		perror_msg_and_fail("pipe");
	pid = fork();
	if (pid < 0)
		perror_msg_and_fail("fork");

	if (!pid) {
		(void) close(1);
		raise(SIGSTOP);
		char c;
		assert(read(0, &c, sizeof(c)) == 1);
		return 0;
	}

	(void) close(0);

	assert(do_wait4(pid, s, WSTOPPED, rusage) == pid);
	assert(WIFSTOPPED(*s) && WSTOPSIG(*s) == SIGSTOP);
	tprintf("wait4(%d, [{WIFSTOPPED(s) && WSTOPSIG(s) == SIGSTOP}]"
		", WSTOPPED, %s) = %d\n", pid, sprint_rusage(rusage), pid);

	if (kill(pid, SIGCONT))
		perror_msg_and_fail("kill(SIGCONT)");

# if defined WCONTINUED && defined WIFCONTINUED
	assert(do_wait4(pid, s, WCONTINUED, rusage) == pid);
	assert(WIFCONTINUED(*s));
	tprintf("wait4(%d, [{WIFCONTINUED(s)}], WCONTINUED"
		", %s) = %d\n", pid, sprint_rusage(rusage), pid);
# endif /* WCONTINUED && WIFCONTINUED */

	assert(write(1, "", 1) == 1);
	(void) close(1);

	assert(do_wait4(pid, s, 0, rusage) == pid);
	assert(WIFEXITED(*s) && WEXITSTATUS(*s) == 0);
	tprintf("wait4(%d, [{WIFEXITED(s) && WEXITSTATUS(s) == 0}], 0"
		", %s) = %d\n", pid, sprint_rusage(rusage), pid);

	assert(wait4(-1, s, WNOHANG|WSTOPPED|__WALL, rusage) == -1);
	tprintf("wait4(-1, %p, WNOHANG|WSTOPPED|__WALL, %p) = -1 %s (%m)\n",
		s, rusage, errno2name());

	tprintf("%s\n", "+++ exited with 0 +++");
	return 0;
}

#else

SKIP_MAIN_UNDEFINED("__NR_wait4")

#endif /* __NR_wait4 */
