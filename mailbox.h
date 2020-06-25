#ifndef MAILBOX_H
#define MAILBOX_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  bool      full;
  uint64_t  data;
} mailbox_t;

extern volatile mailbox_t mailbox;

static inline void mailbox_send( uint64_t data ) {
  // Wait for previous to finish
  while( mailbox.full != false );
  // Place data into mailbox
  mailbox.data = data;
  // Indicate availability
  mailbox.full = true;
}

static inline void mailbox_recv( uint64_t *data ) {
  // Wait for previous to finish
  while( mailbox.full != true );
  // Fetch data from mailbox
  *data = mailbox.data;
  // Indicate availability
  mailbox.full = false;
}

#ifdef __cplusplus
}
#endif

#endif /*MAILBOX_H*/
