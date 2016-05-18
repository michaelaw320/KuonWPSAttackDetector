/*
 * utils.c
 *
 *  Created on: Apr 22, 2016
 *      Author: michael
 */

#include "utils.h"

void ParseAPAddressToByte(char* ap_address_str, unsigned char* ap_address_byte) {
	if(strlen(ap_address_str) == 12) {
		char buffer[2];
		int i,j;
		j = 0;
		for(i=0;i<6;i++) {
			buffer[0] = ap_address_str[j++];
			buffer[1] = ap_address_str[j++];
			ap_address_byte[i] = (unsigned char) strtoul(buffer, NULL, 16);
		}
	}
}

int InitCounter() {
	/**
	 * Returns 1 if success, return 0 if fails
	 */
	int i;
	COUNTER = (time_t*) malloc(THRESHOLD*sizeof(time_t));
	if (COUNTER == NULL) {
		return 0;
	} else {
		for (i=0; i < THRESHOLD; i++) {
			COUNTER[i] = -1;
		}
		return 1;
	}
}
void PurgeExpiredTime() {
	int i;
	time_t current_time = time(NULL);
	for(i=0; i < THRESHOLD; i++) {
		if (difftime(current_time, COUNTER[i]) > TTL*60 && COUNTER[i] != -1) {
			//Purge it with 0
			COUNTER[i] = -1;
		}
	}
}
int GetCounterCount() {
	int i;
	int count = 0;
	for(i=0; i < THRESHOLD; i++) {
		if (COUNTER[i] != -1) {
			count++;
		}
	}
	return count;
}
int AddCurrentTime() {
	/**
	 * Returns 1 if success, return 0 if fails
	 */
	int i;
	int ret = 0;
	time_t current_time;

	//Get current Time
	time(&current_time);

	//Find first available slot
	for(i=0; i < THRESHOLD; i++) {
		if (COUNTER[i] == -1) {
			COUNTER[i] = current_time;
			ret = 1;
			break;
		}
	}
	return ret;
}

void DestroyCounter() {
	free(COUNTER);
}

