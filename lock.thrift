
service Locker {

   // client life management interface :
   bool client_register(1:i32 client_id,2:bool is_retry ) ,
   bool client_exit(1:i32 client_id , 2:bool is_retry ) ,

   // lock management
   bool lock_request_register(1:i32 client_id, 2:bool is_retry ),
   bool lock_request_check(1:i32 client_id, 2:bool is_retry ),
   bool lock_request_release(1:i32 client_id, 2:bool is_retry ),
}

