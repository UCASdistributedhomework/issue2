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

    int message_index = 0;

    try {
        transport->open();

        client.client_register(client_id,message_index,false);
        // register requst
        message_index++;
        client.lock_request_register(client_id,message_index,false);
        // wait until get resource
        message_index++;
        while( ! client.lock_request_check(client_id,message_index,false) ) 
        {
            sleep(1);
        }
        // use the resource now .
        //......
        // release lock
        message_index++;
        client.lock_request_release(client_id,message_index,false);
        message_index++;
        client.client_exit(client_id,message_index,false);
        transport->close();
    } catch (TException& tx) {
        cout << "ERROR: " << tx.what() << endl;
    }
}
