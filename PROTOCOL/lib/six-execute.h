#ifndef SIX_EXECUTE_H
#define SIX_EXECUTE_H

typedef struct environment
{
  enum MODE {HEARTBEAT, ROTATION};
  int HEARTBEAT_FREQ;
  int ROTATION_SPEED;
};

#endif
