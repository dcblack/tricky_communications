#ifndef MAILBOX_HPP
#define MAILBOX_HPP

#include <cstdint>
#define uint64_t Data_t;

struct Mailbox {

  Mailbox( void ) = default;
  Mailbox( const Mailbox& ) = delete;

  void send( Data_t data ) {
    // Wait for previous to finish
    while( m_full != false );
    // Place data into mailbox
    m_data = data;
    // Indicate availability
    m_full = true;
  }

  void recv( Data_t& data ) {
    // Wait for previous to finish
    while( m_full != true );
    // Fetch data from mailbox
    data = m_data;
    // Indicate availability
    m_full = false;
  }

private:
  struct pImpl_t;
  pImpl_t* m;
};

#endif /*MAILBOX_HPP*/
