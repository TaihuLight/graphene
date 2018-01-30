#ifndef _LINUX_GRAPHENE_H
#define _LINUX_GRAPHENE_H

#include <asm/types.h>
#include <linux/ioctl.h>
#include <linux/in.h>
#include <linux/in6.h>

/* comment out this line to turn off debugging */
//#define GRAPHENE_DEBUG

#define GRAPHENE_UNIX_PREFIX_DIR	"@/graphene/"
#define GRAPHENE_UNIX_PREFIX_FMT	GRAPHENE_UNIX_PREFIX_DIR "%08x/"
#define GRAPHENE_UNIX_PREFIX_SIZE	(sizeof(GRAPHENE_UNIX_PREFIX_DIR) + 8)
#define GRAPHENE_MCAST_GROUP	"239.0.0.1"

#define GRM_SET_SANDBOX		_IOW('k', 16, void *)

/* policy types (up to 32bits) */
#define GRAPHENE_UNIX_PREFIX	0000
#define GRAPHENE_MCAST_PORT	0001
#define GRAPHENE_FS_PATH	0002
#define GRAPHENE_NET_RULE	0003

#define GRAPHENE_POLICY_TYPES	0007

#define GRAPHENE_FS_RECURSIVE	0010
#define GRAPHENE_FS_READ	0020
#define GRAPHENE_FS_WRITE	0040

#define GRAPHENE_NET_BIND	0100

typedef __u32 graphene_policy_type_t;

struct graphene_user_policy {
	graphene_policy_type_t	type;
	const void *		value;
};

struct graphene_net_addr {
	union {
		struct in_addr 		sin_addr;
		struct in6_addr		sin6_addr;
	} addr;
	unsigned short		port_begin;
	unsigned short		port_end;
};

struct graphene_net_rule {
	unsigned short			family;
	struct graphene_net_addr	addr;
};

struct graphene_policies {
	size_t				npolicies;
	struct graphene_user_policy	policies[];
};

#ifdef __KERNEL__

#include <linux/types.h>
#include <linux/sched.h>
#include <linux/net.h>
#include <linux/list.h>
#include <linux/spinlock.h>

struct task_struct;
struct path;
struct qstr;

struct graphene_path {
	struct list_head	list;
	struct filename *	path;
	size_t			path_len;
	graphene_policy_type_t	type;
};

#define ADDR_ANY		0x1
#define PORT_ANY		0x2

struct graphene_net {
	struct list_head	list;
	short			family;
	unsigned char		flags;
	struct graphene_net_addr	addr;
};

struct graphene_unix {
	atomic_t		count;
	struct list_head	list;
	struct path		root;
	struct qstr		prefix;
};

struct graphene_info {
	atomic_t		gi_count;
	u32			gi_sid;
	struct filename *	gi_loader_name;
	char			gi_unix[GRAPHENE_UNIX_PREFIX_SIZE + 1];
	struct list_head	gi_paths;
	struct list_head	gi_rpaths;
	struct list_head	gi_binds;
	struct list_head	gi_peers;
	unsigned short		gi_mcast_port;
	struct file *		gi_mcast_sock;
};

int check_open_path(struct graphene_info *gi, const char *path, int flags);

int check_stat_path(struct graphene_info *gi, const char *path);

int check_bind_addr(struct graphene_info *gi, struct socket *sock,
		     struct sockaddr *addr, size_t addrlen);

int check_connect_addr(struct graphene_info *gi, struct socket *sock,
		        struct sockaddr *addr, size_t addrlen);

int set_sandbox(struct file *file,
		const struct graphene_policies __user *gpolicies);

#endif /* __KERNEL__ */

#endif
