/* Generated by ./xlat/gen.sh from ./xlat/loop_cmds.in; do not edit. */

#include "gcc_compat.h"
#include "static_assert.h"

#if defined(LOOP_SET_FD) || (defined(HAVE_DECL_LOOP_SET_FD) && HAVE_DECL_LOOP_SET_FD)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((LOOP_SET_FD) == (0x4C00), "LOOP_SET_FD != 0x4C00");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define LOOP_SET_FD 0x4C00
#endif
#if defined(LOOP_CLR_FD) || (defined(HAVE_DECL_LOOP_CLR_FD) && HAVE_DECL_LOOP_CLR_FD)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((LOOP_CLR_FD) == (0x4C01), "LOOP_CLR_FD != 0x4C01");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define LOOP_CLR_FD 0x4C01
#endif
#if defined(LOOP_SET_STATUS) || (defined(HAVE_DECL_LOOP_SET_STATUS) && HAVE_DECL_LOOP_SET_STATUS)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((LOOP_SET_STATUS) == (0x4C02), "LOOP_SET_STATUS != 0x4C02");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define LOOP_SET_STATUS 0x4C02
#endif
#if defined(LOOP_GET_STATUS) || (defined(HAVE_DECL_LOOP_GET_STATUS) && HAVE_DECL_LOOP_GET_STATUS)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((LOOP_GET_STATUS) == (0x4C03), "LOOP_GET_STATUS != 0x4C03");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define LOOP_GET_STATUS 0x4C03
#endif
#if defined(LOOP_SET_STATUS64) || (defined(HAVE_DECL_LOOP_SET_STATUS64) && HAVE_DECL_LOOP_SET_STATUS64)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((LOOP_SET_STATUS64) == (0x4C04), "LOOP_SET_STATUS64 != 0x4C04");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define LOOP_SET_STATUS64 0x4C04
#endif
#if defined(LOOP_GET_STATUS64) || (defined(HAVE_DECL_LOOP_GET_STATUS64) && HAVE_DECL_LOOP_GET_STATUS64)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((LOOP_GET_STATUS64) == (0x4C05), "LOOP_GET_STATUS64 != 0x4C05");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define LOOP_GET_STATUS64 0x4C05
#endif
#if defined(LOOP_CHANGE_FD) || (defined(HAVE_DECL_LOOP_CHANGE_FD) && HAVE_DECL_LOOP_CHANGE_FD)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((LOOP_CHANGE_FD) == (0x4C06), "LOOP_CHANGE_FD != 0x4C06");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define LOOP_CHANGE_FD 0x4C06
#endif
#if defined(LOOP_SET_CAPACITY) || (defined(HAVE_DECL_LOOP_SET_CAPACITY) && HAVE_DECL_LOOP_SET_CAPACITY)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((LOOP_SET_CAPACITY) == (0x4C07), "LOOP_SET_CAPACITY != 0x4C07");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define LOOP_SET_CAPACITY 0x4C07
#endif
#if defined(LOOP_SET_DIRECT_IO) || (defined(HAVE_DECL_LOOP_SET_DIRECT_IO) && HAVE_DECL_LOOP_SET_DIRECT_IO)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((LOOP_SET_DIRECT_IO) == (0x4C08), "LOOP_SET_DIRECT_IO != 0x4C08");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define LOOP_SET_DIRECT_IO 0x4C08
#endif
#if defined(LOOP_SET_BLOCK_SIZE) || (defined(HAVE_DECL_LOOP_SET_BLOCK_SIZE) && HAVE_DECL_LOOP_SET_BLOCK_SIZE)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((LOOP_SET_BLOCK_SIZE) == (0x4C09), "LOOP_SET_BLOCK_SIZE != 0x4C09");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define LOOP_SET_BLOCK_SIZE 0x4C09
#endif
#if defined(LOOP_CTL_ADD) || (defined(HAVE_DECL_LOOP_CTL_ADD) && HAVE_DECL_LOOP_CTL_ADD)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((LOOP_CTL_ADD) == (0x4C80), "LOOP_CTL_ADD != 0x4C80");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define LOOP_CTL_ADD 0x4C80
#endif
#if defined(LOOP_CTL_REMOVE) || (defined(HAVE_DECL_LOOP_CTL_REMOVE) && HAVE_DECL_LOOP_CTL_REMOVE)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((LOOP_CTL_REMOVE) == (0x4C81), "LOOP_CTL_REMOVE != 0x4C81");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define LOOP_CTL_REMOVE 0x4C81
#endif
#if defined(LOOP_CTL_GET_FREE) || (defined(HAVE_DECL_LOOP_CTL_GET_FREE) && HAVE_DECL_LOOP_CTL_GET_FREE)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((LOOP_CTL_GET_FREE) == (0x4C82), "LOOP_CTL_GET_FREE != 0x4C82");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define LOOP_CTL_GET_FREE 0x4C82
#endif

#ifndef XLAT_MACROS_ONLY

# ifdef IN_MPERS

extern const struct xlat loop_cmds[];

# else

static const struct xlat_data loop_cmds_xdata[] = {
 XLAT(LOOP_SET_FD),
 #define XLAT_VAL_0 ((unsigned) (LOOP_SET_FD))
 #define XLAT_STR_0 STRINGIFY(LOOP_SET_FD)
 XLAT(LOOP_CLR_FD),
 #define XLAT_VAL_1 ((unsigned) (LOOP_CLR_FD))
 #define XLAT_STR_1 STRINGIFY(LOOP_CLR_FD)
 XLAT(LOOP_SET_STATUS),
 #define XLAT_VAL_2 ((unsigned) (LOOP_SET_STATUS))
 #define XLAT_STR_2 STRINGIFY(LOOP_SET_STATUS)
 XLAT(LOOP_GET_STATUS),
 #define XLAT_VAL_3 ((unsigned) (LOOP_GET_STATUS))
 #define XLAT_STR_3 STRINGIFY(LOOP_GET_STATUS)
 XLAT(LOOP_SET_STATUS64),
 #define XLAT_VAL_4 ((unsigned) (LOOP_SET_STATUS64))
 #define XLAT_STR_4 STRINGIFY(LOOP_SET_STATUS64)
 XLAT(LOOP_GET_STATUS64),
 #define XLAT_VAL_5 ((unsigned) (LOOP_GET_STATUS64))
 #define XLAT_STR_5 STRINGIFY(LOOP_GET_STATUS64)
 XLAT(LOOP_CHANGE_FD),
 #define XLAT_VAL_6 ((unsigned) (LOOP_CHANGE_FD))
 #define XLAT_STR_6 STRINGIFY(LOOP_CHANGE_FD)
 XLAT(LOOP_SET_CAPACITY),
 #define XLAT_VAL_7 ((unsigned) (LOOP_SET_CAPACITY))
 #define XLAT_STR_7 STRINGIFY(LOOP_SET_CAPACITY)
 XLAT(LOOP_SET_DIRECT_IO),
 #define XLAT_VAL_8 ((unsigned) (LOOP_SET_DIRECT_IO))
 #define XLAT_STR_8 STRINGIFY(LOOP_SET_DIRECT_IO)
 XLAT(LOOP_SET_BLOCK_SIZE),
 #define XLAT_VAL_9 ((unsigned) (LOOP_SET_BLOCK_SIZE))
 #define XLAT_STR_9 STRINGIFY(LOOP_SET_BLOCK_SIZE)
 XLAT(LOOP_CTL_ADD),
 #define XLAT_VAL_10 ((unsigned) (LOOP_CTL_ADD))
 #define XLAT_STR_10 STRINGIFY(LOOP_CTL_ADD)
 XLAT(LOOP_CTL_REMOVE),
 #define XLAT_VAL_11 ((unsigned) (LOOP_CTL_REMOVE))
 #define XLAT_STR_11 STRINGIFY(LOOP_CTL_REMOVE)
 XLAT(LOOP_CTL_GET_FREE),
 #define XLAT_VAL_12 ((unsigned) (LOOP_CTL_GET_FREE))
 #define XLAT_STR_12 STRINGIFY(LOOP_CTL_GET_FREE)
};
#  if !(defined HAVE_M32_MPERS || defined HAVE_MX32_MPERS)
static
#  endif
const struct xlat loop_cmds[1] = { {
 .data = loop_cmds_xdata,
 .size = ARRAY_SIZE(loop_cmds_xdata),
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
# endif /* !IN_MPERS */

#endif /* !XLAT_MACROS_ONLY */
