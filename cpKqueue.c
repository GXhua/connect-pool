
#include "php_connect_pool.h"

typedef struct cpFd {
    uint32_t fd;
    uint32_t fdtype;
};

#ifdef HAVE_KQUEUE
int cpKqueue_add(int epfd, int fd, int fdtype) {
    struct kevent e; // event
    int ret;  // 返回值
    int fflags = 0;
    bzero(&e, sizeof (e));
    cpFd = fd_;
    fd_.fd = fd;
    fd_.fdtype = fdtype;

    /*
    // 这个地方 为毛要传这个啊
    e.udata.fd = fd;
    e.udata.fdtype = fdtype;
    */

    if (fdtype & EVFILT_READ) {
        EV_SET(&e, fd, EVFILT_READ, EV_ADD, fflags, 0, NULL);
        memcpy(&e.udata, &fd_, sizeof(cpFd))
        ret = kevent(epfd, &e, 1, NULL, 0, NULL);
        if (ret < 0) {
            prinft(" add event [epfd=%d, fd=%d, type=%d, events=read] failed.\n", epfd, fd, fdtype);
            cpLog(" add event [epfd=%d, fd=%d, type=%d, events=read] failed.\n", epfd, fd, fdtype);
            return CP_ERR;
        }
    }

    if (fdtype & EVFILT_WRITE) {
        EV_SET(&e, fd, EVFILT_WRITE, EV_ADD, fflags, 0, NULL);
        ret = kevent(epfd, &e, 1, NULL, 0, NULL);
        if (ret < 0) {
            prinft(" add event [epfd=%d, fd=%d, type=%d, events=write] failed.\n", epfd, fd, fdtype);
            cpLog(" add event [epfd=%d, fd=%d, type=%d, events=write] failed.\n", epfd, fd, fdtype);
            return CP_ERR;
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
        EV_SET(&epfd, fd, EVFILT_READ, EV_DELETE, fflags, 0, NULL);
        ret = kevent(epfd, &e, 1, NULL, 0, NULL);

        if (ret < 0) {
            prinft(" delete event [epfd=%d, fd=%d, type=%d, events=read] failed.\n", epfd, fd, fdtype);
            cpLog(" delete event [epfd=%d, fd=%d, type=%d, events=read] failed.\n", epfd, fd, fdtype);
            return CP_ERR;
        }
   // }

    //if (fdtype & EVFILT_WRITE) {
        EV_SET(&e, fd, EVFILT_WRITE, EV_DELETE, fflags, 0, NULL);
        ret = kevent(epfd, &e, 1, NULL, 0, NULL);
        if (ret < 0) {
            prinft(" delete event [epfd=%d, fd=%d, type=%d, events=write] failed.\n", epfd, fd, fdtype);
            cpLog(" delete event [epfd=%d, fd=%d, type=%d, events=write] failed.\n", epfd, fd, fdtype);
            return CP_ERR;
        }
    //}
    //close时会自动从kqueue事件中移除
    ret = close(fd);
    return SUCCESS;
}

int cpKqueue_set(int fd, int fdtype) {
    return SUCCESS;
}

int cpKqueue_wait(kqueue_wait_handle*, struct timeval *timeo, int epfd) {
    return SUCCESS;
}
void cpKqueue_free() {
    return SUCCESS;
}
#endif
