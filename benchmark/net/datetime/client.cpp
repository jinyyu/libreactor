#include <Connector.h>
#include <EventLoop.h>
#include <ByteBuffer.h>
#include <Timestamp.h>
#include <Connection.h>
#include <Logger.h>

using namespace net;


void read_cb(ConnectionPtr conn, ByteBuffer* buffer, const Timestamp & timestamp)
{
  std::string str((char*)buffer->data(), buffer->remaining());
  LOG_INFO("%s", str.c_str());
}

void close_cb(ConnectionPtr conn, const Timestamp & timestamp)
{
  conn->loop()->stop();
}

class DatetimeClient
{
public:
  DatetimeClient(const char* ip, int port)
      : ip_(ip), port_(port)
  {
    loop.allocate_receive_buffer(10240);

    InetSocketAddress local = InetSocketAddress();
    client = new Connector(&loop, local);

    client->read_message_callback(read_cb);
    client->connection_closed_callback(close_cb);

  }

  ~DatetimeClient() {
    delete(client);
  }

  void run() {
    InetSocketAddress peer(ip_, port_);
    client->connect(peer);

    loop.run();
  };

private:
  EventLoop loop;

  Connector* client;
  const char* ip_;
  int port_;
};

int main(int argc, char* argv[])
{
  if (argc != 3){
    printf("usage %s <ip> <port>\n", argv[0]);
    return -1;
  }

  DatetimeClient client(argv[1], atoi(argv[2]));
  client.run();
}