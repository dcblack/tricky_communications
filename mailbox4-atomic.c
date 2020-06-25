// Solutions using atomics

//------------------------------------------------------------------------------
#ifndef __cplusplus
#include <stdint.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <threads.h>

typedef struct {
  atomic_bool full;
  volatile uint64_t data;
} mailbox_t;

extern mailbox_t mailbox;

void mailbox_init( void ) {
    mailbox.full = ATOMIC_VAR_INIT(false);
}
void mailbox_send( uint64_t data ) {
  // Wait for previous to finish (spinlock)
  while( mailbox.full != false ) \
    thrd_yield();
  // Place data into mailbox
  mailbox.data = data;
  // Indicate availability
  mailbox.full = true;
}

void mailbox_recv( uint64_t *data ) {
  // Wait for previous to finish (spinlock)
  while( mailbox.full != true ) \
    thrd_yield();
  // Fetch data from mailbox
  *data = mailbox.data;
  // Indicate availability
  mailbox.full = false;
}
#else
//------------------------------------------------------------------------------
#include <cstdint>
#include <atomic>
#include <thread>

struct Mailbox {

  Mailbox( void )= default;
  Mailbox( const Mailbox& ) = delete; // No copying

  void send( uint64_t data ) noexcept {
    // Wait for previous to finish (spinlock)
    while( m_full != false ) \
      std::this_thread::yield();
    // Place data into mailbox
    m_data = data;
    // Indicate availability
    m_full = true;
  }

  void recv( uint64_t& data ) noexcept {
    // Wait for previous to finish (spinlock)
    while( m_full != true ) \
      std::this_thread::yield();
    // Fetch data from mailbox
    data = m_data;
    // Indicate availability
    m_full = false;
  }

private:
  std::atomic<bool>  m_full { false };
  volatile uint64_t  m_data { 0xDEADBEEFull };
};

extern Mailbox mailbox2;
#endif

//------------------------------------------------------------------------------
uint64_t Something( uint64_t v) {
    // Recursive won't be inlined
    if( v <= 1 ) return 1;
    else return Something(v-1) * v;
}

#ifndef __cplusplus
void mailbox_send1( uint64_t data ) {
    mailbox_send( data );
}
void mailbox_recv1( uint64_t* data_ptr ) {
    mailbox_recv( data_ptr );
}

//------------------------------------------------------------------------------
#else

void mailbox_send2( uint64_t data ) {
    mailbox2.send( data );
}
void mailbox_recv2( uint64_t& data ) {
    mailbox2.send( data );
}

#endif
//------------------------------------------------------------------------------
#include <stdlib.h>

int main(void)
{
    uint64_t value = 0xBEEFCAFE;
#ifndef __cplusplus
    mailbox_send1( value );
    printf("%lu\n",Something( value ));
    mailbox_recv1( value );
#else
    mailbox_send2( value );
    printf("%lu\n",Something( value ));
    mailbox_recv2( value );
#endif
    return 0;
}

