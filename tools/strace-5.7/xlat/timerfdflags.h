/* Generated by ./xlat/gen.sh from ./xlat/timerfdflags.in; do not edit. */

#include "gcc_compat.h"
#include "static_assert.h"

#if defined(TFD_TIMER_ABSTIME) || (defined(HAVE_DECL_TFD_TIMER_ABSTIME) && HAVE_DECL_TFD_TIMER_ABSTIME)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((TFD_TIMER_ABSTIME) == ((1 << 0)), "TFD_TIMER_ABSTIME != (1 << 0)");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define TFD_TIMER_ABSTIME (1 << 0)
#endif
#if defined(TFD_TIMER_CANCEL_ON_SET) || (defined(HAVE_DECL_TFD_TIMER_CANCEL_ON_SET) && HAVE_DECL_TFD_TIMER_CANCEL_ON_SET)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((TFD_TIMER_CANCEL_ON_SET) == ((1 << 1)), "TFD_TIMER_CANCEL_ON_SET != (1 << 1)");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define TFD_TIMER_CANCEL_ON_SET (1 << 1)
#endif
#if defined TFD_CLOEXEC || defined O_CLOEXEC
#if defined(TFD_CLOEXEC) || (defined(HAVE_DECL_TFD_CLOEXEC) && HAVE_DECL_TFD_CLOEXEC)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((TFD_CLOEXEC) == (O_CLOEXEC), "TFD_CLOEXEC != O_CLOEXEC");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define TFD_CLOEXEC O_CLOEXEC
#endif
#endif
#if defined(TFD_NONBLOCK) || (defined(HAVE_DECL_TFD_NONBLOCK) && HAVE_DECL_TFD_NONBLOCK)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((TFD_NONBLOCK) == (O_NONBLOCK), "TFD_NONBLOCK != O_NONBLOCK");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define TFD_NONBLOCK O_NONBLOCK
#endif

#ifndef XLAT_MACROS_ONLY

# ifdef IN_MPERS

#  error static const struct xlat timerfdflags in mpers mode

# else

static const struct xlat_data timerfdflags_xdata[] = {


 XLAT(TFD_TIMER_ABSTIME),
 #define XLAT_VAL_0 ((unsigned) (TFD_TIMER_ABSTIME))
 #define XLAT_STR_0 STRINGIFY(TFD_TIMER_ABSTIME)
 XLAT(TFD_TIMER_CANCEL_ON_SET),
 #define XLAT_VAL_1 ((unsigned) (TFD_TIMER_CANCEL_ON_SET))
 #define XLAT_STR_1 STRINGIFY(TFD_TIMER_CANCEL_ON_SET)
#if defined TFD_CLOEXEC || defined O_CLOEXEC
 XLAT(TFD_CLOEXEC),
 #define XLAT_VAL_2 ((unsigned) (TFD_CLOEXEC))
 #define XLAT_STR_2 STRINGIFY(TFD_CLOEXEC)
#endif
 XLAT(TFD_NONBLOCK),
 #define XLAT_VAL_3 ((unsigned) (TFD_NONBLOCK))
 #define XLAT_STR_3 STRINGIFY(TFD_NONBLOCK)
};
static
const struct xlat timerfdflags[1] = { {
 .data = timerfdflags_xdata,
 .size = ARRAY_SIZE(timerfdflags_xdata),
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
# endif /* !IN_MPERS */

#endif /* !XLAT_MACROS_ONLY */
