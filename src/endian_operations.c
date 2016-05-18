/*
 * endian_operations.c
 *
 *  Created on: Apr 21, 2016
 *      Author: michael
 */

#include "endian_operations.h"

int DeterminePlatformEndianess() {
    int i = 1;
    char *p = (char *)&i;

    if (p[0] == 1)
        return LITTLE_ENDIAN;
    else
        return BIG_ENDIAN;
}

void reverse_u_int16_t (u_int16_t* toReverse) {
	unsigned char c1, c2;
	c1 = *toReverse & 255;
	c2 = (*toReverse >> 8) & 255;
	*toReverse = 0;
	*toReverse = (c1 << 8) + c2;
}
