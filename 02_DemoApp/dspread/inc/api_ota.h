#ifndef _API_OTA_H
#define _API_OTA_H


int OsOtaAppUpdate(const char *filename);

int OsOtaFirmwareUpdate(const char* filename);

int OsOtaSpwareUpdate(char *file,char *filename);

int OsOtaFirmwareUpdateNoreboot(const char* filename);

int OsOtaAppCheck(char* data, unsigned int datalen);

#endif
