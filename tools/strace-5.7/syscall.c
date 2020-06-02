/*
 * Copyright (c) 1991, 1992 Paul Kranenburg <pk@cs.few.eur.nl>
 * Copyright (c) 1993 Branko Lankester <branko@hacktic.nl>
 * Copyright (c) 1993, 1994, 1995, 1996 Rick Sladkey <jrs@world.std.com>
 * Copyright (c) 1996-1999 Wichert Akkerman <wichert@cistron.nl>
 * Copyright (c) 1999 IBM Deutschland Entwicklung GmbH, IBM Corporation
 *                     Linux for s390 port by D.J. Barrow
 *                    <barrow_dj@mail.yahoo.com,djbarrow@de.ibm.com>
 * Copyright (c) 1999-2020 The strace developers.
 * All rights reserved.
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "defs.h"
#include "get_personality.h"
#include "mmap_notify.h"
#include "native_defs.h"
#include "ptrace.h"
#include "ptrace_syscall_info.h"
#include "nsig.h"
#include "number_set.h"
#include "delay.h"
#include "retval.h"
#include <limits.h>

/* for struct iovec */
#include <sys/uio.h>

/* for __X32_SYSCALL_BIT */
#include "scno.h"

#include "regs.h"

#if defined(SPARC64)
# undef PTRACE_GETREGS
# define PTRACE_GETREGS PTRACE_GETREGS64
# undef PTRACE_SETREGS
# define PTRACE_SETREGS PTRACE_SETREGS64
#endif

#include "syscall.h"
#include "xstring.h"
#include "syscallent_base_nr.h"

/* Define these shorthand notations to simplify the syscallent files. */
#include "sysent_shorthand_defs.h"

#define SEN(syscall_name) SEN_ ## syscall_name, SYS_FUNC_NAME(sys_ ## syscall_name)

const struct_sysent sysent0[] = {
#include "syscallent.h"
};

#if SUPPORTED_PERSONALITIES > 1
# include PERSONALITY1_INCLUDE_FUNCS
static const struct_sysent sysent1[] = {
# include "syscallent1.h"
};
#endif

#if SUPPORTED_PERSONALITIES > 2
# include PERSONALITY2_INCLUDE_FUNCS
static const struct_sysent sysent2[] = {
# include "syscallent2.h"
};
#endif

/* Now undef them since short defines cause wicked namespace pollution. */
#include "sysent_shorthand_undefs.h"

const char *const errnoent[] = {
#include "errnoent.h"
};
const char *const signalent[] = {
#include "signalent.h"

};
/*
 * `ioctlent[012].h' files are automatically generated by the auxiliary
 * program `ioctlsort', such that the list is sorted by the `code' field.
 * This has the side-effect of resolving the _IO.. macros into
 * plain integers, eliminating the need to include here everything
 * in "/usr/include".
 */

const struct_ioctlent ioctlent0[] = {
#include "ioctlent0.h"
};

#if SUPPORTED_PERSONALITIES > 1
static const struct_ioctlent ioctlent1[] = {
# include "ioctlent1.h"
};
# include PERSONALITY0_INCLUDE_PRINTERS_DECLS
static const struct_printers printers0 = {
# include PERSONALITY0_INCLUDE_PRINTERS_DEFS
};
# include PERSONALITY1_INCLUDE_PRINTERS_DECLS
static const struct_printers printers1 = {
# include PERSONALITY1_INCLUDE_PRINTERS_DEFS
};
#endif

#if SUPPORTED_PERSONALITIES > 2
static const struct_ioctlent ioctlent2[] = {
# include "ioctlent2.h"
};
# include PERSONALITY2_INCLUDE_PRINTERS_DECLS
static const struct_printers printers2 = {
# include PERSONALITY2_INCLUDE_PRINTERS_DEFS
};
#endif

enum {
	nsyscalls0 = ARRAY_SIZE(sysent0)
#if SUPPORTED_PERSONALITIES > 1
	, nsyscalls1 = ARRAY_SIZE(sysent1)
# if SUPPORTED_PERSONALITIES > 2
	, nsyscalls2 = ARRAY_SIZE(sysent2)
# endif
#endif
};

enum {
	nioctlents0 = ARRAY_SIZE(ioctlent0)
#if SUPPORTED_PERSONALITIES > 1
	, nioctlents1 = ARRAY_SIZE(ioctlent1)
# if SUPPORTED_PERSONALITIES > 2
	, nioctlents2 = ARRAY_SIZE(ioctlent2)
# endif
#endif
};

#if SUPPORTED_PERSONALITIES > 1
const struct_sysent *sysent = sysent0;
const struct_ioctlent *ioctlent = ioctlent0;
const struct_printers *printers = &printers0;
#endif

const unsigned int nerrnos = ARRAY_SIZE(errnoent);
const unsigned int nsignals = ARRAY_SIZE(signalent);
unsigned nsyscalls = nsyscalls0;
unsigned nioctlents = nioctlents0;

const unsigned int nsyscall_vec[SUPPORTED_PERSONALITIES] = {
	nsyscalls0,
#if SUPPORTED_PERSONALITIES > 1
	nsyscalls1,
#endif
#if SUPPORTED_PERSONALITIES > 2
	nsyscalls2,
#endif
};
const struct_sysent *const sysent_vec[SUPPORTED_PERSONALITIES] = {
	sysent0,
#if SUPPORTED_PERSONALITIES > 1
	sysent1,
#endif
#if SUPPORTED_PERSONALITIES > 2
	sysent2,
#endif
};

const char *const personality_names[] =
#if defined X86_64
	{"64 bit", "32 bit", "x32"}
#elif defined X32
	{"x32", "32 bit"}
#elif SUPPORTED_PERSONALITIES == 2
	{"64 bit", "32 bit"}
#else
	{STRINGIFY_VAL(__WORDSIZE) " bit"}
#endif
	;

const char *const personality_designators[] =
#if defined X86_64
	{ "64", "32", "x32" }
#elif defined X32
	{ "x32", "32" }
#elif SUPPORTED_PERSONALITIES == 2
	{ "64", "32" }
#else
	{ STRINGIFY_VAL(__WORDSIZE) }
#endif
	;

#if SUPPORTED_PERSONALITIES > 1

unsigned current_personality;

# ifndef current_wordsize
unsigned current_wordsize = PERSONALITY0_WORDSIZE;
static const int personality_wordsize[SUPPORTED_PERSONALITIES] = {
	PERSONALITY0_WORDSIZE,
	PERSONALITY1_WORDSIZE,
#  if SUPPORTED_PERSONALITIES > 2
	PERSONALITY2_WORDSIZE,
#  endif
};
# endif

# ifndef current_klongsize
unsigned current_klongsize = PERSONALITY0_KLONGSIZE;
static const int personality_klongsize[SUPPORTED_PERSONALITIES] = {
	PERSONALITY0_KLONGSIZE,
	PERSONALITY1_KLONGSIZE,
#  if SUPPORTED_PERSONALITIES > 2
	PERSONALITY2_KLONGSIZE,
#  endif
};
# endif

void
set_personality(unsigned int personality)
{
	if (personality == current_personality)
		return;

	if (personality >= SUPPORTED_PERSONALITIES)
		error_msg_and_die("Requested switch to unsupported personality "
				  "%u", personality);

	nsyscalls = nsyscall_vec[personality];
	sysent = sysent_vec[personality];

	switch (personality) {
	case 0:
		ioctlent = ioctlent0;
		nioctlents = nioctlents0;
		printers = &printers0;
		break;

	case 1:
		ioctlent = ioctlent1;
		nioctlents = nioctlents1;
		printers = &printers1;
		break;

# if SUPPORTED_PERSONALITIES > 2
	case 2:
		ioctlent = ioctlent2;
		nioctlents = nioctlents2;
		printers = &printers2;
		break;
# endif
	}

	current_personality = personality;
# ifndef current_wordsize
	current_wordsize = personality_wordsize[personality];
# endif
# ifndef current_klongsize
	current_klongsize = personality_klongsize[personality];
# endif
}

static void
update_personality(struct tcb *tcp, unsigned int personality)
{
	static bool need_mpers_warning[] =
		{ false, !HAVE_PERSONALITY_1_MPERS, !HAVE_PERSONALITY_2_MPERS };

	set_personality(personality);

	if (personality == tcp->currpers)
		return;
	tcp->currpers = personality;

	if (!is_number_in_set(QUIET_PERSONALITY, quiet_set)) {
		printleader(tcp);
		tprintf("[ Process PID=%d runs in %s mode. ]\n",
			tcp->pid, personality_names[personality]);
		line_ended();
	}

	if (need_mpers_warning[personality]) {
		error_msg("WARNING: Proper structure decoding for this "
			  "personality is not supported, please consider "
			  "building strace with mpers support enabled.");
		need_mpers_warning[personality] = false;
	}
}
#endif

#ifdef SYS_socket_subcall
static void
decode_socket_subcall(struct tcb *tcp)
{
	const int call = tcp->u_arg[0];

	if (call < 1 || call >= SYS_socket_nsubcalls)
		return;

	const kernel_ulong_t scno = SYS_socket_subcall + call;
	const unsigned int nargs = sysent[scno].nargs;
	uint64_t buf[nargs];

	if (umoven(tcp, tcp->u_arg[1], nargs * current_wordsize, buf) < 0)
		return;

	tcp->scno = scno;
	tcp->qual_flg = qual_flags(scno);
	tcp->s_ent = &sysent[scno];

	unsigned int i;
	for (i = 0; i < nargs; ++i)
		tcp->u_arg[i] = (sizeof(uint32_t) == current_wordsize)
				? ((uint32_t *) (void *) buf)[i] : buf[i];
}
#endif /* SYS_socket_subcall */

#ifdef SYS_ipc_subcall
static void
decode_ipc_subcall(struct tcb *tcp)
{
	unsigned int call = tcp->u_arg[0];
	const unsigned int version = call >> 16;

	if (version) {
# if defined S390 || defined S390X
		return;
# else
#  ifdef SPARC64
		if (current_wordsize == 8)
			return;
#  endif
		set_tcb_priv_ulong(tcp, version);
		call &= 0xffff;
# endif
	}

	switch (call) {
		case  1: case  2: case  3: case  4:
		case 11: case 12: case 13: case 14:
		case 21: case 22: case 23: case 24:
			break;
		default:
			return;
	}

	tcp->scno = SYS_ipc_subcall + call;
	tcp->qual_flg = qual_flags(tcp->scno);
	tcp->s_ent = &sysent[tcp->scno];

	const unsigned int n = n_args(tcp);
	unsigned int i;
	for (i = 0; i < n; i++)
		tcp->u_arg[i] = tcp->u_arg[i + 1];
}
#endif /* SYS_ipc_subcall */

#ifdef SYS_syscall_subcall
/* The implementation is architecture specific.  */
static void decode_syscall_subcall(struct tcb *);
#endif /* SYS_syscall_subcall */

static void
dumpio(struct tcb *tcp)
{
	int fd = tcp->u_arg[0];
	if (fd < 0)
		return;

	if (is_number_in_set(fd, write_set)) {
		switch (tcp_sysent(tcp)->sen) {
		case SEN_write:
		case SEN_pwrite:
		case SEN_send:
		case SEN_sendto:
		case SEN_mq_timedsend_time32:
		case SEN_mq_timedsend_time64:
			dumpstr(tcp, tcp->u_arg[1], tcp->u_arg[2]);
			break;
		case SEN_writev:
		case SEN_pwritev:
		case SEN_pwritev2:
		case SEN_vmsplice:
			dumpiov_upto(tcp, tcp->u_arg[2], tcp->u_arg[1], -1);
			break;
		case SEN_sendmsg:
			dumpiov_in_msghdr(tcp, tcp->u_arg[1], -1);
			break;
		case SEN_sendmmsg:
			dumpiov_in_mmsghdr(tcp, tcp->u_arg[1]);
			break;
		}
	}

	if (syserror(tcp))
		return;

	if (is_number_in_set(fd, read_set)) {
		switch (tcp_sysent(tcp)->sen) {
		case SEN_read:
		case SEN_pread:
		case SEN_recv:
		case SEN_recvfrom:
		case SEN_mq_timedreceive_time32:
		case SEN_mq_timedreceive_time64:
			dumpstr(tcp, tcp->u_arg[1], tcp->u_rval);
			return;
		case SEN_readv:
		case SEN_preadv:
		case SEN_preadv2:
			dumpiov_upto(tcp, tcp->u_arg[2], tcp->u_arg[1],
				     tcp->u_rval);
			return;
		case SEN_recvmsg:
			dumpiov_in_msghdr(tcp, tcp->u_arg[1], tcp->u_rval);
			return;
		case SEN_recvmmsg:
		case SEN_recvmmsg_time32:
		case SEN_recvmmsg_time64:
			dumpiov_in_mmsghdr(tcp, tcp->u_arg[1]);
			return;
		}
	}
}

static const char *
err_name(uint64_t err)
{
	return err < nerrnos ? errnoent[err] : NULL;
}

void
print_err(int64_t err, bool negated)
{
	const char *str = err_name(negated ? -err : err);

	if (!str || xlat_verbose(xlat_verbosity) != XLAT_STYLE_ABBREV)
		tprintf(negated ? "%" PRId64 : "%" PRIu64, err);
	if (!str || xlat_verbose(xlat_verbosity) == XLAT_STYLE_RAW)
		return;
	(xlat_verbose(xlat_verbosity) == XLAT_STYLE_ABBREV
		? tprintf : tprintf_comment)("%s%s",
					     negated ? "-" : "", str);
}

static void
print_err_ret(kernel_ulong_t ret, unsigned long u_error)
{
	const char *u_error_str = err_name(u_error);

	if (u_error_str)
		tprintf("= %" PRI_kld " %s (%s)",
			ret, u_error_str, strerror(u_error));
	else
		tprintf("= %" PRI_kld " (errno %lu)", ret, u_error);
}

static long get_regs(struct tcb *);
static int get_syscall_args(struct tcb *);
static int get_syscall_result(struct tcb *);
static void get_error(struct tcb *, bool);
static void set_error(struct tcb *, unsigned long);
static void set_success(struct tcb *, kernel_long_t);
static int arch_get_scno(struct tcb *);
static int arch_check_scno(struct tcb *);
static int arch_set_scno(struct tcb *, kernel_ulong_t);
static int arch_get_syscall_args(struct tcb *);
static void arch_get_error(struct tcb *, bool);
static int arch_set_error(struct tcb *);
static int arch_set_success(struct tcb *);
#if MAX_ARGS > 6
static void arch_get_syscall_args_extra(struct tcb *, unsigned int);
#endif

struct inject_opts *inject_vec[SUPPORTED_PERSONALITIES];

static struct inject_opts *
tcb_inject_opts(struct tcb *tcp)
{
	return (scno_in_range(tcp->scno) && tcp->inject_vec[current_personality])
	       ? &tcp->inject_vec[current_personality][tcp->scno] : NULL;
}


static long
tamper_with_syscall_entering(struct tcb *tcp, unsigned int *signo)
{
	if (!tcp->inject_vec[current_personality]) {
		tcp->inject_vec[current_personality] =
			xcalloc(nsyscalls, sizeof(**inject_vec));
		memcpy(tcp->inject_vec[current_personality],
		       inject_vec[current_personality],
		       nsyscalls * sizeof(**inject_vec));
	}

	struct inject_opts *opts = tcb_inject_opts(tcp);

	if (!opts || opts->first == 0 || opts->last == 0)
		return 0;

	if (opts->last != INJECT_LAST_INF)
		--opts->last;

	--opts->first;

	if (opts->first != 0)
		return 0;

	opts->first = opts->step;

	if (!recovering(tcp)) {
		if (opts->data.flags & INJECT_F_SIGNAL)
			*signo = opts->data.signo;
		if (opts->data.flags & (INJECT_F_ERROR | INJECT_F_RETVAL)) {
			kernel_long_t scno =
				(opts->data.flags & INJECT_F_SYSCALL)
				? (kernel_long_t) shuffle_scno(opts->data.scno)
				: -1;

			if (!arch_set_scno(tcp, scno)) {
				tcp->flags |= TCB_TAMPERED;
				if (scno != -1)
					tcp->flags |= TCB_TAMPERED_NO_FAIL;
#if ARCH_NEEDS_SET_ERROR_FOR_SCNO_TAMPERING
				/*
				 * So far it's just a workaround for hppa,
				 * but let's pretend it could be used elsewhere.
				 */
				else {
					kernel_long_t rval =
						(opts->data.flags & INJECT_F_RETVAL) ?
						ENOSYS : retval_get(opts->data.rval_idx);

					tcp->u_error = 0; /* force reset */
					set_error(tcp, rval);
				}
#endif
			}
		}
		if (opts->data.flags & INJECT_F_DELAY_ENTER)
			delay_tcb(tcp, opts->data.delay_idx, true);
		if (opts->data.flags & INJECT_F_DELAY_EXIT)
			tcp->flags |= TCB_INJECT_DELAY_EXIT;
	}

	return 0;
}

static long
tamper_with_syscall_exiting(struct tcb *tcp)
{
	struct inject_opts *opts = tcb_inject_opts(tcp);
	if (!opts)
		return 0;

	if (inject_delay_exit(tcp))
		delay_tcb(tcp, opts->data.delay_idx, false);

	if (!syscall_tampered(tcp))
		return 0;

	if (!syserror(tcp) ^ !!syscall_tampered_nofail(tcp)) {
		error_msg("Failed to tamper with process %d: unexpectedly got"
			  " %serror (return value %#" PRI_klx ", error %lu)",
			  tcp->pid, syscall_tampered_nofail(tcp) ? "" : "no ",
			  tcp->u_rval, tcp->u_error);

		return 1;
	}

	if (opts->data.flags & INJECT_F_RETVAL)
		set_success(tcp, retval_get(opts->data.rval_idx));
	else
		set_error(tcp, retval_get(opts->data.rval_idx));

	return 0;
}

/*
 * Returns:
 * 0: "ignore this ptrace stop", bail out silently.
 * 1: ok, decoded; call
 *    syscall_entering_finish(tcp, syscall_entering_trace(tcp, ...)).
 * other: error; call syscall_entering_finish(tcp, res), where res is the value
 *    returned.
 */
int
syscall_entering_decode(struct tcb *tcp)
{
	int res = get_scno(tcp);
	if (res == 0)
		return res;
	if (res != 1 || (res = get_syscall_args(tcp)) != 1) {
		printleader(tcp);
		tprintf("%s(", tcp_sysent(tcp)->sys_name);
		/*
		 * " <unavailable>" will be added later by the code which
		 * detects ptrace errors.
		 */
		return res;
	}

#ifdef SYS_syscall_subcall
	if (tcp_sysent(tcp)->sen == SEN_syscall)
		decode_syscall_subcall(tcp);
#endif
#if defined SYS_ipc_subcall	\
 || defined SYS_socket_subcall
	switch (tcp_sysent(tcp)->sen) {
# ifdef SYS_ipc_subcall
		case SEN_ipc:
			decode_ipc_subcall(tcp);
			break;
# endif
# ifdef SYS_socket_subcall
		case SEN_socketcall:
			decode_socket_subcall(tcp);
			break;
# endif
	}
#endif

	return 1;
}

int
syscall_entering_trace(struct tcb *tcp, unsigned int *sig)
{
	if (hide_log(tcp)) {
		/*
		 * Restrain from fault injection
		 * while the tracee executes strace code.
		 */
		tcp->qual_flg &= ~QUAL_INJECT;

		switch (tcp_sysent(tcp)->sen) {
			case SEN_execve:
			case SEN_execveat:
			case SEN_execv:
				/*
				 * First exec* syscall makes the log visible.
				 */
				tcp->flags &= ~TCB_HIDE_LOG;
				/*
				 * Check whether this exec* syscall succeeds.
				 */
				tcp->flags |= TCB_CHECK_EXEC_SYSCALL;
				break;
		}
	}

	if (hide_log(tcp) || !traced(tcp) || (tracing_paths && !pathtrace_match(tcp))) {
		tcp->flags |= TCB_FILTERED;
		return 0;
	}

	tcp->flags &= ~TCB_FILTERED;

	if (inject(tcp))
		tamper_with_syscall_entering(tcp, sig);

	if (cflag == CFLAG_ONLY_STATS) {
		return 0;
	}

#ifdef ENABLE_STACKTRACE
	if (stack_trace_enabled &&
	    !check_exec_syscall(tcp) &&
	    tcp_sysent(tcp)->sys_flags & STACKTRACE_CAPTURE_ON_ENTER) {
		unwind_tcb_capture(tcp);
	}
#endif

	if (!is_complete_set(status_set, NUMBER_OF_STATUSES))
		strace_open_memstream(tcp);

	printleader(tcp);
	tprintf("%s(", tcp_sysent(tcp)->sys_name);
	int res = raw(tcp) ? printargs(tcp) : tcp_sysent(tcp)->sys_func(tcp);
	fflush(tcp->outf);
	return res;
}

void
syscall_entering_finish(struct tcb *tcp, int res)
{
	tcp->flags |= TCB_INSYSCALL;
	tcp->sys_func_rval = res;

	/* Measure the entrance time as late as possible to avoid errors. */
	if ((Tflag || cflag) && !filtered(tcp))
		clock_gettime(CLOCK_MONOTONIC, &tcp->etime);

	/* Start tracking system time */
	if (cflag) {
		if (debug_flag) {
			struct timespec dt;

			ts_sub(&dt, &tcp->stime, &tcp->ltime);

			if (ts_nz(&dt))
				debug_func_msg("pid %d: %.9f seconds of system "
					       "time spent since the last "
					       "syscall exit",
					       tcp->pid, ts_float(&dt));
		}

		tcp->ltime = tcp->stime;
	}
}

/* Returns:
 * 0: "bail out".
 * 1: ok.
 * -1: error in one of ptrace ops.
 *
 * If not 0, call syscall_exiting_trace(tcp, res), where res is the return
 *    value. Anyway, call syscall_exiting_finish(tcp) then.
 */
int
syscall_exiting_decode(struct tcb *tcp, struct timespec *pts)
{
	/* Measure the exit time as early as possible to avoid errors. */
	if ((Tflag || cflag) && !filtered(tcp))
		clock_gettime(CLOCK_MONOTONIC, pts);

	if (tcp_sysent(tcp)->sys_flags & MEMORY_MAPPING_CHANGE)
		mmap_notify_report(tcp);

	if (filtered(tcp))
		return 0;

	if (check_exec_syscall(tcp)) {
		/* The check failed, hide the log.  */
		tcp->flags |= TCB_HIDE_LOG;
	}

#if SUPPORTED_PERSONALITIES > 1
	update_personality(tcp, tcp->currpers);
#endif

	return get_syscall_result(tcp);
}

void
print_syscall_resume(struct tcb *tcp)
{
	/* If not in -ff mode, and printing_tcp != tcp,
	 * then the log currently does not end with output
	 * of _our syscall entry_, but with something else.
	 * We need to say which syscall's return is this.
	 *
	 * Forced reprinting via TCB_REPRINT is used only by
	 * "strace -ff -oLOG test/threaded_execve" corner case.
	 * It's the only case when -ff mode needs reprinting.
	 */
	if ((!output_separately && printing_tcp != tcp && !tcp->staged_output_data)
	    || (tcp->flags & TCB_REPRINT)) {
		tcp->flags &= ~TCB_REPRINT;
		printleader(tcp);
		tprintf("<... %s resumed>", tcp_sysent(tcp)->sys_name);
	}
}

int
syscall_exiting_trace(struct tcb *tcp, struct timespec *ts, int res)
{
	if (syscall_tampered(tcp) || inject_delay_exit(tcp))
		tamper_with_syscall_exiting(tcp);

	if (cflag) {
		count_syscall(tcp, ts);
		if (cflag == CFLAG_ONLY_STATS) {
			return 0;
		}
	}

	print_syscall_resume(tcp);
	printing_tcp = tcp;

	tcp->s_prev_ent = NULL;
	if (res != 1) {
		/* There was error in one of prior ptrace ops */
		tprints(") ");
		tabto();
		tprints("= ? <unavailable>\n");
		if (!is_complete_set(status_set, NUMBER_OF_STATUSES)) {
			bool publish = is_number_in_set(STATUS_UNAVAILABLE,
							status_set);
			strace_close_memstream(tcp, publish);
		}
		line_ended();
		return res;
	}
	tcp->s_prev_ent = tcp->s_ent;

	int sys_res = 0;
	if (raw(tcp)) {
		/* sys_res = printargs(tcp); - but it's nop on sysexit */
	} else {
		if (tcp->sys_func_rval & RVAL_DECODED)
			sys_res = tcp->sys_func_rval;
		else
			sys_res = tcp_sysent(tcp)->sys_func(tcp);
	}

	if (!is_complete_set(status_set, NUMBER_OF_STATUSES)) {
		bool publish = syserror(tcp)
			       && is_number_in_set(STATUS_FAILED, status_set);
		publish |= !syserror(tcp)
			   && is_number_in_set(STATUS_SUCCESSFUL, status_set);
		strace_close_memstream(tcp, publish);
		if (!publish) {
			line_ended();
			return 0;
		}
	}

	tprints(") ");
	tabto();

	if (raw(tcp)) {
		if (tcp->u_error)
			print_err_ret(tcp->u_rval, tcp->u_error);
		else
			tprintf("= %#" PRI_klx, tcp->u_rval);

		if (syscall_tampered(tcp))
			tprints(" (INJECTED)");
	} else if (!(sys_res & RVAL_NONE) && tcp->u_error) {
		switch (tcp->u_error) {
		/* Blocked signals do not interrupt any syscalls.
		 * In this case syscalls don't return ERESTARTfoo codes.
		 *
		 * Deadly signals set to SIG_DFL interrupt syscalls
		 * and kill the process regardless of which of the codes below
		 * is returned by the interrupted syscall.
		 * In some cases, kernel forces a kernel-generated deadly
		 * signal to be unblocked and set to SIG_DFL (and thus cause
		 * death) if it is blocked or SIG_IGNed: for example, SIGSEGV
		 * or SIGILL. (The alternative is to leave process spinning
		 * forever on the faulty instruction - not useful).
		 *
		 * SIG_IGNed signals and non-deadly signals set to SIG_DFL
		 * (for example, SIGCHLD, SIGWINCH) interrupt syscalls,
		 * but kernel will always restart them.
		 */
		case ERESTARTSYS:
			/* Most common type of signal-interrupted syscall exit code.
			 * The system call will be restarted with the same arguments
			 * if SA_RESTART is set; otherwise, it will fail with EINTR.
			 */
			tprints("= ? ERESTARTSYS (To be restarted if SA_RESTART is set)");
			break;
		case ERESTARTNOINTR:
			/* Rare. For example, fork() returns this if interrupted.
			 * SA_RESTART is ignored (assumed set): the restart is unconditional.
			 */
			tprints("= ? ERESTARTNOINTR (To be restarted)");
			break;
		case ERESTARTNOHAND:
			/* pause(), rt_sigsuspend() etc use this code.
			 * SA_RESTART is ignored (assumed not set):
			 * syscall won't restart (will return EINTR instead)
			 * even after signal with SA_RESTART set. However,
			 * after SIG_IGN or SIG_DFL signal it will restart
			 * (thus the name "restart only if has no handler").
			 */
			tprints("= ? ERESTARTNOHAND (To be restarted if no handler)");
			break;
		case ERESTART_RESTARTBLOCK:
			/* Syscalls like nanosleep(), poll() which can't be
			 * restarted with their original arguments use this
			 * code. Kernel will execute restart_syscall() instead,
			 * which changes arguments before restarting syscall.
			 * SA_RESTART is ignored (assumed not set) similarly
			 * to ERESTARTNOHAND. (Kernel can't honor SA_RESTART
			 * since restart data is saved in "restart block"
			 * in task struct, and if signal handler uses a syscall
			 * which in turn saves another such restart block,
			 * old data is lost and restart becomes impossible)
			 */
			tprints("= ? ERESTART_RESTARTBLOCK (Interrupted by signal)");
			break;
		default:
			print_err_ret(tcp->u_rval, tcp->u_error);
			break;
		}
		if (syscall_tampered(tcp))
			tprints(" (INJECTED)");
		if ((sys_res & RVAL_STR) && tcp->auxstr)
			tprintf(" (%s)", tcp->auxstr);
	} else {
		if (sys_res & RVAL_NONE)
			tprints("= ?");
		else {
			switch (sys_res & RVAL_MASK) {
			case RVAL_HEX:
#if ANY_WORDSIZE_LESS_THAN_KERNEL_LONG
				if (current_klongsize < sizeof(tcp->u_rval)) {
					tprintf("= %#x",
						(unsigned int) tcp->u_rval);
				} else
#endif
				{
					tprintf("= %#" PRI_klx, tcp->u_rval);
				}
				break;
			case RVAL_OCTAL:
				tprints("= ");
				print_numeric_long_umask(tcp->u_rval);
				break;
			case RVAL_UDECIMAL:
#if ANY_WORDSIZE_LESS_THAN_KERNEL_LONG
				if (current_klongsize < sizeof(tcp->u_rval)) {
					tprintf("= %u",
						(unsigned int) tcp->u_rval);
				} else
#endif
				{
					tprintf("= %" PRI_klu, tcp->u_rval);
				}
				break;
			case RVAL_FD:
				/*
				 * printfd accepts int as fd and it makes
				 * little sense to pass negative fds to it.
				 */
				if ((current_klongsize < sizeof(tcp->u_rval)) ||
				    ((kernel_ulong_t) tcp->u_rval <= INT_MAX)) {
					tprints("= ");
					printfd(tcp, tcp->u_rval);
				} else {
					tprintf("= %" PRI_kld, tcp->u_rval);
				}
				break;
			default:
				error_msg("invalid rval format");
				break;
			}
		}
		if ((sys_res & RVAL_STR) && tcp->auxstr)
			tprintf(" (%s)", tcp->auxstr);
		if (syscall_tampered(tcp))
			tprints(" (INJECTED)");
	}
	if (Tflag) {
		ts_sub(ts, ts, &tcp->etime);
		tprintf(" <%ld", (long) ts->tv_sec);
		if (Tflag_width) {
			tprintf(".%0*ld",
				Tflag_width, (long) ts->tv_nsec / Tflag_scale);
		}
		tprints(">");
	}
	tprints("\n");
	dumpio(tcp);
	line_ended();

#ifdef ENABLE_STACKTRACE
	if (stack_trace_enabled)
		unwind_tcb_print(tcp);
#endif
	return 0;
}

void
syscall_exiting_finish(struct tcb *tcp)
{
	tcp->flags &= ~(TCB_INSYSCALL | TCB_TAMPERED | TCB_INJECT_DELAY_EXIT);
	tcp->sys_func_rval = 0;
	free_tcb_priv_data(tcp);

	if (cflag)
		tcp->ltime = tcp->stime;
}

bool
is_erestart(struct tcb *tcp)
{
	switch (tcp->u_error) {
		case ERESTARTSYS:
		case ERESTARTNOINTR:
		case ERESTARTNOHAND:
		case ERESTART_RESTARTBLOCK:
			return true;
		default:
			return false;
	}
}

static unsigned long saved_u_error;

void
temporarily_clear_syserror(struct tcb *tcp)
{
	saved_u_error = tcp->u_error;
	tcp->u_error = 0;
}

void
restore_cleared_syserror(struct tcb *tcp)
{
	tcp->u_error = saved_u_error;
}

static struct_ptrace_syscall_info ptrace_sci;

static bool
ptrace_syscall_info_is_valid(void)
{
	return ptrace_get_syscall_info_supported &&
	       ptrace_sci.op <= PTRACE_SYSCALL_INFO_SECCOMP;
}

#define XLAT_MACROS_ONLY
#include "xlat/nt_descriptor_types.h"
#undef XLAT_MACROS_ONLY

#define ARCH_MIGHT_USE_SET_REGS 1

#include "arch_regs.c"

#if HAVE_ARCH_GETRVAL2
# include "arch_getrval2.c"
#endif

#include "getregs_old.h"
#ifdef HAVE_GETREGS_OLD
/* Either getregs_old() or set_regs() */
# undef ARCH_MIGHT_USE_SET_REGS
# define ARCH_MIGHT_USE_SET_REGS 0
#endif

#undef ptrace_getregset_or_getregs
#undef ptrace_setregset_or_setregs
#ifdef ARCH_REGS_FOR_GETREGSET

# define ptrace_getregset_or_getregs ptrace_getregset
static long
ptrace_getregset(pid_t pid)
{
# ifdef ARCH_IOVEC_FOR_GETREGSET
	/* variable iovec */
	ARCH_IOVEC_FOR_GETREGSET.iov_len = sizeof(ARCH_REGS_FOR_GETREGSET);
	return ptrace(PTRACE_GETREGSET, pid, NT_PRSTATUS,
		      &ARCH_IOVEC_FOR_GETREGSET);
# else
	/* constant iovec */
	static struct iovec io = {
		.iov_base = &ARCH_REGS_FOR_GETREGSET,
		.iov_len = sizeof(ARCH_REGS_FOR_GETREGSET)
	};
	return ptrace(PTRACE_GETREGSET, pid, NT_PRSTATUS, &io);

# endif
}

# if ARCH_MIGHT_USE_SET_REGS
#  define ptrace_setregset_or_setregs ptrace_setregset
static int
ptrace_setregset(pid_t pid)
{
#  ifdef ARCH_IOVEC_FOR_GETREGSET
	/* variable iovec */
	return ptrace(PTRACE_SETREGSET, pid, NT_PRSTATUS,
		      &ARCH_IOVEC_FOR_GETREGSET);
#  else
	/* constant iovec */
	static struct iovec io = {
		.iov_base = &ARCH_REGS_FOR_GETREGSET,
		.iov_len = sizeof(ARCH_REGS_FOR_GETREGSET)
	};
	return ptrace(PTRACE_SETREGSET, pid, NT_PRSTATUS, &io);
#  endif
}
# endif /* ARCH_MIGHT_USE_SET_REGS */

#elif defined ARCH_REGS_FOR_GETREGS

# define ptrace_getregset_or_getregs ptrace_getregs
static long
ptrace_getregs(pid_t pid)
{
# if defined SPARC || defined SPARC64
	/* SPARC systems have the meaning of data and addr reversed */
	return ptrace(PTRACE_GETREGS, pid, (void *) &ARCH_REGS_FOR_GETREGS, 0);
# else
	return ptrace(PTRACE_GETREGS, pid, NULL, &ARCH_REGS_FOR_GETREGS);
# endif
}

# if ARCH_MIGHT_USE_SET_REGS
#  define ptrace_setregset_or_setregs ptrace_setregs
static int
ptrace_setregs(pid_t pid)
{
#  if defined SPARC || defined SPARC64
	/* SPARC systems have the meaning of data and addr reversed */
	return ptrace(PTRACE_SETREGS, pid, (void *) &ARCH_REGS_FOR_GETREGS, 0);
#  else
	return ptrace(PTRACE_SETREGS, pid, NULL, &ARCH_REGS_FOR_GETREGS);
#  endif
}
# endif /* ARCH_MIGHT_USE_SET_REGS */

#endif /* ARCH_REGS_FOR_GETREGSET || ARCH_REGS_FOR_GETREGS */

static long get_regs_error = -1;

void
clear_regs(struct tcb *tcp)
{
	ptrace_sci.op = 0xff;
	get_regs_error = -1;
}

static long
get_regs(struct tcb *const tcp)
{
#ifdef ptrace_getregset_or_getregs

	if (get_regs_error != -1)
		return get_regs_error;

# ifdef HAVE_GETREGS_OLD
	/*
	 * Try PTRACE_GETREGSET/PTRACE_GETREGS first,
	 * fallback to getregs_old.
	 */
	static int use_getregs_old;
	if (use_getregs_old < 0) {
		return get_regs_error = ptrace_getregset_or_getregs(tcp->pid);
	} else if (use_getregs_old == 0) {
		get_regs_error = ptrace_getregset_or_getregs(tcp->pid);
		if (get_regs_error >= 0) {
			use_getregs_old = -1;
			return get_regs_error;
		}
		if (errno == EPERM || errno == ESRCH)
			return get_regs_error;
		use_getregs_old = 1;
	}
	return get_regs_error = getregs_old(tcp);
# else /* !HAVE_GETREGS_OLD */
	/* Assume that PTRACE_GETREGSET/PTRACE_GETREGS works. */
	get_regs_error = ptrace_getregset_or_getregs(tcp->pid);

#  if defined ARCH_PERSONALITY_0_IOV_SIZE
	if (get_regs_error)
		return get_regs_error;

	switch (ARCH_IOVEC_FOR_GETREGSET.iov_len) {
	case ARCH_PERSONALITY_0_IOV_SIZE:
		update_personality(tcp, 0);
		break;
	case ARCH_PERSONALITY_1_IOV_SIZE:
		update_personality(tcp, 1);
		break;
	default: {
		static bool printed = false;

		if (!printed) {
			error_msg("Unsupported regset size returned by "
				  "PTRACE_GETREGSET: %zu",
				  ARCH_IOVEC_FOR_GETREGSET.iov_len);

			printed = true;
		}

		update_personality(tcp, 0);
	}
	}
#  endif /* ARCH_PERSONALITY_0_IOV_SIZE */

	return get_regs_error;

# endif /* !HAVE_GETREGS_OLD */

#else /* !ptrace_getregset_or_getregs */

# warning get_regs is not implemented for this architecture yet
	return 0;

#endif /* !ptrace_getregset_or_getregs */
}

#ifdef ptrace_setregset_or_setregs
static int
set_regs(pid_t pid)
{
	return ptrace_setregset_or_setregs(pid);
}
#endif /* ptrace_setregset_or_setregs */

struct sysent_buf {
	struct tcb *tcp;
	struct_sysent ent;
	char buf[sizeof("syscall_0x") + sizeof(kernel_ulong_t) * 2];
};

static void
free_sysent_buf(void *ptr)
{
	struct sysent_buf *s = ptr;
	s->tcp->s_prev_ent = s->tcp->s_ent = NULL;
	free(ptr);
}

static bool
strace_get_syscall_info(struct tcb *tcp)
{
	/*
	 * ptrace_get_syscall_info_supported should have been checked
	 * by the caller.
	 */
	if (ptrace_sci.op == 0xff) {
		const size_t size = sizeof(ptrace_sci);
		if (ptrace(PTRACE_GET_SYSCALL_INFO, tcp->pid,
			   (void *) size, &ptrace_sci) < 0) {
			get_regs_error = -2;
			return false;
		}
#if SUPPORTED_PERSONALITIES > 1
		int newpers = get_personality_from_syscall_info(&ptrace_sci);
		if (newpers >= 0)
			update_personality(tcp, newpers);
#endif
	}

	if (entering(tcp)) {
		if (ptrace_sci.op == PTRACE_SYSCALL_INFO_EXIT) {
			error_msg("pid %d: entering"
				  ", ptrace_syscall_info.op == %u",
				  tcp->pid, ptrace_sci.op);
			/* TODO: handle this.  */
		}
	} else {
		if (ptrace_sci.op == PTRACE_SYSCALL_INFO_ENTRY) {
			error_msg("pid %d: exiting"
				  ", ptrace_syscall_info.op == %u",
				  tcp->pid, ptrace_sci.op);
			/* TODO: handle this.  */
		}
	}

	return true;
}

bool
get_instruction_pointer(struct tcb *tcp, kernel_ulong_t *ip)
{
	if (get_regs_error < -1)
		return false;

	if (ptrace_get_syscall_info_supported) {
		if (!strace_get_syscall_info(tcp))
			return false;
		*ip = (kernel_ulong_t) ptrace_sci.instruction_pointer;
		return true;
	}

#if defined ARCH_PC_REG
	if (get_regs(tcp) < 0)
		return false;
	*ip = (kernel_ulong_t) ARCH_PC_REG;
	return true;
#elif defined ARCH_PC_PEEK_ADDR
	if (upeek(tcp, ARCH_PC_PEEK_ADDR, ip) < 0)
		return false;
	return true;
#else
# error Neither ARCH_PC_REG nor ARCH_PC_PEEK_ADDR is defined
#endif
}

bool
get_stack_pointer(struct tcb *tcp, kernel_ulong_t *sp)
{
	if (get_regs_error < -1)
		return false;

	if (ptrace_get_syscall_info_supported) {
		if (!strace_get_syscall_info(tcp))
			return false;
		*sp = (kernel_ulong_t) ptrace_sci.stack_pointer;
		return true;
	}

#if defined ARCH_SP_REG
	if (get_regs(tcp) < 0)
		return false;
	*sp = (kernel_ulong_t) ARCH_SP_REG;
	return true;
#elif defined ARCH_SP_PEEK_ADDR
	if (upeek(tcp, ARCH_SP_PEEK_ADDR, sp) < 0)
		return false;
	return true;
#else
	return false;
#endif
}

static int
get_syscall_regs(struct tcb *tcp)
{
	if (get_regs_error != -1)
		return get_regs_error;

	if (ptrace_get_syscall_info_supported)
		return strace_get_syscall_info(tcp) ? 0 : get_regs_error;

	return get_regs(tcp);
}

const struct_sysent stub_sysent = {
	.nargs = MAX_ARGS,
	.sys_flags = MEMORY_MAPPING_CHANGE,
	.sen = SEN_printargs,
	.sys_func = printargs,
	.sys_name = "????",
};

/*
 * Returns:
 * 0: "ignore this ptrace stop", syscall_entering_decode() should return a "bail
 *    out silently" code.
 * 1: ok, continue in syscall_entering_decode().
 * other: error, syscall_entering_decode() should print error indicator
 *    ("????" etc) and return an appropriate code.
 */
int
get_scno(struct tcb *tcp)
{
	tcp->scno = -1;
	tcp->s_ent = NULL;
	tcp->qual_flg = QUAL_RAW | DEFAULT_QUAL_FLAGS;

	if (get_syscall_regs(tcp) < 0)
		return -1;

	if (ptrace_syscall_info_is_valid()) {
		/* Apply arch-specific workarounds.  */
		int rc = arch_check_scno(tcp);
		if (rc != 1)
			return rc;
		tcp->scno = ptrace_sci.entry.nr;
	} else {
		int rc = arch_get_scno(tcp);
		if (rc != 1)
			return rc;
	}

	tcp->scno = shuffle_scno(tcp->scno);

	if (scno_is_valid(tcp->scno)) {
		tcp->s_ent = &sysent[tcp->scno];
		tcp->qual_flg = qual_flags(tcp->scno);
	} else {
		struct sysent_buf *s = xzalloc(sizeof(*s));

		s->tcp = tcp;
		s->ent = stub_sysent;
		s->ent.sys_name = s->buf;
		xsprintf(s->buf, "syscall_%#" PRI_klx, shuffle_scno(tcp->scno));

		tcp->s_ent = &s->ent;

		set_tcb_priv_data(tcp, s, free_sysent_buf);

		debug_msg("pid %d invalid syscall %#" PRI_klx,
			  tcp->pid, shuffle_scno(tcp->scno));
	}

	/*
	 * We refrain from argument decoding during recovering
	 * as tracee memory mappings has changed and the registers
	 * are very likely pointing to garbage already.
	 */
	if (recovering(tcp))
		tcp->qual_flg |= QUAL_RAW;

	return 1;
}

static int
get_syscall_args(struct tcb *tcp)
{
	if (ptrace_syscall_info_is_valid()) {
		const unsigned int n =
			MIN(ARRAY_SIZE(tcp->u_arg),
			    ARRAY_SIZE(ptrace_sci.entry.args));
		for (unsigned int i = 0; i < n; ++i)
			tcp->u_arg[i] = ptrace_sci.entry.args[i];
#if SUPPORTED_PERSONALITIES > 1
		if (tcp_sysent(tcp)->sys_flags & COMPAT_SYSCALL_TYPES) {
			for (unsigned int i = 0; i < n; ++i)
				tcp->u_arg[i] = (uint32_t) tcp->u_arg[i];
		}
#endif
		/*
		 * So far it's just a workaround for mips o32,
		 * but let's pretend it could be used elsewhere.
		 */
#if MAX_ARGS > 6
		arch_get_syscall_args_extra(tcp, n);
#endif
		return 1;
	}
	return arch_get_syscall_args(tcp);
}

#ifdef ptrace_getregset_or_getregs
# define get_syscall_result_regs get_syscall_regs
#else
static int get_syscall_result_regs(struct tcb *);
#endif

/* Returns:
 * 1: ok, continue in syscall_exiting_trace().
 * -1: error, syscall_exiting_trace() should print error indicator
 *    ("????" etc) and bail out.
 */
static int
get_syscall_result(struct tcb *tcp)
{
	if (get_syscall_result_regs(tcp) < 0)
		return -1;
	get_error(tcp,
		  (!(tcp_sysent(tcp)->sys_flags & SYSCALL_NEVER_FAILS)
			|| syscall_tampered(tcp))
                  && !syscall_tampered_nofail(tcp));

	return 1;
}

static void
get_error(struct tcb *tcp, const bool check_errno)
{
	if (ptrace_syscall_info_is_valid()) {
		if (ptrace_sci.exit.is_error) {
			tcp->u_rval = -1;
			tcp->u_error = -ptrace_sci.exit.rval;
		} else {
			tcp->u_error = 0;
			tcp->u_rval = ptrace_sci.exit.rval;
		}
	} else {
		tcp->u_error = 0;
		arch_get_error(tcp, check_errno);
	}
}

static void
set_error(struct tcb *tcp, unsigned long new_error)
{
	const unsigned long old_error = tcp->u_error;

	if (new_error == old_error || new_error > MAX_ERRNO_VALUE)
		return;

#ifdef ptrace_setregset_or_setregs
	/* if we are going to invoke set_regs, call get_regs first */
	if (get_regs(tcp) < 0)
		return;
#endif

	tcp->u_error = new_error;
	if (arch_set_error(tcp)) {
		tcp->u_error = old_error;
		/* arch_set_error does not update u_rval */
	} else {
		if (ptrace_syscall_info_is_valid())
			tcp->u_rval = -1;
		else
			get_error(tcp, !(tcp_sysent(tcp)->sys_flags &
					 SYSCALL_NEVER_FAILS));
	}
}

static void
set_success(struct tcb *tcp, kernel_long_t new_rval)
{
	const kernel_long_t old_rval = tcp->u_rval;

#ifdef ptrace_setregset_or_setregs
	/* if we are going to invoke set_regs, call get_regs first */
	if (get_regs(tcp) < 0)
		return;
#endif

	tcp->u_rval = new_rval;
	if (arch_set_success(tcp)) {
		tcp->u_rval = old_rval;
		/* arch_set_success does not update u_error */
	} else {
		if (ptrace_syscall_info_is_valid())
			tcp->u_error = 0;
		else
			get_error(tcp, !(tcp_sysent(tcp)->sys_flags &
					 SYSCALL_NEVER_FAILS));
	}
}

#include "get_scno.c"
#include "check_scno.c"
#include "set_scno.c"
#include "get_syscall_args.c"
#ifndef ptrace_getregset_or_getregs
# include "get_syscall_result.c"
#endif
#include "get_error.c"
#include "set_error.c"
#ifdef HAVE_GETREGS_OLD
# include "getregs_old.c"
#endif
#include "shuffle_scno.c"

const char *
syscall_name(kernel_ulong_t scno)
{
	return scno_is_valid(scno) ? sysent[scno].sys_name : NULL;
}
