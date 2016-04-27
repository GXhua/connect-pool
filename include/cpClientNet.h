/* 
 * File:   ClientNet.h
 * Author: guoxinhua
 *
 * Created on 2014年9月30日, 上午10:52
 */

#ifndef CP_CLIENTNET_H
#define	CP_CLIENTNET_H

#ifdef	__cplusplus
extern "C" {
#endif

    typedef struct _cpClient {
        int sock;
        double timeout;

        struct sockaddr_in serv_addr;
        struct sockaddr_in remote_addr;

        cpMasterInfo info;
        int server_fd;

        int (*lock)(struct _cpGroup *);
        int (*unLock)(struct _cpGroup *);
    } cpClient;

    int cpClient_close(cpClient *cli);
    int cpClient_send(int sock, char *data, int length, int flag);
    int cpClient_create(cpClient *cli);
    int cpClient_recv(int sock, void *data, int len, int waitall);
    int cpClient_connect(cpClient *cli, char *host, int port, double timeout, int nonblock);

#define CONN(cli)                                 (&CPGS->conlist[cli->server_fd])
#define CON_FORMART_KEY(str,port) sprintf((str), "connect_pool_sock%s" , (port));
#define CON_FAIL_MESSAGE                         "connect to pool_server fail"

#ifdef	__cplusplus
}
#endif

#endif	/* CLIENTNET_H */

