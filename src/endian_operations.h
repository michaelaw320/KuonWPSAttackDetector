/*
 * endian_operations.h
 *
 *  Created on: Apr 21, 2016
 *      Author: michael
 */

#ifndef SRC_ENDIAN_OPERATIONS_H_
#define SRC_ENDIAN_OPERATIONS_H_

#include <stdlib.h>

#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN 0
#endif
#ifndef BIG_ENDIAN
#define BIG_ENDIAN    1
#endif

int DeterminePlatformEndianess();

void reverse_u_int16_t (u_int16_t* toReverse);

#endif /* SRC_ENDIAN_OPERATIONS_H_ */
