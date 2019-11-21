#ifndef __CLIENT_SWAP_H__
#define __CLIENT_SWAP_H__

#include <iostream>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include "gen-cpp/Locker.h"

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;


class ClientSwap {
    public :
        ClientSwap(int id) : client_id(id) , index(1) {
            // create connection and client
            std::shared_ptr<TTransport>   socket(new      TSocket("localhost", 9090));
            transport = std::shared_ptr<TTransport>(new   TBufferedTransport(socket));
            std::shared_ptr<TProtocol>    protocol(new    TBinaryProtocol(transport));
            client = new LockerClient(protocol);
            // auto register itself
            transport->open();
            client->client_register(client_id,index++);
        }
        ~ClientSwap() {
            // auto release itself
            client->client_exit(client_id,index++);
            // close connection and client
            transport->close();
            delete client ;
            client = NULL ;
        }

        bool Lock() {
            bool ret = false;
            try {
                // register requst
                ret = client->lock_request_register(client_id,index++);
                // wait until get resource
                while( ! client->lock_request_check(client_id,index++) ) 
                {
                    sleep(1);
                }
            } catch (TException& tx) {
                std::cout << "ERROR: " << tx.what() << std::endl;
                return false ;
            }
            return true ;
        }
        bool Unlock() {
            bool ret = false;
            try {
                ret = client->lock_request_release(client_id,index++);
            } catch (TException& tx) {
                std::cout << "ERROR: " << tx.what() << std::endl;
                return false ;
            }
            return ret;
        }
    private:
        std::shared_ptr<TTransport>   transport;
        LockerClient                  * client;
        int index ;
        const int client_id ;
};

#endif //__CLIENT_SWAP_H__
