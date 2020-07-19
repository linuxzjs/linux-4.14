/* Generated by ./xlat/gen.sh from ./xlat/packet_mreq_type.in; do not edit. */

#include "gcc_compat.h"
#include "static_assert.h"


#ifndef XLAT_MACROS_ONLY

# ifdef IN_MPERS

#  error static const struct xlat packet_mreq_type in mpers mode

# else

static const struct xlat_data packet_mreq_type_xdata[] = {
#if defined(PACKET_MR_MULTICAST) || (defined(HAVE_DECL_PACKET_MR_MULTICAST) && HAVE_DECL_PACKET_MR_MULTICAST)
  XLAT(PACKET_MR_MULTICAST),
 #define XLAT_VAL_0 ((unsigned) (PACKET_MR_MULTICAST))
 #define XLAT_STR_0 STRINGIFY(PACKET_MR_MULTICAST)
#endif
#if defined(PACKET_MR_PROMISC) || (defined(HAVE_DECL_PACKET_MR_PROMISC) && HAVE_DECL_PACKET_MR_PROMISC)
  XLAT(PACKET_MR_PROMISC),
 #define XLAT_VAL_1 ((unsigned) (PACKET_MR_PROMISC))
 #define XLAT_STR_1 STRINGIFY(PACKET_MR_PROMISC)
#endif
#if defined(PACKET_MR_ALLMULTI) || (defined(HAVE_DECL_PACKET_MR_ALLMULTI) && HAVE_DECL_PACKET_MR_ALLMULTI)
  XLAT(PACKET_MR_ALLMULTI),
 #define XLAT_VAL_2 ((unsigned) (PACKET_MR_ALLMULTI))
 #define XLAT_STR_2 STRINGIFY(PACKET_MR_ALLMULTI)
#endif
#if defined(PACKET_MR_UNICAST) || (defined(HAVE_DECL_PACKET_MR_UNICAST) && HAVE_DECL_PACKET_MR_UNICAST)
  XLAT(PACKET_MR_UNICAST),
 #define XLAT_VAL_3 ((unsigned) (PACKET_MR_UNICAST))
 #define XLAT_STR_3 STRINGIFY(PACKET_MR_UNICAST)
#endif
};
static
const struct xlat packet_mreq_type[1] = { {
 .data = packet_mreq_type_xdata,
 .size = ARRAY_SIZE(packet_mreq_type_xdata),
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