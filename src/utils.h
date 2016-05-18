/*
 * utils.h
 *
 *  Created on: Apr 22, 2016
 *      Author: michael
 */

#ifndef SRC_UTILS_H_
#define SRC_UTILS_H_

#include <string.h>
#include <stdlib.h>
#include <time.h>

extern time_t* COUNTER;
extern int THRESHOLD;
extern int TTL;

void ParseAPAddressToByte(char* ap_address_str, unsigned char* ap_address_byte);

int InitCounter();
void PurgeExpiredTime();
int GetCounterCount();
int AddCurrentTime();
void DestroyCounter();

#endif /* SRC_UTILS_H_ */
