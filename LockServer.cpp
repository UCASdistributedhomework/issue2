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
#include <string.h>
#include <queue>

#include "gen-cpp/Locker.h"

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::concurrency;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::server;

const int  MAXNCLIENT = 100000;
queue<int> client_request_queue;
bool client_request_flag[MAXNCLIENT];
bool client_alive_flag[MAXNCLIENT];
int client_message_index[MAXNCLIENT];
int lock_client;//-1 is free

void assignTheKey(){
  if(lock_client!=-1){
    return;
  }
  while(!client_request_queue.empty()){
    int client = client_request_queue.front();
    client_request_queue.pop();
    if(client_request_flag[client]){
      lock_client = client;
      client_request_flag[lock_client] = false;
      break;
    }
  }
}

class LockerHandler : virtual public LockerIf {
 public:
  LockerHandler() {
    // Your initialization goes here
    /*
    memset(client_request_flag,0,sizeof(client_request_flag));
    memset(client_alive_flag,0,sizeof(client_alive_flag));
    request_client_queue.clear();
    lock_client = -1;
    */
  }

  bool client_register(const int32_t client_id, const int32_t message_index, const bool is_retry) {
    // Your implementation goes here
    //assume only exisit one client_id for one client
    if(client_id>=MAXNCLIENT){
      printf("warning : client_id is outrange!\n");
      return false;
    }
    client_alive_flag[client_id] = true;
    client_request_flag[client_id] = false;
    client_message_index[client_id] = 0;
    printf("client_register\n");
    return true;
  }

  bool client_exit(const int32_t client_id, const int32_t message_index, const bool is_retry) {
    // Your implementation goes here
    if(message_index<client_message_index[client_id]){
      return false;
    }
    client_request_flag[client_id] = false;
    client_alive_flag[client_id] = false;
    if(lock_client==client_id){
      lock_client = -1;
      assignTheKey();
    }
    printf("client_exit\n");
    return true;
  }

  bool lock_request_register(const int32_t client_id, const int32_t message_index, const bool is_retry) {
    // Your implementation goes here
    if(client_alive_flag[client_id]){
      if(message_index<client_message_index[client_id]){
        printf("warning : timeout request\n");
        return false;
      }
      client_message_index[client_id] = message_index;
      if(!client_request_flag[client_id]){
        if(lock_client == -1){
          // no client before ,get the key
          lock_client = client_id;
        }else{
          // wait the key
          client_request_flag[client_id] = true;
          client_request_queue.push(client_id);
        }
        printf("lock_request_register\n");
        return true;
      }
    }
    printf("warning : bad request\n");
    return false;
  }

  bool lock_request_check(const int32_t client_id, const int32_t message_index, const bool is_retry) {
    // Your implementation goes here
    return lock_client == client_id;
    printf("lock_request_check\n");
  }

  bool lock_request_release(const int32_t client_id, const int32_t message_index, const bool is_retry) {
    // Your implementation goes here
    if(message_index<client_message_index[client_id]){
      printf("warning : timeout release\n");
      return false;
    }
    client_message_index[client_id] = message_index;
    if(lock_client==client_id){
      lock_client = -1;
      assignTheKey();
      printf("lock_request_release\n");
      return true;
    }
    printf("warning : invalid release\n");
    return false;
  }

};

void initServer(){
    memset(client_request_flag,0,sizeof(client_request_flag));
    memset(client_alive_flag,0,sizeof(client_alive_flag));
    lock_client = -1;
}

int main() {

    // create a thread for a connecton
    TThreadedServer server(
            std::make_shared<LockerProcessor>(std::make_shared<LockerHandler>()),
            std::make_shared<TServerSocket>(9090), //port
            std::make_shared<TBufferedTransportFactory>(),
            std::make_shared<TBinaryProtocolFactory>());
    cout << "Initing the server..." << endl;
    initServer();
    cout << "Starting the server..." << endl;
    
    server.serve();
    cout << "Done." << endl;
    return 0;
}