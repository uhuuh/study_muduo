#include <map>
#include "base.hpp"
#include "EventLoopPool.hpp"
#include "TCPAcceptor.hpp"
#include "TCPConnection.hpp"
using namespace std;


class TCPServer: public noncopyable {
public:
    TCPServer(const string& ip, int port, int n_thread = 0);
    ~TCPServer() = default;

    void run();
    UserCallback user_cb;
private:
    void create_conn(int fd, const string peer_ip, const int peer_port);
    void remove_conn(int fd);

    const string ip;
    const int port;
    unique_ptr<EventloopPool> loop_pool;
    EventLoop* main_loop;
    unique_ptr<TCPAcceptor> acceptor;
    map<int, std::unique_ptr<TCPConnection>> conn_map;
    const static int max_conn_num = 20000;
};
