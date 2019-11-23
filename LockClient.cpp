#include <iostream>
#include <time.h>
#include "ClientSwap.h"



int main(int argc , char ** argv ) {

    if( argc != 2 ) {  std::cout<<"Usage : client  <client_id> \n";  return 1 ; }
    int client_id = std::stoi(argv[1]);
    if( client_id < 1 ) { std::cout<<"FATAL : invalid client_id "<<client_id<<" \n "; return 1 ; }
    std::cout<<"client start , current client_id is "<<client_id<<"\n";

    srand((unsigned)time(NULL));

    ClientSwap client(client_id);
    for( int i = 0 ; i < 10 ; i ++ )
    {
        std::cout << "client request round : " << i << std::endl; 
        while(!client.Lock()){
            std::cout << "  client request lock failed"<< std::endl;
            sleep(1);
        }
        std::cout << "  client request lock success"<< std::endl;
        // use resource to do something ...
        sleep(5);
        while(!client.Unlock()){
            std::cout << "  client release lock failed"<< std::endl;
            sleep(1);
        }
        std::cout << "  client release lock success"<< std::endl;
    }
    std::cout<<"client end, current client_id is "<<client_id<<"\n";
}
