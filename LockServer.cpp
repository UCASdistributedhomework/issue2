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
using namespace apache::thrift;
using namespace apache::thrift::concurrency;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::server;

class LockerHandler : virtual public LockerIf  , virtual public IHandler {
    public:

        LockerHandler() {
            // Your initialization goes here
            std::cerr<<"Create hander "<<this<<" now ..."<<std::endl;
        }

        void Mark() override {
            the_mark = true ;
        }

        bool client_register(const int32_t client_id, const int32_t index ) {
            // Your implementation goes here
            if( index < prev_index ) return false ;
            if( index == prev_index ) return true ;
            ServerSingleton::get().RegisterClient(client_id,this);
            std::cerr<<"client register "<<client_id<<" by "<<this<<" now ..."<<std::endl;
            prev_index = index;
            return true ;
        }

        bool client_exit(const int32_t client_id, const int32_t index) {
            if( index < prev_index ) return false ;
            if( index == prev_index ) return true ;
            // Your implementation goes here
            ServerSingleton::get().RegisterClient(client_id,this);
            std::cerr<<"client exit "<<client_id<<" by "<<this<<" now ..."<<std::endl;
            prev_index = index;
            return true ;
        }

        bool lock_request_register(const int32_t client_id, const int32_t index) {
            if( index < prev_index ) return false ;
            if( index == prev_index ) return true ;
            // Your implementation goes here
            ServerSingleton::get().RequestResource(client_id);
            std::cerr<<"client requst resource "<<client_id<<" by "<<this<<" now ..."<<std::endl;
            prev_index = index;
            return true ;
        }

        bool lock_request_check(const int32_t client_id, const int32_t index) {
            // Your implementation goes here
            if( index < prev_index ) return false ;
            if( index == prev_index ) return true ;
            std::cerr<<"client check "<<client_id<<" and result is "<<the_mark<<" now ..."<<std::endl;
            prev_index = index;
            return the_mark;
        }

        bool lock_request_release(const int32_t client_id,const int32_t index) {
            // Your implementation goes here
            if( index < prev_index ) return false ;
            if( index == prev_index ) return true ;
            ServerSingleton::get().RelaseResource(client_id);
            std::cerr<<"client release resource  "<<client_id<<" by "<<this<<" now ..."<<std::endl;
            prev_index = index;
            return true ;
        }
        private :
        bool the_mark ; // whether resource is avaliable or not 
        int prev_index ; // prev client requst id , ignore all prer but delayed requst .
};


int main() {

    // create a thread for a connecton
    TThreadedServer server(
            std::make_shared<LockerProcessor>(std::make_shared<LockerHandler>()),
            std::make_shared<TServerSocket>(9090), //port
            std::make_shared<TBufferedTransportFactory>(),
            std::make_shared<TBinaryProtocolFactory>());

    std::cout << "Starting the server..." << std::endl;
    server.serve();
    std::cout << "Done." << std::endl;
    return 0;
}
