/*
 * *****************************************************************
 * *                                                               *
 * *    Copyright (c) Digital Equipment Corporation, 1991, 1996    *
 * *                                                               *
 * *   All Rights Reserved.  Unpublished rights  reserved  under   *
 * *   the copyright laws of the United States.                    *
 * *                                                               *
 * *   The software contained on this media  is  proprietary  to   *
 * *   and  embodies  the  confidential  technology  of  Digital   *
 * *   Equipment Corporation.  Possession, use,  duplication  or   *
 * *   dissemination of the software and media is authorized only  *
 * *   pursuant to a valid written license from Digital Equipment  *
 * *   Corporation.                                                *
 * *                                                               *
 * *   RESTRICTED RIGHTS LEGEND   Use, duplication, or disclosure  *
 * *   by the U.S. Government is subject to restrictions  as  set  *
 * *   forth in Subparagraph (c)(1)(ii)  of  DFARS  252.227-7013,  *
 * *   or  in  FAR 52.227-19, as applicable.                       *
 * *                                                               *
 * *****************************************************************
 */
/*
 * HISTORY
 */
#pragma ident "@(#)$RCSfile: pfwrite.c,v $ $Revision: 1.1.2.2 $ (DEC) $Date: 1995/12/06 16:21:25 $"

/*
 * pfwrite.c
 *
 * Simple program to demonstrate how to write packets using the packet
 * filter.  Sends one bogus packet, to a unicast address (8:0:2b:38:10:cc)
 * that we know is not on your network.
 *
 * Input:
 *      None
 *
 * Output:
 *      All output sent to stderr.
 *
 * Compile:
 *	cc -o pfwrite pfwrite.c
 *
 * Usage:
 *	pfwrite [interface]
 *
 * Comments:
 *  Monitor the output with "tcpdump ether dst 8:0:2b:38:10:cc"
 *  or "tcpdump fddi dst 8:0:2b:38:10:cc".  You may need to specify
 *  an interface to tcpdump, e.g. "tcpdump -i fta0 fddi dst 8:0:2b:38:10:cc".
 */

/*
 * Digital Equipment Corporation supplies this software example on 
 * an "as-is" basis for general customer use.  Note that Digital 
 * does not offer any support for it, nor is it covered under any 
 * of Digital's support contracts. 
 */
 

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <sys/time.h>
#include <net/pfilt.h>
#include <netinet/in.h>
#include <net/if.h>
#include <netinet/if_ether.h>
#include <netinet/if_fddi.h>
#include <net/if_llc.h>

#define	DATABYTES 40		/* Maximum length of packet data */

/* Example Ethernet packet */
struct {
	struct ether_header hdr;
	char data[DATABYTES];
} EthernetPacket;

/* Example FDDI packet */
struct {
	struct fddi_header hdr;
	struct llc llc;
	char data[DATABYTES];
} FDDIPacket;

main(argc, argv)
int argc;
char **argv;
{
        int fd;				/* file descriptor for packet filter */
	char *ifname;			/* name of interface to send on */
	struct endevp devp;		/* device parameters */
	char *datap;			/* pointer to packet */
	int datasize;			/* packet size in bytes */

	if (argc <= 1)
	    ifname = "pf0";		/* defaults to first interface */
	else
	    ifname = argv[1];

	/* Open interface for reading and writing */
        if ((fd = pfopen(ifname, O_RDWR )) < 0) {
            perror("pfopen");
	    exit(1);
        }

	/* Obtain device parameters to see what kind of LAN it is */
        if (ioctl(fd, EIOCDEVP, &devp) == -1) {
            perror("ioctl EIOCDEVP");
            exit(1);
        }

	switch (devp.end_dev_type) {
	    case ENDT_10MB:		/* Ethernet */
		datap = (char *)&EthernetPacket;
		datasize = sizeof(EthernetPacket);
		/* Set up the Ethernet destination address */
		EthernetPacket.hdr.ether_dhost[0] = 0x08;
		EthernetPacket.hdr.ether_dhost[1] = 0x00;
		EthernetPacket.hdr.ether_dhost[2] = 0x2b;
		EthernetPacket.hdr.ether_dhost[3] = 0x38;
		EthernetPacket.hdr.ether_dhost[4] = 0x10;
		EthernetPacket.hdr.ether_dhost[5] = 0xCC;
		/* Ethernet source address is set by kernel */
		bzero(EthernetPacket.hdr.ether_shost,
			sizeof(EthernetPacket.hdr.ether_shost));
		/* Set a bogus Ethernet type */
		EthernetPacket.hdr.ether_type = 0xFFFF;
		/* Set a little data */
		strncpy(EthernetPacket.data, "Hello network", DATABYTES);
		break;

	    case ENDT_FDDI:		/* FDDI */
		datap = (char *)&FDDIPacket;
		datasize = sizeof(FDDIPacket);
		/* Set FDDI Frame Control bits */
		FDDIPacket.hdr.fddi_fc = FDDIFC_LLC_ASYNC | FDDIFC_LLC_PRI7;
		/* Set up the FDDI destination address */
		FDDIPacket.hdr.fddi_dhost[0] = 0x08;
		FDDIPacket.hdr.fddi_dhost[1] = 0x00;
		FDDIPacket.hdr.fddi_dhost[2] = 0x2b;
		FDDIPacket.hdr.fddi_dhost[3] = 0x38;
		FDDIPacket.hdr.fddi_dhost[4] = 0x10;
		FDDIPacket.hdr.fddi_dhost[5] = 0xCC;
		/* FDDI source address is set by kernel */
		bzero(FDDIPacket.hdr.fddi_shost,
			sizeof(FDDIPacket.hdr.fddi_shost));
		/* Set up IEEE 802.2 header */
		FDDIPacket.llc.llc_dsap = LLC_SNAP_LSAP;
		FDDIPacket.llc.llc_ssap = LLC_SNAP_LSAP;
		FDDIPacket.llc.llc_snap.control = LLC_UI;
		bzero(FDDIPacket.llc.llc_snap.org_code,
			sizeof(FDDIPacket.llc.llc_snap.org_code));
		/* Set a bogus packet type */
		FDDIPacket.llc.llc_snap.ether_type = 0xFFFF;
		/* Set a little data */
		strncpy(FDDIPacket.data, "Hello network", DATABYTES);
		break;

	   default:
		fprintf(stderr,
			"Unsupported packet filter device type %d\n",
			devp.end_dev_type);
		exit(1);
		break;
	}

	/* Send the packet */
        if (write (fd, datap, datasize) != datasize) {
            perror ("write");
	    exit(1);
        }
	
	close(fd);
	exit(0);
}




