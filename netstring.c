#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "netstring.h"

#define LEN(a) (sizeof(a)/sizeof((a)[0]))
#define MAX_SZ_BYTES sizeof(size_t)

struct msg mk_msg(struct msg m) {
	struct msg ret;
	size_t alloc_sz;
	char sz_buf[MAX_SZ_BYTES+1] = {0};
	char *buf, *bufp;
	size_t i;

	snprintf(sz_buf, LEN(sz_buf), "%zX", m.len);

	/* Header + colon + newline + NUL */
	alloc_sz = strlen(sz_buf) + 1 + m.len + 2;
	
	if (alloc_sz <= m.len)  /* check for overflow */
		goto OVERFLOW;
	buf = malloc(alloc_sz);
	if (!buf)
		goto OVERFLOW;

	bufp = buf;
	i = strlen(sz_buf);
	memcpy(bufp, sz_buf, i);
	bufp += i;
	*bufp++ = ':';
	memcpy(bufp, m.buf, m.len);
	bufp += m.len;
	*bufp++ = '\n';
	*bufp = '\0';

	ret.len = alloc_sz;
	ret.buf = buf;
	return ret;

OVERFLOW:
	ret.len = 0;
	ret.buf = NULL;
	return ret;
}

static short hex_digit(char c) {
	if ('a' <= c && c <= 'f')
		return 10 + c - 'a';
	if ('A' <= c && c <= 'F')
		return 10 + c - 'A';
	if ('0' <= c && c <= '9')
		return c - '0';
	fprintf(stderr, "hex_digit: character '%c' is not a hex digit\n", c);
	return -1;
}

struct msg read_msg(struct msg m) {
	size_t max_sz_bytes = sizeof(size_t);
	struct msg nmsg;
	size_t i;
	short d;

	nmsg.len = 0;
	for (i=0; i < m.len && m.buf[i] != ':'; i++) {
		if (i > max_sz_bytes) {
			fprintf(stderr, "read_msg: message size overflows size_t\n");
			goto BAD_DIGIT;
		}
		d = hex_digit(m.buf[i]);
		if (d < 0)
			goto BAD_DIGIT;
		nmsg.len = nmsg.len*16 + d;
	}

	if (i >= m.len || m.buf[i] != ':') {
		fprintf(stderr, "read_msg: ran out of input looking for colon delimeter\n");
		goto BAD_DIGIT;
	}

	nmsg.buf = malloc(nmsg.len);
	if (!nmsg.buf) {
		fprintf(stderr, "read_msg: failed to allocate buffer\n");
		goto BAD_DIGIT;
	}

	/* make sure there are enough input bytes available
	  * (excluding the colon and trailing newline) */
	if (nmsg.len > m.len - i - 2) {
		fprintf(stderr, "read_msg: Not enough input bytes to read message of size %zu\n", nmsg.len);
		goto BAD_ALLOC;
	}

	memcpy(nmsg.buf, &(m.buf[i+1]), nmsg.len);
	return nmsg;
BAD_ALLOC:
	free(nmsg.buf);
BAD_DIGIT:
	nmsg.len = 0;
	nmsg.buf = NULL;
	return nmsg;
}