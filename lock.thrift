
service Locker {

   // client increase the index for each new package
   // if client re-send a package , do not increase the index

   // client life management interface :
   bool client_register(1:i32 client_id,2:i32 index ) ,
   bool client_exit(1:i32 client_id , 2:i32 index) ,

   // lock management
   bool lock_request_register(1:i32 client_id, 2:i32 index),
   bool lock_request_check(1:i32 client_id, 2:i32 index),
   bool lock_request_release(1:i32 client_id, 2:i32 index ),
}

