/* Generated by ./xlat/gen.sh from ./xlat/pr_spec_get_store_bypass_flags.in; do not edit. */

#include "gcc_compat.h"
#include "static_assert.h"

#if defined(PR_SPEC_NOT_AFFECTED) || (defined(HAVE_DECL_PR_SPEC_NOT_AFFECTED) && HAVE_DECL_PR_SPEC_NOT_AFFECTED)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((PR_SPEC_NOT_AFFECTED) == (0), "PR_SPEC_NOT_AFFECTED != 0");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define PR_SPEC_NOT_AFFECTED 0
#endif
#if defined(PR_SPEC_PRCTL) || (defined(HAVE_DECL_PR_SPEC_PRCTL) && HAVE_DECL_PR_SPEC_PRCTL)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((PR_SPEC_PRCTL) == ((1 << 0)), "PR_SPEC_PRCTL != (1 << 0)");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define PR_SPEC_PRCTL (1 << 0)
#endif
#if defined(PR_SPEC_ENABLE) || (defined(HAVE_DECL_PR_SPEC_ENABLE) && HAVE_DECL_PR_SPEC_ENABLE)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((PR_SPEC_ENABLE) == ((1 << 1)), "PR_SPEC_ENABLE != (1 << 1)");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define PR_SPEC_ENABLE (1 << 1)
#endif
#if defined(PR_SPEC_DISABLE) || (defined(HAVE_DECL_PR_SPEC_DISABLE) && HAVE_DECL_PR_SPEC_DISABLE)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((PR_SPEC_DISABLE) == ((1 << 2)), "PR_SPEC_DISABLE != (1 << 2)");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define PR_SPEC_DISABLE (1 << 2)
#endif
#if defined(PR_SPEC_FORCE_DISABLE) || (defined(HAVE_DECL_PR_SPEC_FORCE_DISABLE) && HAVE_DECL_PR_SPEC_FORCE_DISABLE)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((PR_SPEC_FORCE_DISABLE) == ((1 << 3)), "PR_SPEC_FORCE_DISABLE != (1 << 3)");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define PR_SPEC_FORCE_DISABLE (1 << 3)
#endif
#if defined(PR_SPEC_DISABLE_NOEXEC) || (defined(HAVE_DECL_PR_SPEC_DISABLE_NOEXEC) && HAVE_DECL_PR_SPEC_DISABLE_NOEXEC)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((PR_SPEC_DISABLE_NOEXEC) == ((1 << 4)), "PR_SPEC_DISABLE_NOEXEC != (1 << 4)");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define PR_SPEC_DISABLE_NOEXEC (1 << 4)
#endif

#ifndef XLAT_MACROS_ONLY

# ifdef IN_MPERS

#  error static const struct xlat pr_spec_get_store_bypass_flags in mpers mode

# else

static const struct xlat_data pr_spec_get_store_bypass_flags_xdata[] = {
 XLAT(PR_SPEC_NOT_AFFECTED),
 #define XLAT_VAL_0 ((unsigned) (PR_SPEC_NOT_AFFECTED))
 #define XLAT_STR_0 STRINGIFY(PR_SPEC_NOT_AFFECTED)
 XLAT(PR_SPEC_PRCTL),
 #define XLAT_VAL_1 ((unsigned) (PR_SPEC_PRCTL))
 #define XLAT_STR_1 STRINGIFY(PR_SPEC_PRCTL)
 XLAT(PR_SPEC_ENABLE),
 #define XLAT_VAL_2 ((unsigned) (PR_SPEC_ENABLE))
 #define XLAT_STR_2 STRINGIFY(PR_SPEC_ENABLE)
 XLAT(PR_SPEC_DISABLE),
 #define XLAT_VAL_3 ((unsigned) (PR_SPEC_DISABLE))
 #define XLAT_STR_3 STRINGIFY(PR_SPEC_DISABLE)
 XLAT(PR_SPEC_FORCE_DISABLE),
 #define XLAT_VAL_4 ((unsigned) (PR_SPEC_FORCE_DISABLE))
 #define XLAT_STR_4 STRINGIFY(PR_SPEC_FORCE_DISABLE)
 XLAT(PR_SPEC_DISABLE_NOEXEC),
 #define XLAT_VAL_5 ((unsigned) (PR_SPEC_DISABLE_NOEXEC))
 #define XLAT_STR_5 STRINGIFY(PR_SPEC_DISABLE_NOEXEC)
};
static
const struct xlat pr_spec_get_store_bypass_flags[1] = { {
 .data = pr_spec_get_store_bypass_flags_xdata,
 .size = ARRAY_SIZE(pr_spec_get_store_bypass_flags_xdata),
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
# endif /* !IN_MPERS */

#endif /* !XLAT_MACROS_ONLY */
