#ifndef SIX_EXECUTE_H
#define SIX_EXECUTE_H

typedef struct
{
  enum MODE {FADE, HEARTBEAT, ROTATION};
  int HEARTBEAT_FREQ;
  int ROTATION_SPEED;
} environment_t;

#endif
