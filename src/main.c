/*
 * main.c
 *
 *  Created on: Apr 6, 2016
 *      Author: michael
 */

#include <stdio.h>
#include <stdlib.h>
#include <pcap/pcap.h>
#include <string.h>
#include <syslog.h>
#include "structure.h"
#include "endian_operations.h"
#include "utils.h"

//#define DEBUG 1

/* Global Variables */
int PLATFORM_ENDIANESS;
unsigned char AP_ADDRESS[ADDR_SIZE];
time_t* COUNTER;
int THRESHOLD;
int TTL;

/* private callback function when packet have captured */
static void processor(u_char *pd, const struct pcap_pkthdr *h,
		const u_char *packet) {
		/**
		 * Get radiotap header length
		 */
		u_int16_t rtheaderlen;
		const struct ieee80211_radiotap_header *rtheader;
		rtheader = (struct ieee80211_radiotap_header*) (packet);
		rtheaderlen = rtheader->it_len;
		if (PLATFORM_ENDIANESS == BIG_ENDIAN) {
			// convert endian order
			reverse_u_int16_t(&rtheaderlen);
		} else {
			// do nothing, it's already in little endian
		}

	#ifdef DEBUG
		printf("Radiotap len: %hu \n", rtheaderlen);
	#endif

		/**
		 * Check and filter sender address
		 */
		const struct ieee80211_data_header *dtheader;
		dtheader = (struct ieee80211_data_header*) (packet + rtheaderlen);
		const unsigned char* src_addr = dtheader->addr2;

	#ifdef DEBUG
		int i;
		printf("Source address: ");
		for(i=0;i<ADDR_SIZE;i++)printf("%x", src_addr[i]); printf(" cmp with ");
		for(i=0;i<ADDR_SIZE;i++)printf("%x", AP_ADDRESS[i]);
		printf("\n");
		const unsigned char* dst_addr = dtheader->addr1;
		printf("Dest address: ");
		for(i=0;i<ADDR_SIZE;i++)printf("%x", dst_addr[i]);
		printf("\n");
	#endif

		if (memcmp(AP_ADDRESS, src_addr, ADDR_SIZE) == 0) {
			const struct eap_header *eap;
			eap = (struct eap_header*) (packet + rtheaderlen
					+ sizeof(struct ieee80211_data_header)
					+ sizeof(struct llc_header));
			if (eap->eap_type == WSC_TYPE && eap->opcode == WSC_NACK_OPCODE) {
				const struct wsc_eapol *wsc;
				wsc = (struct wsc_eapol*) (packet + rtheaderlen
						+ sizeof(struct ieee80211_data_header)
						+ sizeof(struct llc_header) + sizeof(struct eap_header));

	#ifdef DEBUG
				printf("eap vendor type: %x\n", eap->eap_vendor_type);

				printf("WSC Opcode: %x \t WSC_Message_type: %x \n", eap->opcode, wsc->message_type);
	#endif

				if (wsc->message_type == WSC_NACK_MESSAGE) {
					PurgeExpiredTime();
					AddCurrentTime();
					int count = GetCounterCount();
					syslog(LOG_WARNING, "Kuon Packet Capture: <<<WSC_NACK DETECTED>>> Counter: %d, Threshold: %d, TTL: %d min", count, THRESHOLD, TTL);
					if (count >= THRESHOLD) {
						syslog(LOG_CRIT, "WPS Attack Detected, sending notification");
						pcap_breakloop((pcap_t*) pd);
						return;
					}
				}

	#ifdef DEBUG
				printf("Packet match with source addr");
				printf("\n");
				printf("Frame Length: %d ;Frame content: ", h->len);
				for(i=0; i < h->len; i++) {
					if(i%16 == 0) {
						printf("\n");
					} else if (i%8 == 0) {
						printf("   ");
					}
					if(packet[i] < 0x10)
					printf("0%x ", packet[i]);
					else
					printf("%x ", packet[i]);

				}
				printf("\n\n");
	#endif
			}
		}
}

int main(int argc, char* argv[]) {
	/**
	 * Parameter: BSSID(without ':'), Threshold, TTL (in minutes), capture device
	 * Return value:	0 : exit normally, detected WSC_NACK to threshold
	 * 					1 : initialization error
	 * 					2 : libpcap init error
	 * 					3 : libpcap filter fails
	 */
	/**
	 * Declarations
	 */
	PLATFORM_ENDIANESS = DeterminePlatformEndianess();
	char* ap_address_str;
	/* the error code buf of libpcap, PCAP_ERRBUF_SIZE = 256 */
	char ebuf[PCAP_ERRBUF_SIZE];
	char *dev;
	pcap_t *pd;
	//int pcap_loop_ret;
	char* filterString = "type data subtype data";
	struct bpf_program filter;

	if (argc == 5) {
		ap_address_str = argv[1];
		ParseAPAddressToByte(ap_address_str, AP_ADDRESS);
		THRESHOLD = atoi(argv[2]);
		TTL = atoi(argv[3]);
		dev = argv[4];
	} else {
		syslog(LOG_ERR, "Parameter is not complete, parameter as follows BSSID(NO :),Threshold, TTL(in minutes),CaptureDevice");
		return (1);
	}

	if (dev == NULL) {
		/* e.g. "no suitable device found" */
		syslog(LOG_ERR, "Device is NULL");
		return (2);
	}

	if (InitCounter() == 0) {
		syslog(LOG_ERR, "Cannot allocate memory for counter!");
		return (2);
	}

	/* create capture handler of libpcap */
	pd = pcap_create(dev, ebuf);
	if (pd == NULL) {
		/* e.g. "Operation not permitted" */
		syslog(LOG_ERR,"pcap_open_live error: %s\n", ebuf);
		return (2);
	}
	pcap_activate(pd);

	// Compiles the filter to kernel
	if (pcap_compile(pd, &filter, filterString, 0, PCAP_NETMASK_UNKNOWN)
			== -1) {
		syslog(LOG_ERR, "Couldn't parse filter %s: %s\n", filterString,
				pcap_geterr(pd));
		return (3);
	}
	if (pcap_setfilter(pd, &filter) == -1) {
		syslog(LOG_ERR, "Couldn't install filter %s: %s\n", filterString,
				pcap_geterr(pd));
		return (3);
	}

	/* start the loop of capture, loop until exit */
	syslog(LOG_INFO, "Kuon Packet Capture Started");
	//pcap_loop_ret = pcap_loop(pd, -1, mycb, (u_char*) pd);
	pcap_loop(pd, -1, processor, (u_char*) pd);

	pcap_close(pd);
	DestroyCounter();
	return 0;
}
