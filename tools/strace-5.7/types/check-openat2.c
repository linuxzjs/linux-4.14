/* Generated by ./types/gen.sh from ./types/openat2.h; do not edit. */
#include "defs.h"
#include "static_assert.h"
#include "types/openat2.h"

#ifdef HAVE_STRUCT_OPEN_HOW
# ifdef HAVE_STRUCT_OPEN_HOW_RESOLVE
static_assert(sizeof(struct open_how) == sizeof(struct_open_how),
      "struct open_how size mismatch, please update the decoder or fix the kernel");
# else
static_assert(sizeof(struct open_how) <= offsetof(struct_open_how, resolve),
"struct open_how size mismatch, please update the decoder or fix the kernel");
# endif /* HAVE_STRUCT_OPEN_HOW_RESOLVE */
#endif /* HAVE_STRUCT_OPEN_HOW */