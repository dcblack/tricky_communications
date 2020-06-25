// Add memory barrier to prevent processor re-ordering

//------------------------------------------------------------------------------
#ifndef __cplusplus
#include <stdint.h>
#include <stdbool.h>

typedef struct {
  volatile bool      full;
  volatile uint64_t  data;
} mailbox_t;

extern mailbox_t mailbox;

void mailbox_send( uint64_t data ) {
  // Wait for previous to finish (spinlock)
  while( mailbox.full != false );
  asm volatile("dmb" ::: "memory"); // barrier
  // Place data into mailbox
  mailbox.data = data;
  asm volatile("dmb" ::: "memory"); // barrier
  // Indicate availability
  mailbox.full = true;
}

void mailbox_recv( uint64_t *data ) {
  // Wait for previous to finish (spinlock)
  while( mailbox.full != true );
  asm volatile("dmb" ::: "memory"); // barrier
  // Fetch data from mailbox
  asm volatile("dmb" ::: "memory"); // barrier
  *data = mailbox.data;
  // Indicate availability
  mailbox.full = false;
}
#else
//------------------------------------------------------------------------------
#include <cstdint>

struct Mailbox {

  Mailbox( void ) = default;
  Mailbox( const Mailbox& ) = delete; // No copying

  void send( uint64_t data ) noexcept {
    // Wait for previous to finish (spinlock)
    while( m_full != false );
    asm volatile("dmb" ::: "memory"); // barrier
    // Place data into mailbox
    m_data = data;
    asm volatile("dmb" ::: "memory"); // barrier
    // Indicate availability
    m_full = true;
  }

  void recv( uint64_t& data ) noexcept {
    // Wait for previous to finish (spinlock)
    while( m_full != true );
    asm volatile("dmb" ::: "memory"); // barrier
    // Fetch data from mailbox
    data = m_data;
    asm volatile("dmb" ::: "memory"); // barrier
    // Indicate availability
    m_full = false;
  }

private:
  volatile bool      m_full { false };
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

