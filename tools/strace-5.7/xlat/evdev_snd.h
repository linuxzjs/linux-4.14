/* Generated by ./xlat/gen.sh from ./xlat/evdev_snd.in; do not edit. */

#include "gcc_compat.h"
#include "static_assert.h"

#if defined(SND_CLICK) || (defined(HAVE_DECL_SND_CLICK) && HAVE_DECL_SND_CLICK)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((SND_CLICK) == (0x00), "SND_CLICK != 0x00");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define SND_CLICK 0x00
#endif
#if defined(SND_BELL) || (defined(HAVE_DECL_SND_BELL) && HAVE_DECL_SND_BELL)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((SND_BELL) == (0x01), "SND_BELL != 0x01");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define SND_BELL 0x01
#endif
#if defined(SND_TONE) || (defined(HAVE_DECL_SND_TONE) && HAVE_DECL_SND_TONE)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((SND_TONE) == (0x02), "SND_TONE != 0x02");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define SND_TONE 0x02
#endif

#ifndef XLAT_MACROS_ONLY

# ifdef IN_MPERS

#  error static const struct xlat evdev_snd in mpers mode

# else

static const struct xlat_data evdev_snd_xdata[] = {
 [SND_CLICK] = XLAT(SND_CLICK),
 #define XLAT_VAL_0 ((unsigned) (SND_CLICK))
 #define XLAT_STR_0 STRINGIFY(SND_CLICK)
 [SND_BELL] = XLAT(SND_BELL),
 #define XLAT_VAL_1 ((unsigned) (SND_BELL))
 #define XLAT_STR_1 STRINGIFY(SND_BELL)
 [SND_TONE] = XLAT(SND_TONE),
 #define XLAT_VAL_2 ((unsigned) (SND_TONE))
 #define XLAT_STR_2 STRINGIFY(SND_TONE)
};
static
const struct xlat evdev_snd[1] = { {
 .data = evdev_snd_xdata,
 .size = ARRAY_SIZE(evdev_snd_xdata),
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