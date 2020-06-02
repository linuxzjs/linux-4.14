/* Generated by ./xlat/gen.sh from ./xlat/btrfs_features_compat_ro.in; do not edit. */

#include "gcc_compat.h"
#include "static_assert.h"

#if defined(BTRFS_FEATURE_COMPAT_RO_FREE_SPACE_TREE) || (defined(HAVE_DECL_BTRFS_FEATURE_COMPAT_RO_FREE_SPACE_TREE) && HAVE_DECL_BTRFS_FEATURE_COMPAT_RO_FREE_SPACE_TREE)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((BTRFS_FEATURE_COMPAT_RO_FREE_SPACE_TREE) == ((1ULL << 0)), "BTRFS_FEATURE_COMPAT_RO_FREE_SPACE_TREE != (1ULL << 0)");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define BTRFS_FEATURE_COMPAT_RO_FREE_SPACE_TREE (1ULL << 0)
#endif
#if defined(BTRFS_FEATURE_COMPAT_RO_FREE_SPACE_TREE_VALID) || (defined(HAVE_DECL_BTRFS_FEATURE_COMPAT_RO_FREE_SPACE_TREE_VALID) && HAVE_DECL_BTRFS_FEATURE_COMPAT_RO_FREE_SPACE_TREE_VALID)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((BTRFS_FEATURE_COMPAT_RO_FREE_SPACE_TREE_VALID) == ((1ULL << 1)), "BTRFS_FEATURE_COMPAT_RO_FREE_SPACE_TREE_VALID != (1ULL << 1)");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define BTRFS_FEATURE_COMPAT_RO_FREE_SPACE_TREE_VALID (1ULL << 1)
#endif

#ifndef XLAT_MACROS_ONLY

# ifdef IN_MPERS

extern const struct xlat btrfs_features_compat_ro[];

# else

static const struct xlat_data btrfs_features_compat_ro_xdata[] = {
 XLAT_TYPE(uint64_t, BTRFS_FEATURE_COMPAT_RO_FREE_SPACE_TREE),
 #define XLAT_VAL_0 ((uint64_t) (BTRFS_FEATURE_COMPAT_RO_FREE_SPACE_TREE))
 #define XLAT_STR_0 STRINGIFY(BTRFS_FEATURE_COMPAT_RO_FREE_SPACE_TREE)
 XLAT_TYPE(uint64_t, BTRFS_FEATURE_COMPAT_RO_FREE_SPACE_TREE_VALID),
 #define XLAT_VAL_1 ((uint64_t) (BTRFS_FEATURE_COMPAT_RO_FREE_SPACE_TREE_VALID))
 #define XLAT_STR_1 STRINGIFY(BTRFS_FEATURE_COMPAT_RO_FREE_SPACE_TREE_VALID)
};
#  if !(defined HAVE_M32_MPERS || defined HAVE_MX32_MPERS)
static
#  endif
const struct xlat btrfs_features_compat_ro[1] = { {
 .data = btrfs_features_compat_ro_xdata,
 .size = ARRAY_SIZE(btrfs_features_compat_ro_xdata),
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
