/* Generated by ./xlat/gen.sh from ./xlat/itimer_which.in; do not edit. */

#include "gcc_compat.h"
#include "static_assert.h"

#if defined(ITIMER_REAL) || (defined(HAVE_DECL_ITIMER_REAL) && HAVE_DECL_ITIMER_REAL)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((ITIMER_REAL) == (0), "ITIMER_REAL != 0");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define ITIMER_REAL 0
#endif
#if defined(ITIMER_VIRTUAL) || (defined(HAVE_DECL_ITIMER_VIRTUAL) && HAVE_DECL_ITIMER_VIRTUAL)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((ITIMER_VIRTUAL) == (1), "ITIMER_VIRTUAL != 1");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define ITIMER_VIRTUAL 1
#endif
#if defined(ITIMER_PROF) || (defined(HAVE_DECL_ITIMER_PROF) && HAVE_DECL_ITIMER_PROF)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((ITIMER_PROF) == (2), "ITIMER_PROF != 2");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define ITIMER_PROF 2
#endif

#ifndef XLAT_MACROS_ONLY

# ifdef IN_MPERS

#  error static const struct xlat itimer_which in mpers mode

# else

static const struct xlat_data itimer_which_xdata[] = {
 [ITIMER_REAL] = XLAT(ITIMER_REAL),
 #define XLAT_VAL_0 ((unsigned) (ITIMER_REAL))
 #define XLAT_STR_0 STRINGIFY(ITIMER_REAL)
 [ITIMER_VIRTUAL] = XLAT(ITIMER_VIRTUAL),
 #define XLAT_VAL_1 ((unsigned) (ITIMER_VIRTUAL))
 #define XLAT_STR_1 STRINGIFY(ITIMER_VIRTUAL)
 [ITIMER_PROF] = XLAT(ITIMER_PROF),
 #define XLAT_VAL_2 ((unsigned) (ITIMER_PROF))
 #define XLAT_STR_2 STRINGIFY(ITIMER_PROF)
};
static
const struct xlat itimer_which[1] = { {
 .data = itimer_which_xdata,
 .size = ARRAY_SIZE(itimer_which_xdata),
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
  ,
} };

#  undef XLAT_STR_0
#  undef XLAT_VAL_0
#  undef XLAT_STR_1
#  undef XLAT_VAL_1
#  undef XLAT_STR_2
#  undef XLAT_VAL_2
# endif /* !IN_MPERS */

#endif /* !XLAT_MACROS_ONLY */