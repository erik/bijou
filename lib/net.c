/* Copyright 2010, Erik Price */

#include "bijou.h"
#include "internal.h"
#include "vm.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define SHOULD_BE(tval, type) if((tval).tt != (type)) { \
	fprintf(stderr, "Expected type %d,"	        \
	"but got %d (%s)\n", type, (tval).tt, TValue_to_string(tval)); \
	exit(1); \
    }

/*
 * Args -
 * 	node [String]
 *	port [String]
 * Returns -
 *	info [struct addrinfo]
 */
int    args_getaddrinfo = 2;
TValue func_getaddrinfo(VM, BijouBlock* blk, int argc, TValue* argv)
{
    UNUSED(vm);
    UNUSED(blk);
    UNUSED(argc);

    int status;
    struct addrinfo hints;
    struct addrinfo **servinfo = B_ALLOC(struct addrinfo*);

    memset(&hints, 0, sizeof(hints));

    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    SHOULD_BE(argv[0], BIJOU_TSTRING);
    SHOULD_BE(argv[1], BIJOU_TSTRING);
    char *node = argv[0].value.s.ptr;
    char *port = argv[1].value.s.ptr;

    status = getaddrinfo(node, port, &hints, servinfo);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }

    return create_pointer(*servinfo);

}

/*
 * Args -
 *	info [addrinfo]
 * Returns -
 *	socket fd [int]
 */
int    args_socket = 1;
TValue func_socket(VM, BijouBlock* blk, int argc, TValue* argv)
{
    UNUSED(vm);
    UNUSED(blk);
    UNUSED(argc);

    struct addrinfo *info;
    int sock;

    SHOULD_BE(argv[0], BIJOU_TPOINTER);
    info = (struct addrinfo *)argv[0].value.pointer;

    sock = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
    if (sock == -1) {
        fprintf(stderr, "Socket creation failed: %s\n", strerror(errno));
        exit(1);
    }

    return create_bijou_Number(sock);

}

/*
 * Args -
 *	socket fd [int]
 *	info   [addrinfo]
 * Returns -
 *	status [int]
 */
int    args_bind = 2;
TValue func_bind(VM, BijouBlock* blk, int argc, TValue* argv)
{
    UNUSED(vm);
    UNUSED(blk);
    UNUSED(argc);

    int sock;
    struct addrinfo *info;

    SHOULD_BE(argv[0], BIJOU_TNUMBER);
    SHOULD_BE(argv[1], BIJOU_TPOINTER);

    sock = (int)argv[0].value.n;
    info = (struct addrinfo *)argv[0].value.pointer;

    int status = bind(sock, info->ai_addr, info->ai_addrlen);

    if (status == -1) {
        fprintf(stderr, "Bind failed: %s\n", strerror(errno));
        exit(1);
    }

    return create_bijou_Number(status);
}


/*
 * Args -
 *	socket fd[int]
 *	info [addrinfo]
 * Returns -
 *	status [int]
 */
int    args_connect = 2;
TValue func_connect(VM, BijouBlock* blk, int argc, TValue* argv)
{
    UNUSED(vm);
    UNUSED(blk);
    UNUSED(argc);

    int sock;
    struct addrinfo *info;

    SHOULD_BE(argv[0], BIJOU_TNUMBER);
    SHOULD_BE(argv[1], BIJOU_TPOINTER);

    sock = (int)argv[0].value.n;
    info = (struct addrinfo *)argv[1].value.pointer;

    int status = connect(sock, info->ai_addr, info->ai_addrlen);
    if (status == -1) {
        fprintf(stderr, "Connect failed: %s\n", strerror(errno));
        exit(1);
    }

    return create_bijou_Number(status);
}

/*
 * Args -
 *	socket fd [int]
 *	backlog [int]
 * Returns -
 *	status [int]
 */
int    args_listen = 2;
TValue func_listen(VM, BijouBlock* blk, int argc, TValue* argv)
{
    UNUSED(vm);
    UNUSED(blk);
    UNUSED(argc);

    int sock;
    int backlog;

    SHOULD_BE(argv[0], BIJOU_TNUMBER);
    SHOULD_BE(argv[1], BIJOU_TNUMBER);

    sock = (int)argv[0].value.n;
    backlog = (int)numvalue(&argv[1]);

    int status = listen(sock, backlog);
    if (status == -1) {
        fprintf(stderr, "Listen failed: %s\n", strerror(errno));
        exit(1);
    }

    return create_bijou_Number(status);
}

/* TODO: accept() */

/*
 * Args -
 * 	socket fd [int]
 *	message   [String]
 * Returns -
 * 	bytes sent [int]
 */
int    args_send = 2;
TValue func_send(VM, BijouBlock* blk, int argc, TValue* argv)
{
    UNUSED(vm);
    UNUSED(blk);
    UNUSED(argc);

    int sock;
    char *msg;
    int size;

    SHOULD_BE(argv[0], BIJOU_TNUMBER);
    SHOULD_BE(argv[1], BIJOU_TSTRING);

    sock = (int)argv[0].value.n;
    msg  = argv[1].value.s.ptr;
    size = argv[1].value.s.len;

    int bytes_sent = send(sock, msg, size, 0);
    if (bytes_sent == -1) {
        fprintf(stderr, "Send failed: %s\n", strerror(errno));
        exit(1);
    }

    return create_bijou_Number(bytes_sent);

}

/*
 * Args -
 *	socket fd [int]
 *	max read [int]
 * Returns -
 *	message [String]
 */
int    args_recv = 2;
TValue func_recv(VM, BijouBlock* blk, int argc, TValue* argv)
{
    UNUSED(vm);
    UNUSED(blk);
    UNUSED(argc);

    int sock;
    char *buf;
    int len;

    SHOULD_BE(argv[0], BIJOU_TNUMBER);
    SHOULD_BE(argv[1], BIJOU_TNUMBER);

    sock = (int)argv[0].value.n;
    len  = (int)argv[1].value.n;

    buf = B_MALLOC(len);

    int read = recv(sock, buf, len, 0);
    if (read == -1) {
        fprintf(stderr, "Recieve failed: %s\n", strerror(errno));
        exit(1);
    }

    return create_TValue_string(BijouString_new(buf));
}
