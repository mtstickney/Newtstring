#ifndef NETSTRING_H
#define NETSTRING_H

struct msg
{
	size_t len;
	char *buf;
};

struct msg mk_msg(struct msg);
struct msg read_msg(struct msg);

#endif /* NETSTRING_H */