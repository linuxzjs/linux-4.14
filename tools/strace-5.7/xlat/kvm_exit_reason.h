/* Generated by ./xlat/gen.sh from ./xlat/kvm_exit_reason.in; do not edit. */

#include "gcc_compat.h"
#include "static_assert.h"

#if defined(KVM_EXIT_UNKNOWN) || (defined(HAVE_DECL_KVM_EXIT_UNKNOWN) && HAVE_DECL_KVM_EXIT_UNKNOWN)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((KVM_EXIT_UNKNOWN) == (0), "KVM_EXIT_UNKNOWN != 0");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define KVM_EXIT_UNKNOWN 0
#endif
#if defined(KVM_EXIT_EXCEPTION) || (defined(HAVE_DECL_KVM_EXIT_EXCEPTION) && HAVE_DECL_KVM_EXIT_EXCEPTION)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((KVM_EXIT_EXCEPTION) == (1), "KVM_EXIT_EXCEPTION != 1");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define KVM_EXIT_EXCEPTION 1
#endif
#if defined(KVM_EXIT_IO) || (defined(HAVE_DECL_KVM_EXIT_IO) && HAVE_DECL_KVM_EXIT_IO)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((KVM_EXIT_IO) == (2), "KVM_EXIT_IO != 2");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define KVM_EXIT_IO 2
#endif
#if defined(KVM_EXIT_HYPERCALL) || (defined(HAVE_DECL_KVM_EXIT_HYPERCALL) && HAVE_DECL_KVM_EXIT_HYPERCALL)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((KVM_EXIT_HYPERCALL) == (3), "KVM_EXIT_HYPERCALL != 3");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define KVM_EXIT_HYPERCALL 3
#endif
#if defined(KVM_EXIT_DEBUG) || (defined(HAVE_DECL_KVM_EXIT_DEBUG) && HAVE_DECL_KVM_EXIT_DEBUG)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((KVM_EXIT_DEBUG) == (4), "KVM_EXIT_DEBUG != 4");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define KVM_EXIT_DEBUG 4
#endif
#if defined(KVM_EXIT_HLT) || (defined(HAVE_DECL_KVM_EXIT_HLT) && HAVE_DECL_KVM_EXIT_HLT)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((KVM_EXIT_HLT) == (5), "KVM_EXIT_HLT != 5");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define KVM_EXIT_HLT 5
#endif
#if defined(KVM_EXIT_MMIO) || (defined(HAVE_DECL_KVM_EXIT_MMIO) && HAVE_DECL_KVM_EXIT_MMIO)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((KVM_EXIT_MMIO) == (6), "KVM_EXIT_MMIO != 6");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define KVM_EXIT_MMIO 6
#endif
#if defined(KVM_EXIT_IRQ_WINDOW_OPEN) || (defined(HAVE_DECL_KVM_EXIT_IRQ_WINDOW_OPEN) && HAVE_DECL_KVM_EXIT_IRQ_WINDOW_OPEN)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((KVM_EXIT_IRQ_WINDOW_OPEN) == (7), "KVM_EXIT_IRQ_WINDOW_OPEN != 7");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define KVM_EXIT_IRQ_WINDOW_OPEN 7
#endif
#if defined(KVM_EXIT_SHUTDOWN) || (defined(HAVE_DECL_KVM_EXIT_SHUTDOWN) && HAVE_DECL_KVM_EXIT_SHUTDOWN)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((KVM_EXIT_SHUTDOWN) == (8), "KVM_EXIT_SHUTDOWN != 8");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define KVM_EXIT_SHUTDOWN 8
#endif
#if defined(KVM_EXIT_FAIL_ENTRY) || (defined(HAVE_DECL_KVM_EXIT_FAIL_ENTRY) && HAVE_DECL_KVM_EXIT_FAIL_ENTRY)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((KVM_EXIT_FAIL_ENTRY) == (9), "KVM_EXIT_FAIL_ENTRY != 9");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define KVM_EXIT_FAIL_ENTRY 9
#endif
#if defined(KVM_EXIT_INTR) || (defined(HAVE_DECL_KVM_EXIT_INTR) && HAVE_DECL_KVM_EXIT_INTR)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((KVM_EXIT_INTR) == (10), "KVM_EXIT_INTR != 10");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define KVM_EXIT_INTR 10
#endif
#if defined(KVM_EXIT_SET_TPR) || (defined(HAVE_DECL_KVM_EXIT_SET_TPR) && HAVE_DECL_KVM_EXIT_SET_TPR)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((KVM_EXIT_SET_TPR) == (11), "KVM_EXIT_SET_TPR != 11");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define KVM_EXIT_SET_TPR 11
#endif
#if defined(KVM_EXIT_TPR_ACCESS) || (defined(HAVE_DECL_KVM_EXIT_TPR_ACCESS) && HAVE_DECL_KVM_EXIT_TPR_ACCESS)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((KVM_EXIT_TPR_ACCESS) == (12), "KVM_EXIT_TPR_ACCESS != 12");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define KVM_EXIT_TPR_ACCESS 12
#endif
#if defined(KVM_EXIT_S390_SIEIC) || (defined(HAVE_DECL_KVM_EXIT_S390_SIEIC) && HAVE_DECL_KVM_EXIT_S390_SIEIC)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((KVM_EXIT_S390_SIEIC) == (13), "KVM_EXIT_S390_SIEIC != 13");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define KVM_EXIT_S390_SIEIC 13
#endif
#if defined(KVM_EXIT_S390_RESET) || (defined(HAVE_DECL_KVM_EXIT_S390_RESET) && HAVE_DECL_KVM_EXIT_S390_RESET)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((KVM_EXIT_S390_RESET) == (14), "KVM_EXIT_S390_RESET != 14");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define KVM_EXIT_S390_RESET 14
#endif
#if defined(KVM_EXIT_DCR) || (defined(HAVE_DECL_KVM_EXIT_DCR) && HAVE_DECL_KVM_EXIT_DCR)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((KVM_EXIT_DCR) == (15), "KVM_EXIT_DCR != 15");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define KVM_EXIT_DCR 15
#endif
#if defined(KVM_EXIT_NMI) || (defined(HAVE_DECL_KVM_EXIT_NMI) && HAVE_DECL_KVM_EXIT_NMI)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((KVM_EXIT_NMI) == (16), "KVM_EXIT_NMI != 16");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define KVM_EXIT_NMI 16
#endif
#if defined(KVM_EXIT_INTERNAL_ERROR) || (defined(HAVE_DECL_KVM_EXIT_INTERNAL_ERROR) && HAVE_DECL_KVM_EXIT_INTERNAL_ERROR)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((KVM_EXIT_INTERNAL_ERROR) == (17), "KVM_EXIT_INTERNAL_ERROR != 17");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define KVM_EXIT_INTERNAL_ERROR 17
#endif
#if defined(KVM_EXIT_OSI) || (defined(HAVE_DECL_KVM_EXIT_OSI) && HAVE_DECL_KVM_EXIT_OSI)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((KVM_EXIT_OSI) == (18), "KVM_EXIT_OSI != 18");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define KVM_EXIT_OSI 18
#endif
#if defined(KVM_EXIT_PAPR_HCALL) || (defined(HAVE_DECL_KVM_EXIT_PAPR_HCALL) && HAVE_DECL_KVM_EXIT_PAPR_HCALL)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((KVM_EXIT_PAPR_HCALL) == (19), "KVM_EXIT_PAPR_HCALL != 19");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define KVM_EXIT_PAPR_HCALL 19
#endif
#if defined(KVM_EXIT_S390_UCONTROL) || (defined(HAVE_DECL_KVM_EXIT_S390_UCONTROL) && HAVE_DECL_KVM_EXIT_S390_UCONTROL)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((KVM_EXIT_S390_UCONTROL) == (20), "KVM_EXIT_S390_UCONTROL != 20");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define KVM_EXIT_S390_UCONTROL 20
#endif
#if defined(KVM_EXIT_WATCHDOG) || (defined(HAVE_DECL_KVM_EXIT_WATCHDOG) && HAVE_DECL_KVM_EXIT_WATCHDOG)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((KVM_EXIT_WATCHDOG) == (21), "KVM_EXIT_WATCHDOG != 21");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define KVM_EXIT_WATCHDOG 21
#endif
#if defined(KVM_EXIT_S390_TSCH) || (defined(HAVE_DECL_KVM_EXIT_S390_TSCH) && HAVE_DECL_KVM_EXIT_S390_TSCH)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((KVM_EXIT_S390_TSCH) == (22), "KVM_EXIT_S390_TSCH != 22");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define KVM_EXIT_S390_TSCH 22
#endif
#if defined(KVM_EXIT_EPR) || (defined(HAVE_DECL_KVM_EXIT_EPR) && HAVE_DECL_KVM_EXIT_EPR)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((KVM_EXIT_EPR) == (23), "KVM_EXIT_EPR != 23");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define KVM_EXIT_EPR 23
#endif
#if defined(KVM_EXIT_SYSTEM_EVENT) || (defined(HAVE_DECL_KVM_EXIT_SYSTEM_EVENT) && HAVE_DECL_KVM_EXIT_SYSTEM_EVENT)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((KVM_EXIT_SYSTEM_EVENT) == (24), "KVM_EXIT_SYSTEM_EVENT != 24");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define KVM_EXIT_SYSTEM_EVENT 24
#endif
#if defined(KVM_EXIT_S390_STSI) || (defined(HAVE_DECL_KVM_EXIT_S390_STSI) && HAVE_DECL_KVM_EXIT_S390_STSI)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((KVM_EXIT_S390_STSI) == (25), "KVM_EXIT_S390_STSI != 25");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define KVM_EXIT_S390_STSI 25
#endif
#if defined(KVM_EXIT_IOAPIC_EOI) || (defined(HAVE_DECL_KVM_EXIT_IOAPIC_EOI) && HAVE_DECL_KVM_EXIT_IOAPIC_EOI)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((KVM_EXIT_IOAPIC_EOI) == (26), "KVM_EXIT_IOAPIC_EOI != 26");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define KVM_EXIT_IOAPIC_EOI 26
#endif
#if defined(KVM_EXIT_HYPERV) || (defined(HAVE_DECL_KVM_EXIT_HYPERV) && HAVE_DECL_KVM_EXIT_HYPERV)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((KVM_EXIT_HYPERV) == (27), "KVM_EXIT_HYPERV != 27");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define KVM_EXIT_HYPERV 27
#endif
#if defined(KVM_EXIT_ARM_NISV) || (defined(HAVE_DECL_KVM_EXIT_ARM_NISV) && HAVE_DECL_KVM_EXIT_ARM_NISV)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((KVM_EXIT_ARM_NISV) == (28), "KVM_EXIT_ARM_NISV != 28");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define KVM_EXIT_ARM_NISV 28
#endif

#ifndef XLAT_MACROS_ONLY

# ifdef IN_MPERS

#  error static const struct xlat kvm_exit_reason in mpers mode

# else

static const struct xlat_data kvm_exit_reason_xdata[] = {
 [KVM_EXIT_UNKNOWN] = XLAT(KVM_EXIT_UNKNOWN),
 #define XLAT_VAL_0 ((unsigned) (KVM_EXIT_UNKNOWN))
 #define XLAT_STR_0 STRINGIFY(KVM_EXIT_UNKNOWN)
 [KVM_EXIT_EXCEPTION] = XLAT(KVM_EXIT_EXCEPTION),
 #define XLAT_VAL_1 ((unsigned) (KVM_EXIT_EXCEPTION))
 #define XLAT_STR_1 STRINGIFY(KVM_EXIT_EXCEPTION)
 [KVM_EXIT_IO] = XLAT(KVM_EXIT_IO),
 #define XLAT_VAL_2 ((unsigned) (KVM_EXIT_IO))
 #define XLAT_STR_2 STRINGIFY(KVM_EXIT_IO)
 [KVM_EXIT_HYPERCALL] = XLAT(KVM_EXIT_HYPERCALL),
 #define XLAT_VAL_3 ((unsigned) (KVM_EXIT_HYPERCALL))
 #define XLAT_STR_3 STRINGIFY(KVM_EXIT_HYPERCALL)
 [KVM_EXIT_DEBUG] = XLAT(KVM_EXIT_DEBUG),
 #define XLAT_VAL_4 ((unsigned) (KVM_EXIT_DEBUG))
 #define XLAT_STR_4 STRINGIFY(KVM_EXIT_DEBUG)
 [KVM_EXIT_HLT] = XLAT(KVM_EXIT_HLT),
 #define XLAT_VAL_5 ((unsigned) (KVM_EXIT_HLT))
 #define XLAT_STR_5 STRINGIFY(KVM_EXIT_HLT)
 [KVM_EXIT_MMIO] = XLAT(KVM_EXIT_MMIO),
 #define XLAT_VAL_6 ((unsigned) (KVM_EXIT_MMIO))
 #define XLAT_STR_6 STRINGIFY(KVM_EXIT_MMIO)
 [KVM_EXIT_IRQ_WINDOW_OPEN] = XLAT(KVM_EXIT_IRQ_WINDOW_OPEN),
 #define XLAT_VAL_7 ((unsigned) (KVM_EXIT_IRQ_WINDOW_OPEN))
 #define XLAT_STR_7 STRINGIFY(KVM_EXIT_IRQ_WINDOW_OPEN)
 [KVM_EXIT_SHUTDOWN] = XLAT(KVM_EXIT_SHUTDOWN),
 #define XLAT_VAL_8 ((unsigned) (KVM_EXIT_SHUTDOWN))
 #define XLAT_STR_8 STRINGIFY(KVM_EXIT_SHUTDOWN)
 [KVM_EXIT_FAIL_ENTRY] = XLAT(KVM_EXIT_FAIL_ENTRY),
 #define XLAT_VAL_9 ((unsigned) (KVM_EXIT_FAIL_ENTRY))
 #define XLAT_STR_9 STRINGIFY(KVM_EXIT_FAIL_ENTRY)
 [KVM_EXIT_INTR] = XLAT(KVM_EXIT_INTR),
 #define XLAT_VAL_10 ((unsigned) (KVM_EXIT_INTR))
 #define XLAT_STR_10 STRINGIFY(KVM_EXIT_INTR)
 [KVM_EXIT_SET_TPR] = XLAT(KVM_EXIT_SET_TPR),
 #define XLAT_VAL_11 ((unsigned) (KVM_EXIT_SET_TPR))
 #define XLAT_STR_11 STRINGIFY(KVM_EXIT_SET_TPR)
 [KVM_EXIT_TPR_ACCESS] = XLAT(KVM_EXIT_TPR_ACCESS),
 #define XLAT_VAL_12 ((unsigned) (KVM_EXIT_TPR_ACCESS))
 #define XLAT_STR_12 STRINGIFY(KVM_EXIT_TPR_ACCESS)
 [KVM_EXIT_S390_SIEIC] = XLAT(KVM_EXIT_S390_SIEIC),
 #define XLAT_VAL_13 ((unsigned) (KVM_EXIT_S390_SIEIC))
 #define XLAT_STR_13 STRINGIFY(KVM_EXIT_S390_SIEIC)
 [KVM_EXIT_S390_RESET] = XLAT(KVM_EXIT_S390_RESET),
 #define XLAT_VAL_14 ((unsigned) (KVM_EXIT_S390_RESET))
 #define XLAT_STR_14 STRINGIFY(KVM_EXIT_S390_RESET)

 [KVM_EXIT_DCR] = XLAT(KVM_EXIT_DCR),
 #define XLAT_VAL_15 ((unsigned) (KVM_EXIT_DCR))
 #define XLAT_STR_15 STRINGIFY(KVM_EXIT_DCR)
 [KVM_EXIT_NMI] = XLAT(KVM_EXIT_NMI),
 #define XLAT_VAL_16 ((unsigned) (KVM_EXIT_NMI))
 #define XLAT_STR_16 STRINGIFY(KVM_EXIT_NMI)
 [KVM_EXIT_INTERNAL_ERROR] = XLAT(KVM_EXIT_INTERNAL_ERROR),
 #define XLAT_VAL_17 ((unsigned) (KVM_EXIT_INTERNAL_ERROR))
 #define XLAT_STR_17 STRINGIFY(KVM_EXIT_INTERNAL_ERROR)
 [KVM_EXIT_OSI] = XLAT(KVM_EXIT_OSI),
 #define XLAT_VAL_18 ((unsigned) (KVM_EXIT_OSI))
 #define XLAT_STR_18 STRINGIFY(KVM_EXIT_OSI)
 [KVM_EXIT_PAPR_HCALL] = XLAT(KVM_EXIT_PAPR_HCALL),
 #define XLAT_VAL_19 ((unsigned) (KVM_EXIT_PAPR_HCALL))
 #define XLAT_STR_19 STRINGIFY(KVM_EXIT_PAPR_HCALL)
 [KVM_EXIT_S390_UCONTROL] = XLAT(KVM_EXIT_S390_UCONTROL),
 #define XLAT_VAL_20 ((unsigned) (KVM_EXIT_S390_UCONTROL))
 #define XLAT_STR_20 STRINGIFY(KVM_EXIT_S390_UCONTROL)
 [KVM_EXIT_WATCHDOG] = XLAT(KVM_EXIT_WATCHDOG),
 #define XLAT_VAL_21 ((unsigned) (KVM_EXIT_WATCHDOG))
 #define XLAT_STR_21 STRINGIFY(KVM_EXIT_WATCHDOG)
 [KVM_EXIT_S390_TSCH] = XLAT(KVM_EXIT_S390_TSCH),
 #define XLAT_VAL_22 ((unsigned) (KVM_EXIT_S390_TSCH))
 #define XLAT_STR_22 STRINGIFY(KVM_EXIT_S390_TSCH)
 [KVM_EXIT_EPR] = XLAT(KVM_EXIT_EPR),
 #define XLAT_VAL_23 ((unsigned) (KVM_EXIT_EPR))
 #define XLAT_STR_23 STRINGIFY(KVM_EXIT_EPR)
 [KVM_EXIT_SYSTEM_EVENT] = XLAT(KVM_EXIT_SYSTEM_EVENT),
 #define XLAT_VAL_24 ((unsigned) (KVM_EXIT_SYSTEM_EVENT))
 #define XLAT_STR_24 STRINGIFY(KVM_EXIT_SYSTEM_EVENT)
 [KVM_EXIT_S390_STSI] = XLAT(KVM_EXIT_S390_STSI),
 #define XLAT_VAL_25 ((unsigned) (KVM_EXIT_S390_STSI))
 #define XLAT_STR_25 STRINGIFY(KVM_EXIT_S390_STSI)
 [KVM_EXIT_IOAPIC_EOI] = XLAT(KVM_EXIT_IOAPIC_EOI),
 #define XLAT_VAL_26 ((unsigned) (KVM_EXIT_IOAPIC_EOI))
 #define XLAT_STR_26 STRINGIFY(KVM_EXIT_IOAPIC_EOI)
 [KVM_EXIT_HYPERV] = XLAT(KVM_EXIT_HYPERV),
 #define XLAT_VAL_27 ((unsigned) (KVM_EXIT_HYPERV))
 #define XLAT_STR_27 STRINGIFY(KVM_EXIT_HYPERV)
 [KVM_EXIT_ARM_NISV] = XLAT(KVM_EXIT_ARM_NISV),
 #define XLAT_VAL_28 ((unsigned) (KVM_EXIT_ARM_NISV))
 #define XLAT_STR_28 STRINGIFY(KVM_EXIT_ARM_NISV)
};
static
const struct xlat kvm_exit_reason[1] = { {
 .data = kvm_exit_reason_xdata,
 .size = ARRAY_SIZE(kvm_exit_reason_xdata),
 .type = XT_INDEXED,
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
# endif /* !IN_MPERS */

#endif /* !XLAT_MACROS_ONLY */
