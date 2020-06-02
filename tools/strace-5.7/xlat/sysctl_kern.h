/* Generated by ./xlat/gen.sh from ./xlat/sysctl_kern.in; do not edit. */

#include "gcc_compat.h"
#include "static_assert.h"


#ifndef XLAT_MACROS_ONLY

# ifdef IN_MPERS

#  error static const struct xlat sysctl_kern in mpers mode

# else

static const struct xlat_data sysctl_kern_xdata[] = {
 XLAT(KERN_OSTYPE),
 #define XLAT_VAL_0 ((unsigned) (KERN_OSTYPE))
 #define XLAT_STR_0 STRINGIFY(KERN_OSTYPE)
 XLAT(KERN_OSRELEASE),
 #define XLAT_VAL_1 ((unsigned) (KERN_OSRELEASE))
 #define XLAT_STR_1 STRINGIFY(KERN_OSRELEASE)
 XLAT(KERN_OSREV),
 #define XLAT_VAL_2 ((unsigned) (KERN_OSREV))
 #define XLAT_STR_2 STRINGIFY(KERN_OSREV)
 XLAT(KERN_VERSION),
 #define XLAT_VAL_3 ((unsigned) (KERN_VERSION))
 #define XLAT_STR_3 STRINGIFY(KERN_VERSION)
 XLAT(KERN_SECUREMASK),
 #define XLAT_VAL_4 ((unsigned) (KERN_SECUREMASK))
 #define XLAT_STR_4 STRINGIFY(KERN_SECUREMASK)
 XLAT(KERN_PROF),
 #define XLAT_VAL_5 ((unsigned) (KERN_PROF))
 #define XLAT_STR_5 STRINGIFY(KERN_PROF)
 XLAT(KERN_NODENAME),
 #define XLAT_VAL_6 ((unsigned) (KERN_NODENAME))
 #define XLAT_STR_6 STRINGIFY(KERN_NODENAME)
 XLAT(KERN_DOMAINNAME),
 #define XLAT_VAL_7 ((unsigned) (KERN_DOMAINNAME))
 #define XLAT_STR_7 STRINGIFY(KERN_DOMAINNAME)
 XLAT(KERN_PANIC),
 #define XLAT_VAL_8 ((unsigned) (KERN_PANIC))
 #define XLAT_STR_8 STRINGIFY(KERN_PANIC)
 XLAT(KERN_REALROOTDEV),
 #define XLAT_VAL_9 ((unsigned) (KERN_REALROOTDEV))
 #define XLAT_STR_9 STRINGIFY(KERN_REALROOTDEV)
 XLAT(KERN_SPARC_REBOOT),
 #define XLAT_VAL_10 ((unsigned) (KERN_SPARC_REBOOT))
 #define XLAT_STR_10 STRINGIFY(KERN_SPARC_REBOOT)
 XLAT(KERN_CTLALTDEL),
 #define XLAT_VAL_11 ((unsigned) (KERN_CTLALTDEL))
 #define XLAT_STR_11 STRINGIFY(KERN_CTLALTDEL)
 XLAT(KERN_PRINTK),
 #define XLAT_VAL_12 ((unsigned) (KERN_PRINTK))
 #define XLAT_STR_12 STRINGIFY(KERN_PRINTK)
 XLAT(KERN_NAMETRANS),
 #define XLAT_VAL_13 ((unsigned) (KERN_NAMETRANS))
 #define XLAT_STR_13 STRINGIFY(KERN_NAMETRANS)
 XLAT(KERN_PPC_HTABRECLAIM),
 #define XLAT_VAL_14 ((unsigned) (KERN_PPC_HTABRECLAIM))
 #define XLAT_STR_14 STRINGIFY(KERN_PPC_HTABRECLAIM)
 XLAT(KERN_PPC_ZEROPAGED),
 #define XLAT_VAL_15 ((unsigned) (KERN_PPC_ZEROPAGED))
 #define XLAT_STR_15 STRINGIFY(KERN_PPC_ZEROPAGED)
 XLAT(KERN_PPC_POWERSAVE_NAP),
 #define XLAT_VAL_16 ((unsigned) (KERN_PPC_POWERSAVE_NAP))
 #define XLAT_STR_16 STRINGIFY(KERN_PPC_POWERSAVE_NAP)
 XLAT(KERN_MODPROBE),
 #define XLAT_VAL_17 ((unsigned) (KERN_MODPROBE))
 #define XLAT_STR_17 STRINGIFY(KERN_MODPROBE)
 XLAT(KERN_SG_BIG_BUFF),
 #define XLAT_VAL_18 ((unsigned) (KERN_SG_BIG_BUFF))
 #define XLAT_STR_18 STRINGIFY(KERN_SG_BIG_BUFF)
 XLAT(KERN_ACCT),
 #define XLAT_VAL_19 ((unsigned) (KERN_ACCT))
 #define XLAT_STR_19 STRINGIFY(KERN_ACCT)
 XLAT(KERN_PPC_L2CR),
 #define XLAT_VAL_20 ((unsigned) (KERN_PPC_L2CR))
 #define XLAT_STR_20 STRINGIFY(KERN_PPC_L2CR)
 XLAT(KERN_RTSIGNR),
 #define XLAT_VAL_21 ((unsigned) (KERN_RTSIGNR))
 #define XLAT_STR_21 STRINGIFY(KERN_RTSIGNR)
 XLAT(KERN_RTSIGMAX),
 #define XLAT_VAL_22 ((unsigned) (KERN_RTSIGMAX))
 #define XLAT_STR_22 STRINGIFY(KERN_RTSIGMAX)
 XLAT(KERN_SHMMAX),
 #define XLAT_VAL_23 ((unsigned) (KERN_SHMMAX))
 #define XLAT_STR_23 STRINGIFY(KERN_SHMMAX)
 XLAT(KERN_MSGMAX),
 #define XLAT_VAL_24 ((unsigned) (KERN_MSGMAX))
 #define XLAT_STR_24 STRINGIFY(KERN_MSGMAX)
 XLAT(KERN_MSGMNB),
 #define XLAT_VAL_25 ((unsigned) (KERN_MSGMNB))
 #define XLAT_STR_25 STRINGIFY(KERN_MSGMNB)
 XLAT(KERN_MSGPOOL),
 #define XLAT_VAL_26 ((unsigned) (KERN_MSGPOOL))
 #define XLAT_STR_26 STRINGIFY(KERN_MSGPOOL)
 XLAT(KERN_SYSRQ),
 #define XLAT_VAL_27 ((unsigned) (KERN_SYSRQ))
 #define XLAT_STR_27 STRINGIFY(KERN_SYSRQ)
 XLAT(KERN_MAX_THREADS),
 #define XLAT_VAL_28 ((unsigned) (KERN_MAX_THREADS))
 #define XLAT_STR_28 STRINGIFY(KERN_MAX_THREADS)
 XLAT(KERN_RANDOM),
 #define XLAT_VAL_29 ((unsigned) (KERN_RANDOM))
 #define XLAT_STR_29 STRINGIFY(KERN_RANDOM)
 XLAT(KERN_SHMALL),
 #define XLAT_VAL_30 ((unsigned) (KERN_SHMALL))
 #define XLAT_STR_30 STRINGIFY(KERN_SHMALL)
 XLAT(KERN_MSGMNI),
 #define XLAT_VAL_31 ((unsigned) (KERN_MSGMNI))
 #define XLAT_STR_31 STRINGIFY(KERN_MSGMNI)
 XLAT(KERN_SEM),
 #define XLAT_VAL_32 ((unsigned) (KERN_SEM))
 #define XLAT_STR_32 STRINGIFY(KERN_SEM)
 XLAT(KERN_SPARC_STOP_A),
 #define XLAT_VAL_33 ((unsigned) (KERN_SPARC_STOP_A))
 #define XLAT_STR_33 STRINGIFY(KERN_SPARC_STOP_A)
 XLAT(KERN_SHMMNI),
 #define XLAT_VAL_34 ((unsigned) (KERN_SHMMNI))
 #define XLAT_STR_34 STRINGIFY(KERN_SHMMNI)
 XLAT(KERN_OVERFLOWUID),
 #define XLAT_VAL_35 ((unsigned) (KERN_OVERFLOWUID))
 #define XLAT_STR_35 STRINGIFY(KERN_OVERFLOWUID)
 XLAT(KERN_OVERFLOWGID),
 #define XLAT_VAL_36 ((unsigned) (KERN_OVERFLOWGID))
 #define XLAT_STR_36 STRINGIFY(KERN_OVERFLOWGID)
 XLAT(KERN_SHMPATH),
 #define XLAT_VAL_37 ((unsigned) (KERN_SHMPATH))
 #define XLAT_STR_37 STRINGIFY(KERN_SHMPATH)
 XLAT(KERN_HOTPLUG),
 #define XLAT_VAL_38 ((unsigned) (KERN_HOTPLUG))
 #define XLAT_STR_38 STRINGIFY(KERN_HOTPLUG)
 XLAT(KERN_IEEE_EMULATION_WARNINGS),
 #define XLAT_VAL_39 ((unsigned) (KERN_IEEE_EMULATION_WARNINGS))
 #define XLAT_STR_39 STRINGIFY(KERN_IEEE_EMULATION_WARNINGS)
 XLAT(KERN_S390_USER_DEBUG_LOGGING),
 #define XLAT_VAL_40 ((unsigned) (KERN_S390_USER_DEBUG_LOGGING))
 #define XLAT_STR_40 STRINGIFY(KERN_S390_USER_DEBUG_LOGGING)
 XLAT(KERN_CORE_USES_PID),
 #define XLAT_VAL_41 ((unsigned) (KERN_CORE_USES_PID))
 #define XLAT_STR_41 STRINGIFY(KERN_CORE_USES_PID)
 XLAT(KERN_TAINTED),
 #define XLAT_VAL_42 ((unsigned) (KERN_TAINTED))
 #define XLAT_STR_42 STRINGIFY(KERN_TAINTED)
 XLAT(KERN_CADPID),
 #define XLAT_VAL_43 ((unsigned) (KERN_CADPID))
 #define XLAT_STR_43 STRINGIFY(KERN_CADPID)
 XLAT(KERN_PIDMAX),
 #define XLAT_VAL_44 ((unsigned) (KERN_PIDMAX))
 #define XLAT_STR_44 STRINGIFY(KERN_PIDMAX)
 XLAT(KERN_CORE_PATTERN),
 #define XLAT_VAL_45 ((unsigned) (KERN_CORE_PATTERN))
 #define XLAT_STR_45 STRINGIFY(KERN_CORE_PATTERN)
 XLAT(KERN_PANIC_ON_OOPS),
 #define XLAT_VAL_46 ((unsigned) (KERN_PANIC_ON_OOPS))
 #define XLAT_STR_46 STRINGIFY(KERN_PANIC_ON_OOPS)
 XLAT(KERN_HPPA_PWRSW),
 #define XLAT_VAL_47 ((unsigned) (KERN_HPPA_PWRSW))
 #define XLAT_STR_47 STRINGIFY(KERN_HPPA_PWRSW)
 XLAT(KERN_HPPA_UNALIGNED),
 #define XLAT_VAL_48 ((unsigned) (KERN_HPPA_UNALIGNED))
 #define XLAT_STR_48 STRINGIFY(KERN_HPPA_UNALIGNED)
#if defined(KERN_PRINTK_RATELIMIT) || (defined(HAVE_DECL_KERN_PRINTK_RATELIMIT) && HAVE_DECL_KERN_PRINTK_RATELIMIT)
  XLAT(KERN_PRINTK_RATELIMIT),
 #define XLAT_VAL_49 ((unsigned) (KERN_PRINTK_RATELIMIT))
 #define XLAT_STR_49 STRINGIFY(KERN_PRINTK_RATELIMIT)
#endif
#if defined(KERN_PRINTK_RATELIMIT_BURST) || (defined(HAVE_DECL_KERN_PRINTK_RATELIMIT_BURST) && HAVE_DECL_KERN_PRINTK_RATELIMIT_BURST)
  XLAT(KERN_PRINTK_RATELIMIT_BURST),
 #define XLAT_VAL_50 ((unsigned) (KERN_PRINTK_RATELIMIT_BURST))
 #define XLAT_STR_50 STRINGIFY(KERN_PRINTK_RATELIMIT_BURST)
#endif
#if defined(KERN_PTY) || (defined(HAVE_DECL_KERN_PTY) && HAVE_DECL_KERN_PTY)
  XLAT(KERN_PTY),
 #define XLAT_VAL_51 ((unsigned) (KERN_PTY))
 #define XLAT_STR_51 STRINGIFY(KERN_PTY)
#endif
#if defined(KERN_NGROUPS_MAX) || (defined(HAVE_DECL_KERN_NGROUPS_MAX) && HAVE_DECL_KERN_NGROUPS_MAX)
  XLAT(KERN_NGROUPS_MAX),
 #define XLAT_VAL_52 ((unsigned) (KERN_NGROUPS_MAX))
 #define XLAT_STR_52 STRINGIFY(KERN_NGROUPS_MAX)
#endif
#if defined(KERN_SPARC_SCONS_PWROFF) || (defined(HAVE_DECL_KERN_SPARC_SCONS_PWROFF) && HAVE_DECL_KERN_SPARC_SCONS_PWROFF)
  XLAT(KERN_SPARC_SCONS_PWROFF),
 #define XLAT_VAL_53 ((unsigned) (KERN_SPARC_SCONS_PWROFF))
 #define XLAT_STR_53 STRINGIFY(KERN_SPARC_SCONS_PWROFF)
#endif
#if defined(KERN_HZ_TIMER) || (defined(HAVE_DECL_KERN_HZ_TIMER) && HAVE_DECL_KERN_HZ_TIMER)
  XLAT(KERN_HZ_TIMER),
 #define XLAT_VAL_54 ((unsigned) (KERN_HZ_TIMER))
 #define XLAT_STR_54 STRINGIFY(KERN_HZ_TIMER)
#endif
#if defined(KERN_UNKNOWN_NMI_PANIC) || (defined(HAVE_DECL_KERN_UNKNOWN_NMI_PANIC) && HAVE_DECL_KERN_UNKNOWN_NMI_PANIC)
  XLAT(KERN_UNKNOWN_NMI_PANIC),
 #define XLAT_VAL_55 ((unsigned) (KERN_UNKNOWN_NMI_PANIC))
 #define XLAT_STR_55 STRINGIFY(KERN_UNKNOWN_NMI_PANIC)
#endif
#if defined(KERN_BOOTLOADER_TYPE) || (defined(HAVE_DECL_KERN_BOOTLOADER_TYPE) && HAVE_DECL_KERN_BOOTLOADER_TYPE)
  XLAT(KERN_BOOTLOADER_TYPE),
 #define XLAT_VAL_56 ((unsigned) (KERN_BOOTLOADER_TYPE))
 #define XLAT_STR_56 STRINGIFY(KERN_BOOTLOADER_TYPE)
#endif
#if defined(KERN_RANDOMIZE) || (defined(HAVE_DECL_KERN_RANDOMIZE) && HAVE_DECL_KERN_RANDOMIZE)
  XLAT(KERN_RANDOMIZE),
 #define XLAT_VAL_57 ((unsigned) (KERN_RANDOMIZE))
 #define XLAT_STR_57 STRINGIFY(KERN_RANDOMIZE)
#endif
#if defined(KERN_SETUID_DUMPABLE) || (defined(HAVE_DECL_KERN_SETUID_DUMPABLE) && HAVE_DECL_KERN_SETUID_DUMPABLE)
  XLAT(KERN_SETUID_DUMPABLE),
 #define XLAT_VAL_58 ((unsigned) (KERN_SETUID_DUMPABLE))
 #define XLAT_STR_58 STRINGIFY(KERN_SETUID_DUMPABLE)
#endif
#if defined(KERN_SPIN_RETRY) || (defined(HAVE_DECL_KERN_SPIN_RETRY) && HAVE_DECL_KERN_SPIN_RETRY)
  XLAT(KERN_SPIN_RETRY),
 #define XLAT_VAL_59 ((unsigned) (KERN_SPIN_RETRY))
 #define XLAT_STR_59 STRINGIFY(KERN_SPIN_RETRY)
#endif
#if defined(KERN_ACPI_VIDEO_FLAGS) || (defined(HAVE_DECL_KERN_ACPI_VIDEO_FLAGS) && HAVE_DECL_KERN_ACPI_VIDEO_FLAGS)
  XLAT(KERN_ACPI_VIDEO_FLAGS),
 #define XLAT_VAL_60 ((unsigned) (KERN_ACPI_VIDEO_FLAGS))
 #define XLAT_STR_60 STRINGIFY(KERN_ACPI_VIDEO_FLAGS)
#endif
#if defined(KERN_IA64_UNALIGNED) || (defined(HAVE_DECL_KERN_IA64_UNALIGNED) && HAVE_DECL_KERN_IA64_UNALIGNED)
  XLAT(KERN_IA64_UNALIGNED),
 #define XLAT_VAL_61 ((unsigned) (KERN_IA64_UNALIGNED))
 #define XLAT_STR_61 STRINGIFY(KERN_IA64_UNALIGNED)
#endif
#if defined(KERN_COMPAT_LOG) || (defined(HAVE_DECL_KERN_COMPAT_LOG) && HAVE_DECL_KERN_COMPAT_LOG)
  XLAT(KERN_COMPAT_LOG),
 #define XLAT_VAL_62 ((unsigned) (KERN_COMPAT_LOG))
 #define XLAT_STR_62 STRINGIFY(KERN_COMPAT_LOG)
#endif
#if defined(KERN_MAX_LOCK_DEPTH) || (defined(HAVE_DECL_KERN_MAX_LOCK_DEPTH) && HAVE_DECL_KERN_MAX_LOCK_DEPTH)
  XLAT(KERN_MAX_LOCK_DEPTH),
 #define XLAT_VAL_63 ((unsigned) (KERN_MAX_LOCK_DEPTH))
 #define XLAT_STR_63 STRINGIFY(KERN_MAX_LOCK_DEPTH)
#endif
#if defined(KERN_NMI_WATCHDOG) || (defined(HAVE_DECL_KERN_NMI_WATCHDOG) && HAVE_DECL_KERN_NMI_WATCHDOG)
  XLAT(KERN_NMI_WATCHDOG),
 #define XLAT_VAL_64 ((unsigned) (KERN_NMI_WATCHDOG))
 #define XLAT_STR_64 STRINGIFY(KERN_NMI_WATCHDOG)
#endif
#if defined(KERN_PANIC_ON_NMI) || (defined(HAVE_DECL_KERN_PANIC_ON_NMI) && HAVE_DECL_KERN_PANIC_ON_NMI)
  XLAT(KERN_PANIC_ON_NMI),
 #define XLAT_VAL_65 ((unsigned) (KERN_PANIC_ON_NMI))
 #define XLAT_STR_65 STRINGIFY(KERN_PANIC_ON_NMI)
#endif
#if defined(KERN_PANIC_ON_WARN) || (defined(HAVE_DECL_KERN_PANIC_ON_WARN) && HAVE_DECL_KERN_PANIC_ON_WARN)
  XLAT(KERN_PANIC_ON_WARN),
 #define XLAT_VAL_66 ((unsigned) (KERN_PANIC_ON_WARN))
 #define XLAT_STR_66 STRINGIFY(KERN_PANIC_ON_WARN)
#endif
#if defined(KERN_PANIC_PRINT) || (defined(HAVE_DECL_KERN_PANIC_PRINT) && HAVE_DECL_KERN_PANIC_PRINT)
  XLAT(KERN_PANIC_PRINT),
 #define XLAT_VAL_67 ((unsigned) (KERN_PANIC_PRINT))
 #define XLAT_STR_67 STRINGIFY(KERN_PANIC_PRINT)
#endif
};
static
const struct xlat sysctl_kern[1] = { {
 .data = sysctl_kern_xdata,
 .size = ARRAY_SIZE(sysctl_kern_xdata),
 .type = XT_NORMAL,
 .flags_mask = 0
#  ifdef XLAT_VAL_0
  | XLAT_VAL_0
#  endif
#  ifdef XLAT_VAL_1
  | XLAT_VAL_1
#  endif
#  ifdef XLAT_VAL_2
  | XLAT_VAL_2
#  endif
#  ifdef XLAT_VAL_3
  | XLAT_VAL_3
#  endif
#  ifdef XLAT_VAL_4
  | XLAT_VAL_4
#  endif
#  ifdef XLAT_VAL_5
  | XLAT_VAL_5
#  endif
#  ifdef XLAT_VAL_6
  | XLAT_VAL_6
#  endif
#  ifdef XLAT_VAL_7
  | XLAT_VAL_7
#  endif
#  ifdef XLAT_VAL_8
  | XLAT_VAL_8
#  endif
#  ifdef XLAT_VAL_9
  | XLAT_VAL_9
#  endif
#  ifdef XLAT_VAL_10
  | XLAT_VAL_10
#  endif
#  ifdef XLAT_VAL_11
  | XLAT_VAL_11
#  endif
#  ifdef XLAT_VAL_12
  | XLAT_VAL_12
#  endif
#  ifdef XLAT_VAL_13
  | XLAT_VAL_13
#  endif
#  ifdef XLAT_VAL_14
  | XLAT_VAL_14
#  endif
#  ifdef XLAT_VAL_15
  | XLAT_VAL_15
#  endif
#  ifdef XLAT_VAL_16
  | XLAT_VAL_16
#  endif
#  ifdef XLAT_VAL_17
  | XLAT_VAL_17
#  endif
#  ifdef XLAT_VAL_18
  | XLAT_VAL_18
#  endif
#  ifdef XLAT_VAL_19
  | XLAT_VAL_19
#  endif
#  ifdef XLAT_VAL_20
  | XLAT_VAL_20
#  endif
#  ifdef XLAT_VAL_21
  | XLAT_VAL_21
#  endif
#  ifdef XLAT_VAL_22
  | XLAT_VAL_22
#  endif
#  ifdef XLAT_VAL_23
  | XLAT_VAL_23
#  endif
#  ifdef XLAT_VAL_24
  | XLAT_VAL_24
#  endif
#  ifdef XLAT_VAL_25
  | XLAT_VAL_25
#  endif
#  ifdef XLAT_VAL_26
  | XLAT_VAL_26
#  endif
#  ifdef XLAT_VAL_27
  | XLAT_VAL_27
#  endif
#  ifdef XLAT_VAL_28
  | XLAT_VAL_28
#  endif
#  ifdef XLAT_VAL_29
  | XLAT_VAL_29
#  endif
#  ifdef XLAT_VAL_30
  | XLAT_VAL_30
#  endif
#  ifdef XLAT_VAL_31
  | XLAT_VAL_31
#  endif
#  ifdef XLAT_VAL_32
  | XLAT_VAL_32
#  endif
#  ifdef XLAT_VAL_33
  | XLAT_VAL_33
#  endif
#  ifdef XLAT_VAL_34
  | XLAT_VAL_34
#  endif
#  ifdef XLAT_VAL_35
  | XLAT_VAL_35
#  endif
#  ifdef XLAT_VAL_36
  | XLAT_VAL_36
#  endif
#  ifdef XLAT_VAL_37
  | XLAT_VAL_37
#  endif
#  ifdef XLAT_VAL_38
  | XLAT_VAL_38
#  endif
#  ifdef XLAT_VAL_39
  | XLAT_VAL_39
#  endif
#  ifdef XLAT_VAL_40
  | XLAT_VAL_40
#  endif
#  ifdef XLAT_VAL_41
  | XLAT_VAL_41
#  endif
#  ifdef XLAT_VAL_42
  | XLAT_VAL_42
#  endif
#  ifdef XLAT_VAL_43
  | XLAT_VAL_43
#  endif
#  ifdef XLAT_VAL_44
  | XLAT_VAL_44
#  endif
#  ifdef XLAT_VAL_45
  | XLAT_VAL_45
#  endif
#  ifdef XLAT_VAL_46
  | XLAT_VAL_46
#  endif
#  ifdef XLAT_VAL_47
  | XLAT_VAL_47
#  endif
#  ifdef XLAT_VAL_48
  | XLAT_VAL_48
#  endif
#  ifdef XLAT_VAL_49
  | XLAT_VAL_49
#  endif
#  ifdef XLAT_VAL_50
  | XLAT_VAL_50
#  endif
#  ifdef XLAT_VAL_51
  | XLAT_VAL_51
#  endif
#  ifdef XLAT_VAL_52
  | XLAT_VAL_52
#  endif
#  ifdef XLAT_VAL_53
  | XLAT_VAL_53
#  endif
#  ifdef XLAT_VAL_54
  | XLAT_VAL_54
#  endif
#  ifdef XLAT_VAL_55
  | XLAT_VAL_55
#  endif
#  ifdef XLAT_VAL_56
  | XLAT_VAL_56
#  endif
#  ifdef XLAT_VAL_57
  | XLAT_VAL_57
#  endif
#  ifdef XLAT_VAL_58
  | XLAT_VAL_58
#  endif
#  ifdef XLAT_VAL_59
  | XLAT_VAL_59
#  endif
#  ifdef XLAT_VAL_60
  | XLAT_VAL_60
#  endif
#  ifdef XLAT_VAL_61
  | XLAT_VAL_61
#  endif
#  ifdef XLAT_VAL_62
  | XLAT_VAL_62
#  endif
#  ifdef XLAT_VAL_63
  | XLAT_VAL_63
#  endif
#  ifdef XLAT_VAL_64
  | XLAT_VAL_64
#  endif
#  ifdef XLAT_VAL_65
  | XLAT_VAL_65
#  endif
#  ifdef XLAT_VAL_66
  | XLAT_VAL_66
#  endif
#  ifdef XLAT_VAL_67
  | XLAT_VAL_67
#  endif
  ,
 .flags_strsz = 0
#  ifdef XLAT_STR_0
  + sizeof(XLAT_STR_0)
#  endif
#  ifdef XLAT_STR_1
  + sizeof(XLAT_STR_1)
#  endif
#  ifdef XLAT_STR_2
  + sizeof(XLAT_STR_2)
#  endif
#  ifdef XLAT_STR_3
  + sizeof(XLAT_STR_3)
#  endif
#  ifdef XLAT_STR_4
  + sizeof(XLAT_STR_4)
#  endif
#  ifdef XLAT_STR_5
  + sizeof(XLAT_STR_5)
#  endif
#  ifdef XLAT_STR_6
  + sizeof(XLAT_STR_6)
#  endif
#  ifdef XLAT_STR_7
  + sizeof(XLAT_STR_7)
#  endif
#  ifdef XLAT_STR_8
  + sizeof(XLAT_STR_8)
#  endif
#  ifdef XLAT_STR_9
  + sizeof(XLAT_STR_9)
#  endif
#  ifdef XLAT_STR_10
  + sizeof(XLAT_STR_10)
#  endif
#  ifdef XLAT_STR_11
  + sizeof(XLAT_STR_11)
#  endif
#  ifdef XLAT_STR_12
  + sizeof(XLAT_STR_12)
#  endif
#  ifdef XLAT_STR_13
  + sizeof(XLAT_STR_13)
#  endif
#  ifdef XLAT_STR_14
  + sizeof(XLAT_STR_14)
#  endif
#  ifdef XLAT_STR_15
  + sizeof(XLAT_STR_15)
#  endif
#  ifdef XLAT_STR_16
  + sizeof(XLAT_STR_16)
#  endif
#  ifdef XLAT_STR_17
  + sizeof(XLAT_STR_17)
#  endif
#  ifdef XLAT_STR_18
  + sizeof(XLAT_STR_18)
#  endif
#  ifdef XLAT_STR_19
  + sizeof(XLAT_STR_19)
#  endif
#  ifdef XLAT_STR_20
  + sizeof(XLAT_STR_20)
#  endif
#  ifdef XLAT_STR_21
  + sizeof(XLAT_STR_21)
#  endif
#  ifdef XLAT_STR_22
  + sizeof(XLAT_STR_22)
#  endif
#  ifdef XLAT_STR_23
  + sizeof(XLAT_STR_23)
#  endif
#  ifdef XLAT_STR_24
  + sizeof(XLAT_STR_24)
#  endif
#  ifdef XLAT_STR_25
  + sizeof(XLAT_STR_25)
#  endif
#  ifdef XLAT_STR_26
  + sizeof(XLAT_STR_26)
#  endif
#  ifdef XLAT_STR_27
  + sizeof(XLAT_STR_27)
#  endif
#  ifdef XLAT_STR_28
  + sizeof(XLAT_STR_28)
#  endif
#  ifdef XLAT_STR_29
  + sizeof(XLAT_STR_29)
#  endif
#  ifdef XLAT_STR_30
  + sizeof(XLAT_STR_30)
#  endif
#  ifdef XLAT_STR_31
  + sizeof(XLAT_STR_31)
#  endif
#  ifdef XLAT_STR_32
  + sizeof(XLAT_STR_32)
#  endif
#  ifdef XLAT_STR_33
  + sizeof(XLAT_STR_33)
#  endif
#  ifdef XLAT_STR_34
  + sizeof(XLAT_STR_34)
#  endif
#  ifdef XLAT_STR_35
  + sizeof(XLAT_STR_35)
#  endif
#  ifdef XLAT_STR_36
  + sizeof(XLAT_STR_36)
#  endif
#  ifdef XLAT_STR_37
  + sizeof(XLAT_STR_37)
#  endif
#  ifdef XLAT_STR_38
  + sizeof(XLAT_STR_38)
#  endif
#  ifdef XLAT_STR_39
  + sizeof(XLAT_STR_39)
#  endif
#  ifdef XLAT_STR_40
  + sizeof(XLAT_STR_40)
#  endif
#  ifdef XLAT_STR_41
  + sizeof(XLAT_STR_41)
#  endif
#  ifdef XLAT_STR_42
  + sizeof(XLAT_STR_42)
#  endif
#  ifdef XLAT_STR_43
  + sizeof(XLAT_STR_43)
#  endif
#  ifdef XLAT_STR_44
  + sizeof(XLAT_STR_44)
#  endif
#  ifdef XLAT_STR_45
  + sizeof(XLAT_STR_45)
#  endif
#  ifdef XLAT_STR_46
  + sizeof(XLAT_STR_46)
#  endif
#  ifdef XLAT_STR_47
  + sizeof(XLAT_STR_47)
#  endif
#  ifdef XLAT_STR_48
  + sizeof(XLAT_STR_48)
#  endif
#  ifdef XLAT_STR_49
  + sizeof(XLAT_STR_49)
#  endif
#  ifdef XLAT_STR_50
  + sizeof(XLAT_STR_50)
#  endif
#  ifdef XLAT_STR_51
  + sizeof(XLAT_STR_51)
#  endif
#  ifdef XLAT_STR_52
  + sizeof(XLAT_STR_52)
#  endif
#  ifdef XLAT_STR_53
  + sizeof(XLAT_STR_53)
#  endif
#  ifdef XLAT_STR_54
  + sizeof(XLAT_STR_54)
#  endif
#  ifdef XLAT_STR_55
  + sizeof(XLAT_STR_55)
#  endif
#  ifdef XLAT_STR_56
  + sizeof(XLAT_STR_56)
#  endif
#  ifdef XLAT_STR_57
  + sizeof(XLAT_STR_57)
#  endif
#  ifdef XLAT_STR_58
  + sizeof(XLAT_STR_58)
#  endif
#  ifdef XLAT_STR_59
  + sizeof(XLAT_STR_59)
#  endif
#  ifdef XLAT_STR_60
  + sizeof(XLAT_STR_60)
#  endif
#  ifdef XLAT_STR_61
  + sizeof(XLAT_STR_61)
#  endif
#  ifdef XLAT_STR_62
  + sizeof(XLAT_STR_62)
#  endif
#  ifdef XLAT_STR_63
  + sizeof(XLAT_STR_63)
#  endif
#  ifdef XLAT_STR_64
  + sizeof(XLAT_STR_64)
#  endif
#  ifdef XLAT_STR_65
  + sizeof(XLAT_STR_65)
#  endif
#  ifdef XLAT_STR_66
  + sizeof(XLAT_STR_66)
#  endif
#  ifdef XLAT_STR_67
  + sizeof(XLAT_STR_67)
#  endif
  ,
} };

#  undef XLAT_STR_0
#  undef XLAT_VAL_0
#  undef XLAT_STR_1
#  undef XLAT_VAL_1
#  undef XLAT_STR_2
#  undef XLAT_VAL_2
#  undef XLAT_STR_3
#  undef XLAT_VAL_3
#  undef XLAT_STR_4
#  undef XLAT_VAL_4
#  undef XLAT_STR_5
#  undef XLAT_VAL_5
#  undef XLAT_STR_6
#  undef XLAT_VAL_6
#  undef XLAT_STR_7
#  undef XLAT_VAL_7
#  undef XLAT_STR_8
#  undef XLAT_VAL_8
#  undef XLAT_STR_9
#  undef XLAT_VAL_9
#  undef XLAT_STR_10
#  undef XLAT_VAL_10
#  undef XLAT_STR_11
#  undef XLAT_VAL_11
#  undef XLAT_STR_12
#  undef XLAT_VAL_12
#  undef XLAT_STR_13
#  undef XLAT_VAL_13
#  undef XLAT_STR_14
#  undef XLAT_VAL_14
#  undef XLAT_STR_15
#  undef XLAT_VAL_15
#  undef XLAT_STR_16
#  undef XLAT_VAL_16
#  undef XLAT_STR_17
#  undef XLAT_VAL_17
#  undef XLAT_STR_18
#  undef XLAT_VAL_18
#  undef XLAT_STR_19
#  undef XLAT_VAL_19
#  undef XLAT_STR_20
#  undef XLAT_VAL_20
#  undef XLAT_STR_21
#  undef XLAT_VAL_21
#  undef XLAT_STR_22
#  undef XLAT_VAL_22
#  undef XLAT_STR_23
#  undef XLAT_VAL_23
#  undef XLAT_STR_24
#  undef XLAT_VAL_24
#  undef XLAT_STR_25
#  undef XLAT_VAL_25
#  undef XLAT_STR_26
#  undef XLAT_VAL_26
#  undef XLAT_STR_27
#  undef XLAT_VAL_27
#  undef XLAT_STR_28
#  undef XLAT_VAL_28
#  undef XLAT_STR_29
#  undef XLAT_VAL_29
#  undef XLAT_STR_30
#  undef XLAT_VAL_30
#  undef XLAT_STR_31
#  undef XLAT_VAL_31
#  undef XLAT_STR_32
#  undef XLAT_VAL_32
#  undef XLAT_STR_33
#  undef XLAT_VAL_33
#  undef XLAT_STR_34
#  undef XLAT_VAL_34
#  undef XLAT_STR_35
#  undef XLAT_VAL_35
#  undef XLAT_STR_36
#  undef XLAT_VAL_36
#  undef XLAT_STR_37
#  undef XLAT_VAL_37
#  undef XLAT_STR_38
#  undef XLAT_VAL_38
#  undef XLAT_STR_39
#  undef XLAT_VAL_39
#  undef XLAT_STR_40
#  undef XLAT_VAL_40
#  undef XLAT_STR_41
#  undef XLAT_VAL_41
#  undef XLAT_STR_42
#  undef XLAT_VAL_42
#  undef XLAT_STR_43
#  undef XLAT_VAL_43
#  undef XLAT_STR_44
#  undef XLAT_VAL_44
#  undef XLAT_STR_45
#  undef XLAT_VAL_45
#  undef XLAT_STR_46
#  undef XLAT_VAL_46
#  undef XLAT_STR_47
#  undef XLAT_VAL_47
#  undef XLAT_STR_48
#  undef XLAT_VAL_48
#  undef XLAT_STR_49
#  undef XLAT_VAL_49
#  undef XLAT_STR_50
#  undef XLAT_VAL_50
#  undef XLAT_STR_51
#  undef XLAT_VAL_51
#  undef XLAT_STR_52
#  undef XLAT_VAL_52
#  undef XLAT_STR_53
#  undef XLAT_VAL_53
#  undef XLAT_STR_54
#  undef XLAT_VAL_54
#  undef XLAT_STR_55
#  undef XLAT_VAL_55
#  undef XLAT_STR_56
#  undef XLAT_VAL_56
#  undef XLAT_STR_57
#  undef XLAT_VAL_57
#  undef XLAT_STR_58
#  undef XLAT_VAL_58
#  undef XLAT_STR_59
#  undef XLAT_VAL_59
#  undef XLAT_STR_60
#  undef XLAT_VAL_60
#  undef XLAT_STR_61
#  undef XLAT_VAL_61
#  undef XLAT_STR_62
#  undef XLAT_VAL_62
#  undef XLAT_STR_63
#  undef XLAT_VAL_63
#  undef XLAT_STR_64
#  undef XLAT_VAL_64
#  undef XLAT_STR_65
#  undef XLAT_VAL_65
#  undef XLAT_STR_66
#  undef XLAT_VAL_66
#  undef XLAT_STR_67
#  undef XLAT_VAL_67
# endif /* !IN_MPERS */

#endif /* !XLAT_MACROS_ONLY */
