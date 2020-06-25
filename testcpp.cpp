#include "mailbox.hpp"

#include <ctype.h>
#include <iostream>
#include <sstream>
#include <string>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
size_t N = 2'000'000;
std::atomic<uint64_t>   send_count = 0ull;
std::atomic<uint64_t>   rcvd_count = 0ull;
std::mutex              mtx;
volatile bool           ready = false;
std::condition_variable startup;
std::atomic<bool>       shutdown = false;

Mailbox mailbox;

std::mutex              iomtx;
#define MESSAGE(mesg) do {\
  std::scoped_lock<std::mutex> lck(iomtx);\
  std::cout << mesg << std::endl;\
} while(0)

int main( int argc, const char* argv[] )
{
  for( int i=1; i<argc; ++i ) {
    if( isdigit( argv[i][0])  ) {
      std::istringstream is( argv[i] );
      is >> N;
    } else if ( argv[i][0] == '-' ) {
      std::string arg { argv[i] };
      if( arg == "-h" or arg.find("-help") < 2 ) {
        std::cout << "SYNTAX: ./" << argv[0] << " [N]" << std::endl;
        return 1;
      }
    }//endif
  }//endfor

  MESSAGE( "Attempting " << N << " exchanges" );

  std::thread sender([](){
    std::unique_lock<std::mutex> lck(mtx);
    MESSAGE( "Waiting for start signal" );
    while( not ready ) startup.wait( lck );
    MESSAGE( "Sending" );
    uint64_t awake = 1ull;
    for( uint64_t i=0; i<N; ++i ) {
      mailbox.send( i );
      ++send_count;
      if( awake and send_count == awake ) {
        MESSAGE( awake );
        awake <<= 1;
      }
      std::this_thread::yield();
    }//endfor
    MESSAGE( "Send complete" );
  });

  std::thread receiver([](){
    uint64_t j = ~0ull;
    uint64_t k = 0ull;
//  std::unique_lock<std::mutex> lck(mtx);
//  MESSAGE( "Waiting for start signal" );
//  while( not ready ) startup.wait( lck );
    MESSAGE( "Receiving" );
    uint64_t awake = 1ull;
    do {
      mailbox.recv( j );
      if( j != k ) {
        MESSAGE( "Mismatch: expected " << k << ", but got " << j );
        k = j + 1;
      } else {
        ++k;
      }
      ++rcvd_count;
      if( awake and rcvd_count == awake ) {
        MESSAGE( awake );
        awake <<= 1;
      }
      std::this_thread::yield();
    } while( rcvd_count != N and not shutdown );
    MESSAGE( "Receipt complete" );
  });

  std::this_thread::yield();
  {
    std::unique_lock<std::mutex> lck(mtx);
    ready = true;
    std::this_thread::sleep_for( std::chrono::milliseconds( 10 ));
    startup.notify_all();
  }
  {
    MESSAGE( "Waiting for sender to complete" );
    sender.join();
    MESSAGE( "Allowing receiver to complete" );
    std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ));
    if( send_count != rcvd_count ) shutdown = true;
    MESSAGE( "Waiting for receiver to complete" );
    receiver.join();
  }
  MESSAGE( "Completed with " << send_count << " sent and " << rcvd_count << " received" );
  return 0;
}
