dnl Generated by ./xlat/gen.sh from ./xlat/v4l2_control_types.in; do not edit.
AC_DEFUN([st_CHECK_ENUMS_v4l2_control_types],[
AC_CHECK_DECLS(m4_normalize([
V4L2_CTRL_TYPE_INTEGER,
V4L2_CTRL_TYPE_BOOLEAN,
V4L2_CTRL_TYPE_MENU,
V4L2_CTRL_TYPE_BUTTON,
V4L2_CTRL_TYPE_INTEGER64,
V4L2_CTRL_TYPE_CTRL_CLASS,
V4L2_CTRL_TYPE_STRING,
V4L2_CTRL_TYPE_BITMASK,
V4L2_CTRL_TYPE_INTEGER_MENU,
V4L2_CTRL_TYPE_U8,
V4L2_CTRL_TYPE_U16,
V4L2_CTRL_TYPE_U32,
V4L2_CTRL_TYPE_AREA
]),,, [
#include <sys/time.h>
#include <linux/ioctl.h>
#include <linux/types.h>
#include <linux/videodev2.h>
])])])