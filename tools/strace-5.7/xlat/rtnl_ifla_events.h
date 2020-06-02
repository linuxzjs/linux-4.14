/* Generated by ./xlat/gen.sh from ./xlat/rtnl_ifla_events.in; do not edit. */

#include "gcc_compat.h"
#include "static_assert.h"

#if defined(IFLA_EVENT_NONE) || (defined(HAVE_DECL_IFLA_EVENT_NONE) && HAVE_DECL_IFLA_EVENT_NONE)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((IFLA_EVENT_NONE) == (0), "IFLA_EVENT_NONE != 0");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define IFLA_EVENT_NONE 0
#endif
#if defined(IFLA_EVENT_REBOOT) || (defined(HAVE_DECL_IFLA_EVENT_REBOOT) && HAVE_DECL_IFLA_EVENT_REBOOT)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((IFLA_EVENT_REBOOT) == (1), "IFLA_EVENT_REBOOT != 1");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define IFLA_EVENT_REBOOT 1
#endif
#if defined(IFLA_EVENT_FEATURES) || (defined(HAVE_DECL_IFLA_EVENT_FEATURES) && HAVE_DECL_IFLA_EVENT_FEATURES)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((IFLA_EVENT_FEATURES) == (2), "IFLA_EVENT_FEATURES != 2");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define IFLA_EVENT_FEATURES 2
#endif
#if defined(IFLA_EVENT_BONDING_FAILOVER) || (defined(HAVE_DECL_IFLA_EVENT_BONDING_FAILOVER) && HAVE_DECL_IFLA_EVENT_BONDING_FAILOVER)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((IFLA_EVENT_BONDING_FAILOVER) == (3), "IFLA_EVENT_BONDING_FAILOVER != 3");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define IFLA_EVENT_BONDING_FAILOVER 3
#endif
#if defined(IFLA_EVENT_NOTIFY_PEERS) || (defined(HAVE_DECL_IFLA_EVENT_NOTIFY_PEERS) && HAVE_DECL_IFLA_EVENT_NOTIFY_PEERS)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((IFLA_EVENT_NOTIFY_PEERS) == (4), "IFLA_EVENT_NOTIFY_PEERS != 4");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define IFLA_EVENT_NOTIFY_PEERS 4
#endif
#if defined(IFLA_EVENT_IGMP_RESEND) || (defined(HAVE_DECL_IFLA_EVENT_IGMP_RESEND) && HAVE_DECL_IFLA_EVENT_IGMP_RESEND)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((IFLA_EVENT_IGMP_RESEND) == (5), "IFLA_EVENT_IGMP_RESEND != 5");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define IFLA_EVENT_IGMP_RESEND 5
#endif
#if defined(IFLA_EVENT_BONDING_OPTIONS) || (defined(HAVE_DECL_IFLA_EVENT_BONDING_OPTIONS) && HAVE_DECL_IFLA_EVENT_BONDING_OPTIONS)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((IFLA_EVENT_BONDING_OPTIONS) == (6), "IFLA_EVENT_BONDING_OPTIONS != 6");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define IFLA_EVENT_BONDING_OPTIONS 6
#endif

#ifndef XLAT_MACROS_ONLY

# ifdef IN_MPERS

#  error static const struct xlat rtnl_ifla_events in mpers mode

# else

static const struct xlat_data rtnl_ifla_events_xdata[] = {
 [IFLA_EVENT_NONE] = XLAT(IFLA_EVENT_NONE),
 #define XLAT_VAL_0 ((unsigned) (IFLA_EVENT_NONE))
 #define XLAT_STR_0 STRINGIFY(IFLA_EVENT_NONE)
 [IFLA_EVENT_REBOOT] = XLAT(IFLA_EVENT_REBOOT),
 #define XLAT_VAL_1 ((unsigned) (IFLA_EVENT_REBOOT))
 #define XLAT_STR_1 STRINGIFY(IFLA_EVENT_REBOOT)
 [IFLA_EVENT_FEATURES] = XLAT(IFLA_EVENT_FEATURES),
 #define XLAT_VAL_2 ((unsigned) (IFLA_EVENT_FEATURES))
 #define XLAT_STR_2 STRINGIFY(IFLA_EVENT_FEATURES)
 [IFLA_EVENT_BONDING_FAILOVER] = XLAT(IFLA_EVENT_BONDING_FAILOVER),
 #define XLAT_VAL_3 ((unsigned) (IFLA_EVENT_BONDING_FAILOVER))
 #define XLAT_STR_3 STRINGIFY(IFLA_EVENT_BONDING_FAILOVER)
 [IFLA_EVENT_NOTIFY_PEERS] = XLAT(IFLA_EVENT_NOTIFY_PEERS),
 #define XLAT_VAL_4 ((unsigned) (IFLA_EVENT_NOTIFY_PEERS))
 #define XLAT_STR_4 STRINGIFY(IFLA_EVENT_NOTIFY_PEERS)
 [IFLA_EVENT_IGMP_RESEND] = XLAT(IFLA_EVENT_IGMP_RESEND),
 #define XLAT_VAL_5 ((unsigned) (IFLA_EVENT_IGMP_RESEND))
 #define XLAT_STR_5 STRINGIFY(IFLA_EVENT_IGMP_RESEND)
 [IFLA_EVENT_BONDING_OPTIONS] = XLAT(IFLA_EVENT_BONDING_OPTIONS),
 #define XLAT_VAL_6 ((unsigned) (IFLA_EVENT_BONDING_OPTIONS))
 #define XLAT_STR_6 STRINGIFY(IFLA_EVENT_BONDING_OPTIONS)
};
static
const struct xlat rtnl_ifla_events[1] = { {
 .data = rtnl_ifla_events_xdata,
 .size = ARRAY_SIZE(rtnl_ifla_events_xdata),
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
# endif /* !IN_MPERS */

#endif /* !XLAT_MACROS_ONLY */
