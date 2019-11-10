#include <iostream>

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include "gen-cpp/Locker.h"

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;


int main(int argc , char ** argv ) {

    if( argc != 2 ) {  cout<<"Usage : client  <client_id> \n";  return 1 ; }
    int client_id = std::stoi(argv[1]);
    if( client_id < 1 ) { cout<<"FATAL : invalid client_id "<<client_id<<" \n "; return 1 ; }
    cout<<"current client_id "<<client_id<<"\n";
    std::shared_ptr<TTransport>   socket(new      TSocket("localhost", 9090));
    std::shared_ptr<TTransport>   transport(new   TBufferedTransport(socket));
    std::shared_ptr<TProtocol>    protocol(new    TBinaryProtocol(transport));
    LockerClient                  client(protocol);

    int index = 1;
    try {
        transport->open();

        client.client_register(client_id,index++);
        // register requst
        client.lock_request_register(client_id,index++);
        // wait until get resource
        while( ! client.lock_request_check(client_id,index++) ) 
        {
            sleep(1);
        }
        // use the resource now .
        //......
        sleep(10);
        // release lock
        client.lock_request_release(client_id,index++);
        client.client_exit(client_id,index++);
        transport->close();
    } catch (TException& tx) {
        cout << "ERROR: " << tx.what() << endl;
    }
}
