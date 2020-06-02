/* Generated by ./xlat/gen.sh from ./xlat/v4l2_frameinterval_types.in; do not edit. */

#include "gcc_compat.h"
#include "static_assert.h"

#if defined(V4L2_FRMIVAL_TYPE_DISCRETE) || (defined(HAVE_DECL_V4L2_FRMIVAL_TYPE_DISCRETE) && HAVE_DECL_V4L2_FRMIVAL_TYPE_DISCRETE)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((V4L2_FRMIVAL_TYPE_DISCRETE) == (1), "V4L2_FRMIVAL_TYPE_DISCRETE != 1");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define V4L2_FRMIVAL_TYPE_DISCRETE 1
#endif
#if defined(V4L2_FRMIVAL_TYPE_CONTINUOUS) || (defined(HAVE_DECL_V4L2_FRMIVAL_TYPE_CONTINUOUS) && HAVE_DECL_V4L2_FRMIVAL_TYPE_CONTINUOUS)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((V4L2_FRMIVAL_TYPE_CONTINUOUS) == (2), "V4L2_FRMIVAL_TYPE_CONTINUOUS != 2");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define V4L2_FRMIVAL_TYPE_CONTINUOUS 2
#endif
#if defined(V4L2_FRMIVAL_TYPE_STEPWISE) || (defined(HAVE_DECL_V4L2_FRMIVAL_TYPE_STEPWISE) && HAVE_DECL_V4L2_FRMIVAL_TYPE_STEPWISE)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((V4L2_FRMIVAL_TYPE_STEPWISE) == (3), "V4L2_FRMIVAL_TYPE_STEPWISE != 3");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define V4L2_FRMIVAL_TYPE_STEPWISE 3
#endif

#ifndef XLAT_MACROS_ONLY

# ifdef IN_MPERS

extern const struct xlat v4l2_frameinterval_types[];

# else

static const struct xlat_data v4l2_frameinterval_types_xdata[] = {
 [V4L2_FRMIVAL_TYPE_DISCRETE] = XLAT(V4L2_FRMIVAL_TYPE_DISCRETE),
 #define XLAT_VAL_0 ((unsigned) (V4L2_FRMIVAL_TYPE_DISCRETE))
 #define XLAT_STR_0 STRINGIFY(V4L2_FRMIVAL_TYPE_DISCRETE)
 [V4L2_FRMIVAL_TYPE_CONTINUOUS] = XLAT(V4L2_FRMIVAL_TYPE_CONTINUOUS),
 #define XLAT_VAL_1 ((unsigned) (V4L2_FRMIVAL_TYPE_CONTINUOUS))
 #define XLAT_STR_1 STRINGIFY(V4L2_FRMIVAL_TYPE_CONTINUOUS)
 [V4L2_FRMIVAL_TYPE_STEPWISE] = XLAT(V4L2_FRMIVAL_TYPE_STEPWISE),
 #define XLAT_VAL_2 ((unsigned) (V4L2_FRMIVAL_TYPE_STEPWISE))
 #define XLAT_STR_2 STRINGIFY(V4L2_FRMIVAL_TYPE_STEPWISE)
};
#  if !(defined HAVE_M32_MPERS || defined HAVE_MX32_MPERS)
static
#  endif
const struct xlat v4l2_frameinterval_types[1] = { {
 .data = v4l2_frameinterval_types_xdata,
 .size = ARRAY_SIZE(v4l2_frameinterval_types_xdata),
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
