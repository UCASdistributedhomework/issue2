#include <thrift/concurrency/ThreadManager.h>
#include <thrift/concurrency/ThreadFactory.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/server/TThreadPoolServer.h>
#include <thrift/server/TThreadedServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include <thrift/TToString.h>

#include <iostream>
#include <stdexcept>
#include <sstream>

#include "gen-cpp/Locker.h"

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::concurrency;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::server;


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

int main() {

    // create a thread for a connecton
    TThreadedServer server(
            std::make_shared<LockerProcessor>(std::make_shared<LockerHandler>()),
            std::make_shared<TServerSocket>(9090), //port
            std::make_shared<TBufferedTransportFactory>(),
            std::make_shared<TBinaryProtocolFactory>());

    cout << "Starting the server..." << endl;
    server.serve();
    cout << "Done." << endl;
    return 0;
}
