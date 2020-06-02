/* Generated by ./xlat/gen.sh from ./xlat/bluetooth_l2_psm.in; do not edit. */

#include "gcc_compat.h"
#include "static_assert.h"

#if defined(L2CAP_PSM_SDP) || (defined(HAVE_DECL_L2CAP_PSM_SDP) && HAVE_DECL_L2CAP_PSM_SDP)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((L2CAP_PSM_SDP) == (0x0001), "L2CAP_PSM_SDP != 0x0001");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define L2CAP_PSM_SDP 0x0001
#endif
#if defined(L2CAP_PSM_RFCOMM) || (defined(HAVE_DECL_L2CAP_PSM_RFCOMM) && HAVE_DECL_L2CAP_PSM_RFCOMM)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((L2CAP_PSM_RFCOMM) == (0x0003), "L2CAP_PSM_RFCOMM != 0x0003");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define L2CAP_PSM_RFCOMM 0x0003
#endif
#if defined(L2CAP_PSM_3DSP) || (defined(HAVE_DECL_L2CAP_PSM_3DSP) && HAVE_DECL_L2CAP_PSM_3DSP)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((L2CAP_PSM_3DSP) == (0x0021), "L2CAP_PSM_3DSP != 0x0021");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define L2CAP_PSM_3DSP 0x0021
#endif
#if defined(L2CAP_PSM_IPSP) || (defined(HAVE_DECL_L2CAP_PSM_IPSP) && HAVE_DECL_L2CAP_PSM_IPSP)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((L2CAP_PSM_IPSP) == (0x0023), "L2CAP_PSM_IPSP != 0x0023");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define L2CAP_PSM_IPSP 0x0023
#endif
#if defined(L2CAP_PSM_LE_DYN_START) || (defined(HAVE_DECL_L2CAP_PSM_LE_DYN_START) && HAVE_DECL_L2CAP_PSM_LE_DYN_START)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((L2CAP_PSM_LE_DYN_START) == (0x0080), "L2CAP_PSM_LE_DYN_START != 0x0080");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define L2CAP_PSM_LE_DYN_START 0x0080
#endif
#if defined(L2CAP_PSM_LE_DYN_END) || (defined(HAVE_DECL_L2CAP_PSM_LE_DYN_END) && HAVE_DECL_L2CAP_PSM_LE_DYN_END)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((L2CAP_PSM_LE_DYN_END) == (0x00ff), "L2CAP_PSM_LE_DYN_END != 0x00ff");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define L2CAP_PSM_LE_DYN_END 0x00ff
#endif
#if defined(L2CAP_PSM_DYN_START) || (defined(HAVE_DECL_L2CAP_PSM_DYN_START) && HAVE_DECL_L2CAP_PSM_DYN_START)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((L2CAP_PSM_DYN_START) == (0x1001), "L2CAP_PSM_DYN_START != 0x1001");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define L2CAP_PSM_DYN_START 0x1001
#endif
#if defined(L2CAP_PSM_AUTO_END) || (defined(HAVE_DECL_L2CAP_PSM_AUTO_END) && HAVE_DECL_L2CAP_PSM_AUTO_END)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((L2CAP_PSM_AUTO_END) == (0x10ff), "L2CAP_PSM_AUTO_END != 0x10ff");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define L2CAP_PSM_AUTO_END 0x10ff
#endif
#if defined(L2CAP_PSM_DYN_END) || (defined(HAVE_DECL_L2CAP_PSM_DYN_END) && HAVE_DECL_L2CAP_PSM_DYN_END)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((L2CAP_PSM_DYN_END) == (0xffff), "L2CAP_PSM_DYN_END != 0xffff");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define L2CAP_PSM_DYN_END 0xffff
#endif

#ifndef XLAT_MACROS_ONLY

# ifdef IN_MPERS

#  error static const struct xlat bluetooth_l2_psm in mpers mode

# else

static const struct xlat_data bluetooth_l2_psm_xdata[] = {
 XLAT(L2CAP_PSM_SDP),
 #define XLAT_VAL_0 ((unsigned) (L2CAP_PSM_SDP))
 #define XLAT_STR_0 STRINGIFY(L2CAP_PSM_SDP)
 XLAT(L2CAP_PSM_RFCOMM),
 #define XLAT_VAL_1 ((unsigned) (L2CAP_PSM_RFCOMM))
 #define XLAT_STR_1 STRINGIFY(L2CAP_PSM_RFCOMM)
 XLAT(L2CAP_PSM_3DSP),
 #define XLAT_VAL_2 ((unsigned) (L2CAP_PSM_3DSP))
 #define XLAT_STR_2 STRINGIFY(L2CAP_PSM_3DSP)
 XLAT(L2CAP_PSM_IPSP),
 #define XLAT_VAL_3 ((unsigned) (L2CAP_PSM_IPSP))
 #define XLAT_STR_3 STRINGIFY(L2CAP_PSM_IPSP)
 XLAT(L2CAP_PSM_LE_DYN_START),
 #define XLAT_VAL_4 ((unsigned) (L2CAP_PSM_LE_DYN_START))
 #define XLAT_STR_4 STRINGIFY(L2CAP_PSM_LE_DYN_START)
 XLAT(L2CAP_PSM_LE_DYN_END),
 #define XLAT_VAL_5 ((unsigned) (L2CAP_PSM_LE_DYN_END))
 #define XLAT_STR_5 STRINGIFY(L2CAP_PSM_LE_DYN_END)
 XLAT(L2CAP_PSM_DYN_START),
 #define XLAT_VAL_6 ((unsigned) (L2CAP_PSM_DYN_START))
 #define XLAT_STR_6 STRINGIFY(L2CAP_PSM_DYN_START)
 XLAT(L2CAP_PSM_AUTO_END),
 #define XLAT_VAL_7 ((unsigned) (L2CAP_PSM_AUTO_END))
 #define XLAT_STR_7 STRINGIFY(L2CAP_PSM_AUTO_END)
 XLAT(L2CAP_PSM_DYN_END),
 #define XLAT_VAL_8 ((unsigned) (L2CAP_PSM_DYN_END))
 #define XLAT_STR_8 STRINGIFY(L2CAP_PSM_DYN_END)
};
static
const struct xlat bluetooth_l2_psm[1] = { {
 .data = bluetooth_l2_psm_xdata,
 .size = ARRAY_SIZE(bluetooth_l2_psm_xdata),
 .type = XT_SORTED,
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
# endif /* !IN_MPERS */

#endif /* !XLAT_MACROS_ONLY */
