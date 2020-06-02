/* Generated by ./xlat/gen.sh from ./xlat/mmap_prot.in; do not edit. */

#include "gcc_compat.h"
#include "static_assert.h"


#ifndef XLAT_MACROS_ONLY

# ifdef IN_MPERS

#  error static const struct xlat mmap_prot in mpers mode

# else

static const struct xlat_data mmap_prot_xdata[] = {
#if defined(PROT_NONE) || (defined(HAVE_DECL_PROT_NONE) && HAVE_DECL_PROT_NONE)
  XLAT(PROT_NONE),
 #define XLAT_VAL_0 ((unsigned) (PROT_NONE))
 #define XLAT_STR_0 STRINGIFY(PROT_NONE)
#endif
#if defined(PROT_READ) || (defined(HAVE_DECL_PROT_READ) && HAVE_DECL_PROT_READ)
  XLAT(PROT_READ),
 #define XLAT_VAL_1 ((unsigned) (PROT_READ))
 #define XLAT_STR_1 STRINGIFY(PROT_READ)
#endif
#if defined(PROT_WRITE) || (defined(HAVE_DECL_PROT_WRITE) && HAVE_DECL_PROT_WRITE)
  XLAT(PROT_WRITE),
 #define XLAT_VAL_2 ((unsigned) (PROT_WRITE))
 #define XLAT_STR_2 STRINGIFY(PROT_WRITE)
#endif
#if defined(PROT_EXEC) || (defined(HAVE_DECL_PROT_EXEC) && HAVE_DECL_PROT_EXEC)
  XLAT(PROT_EXEC),
 #define XLAT_VAL_3 ((unsigned) (PROT_EXEC))
 #define XLAT_STR_3 STRINGIFY(PROT_EXEC)
#endif
#if defined(PROT_SEM) || (defined(HAVE_DECL_PROT_SEM) && HAVE_DECL_PROT_SEM)
  XLAT(PROT_SEM),
 #define XLAT_VAL_4 ((unsigned) (PROT_SEM))
 #define XLAT_STR_4 STRINGIFY(PROT_SEM)
#endif
#if defined(PROT_GROWSDOWN) || (defined(HAVE_DECL_PROT_GROWSDOWN) && HAVE_DECL_PROT_GROWSDOWN)
  XLAT(PROT_GROWSDOWN),
 #define XLAT_VAL_5 ((unsigned) (PROT_GROWSDOWN))
 #define XLAT_STR_5 STRINGIFY(PROT_GROWSDOWN)
#endif
#if defined(PROT_GROWSUP) || (defined(HAVE_DECL_PROT_GROWSUP) && HAVE_DECL_PROT_GROWSUP)
  XLAT(PROT_GROWSUP),
 #define XLAT_VAL_6 ((unsigned) (PROT_GROWSUP))
 #define XLAT_STR_6 STRINGIFY(PROT_GROWSUP)
#endif
#if defined(PROT_SAO) || (defined(HAVE_DECL_PROT_SAO) && HAVE_DECL_PROT_SAO)
  XLAT(PROT_SAO),
 #define XLAT_VAL_7 ((unsigned) (PROT_SAO))
 #define XLAT_STR_7 STRINGIFY(PROT_SAO)
#endif
#if defined(PROT_ADI) || (defined(HAVE_DECL_PROT_ADI) && HAVE_DECL_PROT_ADI)
  XLAT(PROT_ADI),
 #define XLAT_VAL_8 ((unsigned) (PROT_ADI))
 #define XLAT_STR_8 STRINGIFY(PROT_ADI)
#endif
};
static
const struct xlat mmap_prot[1] = { {
 .data = mmap_prot_xdata,
 .size = ARRAY_SIZE(mmap_prot_xdata),
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
