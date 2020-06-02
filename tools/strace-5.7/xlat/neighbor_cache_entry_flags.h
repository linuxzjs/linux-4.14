/* Generated by ./xlat/gen.sh from ./xlat/neighbor_cache_entry_flags.in; do not edit. */

#include "gcc_compat.h"
#include "static_assert.h"

#if defined(NTF_USE) || (defined(HAVE_DECL_NTF_USE) && HAVE_DECL_NTF_USE)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((NTF_USE) == (0x01), "NTF_USE != 0x01");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define NTF_USE 0x01
#endif
#if defined(NTF_SELF) || (defined(HAVE_DECL_NTF_SELF) && HAVE_DECL_NTF_SELF)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((NTF_SELF) == (0x02), "NTF_SELF != 0x02");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define NTF_SELF 0x02
#endif
#if defined(NTF_MASTER) || (defined(HAVE_DECL_NTF_MASTER) && HAVE_DECL_NTF_MASTER)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((NTF_MASTER) == (0x04), "NTF_MASTER != 0x04");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define NTF_MASTER 0x04
#endif
#if defined(NTF_PROXY) || (defined(HAVE_DECL_NTF_PROXY) && HAVE_DECL_NTF_PROXY)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((NTF_PROXY) == (0x08), "NTF_PROXY != 0x08");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define NTF_PROXY 0x08
#endif
#if defined(NTF_EXT_LEARNED) || (defined(HAVE_DECL_NTF_EXT_LEARNED) && HAVE_DECL_NTF_EXT_LEARNED)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((NTF_EXT_LEARNED) == (0x10), "NTF_EXT_LEARNED != 0x10");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define NTF_EXT_LEARNED 0x10
#endif
#if defined(NTF_OFFLOADED) || (defined(HAVE_DECL_NTF_OFFLOADED) && HAVE_DECL_NTF_OFFLOADED)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((NTF_OFFLOADED) == (0x20), "NTF_OFFLOADED != 0x20");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define NTF_OFFLOADED 0x20
#endif
#if defined(NTF_STICKY) || (defined(HAVE_DECL_NTF_STICKY) && HAVE_DECL_NTF_STICKY)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((NTF_STICKY) == (0x40), "NTF_STICKY != 0x40");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define NTF_STICKY 0x40
#endif
#if defined(NTF_ROUTER) || (defined(HAVE_DECL_NTF_ROUTER) && HAVE_DECL_NTF_ROUTER)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((NTF_ROUTER) == (0x80), "NTF_ROUTER != 0x80");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define NTF_ROUTER 0x80
#endif

#ifndef XLAT_MACROS_ONLY

# ifdef IN_MPERS

#  error static const struct xlat neighbor_cache_entry_flags in mpers mode

# else

static const struct xlat_data neighbor_cache_entry_flags_xdata[] = {
 XLAT(NTF_USE),
 #define XLAT_VAL_0 ((unsigned) (NTF_USE))
 #define XLAT_STR_0 STRINGIFY(NTF_USE)
 XLAT(NTF_SELF),
 #define XLAT_VAL_1 ((unsigned) (NTF_SELF))
 #define XLAT_STR_1 STRINGIFY(NTF_SELF)
 XLAT(NTF_MASTER),
 #define XLAT_VAL_2 ((unsigned) (NTF_MASTER))
 #define XLAT_STR_2 STRINGIFY(NTF_MASTER)
 XLAT(NTF_PROXY),
 #define XLAT_VAL_3 ((unsigned) (NTF_PROXY))
 #define XLAT_STR_3 STRINGIFY(NTF_PROXY)
 XLAT(NTF_EXT_LEARNED),
 #define XLAT_VAL_4 ((unsigned) (NTF_EXT_LEARNED))
 #define XLAT_STR_4 STRINGIFY(NTF_EXT_LEARNED)
 XLAT(NTF_OFFLOADED),
 #define XLAT_VAL_5 ((unsigned) (NTF_OFFLOADED))
 #define XLAT_STR_5 STRINGIFY(NTF_OFFLOADED)
 XLAT(NTF_STICKY),
 #define XLAT_VAL_6 ((unsigned) (NTF_STICKY))
 #define XLAT_STR_6 STRINGIFY(NTF_STICKY)
 XLAT(NTF_ROUTER),
 #define XLAT_VAL_7 ((unsigned) (NTF_ROUTER))
 #define XLAT_STR_7 STRINGIFY(NTF_ROUTER)
};
static
const struct xlat neighbor_cache_entry_flags[1] = { {
 .data = neighbor_cache_entry_flags_xdata,
 .size = ARRAY_SIZE(neighbor_cache_entry_flags_xdata),
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
# endif /* !IN_MPERS */

#endif /* !XLAT_MACROS_ONLY */
