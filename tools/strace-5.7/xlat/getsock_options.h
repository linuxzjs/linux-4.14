/* Generated by ./xlat/gen.sh from ./xlat/getsock_options.in; do not edit. */

#include "gcc_compat.h"
#include "static_assert.h"

#if defined __hppa__
#if defined(SO_GET_FILTER) || (defined(HAVE_DECL_SO_GET_FILTER) && HAVE_DECL_SO_GET_FILTER)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((SO_GET_FILTER) == (16410), "SO_GET_FILTER != 16410");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define SO_GET_FILTER 16410
#endif
#else
#if defined(SO_GET_FILTER) || (defined(HAVE_DECL_SO_GET_FILTER) && HAVE_DECL_SO_GET_FILTER)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((SO_GET_FILTER) == (26), "SO_GET_FILTER != 26");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define SO_GET_FILTER 26
#endif
#endif

#ifndef XLAT_MACROS_ONLY

# ifdef IN_MPERS

#  error static const struct xlat getsock_options in mpers mode

# else

static const struct xlat_data getsock_options_xdata[] = {
#if defined __hppa__
 XLAT(SO_GET_FILTER),
 #define XLAT_VAL_0 ((unsigned) (SO_GET_FILTER))
 #define XLAT_STR_0 STRINGIFY(SO_GET_FILTER)
#else
 XLAT(SO_GET_FILTER),
 #define XLAT_VAL_1 ((unsigned) (SO_GET_FILTER))
 #define XLAT_STR_1 STRINGIFY(SO_GET_FILTER)
#endif
};
static
const struct xlat getsock_options[1] = { {
 .data = getsock_options_xdata,
 .size = ARRAY_SIZE(getsock_options_xdata),
 .type = XT_NORMAL,
 .flags_mask = 0
#  ifdef XLAT_VAL_0
  | XLAT_VAL_0
#  endif
#  ifdef XLAT_VAL_1
  | XLAT_VAL_1
#  endif
  ,
 .flags_strsz = 0
#  ifdef XLAT_STR_0
  + sizeof(XLAT_STR_0)
#  endif
#  ifdef XLAT_STR_1
  + sizeof(XLAT_STR_1)
#  endif
  ,
} };

#  undef XLAT_STR_0
#  undef XLAT_VAL_0
#  undef XLAT_STR_1
#  undef XLAT_VAL_1
# endif /* !IN_MPERS */

#endif /* !XLAT_MACROS_ONLY */
