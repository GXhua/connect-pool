/* 
 * File:   Server.h
 * Author: guoxinhua
 *
 * Created on 2014年9月22日, 下午3:52
 */

#ifndef CP_SERVER_H
#define	CP_SERVER_H

#ifdef	__cplusplus
extern "C" {
#endif

#define CP_CPU_NUM               1
//#define CP_CPU_NUM               (int)sysconf(_SC_NPROCESSORS_ONLN)/2==0?1:(int)sysconf(_SC_NPROCESSORS_ONLN)/2
#define CP_BACKLOG               512
#define CP_PIPES_NUM             (CP_WORKER_NUM/CP_WRITER_NUM + 1) //每个写线程pipes数组大小
#define CP_PORT_PDO              6253
#define CP_PORT_REDIS            6253
#define CP_REACTOR_TIMEO_SEC     3
#define CP_REACTOR_TIMEO_USEC    0
#define CP_MAX_FDS               (1024*10) //最大fd值,暂不支持扩容
    //#define CP_MAX_REQUEST           20000
#define CP_MAX_REQUEST           0
#define CP_MAX_WORKER            100
#define CP_MIN_WORKER            2
#define CP_IDEL_TIME             2
#define CP_RECYCLE_NUM           2
#define CP_DEF_MAX_READ_LEN      (1024*1024*5)
#define CP_MAX_READ_LEN          (1024*1024*20)

#define CP_PING_MEM_LEN          1024*1024
#define CP_PING_DIS_LEN          409600  //disable list mem
#define CP_PING_PRO_LEN          409600  //probaly disable
#define CP_PING_MD5_LEN          32        //conf md5 value
#define CP_PING_PID_LEN          4        //conf md5 value
#define CP_PING_GET_DIS(addr) cp_unserialize((addr) + CP_PING_MD5_LEN+CP_PING_PID_LEN, CP_PING_DIS_LEN);
#define CP_PING_GET_PRO(addr) cp_unserialize((addr) + CP_PING_MD5_LEN+CP_PING_PID_LEN+CP_PING_DIS_LEN, CP_PING_PRO_LEN);
#define CP_CONNECT_NORMAL        0
#define CP_CONNECT_RECONNECT     1
#define CP_CONNECT_PING          2

#define CPGC                     ConProxyG.conf
#define CPGL                     ConProxyG
#define CPGS                     ConProxyGS
#define CPWG                     ConProxyWG

#define CP_UNSOCK_BUFSIZE        (4*1024*1024)

#define CP_WORKER_RESTART        4    
#define CP_WORKER_DELING         3
#define CP_WORKER_BUSY           2
#define CP_WORKER_IDLE           1
#define CP_WORKER_DEL            0

#define CP_ACCEPT_AGAIN          1     //是否循环accept，可以一次性处理完全部的listen队列，用于大量并发连接的场景
#define CP_ACCEPT_MAX_COUNT      1    //一次循环的最大accept次数
#define CP_TCP_KEEPCOUNT         5
#define CP_TCP_KEEPIDLE          3600 //1小时
#define CP_TCP_KEEPINTERVAL      60

#define CP_FD_NCON               2
#define CP_FD_RELEASED           0
#define CP_FD_NRELEASED          2
#define CP_FD_WAITING            1

#define CP_TRUE                  1
#define CP_FALSE                 0

#define CP_START_SLEEP           usleep(50000);
#define CP_PING_SLEEP            60

    typedef volatile int8_t volatile_int8;

    typedef struct _cpIdelList {
        struct _cpIdelList *next;
        int worker_id;
    } cpIdelList;

    typedef struct _cpWaitList {
        int fd;
        int len;
        struct _cpWaitList *next;
        struct _cpWaitList *pre;
        char data[0];
    } cpWaitList;

    typedef struct _cpConfig {
        uint16_t backlog;
        uint16_t reactor_num;
        uint16_t recycle_num;
        uint16_t port;

        //连续失败多少次算失效
        uint16_t ser_fail_hits;

        //最多可以剔除多少个结点,防止网络抖动等,导致的全部踢掉
        uint16_t max_fail_num;

        uint8_t idel_time;
        uint8_t use_wait_queue;
        uint8_t daemonize;


        uint64_t max_read_len;

        int max_conn;
        int timeout_sec;
        int timeout_usec;
        int max_request;

        char ini_file[MAX_INI_LENGTH];
        char log_file[128]; //日志文件
    } cpConfig;

    typedef struct _cpThread {
        int id; //0.1.2.3
        int epfd;
        pthread_t thread_id;
        uint16_t event_num;
    } cpThread;

    typedef struct _cpServerG {
        uint8_t running;
        uint8_t process_type;

        uint64_t wait_in_num;
        uint64_t wait_out_num;

        cpConfig conf;
        int epfd;
        void *ping_mem_addr;
    } cpServerG;

    typedef struct _cpConnection {
        int fd; //文件描述符
        //        struct sockaddr_in addr; //socket的地址

        uint16_t worker_id; //指定当前连接在用哪个worker
        uint16_t group_id; //指定当前连接在用哪个group
        uint8_t release; //方式重复release
        uint16_t pth_id; //thread id
    } cpConnection;

    typedef struct _cpGroup {
        int id; //Current worker group  id 0,1,2,3...n
        uint32_t worker_num;
        uint32_t worker_min;
        uint32_t worker_max;
        cpWorker *workers;
        volatile_int8 *workers_status;
        pthread_mutex_t *mutex_lock;
        cpWaitList *WaitList; //获得失败的wait队列
        cpWaitList *WaitTail; //获得失败的wait队列队尾
        char name[100]; //group name
    } cpGroup;

    typedef struct _cpServerGS {
        pid_t master_pid;
        pid_t manager_pid;


        uint32_t connect_count;
        uint16_t reactor_next_i;
        //        uint16_t reactor_round_i;

        cpConnection *conlist;
        uint32_t *workerfd2clientfd_list; //workerfd和客户端fd的对应关系

        cpWorker *ping_workers;

        cpThread *reactor_threads;

        int running;
        cpGroup G[100]; //group TODO extend
        zval* group;
        int group_num;
    } cpServerGS;

    typedef struct _cpWorkerG {
        int id; //Current Proccess Worker's id 0,1,2,3...n
        int clientPid;
        int gid; //current worker's group id
        int working;
        uint64_t max_read_len;
    } cpWorkerG;

    void cpServer_init(zval *conf, char *ini_file);
    int cpServer_create();
    int cpServer_start();

#ifdef	__cplusplus
}
#endif

#endif	/* SERVER_H */

