/* Generated by ./xlat/gen.sh from ./xlat/rtc_ioctl_cmds.in; do not edit. */

#include "gcc_compat.h"
#include "static_assert.h"

#if defined(RTC_AIE_ON) || (defined(HAVE_DECL_RTC_AIE_ON) && HAVE_DECL_RTC_AIE_ON)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((RTC_AIE_ON) == (_IO ('p', 0x01)), "RTC_AIE_ON != _IO ('p', 0x01)");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define RTC_AIE_ON _IO ('p', 0x01)
#endif
#if defined(RTC_AIE_OFF) || (defined(HAVE_DECL_RTC_AIE_OFF) && HAVE_DECL_RTC_AIE_OFF)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((RTC_AIE_OFF) == (_IO ('p', 0x02)), "RTC_AIE_OFF != _IO ('p', 0x02)");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define RTC_AIE_OFF _IO ('p', 0x02)
#endif
#if defined(RTC_UIE_ON) || (defined(HAVE_DECL_RTC_UIE_ON) && HAVE_DECL_RTC_UIE_ON)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((RTC_UIE_ON) == (_IO ('p', 0x03)), "RTC_UIE_ON != _IO ('p', 0x03)");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define RTC_UIE_ON _IO ('p', 0x03)
#endif
#if defined(RTC_UIE_OFF) || (defined(HAVE_DECL_RTC_UIE_OFF) && HAVE_DECL_RTC_UIE_OFF)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((RTC_UIE_OFF) == (_IO ('p', 0x04)), "RTC_UIE_OFF != _IO ('p', 0x04)");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define RTC_UIE_OFF _IO ('p', 0x04)
#endif
#if defined(RTC_PIE_ON) || (defined(HAVE_DECL_RTC_PIE_ON) && HAVE_DECL_RTC_PIE_ON)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((RTC_PIE_ON) == (_IO ('p', 0x05)), "RTC_PIE_ON != _IO ('p', 0x05)");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define RTC_PIE_ON _IO ('p', 0x05)
#endif
#if defined(RTC_PIE_OFF) || (defined(HAVE_DECL_RTC_PIE_OFF) && HAVE_DECL_RTC_PIE_OFF)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((RTC_PIE_OFF) == (_IO ('p', 0x06)), "RTC_PIE_OFF != _IO ('p', 0x06)");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define RTC_PIE_OFF _IO ('p', 0x06)
#endif
#if defined(RTC_WIE_ON) || (defined(HAVE_DECL_RTC_WIE_ON) && HAVE_DECL_RTC_WIE_ON)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((RTC_WIE_ON) == (_IO ('p', 0x0f)), "RTC_WIE_ON != _IO ('p', 0x0f)");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define RTC_WIE_ON _IO ('p', 0x0f)
#endif
#if defined(RTC_WIE_OFF) || (defined(HAVE_DECL_RTC_WIE_OFF) && HAVE_DECL_RTC_WIE_OFF)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((RTC_WIE_OFF) == (_IO ('p', 0x10)), "RTC_WIE_OFF != _IO ('p', 0x10)");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define RTC_WIE_OFF _IO ('p', 0x10)
#endif
#if defined(RTC_ALM_SET) || (defined(HAVE_DECL_RTC_ALM_SET) && HAVE_DECL_RTC_ALM_SET)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((RTC_ALM_SET) == (_IOW('p', 0x07, struct rtc_time)), "RTC_ALM_SET != _IOW('p', 0x07, struct rtc_time)");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define RTC_ALM_SET _IOW('p', 0x07, struct rtc_time)
#endif
#if defined(RTC_ALM_READ) || (defined(HAVE_DECL_RTC_ALM_READ) && HAVE_DECL_RTC_ALM_READ)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((RTC_ALM_READ) == (_IOR('p', 0x08, struct rtc_time)), "RTC_ALM_READ != _IOR('p', 0x08, struct rtc_time)");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define RTC_ALM_READ _IOR('p', 0x08, struct rtc_time)
#endif
#if defined(RTC_RD_TIME) || (defined(HAVE_DECL_RTC_RD_TIME) && HAVE_DECL_RTC_RD_TIME)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((RTC_RD_TIME) == (_IOR('p', 0x09, struct rtc_time)), "RTC_RD_TIME != _IOR('p', 0x09, struct rtc_time)");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define RTC_RD_TIME _IOR('p', 0x09, struct rtc_time)
#endif
#if defined(RTC_SET_TIME) || (defined(HAVE_DECL_RTC_SET_TIME) && HAVE_DECL_RTC_SET_TIME)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((RTC_SET_TIME) == (_IOW('p', 0x0a, struct rtc_time)), "RTC_SET_TIME != _IOW('p', 0x0a, struct rtc_time)");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define RTC_SET_TIME _IOW('p', 0x0a, struct rtc_time)
#endif
#if defined(RTC_WKALM_SET) || (defined(HAVE_DECL_RTC_WKALM_SET) && HAVE_DECL_RTC_WKALM_SET)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((RTC_WKALM_SET) == (_IOW('p', 0x0f, struct rtc_wkalrm)), "RTC_WKALM_SET != _IOW('p', 0x0f, struct rtc_wkalrm)");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define RTC_WKALM_SET _IOW('p', 0x0f, struct rtc_wkalrm)
#endif
#if defined(RTC_WKALM_RD) || (defined(HAVE_DECL_RTC_WKALM_RD) && HAVE_DECL_RTC_WKALM_RD)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((RTC_WKALM_RD) == (_IOR('p', 0x10, struct rtc_wkalrm)), "RTC_WKALM_RD != _IOR('p', 0x10, struct rtc_wkalrm)");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define RTC_WKALM_RD _IOR('p', 0x10, struct rtc_wkalrm)
#endif
#if defined(RTC_PLL_GET) || (defined(HAVE_DECL_RTC_PLL_GET) && HAVE_DECL_RTC_PLL_GET)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((RTC_PLL_GET) == (_IOR('p', 0x11, struct_rtc_pll_info)), "RTC_PLL_GET != _IOR('p', 0x11, struct_rtc_pll_info)");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define RTC_PLL_GET _IOR('p', 0x11, struct_rtc_pll_info)
#endif
#if defined(RTC_PLL_SET) || (defined(HAVE_DECL_RTC_PLL_SET) && HAVE_DECL_RTC_PLL_SET)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((RTC_PLL_SET) == (_IOW('p', 0x12, struct_rtc_pll_info)), "RTC_PLL_SET != _IOW('p', 0x12, struct_rtc_pll_info)");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define RTC_PLL_SET _IOW('p', 0x12, struct_rtc_pll_info)
#endif
#if defined(RTC_VL_READ) || (defined(HAVE_DECL_RTC_VL_READ) && HAVE_DECL_RTC_VL_READ)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((RTC_VL_READ) == (_IOR('p', 0x13, unsigned int)), "RTC_VL_READ != _IOR('p', 0x13, unsigned int)");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define RTC_VL_READ _IOR('p', 0x13, unsigned int)
#endif
#if defined(RTC_VL_CLR) || (defined(HAVE_DECL_RTC_VL_CLR) && HAVE_DECL_RTC_VL_CLR)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((RTC_VL_CLR) == (_IO ('p', 0x14)), "RTC_VL_CLR != _IO ('p', 0x14)");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define RTC_VL_CLR _IO ('p', 0x14)
#endif

#ifndef XLAT_MACROS_ONLY

# ifdef IN_MPERS

extern const struct xlat rtc_ioctl_cmds[];

# else

static const struct xlat_data rtc_ioctl_cmds_xdata[] = {
 XLAT(RTC_AIE_ON),
 #define XLAT_VAL_0 ((unsigned) (RTC_AIE_ON))
 #define XLAT_STR_0 STRINGIFY(RTC_AIE_ON)
 XLAT(RTC_AIE_OFF),
 #define XLAT_VAL_1 ((unsigned) (RTC_AIE_OFF))
 #define XLAT_STR_1 STRINGIFY(RTC_AIE_OFF)
 XLAT(RTC_UIE_ON),
 #define XLAT_VAL_2 ((unsigned) (RTC_UIE_ON))
 #define XLAT_STR_2 STRINGIFY(RTC_UIE_ON)
 XLAT(RTC_UIE_OFF),
 #define XLAT_VAL_3 ((unsigned) (RTC_UIE_OFF))
 #define XLAT_STR_3 STRINGIFY(RTC_UIE_OFF)
 XLAT(RTC_PIE_ON),
 #define XLAT_VAL_4 ((unsigned) (RTC_PIE_ON))
 #define XLAT_STR_4 STRINGIFY(RTC_PIE_ON)
 XLAT(RTC_PIE_OFF),
 #define XLAT_VAL_5 ((unsigned) (RTC_PIE_OFF))
 #define XLAT_STR_5 STRINGIFY(RTC_PIE_OFF)
 XLAT(RTC_WIE_ON),
 #define XLAT_VAL_6 ((unsigned) (RTC_WIE_ON))
 #define XLAT_STR_6 STRINGIFY(RTC_WIE_ON)
 XLAT(RTC_WIE_OFF),
 #define XLAT_VAL_7 ((unsigned) (RTC_WIE_OFF))
 #define XLAT_STR_7 STRINGIFY(RTC_WIE_OFF)
 XLAT(RTC_ALM_SET),
 #define XLAT_VAL_8 ((unsigned) (RTC_ALM_SET))
 #define XLAT_STR_8 STRINGIFY(RTC_ALM_SET)
 XLAT(RTC_ALM_READ),
 #define XLAT_VAL_9 ((unsigned) (RTC_ALM_READ))
 #define XLAT_STR_9 STRINGIFY(RTC_ALM_READ)
 XLAT(RTC_RD_TIME),
 #define XLAT_VAL_10 ((unsigned) (RTC_RD_TIME))
 #define XLAT_STR_10 STRINGIFY(RTC_RD_TIME)
 XLAT(RTC_SET_TIME),
 #define XLAT_VAL_11 ((unsigned) (RTC_SET_TIME))
 #define XLAT_STR_11 STRINGIFY(RTC_SET_TIME)




 XLAT(RTC_WKALM_SET),
 #define XLAT_VAL_12 ((unsigned) (RTC_WKALM_SET))
 #define XLAT_STR_12 STRINGIFY(RTC_WKALM_SET)
 XLAT(RTC_WKALM_RD),
 #define XLAT_VAL_13 ((unsigned) (RTC_WKALM_RD))
 #define XLAT_STR_13 STRINGIFY(RTC_WKALM_RD)
 XLAT(RTC_PLL_GET),
 #define XLAT_VAL_14 ((unsigned) (RTC_PLL_GET))
 #define XLAT_STR_14 STRINGIFY(RTC_PLL_GET)
 XLAT(RTC_PLL_SET),
 #define XLAT_VAL_15 ((unsigned) (RTC_PLL_SET))
 #define XLAT_STR_15 STRINGIFY(RTC_PLL_SET)
 XLAT(RTC_VL_READ),
 #define XLAT_VAL_16 ((unsigned) (RTC_VL_READ))
 #define XLAT_STR_16 STRINGIFY(RTC_VL_READ)
 XLAT(RTC_VL_CLR),
 #define XLAT_VAL_17 ((unsigned) (RTC_VL_CLR))
 #define XLAT_STR_17 STRINGIFY(RTC_VL_CLR)
};
#  if !(defined HAVE_M32_MPERS || defined HAVE_MX32_MPERS)
static
#  endif
const struct xlat rtc_ioctl_cmds[1] = { {
 .data = rtc_ioctl_cmds_xdata,
 .size = ARRAY_SIZE(rtc_ioctl_cmds_xdata),
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
#  ifdef XLAT_VAL_13
  | XLAT_VAL_13
#  endif
#  ifdef XLAT_VAL_14
  | XLAT_VAL_14
#  endif
#  ifdef XLAT_VAL_15
  | XLAT_VAL_15
#  endif
#  ifdef XLAT_VAL_16
  | XLAT_VAL_16
#  endif
#  ifdef XLAT_VAL_17
  | XLAT_VAL_17
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
#  ifdef XLAT_STR_13
  + sizeof(XLAT_STR_13)
#  endif
#  ifdef XLAT_STR_14
  + sizeof(XLAT_STR_14)
#  endif
#  ifdef XLAT_STR_15
  + sizeof(XLAT_STR_15)
#  endif
#  ifdef XLAT_STR_16
  + sizeof(XLAT_STR_16)
#  endif
#  ifdef XLAT_STR_17
  + sizeof(XLAT_STR_17)
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
#  undef XLAT_STR_13
#  undef XLAT_VAL_13
#  undef XLAT_STR_14
#  undef XLAT_VAL_14
#  undef XLAT_STR_15
#  undef XLAT_VAL_15
#  undef XLAT_STR_16
#  undef XLAT_VAL_16
#  undef XLAT_STR_17
#  undef XLAT_VAL_17
# endif /* !IN_MPERS */

#endif /* !XLAT_MACROS_ONLY */
