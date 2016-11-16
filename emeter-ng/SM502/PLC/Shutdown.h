

#ifndef SHUTDOWN_H
#define SHUTDOWN_H
#include <stdint.h>

/**
* Read TI host message specification
*/

#define SOFT_RESET 0
#define SOFT_SHUTDOWN 1

void Shutdown_Request(uint8_t type);

#endif 
