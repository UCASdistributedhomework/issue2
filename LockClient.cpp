#include <iostream>
#include "ClientSwap.h"



int main(int argc , char ** argv ) {

    if( argc != 2 ) {  std::cout<<"Usage : client  <client_id> \n";  return 1 ; }
    int client_id = std::stoi(argv[1]);
    if( client_id < 1 ) { std::cout<<"FATAL : invalid client_id "<<client_id<<" \n "; return 1 ; }
    std::cout<<"client start , current client_id is "<<client_id<<"\n";

    ClientSwap client(client_id);
    for( int i = 0 ; i < 20 ; i ++ )
    {
        client.Lock();
        // use resource to do something ...
        sleep(5);
        client.Unlock();
    }
    std::cout<<"client end, current client_id is "<<client_id<<"\n";
}
