/*
 * structure.h
 *
 *  Created on: Apr 21, 2016
 *      Author: michael
 */

#ifndef SRC_STRUCTURE_H_
#define SRC_STRUCTURE_H_

#ifndef ADDR_SIZE
#define ADDR_SIZE 6
#endif

struct ieee80211_radiotap_header {
        u_int8_t        it_version;     /* set to 0 */
        u_int8_t        it_pad;
        u_int16_t       it_len;         /* entire length */
        u_int32_t       it_present;     /* fields present */
} __attribute__((__packed__));

struct ieee80211_data_header {
	u_int16_t      frame_ctl;
	u_int16_t      duration_id;
	unsigned char  addr1[ADDR_SIZE]; //dst address
	unsigned char  addr2[ADDR_SIZE]; //src address
	unsigned char  addr3[ADDR_SIZE];
	u_int16_t      seq_num;
} __attribute__((__packed__));

struct llc_header {
	u_int8_t dsap;
	u_int8_t ssap;
	u_int8_t ctrl;

	u_int8_t oui[3];
	u_int16_t ethertype;
} __attribute__((__packed__));

struct eap_header {
	u_int8_t version;
	u_int8_t type;
	u_int16_t auth_length;

	u_int8_t code;
	u_int8_t eap_identifier;
	u_int16_t eap_length;
	u_int8_t eap_type;
	unsigned char eap_ext_vendor[3];
	u_int32_t eap_vendor_type;
	u_int8_t opcode; //WSC_NACK code: 0x3
	u_int8_t flag;
} __attribute__((__packed__));

struct wsc_eapol {
	unsigned char wsc_version[5];

	//Message Type
	u_int16_t data_element_type;
	u_int16_t data_element_len;
	u_int8_t message_type; // WSC_NACK : 0x0e

	//Nonces data not included, we're not interested in that
} __attribute__((__packed__));

#ifndef WSC_TYPE
#define WSC_TYPE 254
#endif
#ifndef WSC_NACK_OPCODE
#define WSC_NACK_OPCODE 0x3
#endif
#ifndef WSC_NACK_MESSAGE
#define WSC_NACK_MESSAGE 0x0e
#endif

#endif /* SRC_STRUCTURE_H_ */
