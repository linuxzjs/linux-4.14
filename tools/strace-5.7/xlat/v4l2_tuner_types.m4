dnl Generated by ./xlat/gen.sh from ./xlat/v4l2_tuner_types.in; do not edit.
AC_DEFUN([st_CHECK_ENUMS_v4l2_tuner_types],[
AC_CHECK_DECLS(m4_normalize([
V4L2_TUNER_RADIO,
V4L2_TUNER_ANALOG_TV,
V4L2_TUNER_DIGITAL_TV,
V4L2_TUNER_SDR,
V4L2_TUNER_RF
]),,, [
#include <sys/time.h>
#include <linux/ioctl.h>
#include <linux/types.h>
#include <linux/videodev2.h>
])])])
