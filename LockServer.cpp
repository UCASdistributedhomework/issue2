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
#include "ServerSingleton.h"
#include "Handler.h"
using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::concurrency;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::server;


class LockerHandler : virtual public LockerIf  , virtual public IHandler {
    public:

        LockerHandler() : prev_index(-1) , the_mark(false) {
            // Your initialization goes here
            std::cerr<<"[S] Create handler "<<this<<" now ..."<<std::endl;
        }

        void Mark() override {
            the_mark = true ;
        }

        bool client_register(const int32_t client_id, const int32_t index ) {
            // Your implementation goes here
            if( index < prev_index ) return false ;
            if( index == prev_index ) return true ;
            ServerSingleton::get().RegisterClient(client_id,this);
            std::cerr<<"[S] Client register "<<client_id<<" by "<<this<<" now ..."<<std::endl;
            prev_index = index;
            return true ;
        }

        bool client_exit(const int32_t client_id, const int32_t index) {
            if( index < prev_index ) return false ;
            if( index == prev_index ) return true ;
            // Your implementation goes here
            ServerSingleton::get().RegisterClient(client_id,this);
            std::cerr<<"[S] Client exit "<<client_id<<" by "<<this<<" now ..."<<std::endl;
            prev_index = index;
            return true ;
        }

        bool lock_request_register(const int32_t client_id, const int32_t index) {
            if( index < prev_index ) return false ;
            if( index == prev_index ) return true ;
            // Your implementation goes here
            std::cerr<<"[S] Client requst resource "<<client_id<<" by "<<this<<" now ..."<<std::endl;
            ServerSingleton::get().RequestResource(client_id);
            prev_index = index;
            return true ;
        }

        bool lock_request_check(const int32_t client_id, const int32_t index) {
            // Your implementation goes here
            if( index < prev_index ) return false ;
            if( index == prev_index ) return true ;
            //std::cerr<<"client check "<<client_id<<" and result is "<<the_mark<<" now ..."<<std::endl;
            prev_index = index;
            return the_mark;
        }

        bool lock_request_release(const int32_t client_id,const int32_t index) {
            // Your implementation goes here
            if( index < prev_index ) return false ;
            if( index == prev_index ) return true ;
            std::cerr<<"[S] Client release resource  "<<client_id<<" by "<<this<<" now ..."<<std::endl;
            ServerSingleton::get().RelaseResource(client_id);
            prev_index = index;
            the_mark = false ;
            return true ;
        }
        private :
        bool the_mark ; // whether resource is avaliable or not 
        int prev_index ; // prev client requst id , ignore all prer but delayed requst .
};


class LockerHandlerFactory : virtual public LockerIfFactory {
 public:
  ~LockerHandlerFactory() override = default;
  LockerIf * getHandler(const ::apache::thrift::TConnectionInfo& connInfo) override
  {
    std::shared_ptr<TSocket> sock = std::dynamic_pointer_cast<TSocket>(connInfo.transport);
    return new LockerHandler;
  }

  void releaseHandler(LockerIf*  handler) override {
    delete handler;
  }
};
int main() {

  TThreadedServer server(
    std::make_shared<LockerProcessorFactory>(std::make_shared<LockerHandlerFactory>()),
    std::make_shared<TServerSocket>(9090), //port
    std::make_shared<TBufferedTransportFactory>(),
    std::make_shared<TBinaryProtocolFactory>());

    std::cerr<<"[S] Server starts now ...\n";
    server.serve();
    std::cerr<<"[S] Server quit now .n";
    return 0;
}
