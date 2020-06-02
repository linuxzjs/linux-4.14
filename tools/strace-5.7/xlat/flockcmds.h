/* Generated by ./xlat/gen.sh from ./xlat/flockcmds.in; do not edit. */

#include "gcc_compat.h"
#include "static_assert.h"

#if defined(LOCK_SH) || (defined(HAVE_DECL_LOCK_SH) && HAVE_DECL_LOCK_SH)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((LOCK_SH) == (1), "LOCK_SH != 1");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define LOCK_SH 1
#endif
#if defined(LOCK_EX) || (defined(HAVE_DECL_LOCK_EX) && HAVE_DECL_LOCK_EX)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((LOCK_EX) == (2), "LOCK_EX != 2");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define LOCK_EX 2
#endif
#if defined(LOCK_NB) || (defined(HAVE_DECL_LOCK_NB) && HAVE_DECL_LOCK_NB)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((LOCK_NB) == (4), "LOCK_NB != 4");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define LOCK_NB 4
#endif
#if defined(LOCK_UN) || (defined(HAVE_DECL_LOCK_UN) && HAVE_DECL_LOCK_UN)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((LOCK_UN) == (8), "LOCK_UN != 8");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define LOCK_UN 8
#endif
#if defined(LOCK_MAND) || (defined(HAVE_DECL_LOCK_MAND) && HAVE_DECL_LOCK_MAND)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((LOCK_MAND) == (32), "LOCK_MAND != 32");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define LOCK_MAND 32
#endif
#if defined(LOCK_RW) || (defined(HAVE_DECL_LOCK_RW) && HAVE_DECL_LOCK_RW)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((LOCK_RW) == (192), "LOCK_RW != 192");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define LOCK_RW 192
#endif
#if defined(LOCK_READ) || (defined(HAVE_DECL_LOCK_READ) && HAVE_DECL_LOCK_READ)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((LOCK_READ) == (64), "LOCK_READ != 64");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define LOCK_READ 64
#endif
#if defined(LOCK_WRITE) || (defined(HAVE_DECL_LOCK_WRITE) && HAVE_DECL_LOCK_WRITE)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((LOCK_WRITE) == (128), "LOCK_WRITE != 128");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define LOCK_WRITE 128
#endif

#ifndef XLAT_MACROS_ONLY

# ifdef IN_MPERS

#  error static const struct xlat flockcmds in mpers mode

# else

static const struct xlat_data flockcmds_xdata[] = {
 XLAT(LOCK_SH),
 #define XLAT_VAL_0 ((unsigned) (LOCK_SH))
 #define XLAT_STR_0 STRINGIFY(LOCK_SH)
 XLAT(LOCK_EX),
 #define XLAT_VAL_1 ((unsigned) (LOCK_EX))
 #define XLAT_STR_1 STRINGIFY(LOCK_EX)
 XLAT(LOCK_NB),
 #define XLAT_VAL_2 ((unsigned) (LOCK_NB))
 #define XLAT_STR_2 STRINGIFY(LOCK_NB)
 XLAT(LOCK_UN),
 #define XLAT_VAL_3 ((unsigned) (LOCK_UN))
 #define XLAT_STR_3 STRINGIFY(LOCK_UN)
 XLAT(LOCK_MAND),
 #define XLAT_VAL_4 ((unsigned) (LOCK_MAND))
 #define XLAT_STR_4 STRINGIFY(LOCK_MAND)
 XLAT(LOCK_RW),
 #define XLAT_VAL_5 ((unsigned) (LOCK_RW))
 #define XLAT_STR_5 STRINGIFY(LOCK_RW)
 XLAT(LOCK_READ),
 #define XLAT_VAL_6 ((unsigned) (LOCK_READ))
 #define XLAT_STR_6 STRINGIFY(LOCK_READ)
 XLAT(LOCK_WRITE),
 #define XLAT_VAL_7 ((unsigned) (LOCK_WRITE))
 #define XLAT_STR_7 STRINGIFY(LOCK_WRITE)
};
static
const struct xlat flockcmds[1] = { {
 .data = flockcmds_xdata,
 .size = ARRAY_SIZE(flockcmds_xdata),
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
