dnl Generated by ./xlat/gen.sh from ./xlat/rtnl_link_attrs.in; do not edit.
AC_DEFUN([st_CHECK_ENUMS_rtnl_link_attrs],[
AC_CHECK_DECLS(m4_normalize([
IFLA_UNSPEC,
IFLA_ADDRESS,
IFLA_BROADCAST,
IFLA_IFNAME,
IFLA_MTU,
IFLA_LINK,
IFLA_QDISC,
IFLA_STATS,
IFLA_COST,
IFLA_PRIORITY,
IFLA_MASTER,
IFLA_WIRELESS,
IFLA_PROTINFO,
IFLA_TXQLEN,
IFLA_MAP,
IFLA_WEIGHT,
IFLA_OPERSTATE,
IFLA_LINKMODE,
IFLA_LINKINFO,
IFLA_NET_NS_PID,
IFLA_IFALIAS,
IFLA_NUM_VF,
IFLA_VFINFO_LIST,
IFLA_STATS64,
IFLA_VF_PORTS,
IFLA_PORT_SELF,
IFLA_AF_SPEC,
IFLA_GROUP,
IFLA_NET_NS_FD,
IFLA_EXT_MASK,
IFLA_PROMISCUITY,
IFLA_NUM_TX_QUEUES,
IFLA_NUM_RX_QUEUES,
IFLA_CARRIER,
IFLA_PHYS_PORT_ID,
IFLA_CARRIER_CHANGES,
IFLA_PHYS_SWITCH_ID,
IFLA_LINK_NETNSID,
IFLA_PHYS_PORT_NAME,
IFLA_PROTO_DOWN,
IFLA_GSO_MAX_SEGS,
IFLA_GSO_MAX_SIZE,
IFLA_PAD,
IFLA_XDP,
IFLA_EVENT,
IFLA_NEW_NETNSID,
IFLA_IF_NETNSID,
IFLA_CARRIER_UP_COUNT,
IFLA_CARRIER_DOWN_COUNT,
IFLA_NEW_IFINDEX,
IFLA_MIN_MTU,
IFLA_MAX_MTU,
IFLA_PROP_LIST,
IFLA_ALT_IFNAME,
IFLA_PERM_ADDRESS
]),,, [
#include <sys/socket.h>
#include <linux/rtnetlink.h>
])])])
