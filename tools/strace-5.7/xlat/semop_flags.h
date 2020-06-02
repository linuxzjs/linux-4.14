/* Generated by ./xlat/gen.sh from ./xlat/semop_flags.in; do not edit. */

#include "gcc_compat.h"
#include "static_assert.h"


#ifndef XLAT_MACROS_ONLY

# ifdef IN_MPERS

#  error static const struct xlat semop_flags in mpers mode

# else

static const struct xlat_data semop_flags_xdata[] = {
#if defined(SEM_UNDO) || (defined(HAVE_DECL_SEM_UNDO) && HAVE_DECL_SEM_UNDO)
  XLAT(SEM_UNDO),
 #define XLAT_VAL_0 ((unsigned) (SEM_UNDO))
 #define XLAT_STR_0 STRINGIFY(SEM_UNDO)
#endif
#if defined(IPC_NOWAIT) || (defined(HAVE_DECL_IPC_NOWAIT) && HAVE_DECL_IPC_NOWAIT)
  XLAT(IPC_NOWAIT),
 #define XLAT_VAL_1 ((unsigned) (IPC_NOWAIT))
 #define XLAT_STR_1 STRINGIFY(IPC_NOWAIT)
#endif
};
static
const struct xlat semop_flags[1] = { {
 .data = semop_flags_xdata,
 .size = ARRAY_SIZE(semop_flags_xdata),
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
