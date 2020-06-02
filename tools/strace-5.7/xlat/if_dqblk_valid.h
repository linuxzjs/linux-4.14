/* Generated by ./xlat/gen.sh from ./xlat/if_dqblk_valid.in; do not edit. */

#include "gcc_compat.h"
#include "static_assert.h"

#if defined(QIF_BLIMITS) || (defined(HAVE_DECL_QIF_BLIMITS) && HAVE_DECL_QIF_BLIMITS)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((QIF_BLIMITS) == ((1 << 0)), "QIF_BLIMITS != (1 << 0)");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define QIF_BLIMITS (1 << 0)
#endif
#if defined(QIF_SPACE) || (defined(HAVE_DECL_QIF_SPACE) && HAVE_DECL_QIF_SPACE)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((QIF_SPACE) == ((1 << 1)), "QIF_SPACE != (1 << 1)");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define QIF_SPACE (1 << 1)
#endif
#if defined(QIF_ILIMITS) || (defined(HAVE_DECL_QIF_ILIMITS) && HAVE_DECL_QIF_ILIMITS)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((QIF_ILIMITS) == ((1 << 2)), "QIF_ILIMITS != (1 << 2)");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define QIF_ILIMITS (1 << 2)
#endif
#if defined(QIF_INODES) || (defined(HAVE_DECL_QIF_INODES) && HAVE_DECL_QIF_INODES)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((QIF_INODES) == ((1 << 3)), "QIF_INODES != (1 << 3)");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define QIF_INODES (1 << 3)
#endif
#if defined(QIF_BTIME) || (defined(HAVE_DECL_QIF_BTIME) && HAVE_DECL_QIF_BTIME)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((QIF_BTIME) == ((1 << 4)), "QIF_BTIME != (1 << 4)");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define QIF_BTIME (1 << 4)
#endif
#if defined(QIF_ITIME) || (defined(HAVE_DECL_QIF_ITIME) && HAVE_DECL_QIF_ITIME)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((QIF_ITIME) == ((1 << 5)), "QIF_ITIME != (1 << 5)");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define QIF_ITIME (1 << 5)
#endif

#ifndef XLAT_MACROS_ONLY

# ifdef IN_MPERS

#  error static const struct xlat if_dqblk_valid in mpers mode

# else

static const struct xlat_data if_dqblk_valid_xdata[] = {
 XLAT(QIF_BLIMITS),
 #define XLAT_VAL_0 ((unsigned) (QIF_BLIMITS))
 #define XLAT_STR_0 STRINGIFY(QIF_BLIMITS)
 XLAT(QIF_SPACE),
 #define XLAT_VAL_1 ((unsigned) (QIF_SPACE))
 #define XLAT_STR_1 STRINGIFY(QIF_SPACE)
 XLAT(QIF_ILIMITS),
 #define XLAT_VAL_2 ((unsigned) (QIF_ILIMITS))
 #define XLAT_STR_2 STRINGIFY(QIF_ILIMITS)
 XLAT(QIF_INODES),
 #define XLAT_VAL_3 ((unsigned) (QIF_INODES))
 #define XLAT_STR_3 STRINGIFY(QIF_INODES)
 XLAT(QIF_BTIME),
 #define XLAT_VAL_4 ((unsigned) (QIF_BTIME))
 #define XLAT_STR_4 STRINGIFY(QIF_BTIME)
 XLAT(QIF_ITIME),
 #define XLAT_VAL_5 ((unsigned) (QIF_ITIME))
 #define XLAT_STR_5 STRINGIFY(QIF_ITIME)
};
static
const struct xlat if_dqblk_valid[1] = { {
 .data = if_dqblk_valid_xdata,
 .size = ARRAY_SIZE(if_dqblk_valid_xdata),
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
