#ifndef SIX_EXECUTE_H
#define SIX_EXECUTE_H

typedef struct
{
  unsigned int MAJOR;
  unsigned int MINOR;
  enum { FADE, HEARTBEAT, ROTATION } MODE;
  int HEARTBEAT_FREQ;
  int ROTATION_SPEED;
} environment_t;

#endif
