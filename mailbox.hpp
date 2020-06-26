#ifndef MAILBOX_HPP
#define MAILBOX_HPP

#include <stdint.h>

struct Mailbox {

  using Data_t = uint64_t;

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
  bool   m_full;
  Data_t m_data;
};

#endif /*MAILBOX_HPP*/
