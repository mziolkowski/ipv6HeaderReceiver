/*
 * ipv6hdr.h
 *
 *  Created on: Nov 8, 2017
 *      Author: maciej
 */

#ifndef IPV6HDR_H_
#define IPV6HDR_H_

struct IPv6hdr {
	unsigned int ver: 4;
	unsigned int traf_class: 8;
	unsigned int flow_lab: 20;
	unsigned int length: 16;
	unsigned int next_header: 8;
	unsigned int hop_limit: 8;
	unsigned char src_addr[16];
	unsigned char dest_addr[16];
	struct eleBufora *nowyEle;

};

#endif /* IPV6HDR_H_ */
