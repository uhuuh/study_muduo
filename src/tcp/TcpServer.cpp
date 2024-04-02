#include "TcpServer.h"
#include "Acceptor.h"
#include "TcpConnection.h"
#include "EventloopPool.h"
#include "EventLoop.h"
#include <memory>
// todo 在类型中定义析构函数, 克服使用unique_ptr的时候报不完整type错误
// todo 替换成pimpl模式


TcpServer:: TcpServer(const string& ip, port_t port, int n_thread):
    ip(ip),
    port(port),
    main_loop(new Eventloop()) // 需要一个main loop阻塞主线程
{
    if (n_thread != 0) {
        loop_pool = make_unique<EventloopPool>(n_thread);
    }
}

void TcpServer::run() {
    auto cb = [this] (auto fd, auto peer_ip, auto peer_port) {
        this->create_conn(fd, peer_ip, peer_port);
    };
    acceptor = make_unique<Acceptor>(
        main_loop, 
        cb, 
        ip, 
        port
    );

    INFO(format("server_run | id: {}, ip: {}, port: {}", main_loop->thread_id, ip, port));
    main_loop->run();
}

void TcpServer::create_conn(fd_t fd, const string peer_ip, const int peer_port) {
    auto cb = [this, fd, peer_ip, peer_port]() {
        assertm(conn_map.find(fd) == conn_map.end());

        auto loop = loop_pool ? loop_pool->getLoop() : main_loop;
        auto cb = [this, fd] () {
            this->remove_conn(fd);
        };
        conn_map[fd] = std::make_unique<TcpConnection>(
            loop,
            fd, 
            ip,
            port,
            peer_ip,
            peer_port,
            openCallback,
            closeCallback,
            messageCallback,
            cb
        );

        INFO(format("create_conn | fd: {}, peer_id: {}, peer_port: {}", fd, peer_ip, peer_port));
    };

    main_loop->addCallback(cb);
}

void TcpServer::remove_conn(fd_t fd) {
    auto cb = [this, fd] {
        auto iter = this->conn_map.find(fd);
        this->conn_map.erase(iter); 

        INFO(format("remove_conn | fd: {}", fd));
    };

    main_loop->addCallback(cb);
}

