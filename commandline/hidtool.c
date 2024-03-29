/* Name: hidtool.c
 * Project: hid-data example
 * Author: Christian Starkjohann
 * Creation Date: 2008-04-11
 * Tabsize: 4
 * Copyright: (c) 2008 by OBJECTIVE DEVELOPMENT Software GmbH
 * License: GNU GPL v2 (see License.txt), GNU GPL v3 or proprietary (CommercialLicense.txt)
 * This Revision: $Id$
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hiddata.h"
#include "../firmware/usbconfig.h"  /* for device VID, PID, vendor name and product name */
//#include "../firmware/servos.h"  /* for number of servos */

#define SERVO_NUM 6
#define REPORT_SIZE (SERVO_NUM * 2 + 1)

/* ------------------------------------------------------------------------- */

static char *usbErrorMessage(int errCode) {
	static char buffer[80];

	switch (errCode) {
	case USBOPEN_ERR_ACCESS:
		return "Access to device denied";
	case USBOPEN_ERR_NOTFOUND:
		return "The specified device was not found";
	case USBOPEN_ERR_IO:
		return "Communication error with device";
	default:
		sprintf(buffer, "Unknown USB error %d", errCode);
		return buffer;
	}
	return NULL; /* not reached */
}

double getMillis() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;
}

static usbDevice_t *openDevice(void) {
	usbDevice_t *dev = NULL;
	unsigned char rawVid[2] = { USB_CFG_VENDOR_ID }, rawPid[2] = {
			USB_CFG_DEVICE_ID };
	char vendorName[] = { USB_CFG_VENDOR_NAME, 0 }, productName[] = {
			USB_CFG_DEVICE_NAME, 0 };
	int vid = rawVid[0] + 256 * rawVid[1];
	int pid = rawPid[0] + 256 * rawPid[1];
	int err;

	if ((err = usbhidOpenDevice(&dev, vid, vendorName, pid, productName, 0))
			!= 0) {
		fprintf(stderr, "error finding %s: %s\n", productName,
				usbErrorMessage(err));
		return NULL;
	}
	return dev;
}

/* ------------------------------------------------------------------------- */

static void int16dump(char *buffer, int len) {
	int i;
	FILE *fp = stdout;
	len >>= 1;
	unsigned short *data = buffer;
	for (i = 0; i < len; i++) {
		if (i != 0) {
			if (i % 16 == 0) {
				fprintf(fp, "\n");
			} else {
				fprintf(fp, " ");
			}
		}
		fprintf(fp, "%6d", data[i]);
	}
	if (i != 0)
		fprintf(fp, "\n");
}

static int int16read(char *buffer, char *string, int buflen) {
	char *s;
	int pos = 0;

	unsigned short *data = buffer;

	while ((s = strtok(string, ", ")) != NULL && pos < buflen) {
		string = NULL;
		*data = (unsigned short) strtol(s, NULL, 0);
		data++;
		pos += 2;
	}
	return pos;
}

/* ------------------------------------------------------------------------- */

static void usage(char *myName) {
	fprintf(stderr, "usage:\n");
	fprintf(stderr, "  %s read\n", myName);
	fprintf(stderr, "  %s write <listofpositions>\n", myName);
	fprintf(stderr,
			"hint: to use servos, values should be in range 0..65535.\n");
}

int main(int argc, char **argv) {
	usbDevice_t *dev;
	char buffer[REPORT_SIZE]; /* room for dummy report ID */
	int err;

	if (argc < 2) {
		usage(argv[0]);
		exit(1);
	}
	if ((dev = openDevice()) == NULL)
		exit(1);
	if (strcasecmp(argv[1], "read") == 0) {
		int len = sizeof(buffer);
		if ((err = usbhidGetReport(dev, 0, buffer, &len)) != 0) {
			fprintf(stderr, "error reading data: %s\n", usbErrorMessage(err));
		} else {
			int16dump(buffer + 1, sizeof(buffer) - 1);
		}
	} else if (strcasecmp(argv[1], "write") == 0) {
		int i, pos;
		memset(buffer, 0, sizeof(buffer));
		for (pos = 1, i = 2; i < argc && pos < sizeof(buffer); i++) {
			pos += int16read(buffer + pos, argv[i], sizeof(buffer) - pos);
		}
		if ((err = usbhidSetReport(dev, buffer, sizeof(buffer))) != 0) /* add a dummy report ID */
			fprintf(stderr, "error writing data: %s\n", usbErrorMessage(err));
	} else if (strcasecmp(argv[1], "writeinput") == 0) {
		int i, pos;
		// read initial positions
		int len = sizeof(buffer);
		if ((err = usbhidGetReport(dev, 0, buffer, &len)) != 0) {
			fprintf(stderr, "error reading data: %s\n", usbErrorMessage(err));
		}
		// int16dump(buffer + 1, sizeof(buffer) - 1);
		char input[1000];
		int scanfEof;
		do {
			scanfEof = scanf("%[^\n]%*c", input);
			// printf("[%i] %s\n", scanfEof, input);
			int16read(buffer + 1, input, sizeof(buffer) - 1);
			// int16dump(buffer + 1, sizeof(buffer) - 1);
			buffer[0] ++;
			int retry = 0;
			do {
				if ((err = usbhidSetReport(dev, buffer, sizeof(buffer))) != 0) { /* add a dummy report ID */
					fprintf(stderr, "error writing data: %s\n", usbErrorMessage(err));
				}
			} while (err != 0 && retry ++ < 3);
			if (err != 0) {
				// try to reconnect
				usbhidCloseDevice(dev);
				dev = openDevice();
				if ((err = usbhidSetReport(dev, buffer, sizeof(buffer))) != 0) { /* add a dummy report ID */
					fprintf(stderr, "error writing data: %s\n", usbErrorMessage(err));
				}
			}
			//usleep(20000);
		} while (scanfEof > 0);
//		for (pos = 1, i = 2; i < argc && pos < sizeof(buffer); i++) {
//			pos += int16read(buffer + pos, argv[i], sizeof(buffer) - pos);
//		}
//		if ((err = usbhidSetReport(dev, buffer, sizeof(buffer))) != 0) /* add a dummy report ID */
//			fprintf(stderr, "error writing data: %s\n", usbErrorMessage(err));
	} else {
		usage(argv[0]);
		exit(1);
	}
	usbhidCloseDevice(dev);
	return 0;
}

/* ------------------------------------------------------------------------- */
