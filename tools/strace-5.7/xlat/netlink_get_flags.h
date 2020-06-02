/* Generated by ./xlat/gen.sh from ./xlat/netlink_get_flags.in; do not edit. */

#include "gcc_compat.h"
#include "static_assert.h"


#ifndef XLAT_MACROS_ONLY

# ifdef IN_MPERS

#  error static const struct xlat netlink_get_flags in mpers mode

# else

static const struct xlat_data netlink_get_flags_xdata[] = {
#if defined(NLM_F_DUMP) || (defined(HAVE_DECL_NLM_F_DUMP) && HAVE_DECL_NLM_F_DUMP)
  XLAT(NLM_F_DUMP),
 #define XLAT_VAL_0 ((unsigned) (NLM_F_DUMP))
 #define XLAT_STR_0 STRINGIFY(NLM_F_DUMP)
#endif
#if defined(NLM_F_ROOT) || (defined(HAVE_DECL_NLM_F_ROOT) && HAVE_DECL_NLM_F_ROOT)
  XLAT(NLM_F_ROOT),
 #define XLAT_VAL_1 ((unsigned) (NLM_F_ROOT))
 #define XLAT_STR_1 STRINGIFY(NLM_F_ROOT)
#endif
#if defined(NLM_F_MATCH) || (defined(HAVE_DECL_NLM_F_MATCH) && HAVE_DECL_NLM_F_MATCH)
  XLAT(NLM_F_MATCH),
 #define XLAT_VAL_2 ((unsigned) (NLM_F_MATCH))
 #define XLAT_STR_2 STRINGIFY(NLM_F_MATCH)
#endif
#if defined(NLM_F_ATOMIC) || (defined(HAVE_DECL_NLM_F_ATOMIC) && HAVE_DECL_NLM_F_ATOMIC)
  XLAT(NLM_F_ATOMIC),
 #define XLAT_VAL_3 ((unsigned) (NLM_F_ATOMIC))
 #define XLAT_STR_3 STRINGIFY(NLM_F_ATOMIC)
#endif
};
static
const struct xlat netlink_get_flags[1] = { {
 .data = netlink_get_flags_xdata,
 .size = ARRAY_SIZE(netlink_get_flags_xdata),
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
