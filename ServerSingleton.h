#ifndef __ISSUE2_SINGLETON_H__
#define __ISSUE2_SINGLETON_H_
#include <map>
#include <deque>
#include <mutex>
#include <algorithm>
#include <iostream>
#include "Handler.h"
struct ServerSingleton {

    enum ResourceStatus {
        UNKNOW = 0 ,
        RELEASED = 1 ,
        LOCKED = 2 
    };

    ResourceStatus status ;

    std::mutex the_mutex; // function call must keep atomic for multi-handler in multi-threads .

    int curr_client ; // -1 means no client hold resource

    std::deque<int> requsts; // requst accepted by FIFO
    // client_id -- hander
    std::map< int , IHandler* > waiting_map ;

    void ReleaseClient(int client_id ) 
    {
        std::lock_guard<std::mutex> lock(the_mutex);
        clean_client(client_id);
    }

    void RegisterClient( int client_id , IHandler* hander ) {
        std::lock_guard<std::mutex> lock(the_mutex);
        // filter duplicate requst 
        if( waiting_map.find(client_id) != waiting_map.end() 
                && waiting_map.at(client_id) == hander ) 
            return ;
        clean_client(client_id);
        waiting_map[client_id] = hander;
    }

    void RequestResource( int client_id ) {
        std::lock_guard<std::mutex> lock(the_mutex);
        // filter duplicate requst 
        if( curr_client == client_id 
                || 
         std::find(requsts.begin() ,requsts.end() ,client_id) != requsts.end())
            return ; 
        requsts.push_back(client_id);
        // maybe resource is avaliable now !
        check_notify();
    }

    void RelaseResource( int client_id ) {
        std::lock_guard<std::mutex> lock(the_mutex);
        // filter duplicate requst 
        if( curr_client != client_id ) return ;
        std::cerr<<"Resurce release by  "<<curr_client<<" now ... "<<std::endl;
        curr_client = -1 ;
        status = RELEASED ;
        check_notify();
    }

    static ServerSingleton & get(){
        static ServerSingleton the_one ;
        return the_one;
    }
    private:
    // make sure user can't create it's own ServerSingleton
    ServerSingleton(): curr_client(-1) , status(RELEASED)  {}
    ServerSingleton( const ServerSingleton & a ) {}
    ServerSingleton & operator = ( const ServerSingleton & a ) {}
    // if resource is avaliable , assign it to client .
    void check_notify() {
        if (status != RELEASED ) return ;
        if( requsts.empty() ) return ;
        int next = requsts.front() ;
        waiting_map[next]->Mark() ;
        requsts.pop_front() ;
        curr_client = next ; 
        status = LOCKED ;
        std::cerr<<"Resurce belong to  "<<curr_client<<" now ... "<<std::endl;
    }
    //  clean a client's everything
    void clean_client( int client_id )
    {
        if( curr_client == client_id )
        {
            std::cerr<<"Resurce release by  "<<curr_client<<" now ... "<<std::endl;
            curr_client = -1 ;
            status = RELEASED ;
        }
        if( std::find(requsts.begin() ,requsts.end() ,client_id) != requsts.end() )
            requsts.erase( std::find(requsts.begin() ,requsts.end() ,client_id));
        if( waiting_map.find(client_id) != waiting_map.end() )
            waiting_map.erase(client_id);
        if( status == RELEASED )
            check_notify();
    }
} ; // struct ServerSingleton


#endif //__ISSUE2_SINGLETON_H_
