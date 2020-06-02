/* Generated by ./xlat/gen.sh from ./xlat/btrfs_dev_stats_flags.in; do not edit. */

#include "gcc_compat.h"
#include "static_assert.h"


#ifndef XLAT_MACROS_ONLY

# ifdef IN_MPERS

extern const struct xlat btrfs_dev_stats_flags[];

# else

static const struct xlat_data btrfs_dev_stats_flags_xdata[] = {
#if defined(BTRFS_DEV_STATS_RESET) || (defined(HAVE_DECL_BTRFS_DEV_STATS_RESET) && HAVE_DECL_BTRFS_DEV_STATS_RESET)
  XLAT_TYPE(uint64_t, BTRFS_DEV_STATS_RESET),
 #define XLAT_VAL_0 ((uint64_t) (BTRFS_DEV_STATS_RESET))
 #define XLAT_STR_0 STRINGIFY(BTRFS_DEV_STATS_RESET)
#endif
};
#  if !(defined HAVE_M32_MPERS || defined HAVE_MX32_MPERS)
static
#  endif
const struct xlat btrfs_dev_stats_flags[1] = { {
 .data = btrfs_dev_stats_flags_xdata,
 .size = ARRAY_SIZE(btrfs_dev_stats_flags_xdata),
 .type = XT_NORMAL,
 .flags_mask = 0
#  ifdef XLAT_VAL_0
  | XLAT_VAL_0
#  endif
  ,
 .flags_strsz = 0
#  ifdef XLAT_STR_0
  + sizeof(XLAT_STR_0)
#  endif
  ,
} };

#  undef XLAT_STR_0
#  undef XLAT_VAL_0
# endif /* !IN_MPERS */

#endif /* !XLAT_MACROS_ONLY */
