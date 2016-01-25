
#include "php_connect_pool.h"

typedef struct _cpFd {
    uint32_t fd;
    uint32_t fdtype;
} cpFd;

#ifdef HAVE_KQUEUE
int cpKqueue_add(int epfd, int fd, int fdtype) {
    struct kevent e; // event
    int ret;  // 返回值
    int fflags = 0;
    bzero(&e, sizeof (e));
    cpFd fd_;
    fd_.fd = fd;
    fd_.fdtype = fdtype;

    /*
    // 这个地方 为毛要传这个啊
    e.udata.fd = fd;
    e.udata.fdtype = fdtype;
    */

    if (fdtype & EVFILT_READ) {
        EV_SET(&e, fd, EVFILT_READ, EV_ADD, fflags, 0, NULL);
        memcpy(&e.udata, &fd_, sizeof(cpFd));
        ret = kevent(epfd, &e, 1, NULL, 0, NULL);
        if (ret < 0) {
            printf(" add event [epfd=%d, fd=%d, type=%d, events=read] failed.\n", epfd, fd, fdtype);
            cpLog(" add event [epfd=%d, fd=%d, type=%d, events=read] failed.\n", epfd, fd, fdtype);
            return FAILURE;
        }
    }

    if (fdtype & EVFILT_WRITE) {
        EV_SET(&e, fd, EVFILT_WRITE, EV_ADD, fflags, 0, NULL);
        ret = kevent(epfd, &e, 1, NULL, 0, NULL);
        if (ret < 0) {
            printf(" add event [epfd=%d, fd=%d, type=%d, events=write] failed.\n", epfd, fd, fdtype);
            cpLog(" add event [epfd=%d, fd=%d, type=%d, events=write] failed.\n", epfd, fd, fdtype);
            return FAILURE;
        }
    }
    /*
    e.data.fd = fd;
    e.events = fdtype;
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &e);
    if (ret < 0)
    {
        cpLog("add event fail. Error: %s[%d]", strerror(errno), errno);
        return FAILURE;
    }
    */
    return SUCCESS;
}

int cpKqueue_del(int epfd, int fd) {
    struct kevent e;
    
    int ret;
    int fflags = 0;

    //if (fdtype & EVFILT_READ) {
        EV_SET(&e, fd, EVFILT_READ, EV_DELETE, fflags, 0, NULL);
        ret = kevent(epfd, &e, 1, NULL, 0, NULL);

        if (ret < 0) {
            printf(" delete event [epfd=%d, fd=%d, events=read] failed.\n", epfd, fd);
            cpLog(" delete event [epfd=%d, fd=%d, events=read] failed.\n", epfd, fd);
            return FAILURE;
        }
   // }

    //if (fdtype & EVFILT_WRITE) {
        EV_SET(&e, fd, EVFILT_WRITE, EV_DELETE, fflags, 0, NULL);
        ret = kevent(epfd, &e, 1, NULL, 0, NULL);
        if (ret < 0) {
            printf(" delete event [epfd=%d, fd=%d, events=write] failed.\n", epfd, fd);
            cpLog(" delete event [epfd=%d, fd=%d, events=write] failed.\n", epfd, fd);
            return FAILURE;
        }
    //}
    //close时会自动从kqueue事件中移除
    ret = close(fd);
    return SUCCESS;
}

int cpKqueue_set(int fd, int fdtype) {
    return SUCCESS;
}

void cpKqueue_free() {
    return ;
}

int cpKqueue_wait(epoll_wait_handle* handles, struct timeval *timeo, int epfd) {
    int i, n, ret, usec;
    cpFd fd_;
    if (timeo == NULL)
    {
        usec = CP_MAX_UINT;
    }
    else
    {
        usec = timeo->tv_sec * 1000 + timeo->tv_usec / 1000;
    }

    struct kevent events[CP_REACTOR_MAXEVENTS];
    while(CPGS->running)
    {
        n = kevent(epfd, NULL, 0, events, CP_REACTOR_MAXEVENTS, usec);

        if (n < 0) {
            if (cpReactor_error() < 0)
            {
                printf("kevent error \n");
                cpLog("Epoll[#%d] Error: %s[%d]", fd_.fd, strerror(errno), errno);
                return FAILURE;
            }
            //continue;
        } else if(ret == 0){
            printf("kenvent timeout !\n");
            continue;
        }else{
            for (i =0; i < n; n++) {
                if (events[i].udata) {
                    memcpy(&fd_, &(events[i].udata), sizeof(fd_));
                }

                // 包含读事件
                if (events[i].events & EVFILT_READ) {
                    ret = handles[EVFILT_READ](fd_.fd);
                    if (ret < 0)
                    {
                        cpLog("kqueue [EVFILT_READ] handle failed. fd=%d. Error: %s[%d]", fd_.fd,
                                strerror(errno), errno);
                    }
                }
                else if (events[i].events & EVFILT_WRITE)
                {
                    ret = handles[EVFILT_WRITE](fd_.fd);
                    if (ret < 0)
                    {
                        cpLog("kqueue [EPOLLOUT] handle failed. fd=%d. Error: %s[%d]", fd_.fd,
                                strerror(errno), errno);
                    }
                }

            }
        }

    }

    return 0;
}
#endif












































