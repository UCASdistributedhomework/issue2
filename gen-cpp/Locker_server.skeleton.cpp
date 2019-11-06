// This autogenerated skeleton file illustrates how to build a server.
// You should copy it to another filename to avoid overwriting it.

#include "Locker.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

class LockerHandler : virtual public LockerIf {
 public:
  LockerHandler() {
    // Your initialization goes here
  }

  bool client_register(const int32_t client_id, const bool is_retry) {
    // Your implementation goes here
    printf("client_register\n");
  }

  bool client_exit(const int32_t client_id, const bool is_retry) {
    // Your implementation goes here
    printf("client_exit\n");
  }

  bool lock_request_register(const int32_t client_id, const bool is_retry) {
    // Your implementation goes here
    printf("lock_request_register\n");
  }

  bool lock_request_check(const int32_t client_id, const bool is_retry) {
    // Your implementation goes here
    printf("lock_request_check\n");
  }

  bool lock_request_release(const int32_t client_id, const bool is_retry) {
    // Your implementation goes here
    printf("lock_request_release\n");
  }

};

int main(int argc, char **argv) {
  int port = 9090;
  ::std::shared_ptr<LockerHandler> handler(new LockerHandler());
  ::std::shared_ptr<TProcessor> processor(new LockerProcessor(handler));
  ::std::shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
  ::std::shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  ::std::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

  TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
  server.serve();
  return 0;
}
