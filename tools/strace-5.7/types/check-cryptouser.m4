dnl Generated by ./types/gen.sh from ./types/cryptouser.h; do not edit.
AC_DEFUN([st_CHECK_TYPES_CRYPTOUSER],[
AC_CHECK_HEADERS([linux/cryptouser.h],[

AC_CHECK_TYPES([struct crypto_user_alg],
[AC_CHECK_MEMBERS([struct crypto_user_alg.cru_flags],,,[AC_INCLUDES_DEFAULT
#include <linux/cryptouser.h>])],,[AC_INCLUDES_DEFAULT
#include <linux/cryptouser.h>])

AC_CHECK_TYPES([struct crypto_report_hash],
[AC_CHECK_MEMBERS([struct crypto_report_hash.digestsize],,,[AC_INCLUDES_DEFAULT
#include <linux/cryptouser.h>])],,[AC_INCLUDES_DEFAULT
#include <linux/cryptouser.h>])

AC_CHECK_TYPES([struct crypto_report_cipher],
[AC_CHECK_MEMBERS([struct crypto_report_cipher.max_keysize],,,[AC_INCLUDES_DEFAULT
#include <linux/cryptouser.h>])],,[AC_INCLUDES_DEFAULT
#include <linux/cryptouser.h>])

AC_CHECK_TYPES([struct crypto_report_blkcipher],
[AC_CHECK_MEMBERS([struct crypto_report_blkcipher.ivsize],,,[AC_INCLUDES_DEFAULT
#include <linux/cryptouser.h>])],,[AC_INCLUDES_DEFAULT
#include <linux/cryptouser.h>])

AC_CHECK_TYPES([struct crypto_report_aead],
[AC_CHECK_MEMBERS([struct crypto_report_aead.ivsize],,,[AC_INCLUDES_DEFAULT
#include <linux/cryptouser.h>])],,[AC_INCLUDES_DEFAULT
#include <linux/cryptouser.h>])

AC_CHECK_TYPES([struct crypto_report_rng],
[AC_CHECK_MEMBERS([struct crypto_report_rng.seedsize],,,[AC_INCLUDES_DEFAULT
#include <linux/cryptouser.h>])],,[AC_INCLUDES_DEFAULT
#include <linux/cryptouser.h>])
],[],[AC_INCLUDES_DEFAULT
])])
