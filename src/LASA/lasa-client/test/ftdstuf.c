#include "Auger.h"
#include <stdlib.h>
#include <unistd.h>

#define MAX_DEVICES		1

DWORD write_data(FT_HANDLE dev,char *dat,int size)
{
	DWORD dwBytesWritten;
	FT_STATUS ftStatus; //really an ULONG

	if((ftStatus = FT_Write(dev, dat, size, &dwBytesWritten)) != FT_OK) {
		printf("Error FT_Write(%d)\n", (int)ftStatus);
	}
	return(dwBytesWritten);
} 

DWORD read_data(FT_HANDLE dev, unsigned char *dat)
{
	DWORD dwRxSize,dwBytesRead;
	FT_STATUS ftStatus = FT_OK;

	dwRxSize = 0;			
	while ((dwRxSize < BUF_SIZE) && (ftStatus == FT_OK)) {
		ftStatus = FT_GetQueueStatus(dev, &dwRxSize);
	}
	if(dwRxSize > BUF_SIZE) dwRxSize = BUF_SIZE;
	if(ftStatus == FT_OK) {
		if((ftStatus = FT_Read(dev, dat, dwRxSize, &dwBytesRead)) != FT_OK) {
			printf("Error FT_Read(%d)\n", (int)ftStatus);
		}
	}
	else {
		printf("Error FT_GetQueueStatus(%d)\n", (int)ftStatus);	
	}
	return(dwBytesRead);
}
void flush_data(FT_HANDLE dev)
{
	FT_Purge(dev,FT_PURGE_RX || FT_PURGE_TX);
	sleep(1);
	FT_ResetDevice(dev);
	return;
}

DWORD read_ndata(FT_HANDLE dev, unsigned char *dat,DWORD size)
{
	DWORD dwRxSize,dwBytesRead;
	FT_STATUS ftStatus = FT_OK;
	int k=0;

	dwRxSize = 0;			
	while ((dwRxSize < size) && (ftStatus == FT_OK) ) {
		ftStatus = FT_GetQueueStatus(dev, &dwRxSize);
		k++;
		if(k>1000) break;
	}
	if(dwRxSize == 0) return(0);
	if((ftStatus = FT_Read(dev, dat, size, &dwBytesRead)) != FT_OK) {
		printf("Error FT_Read(%d)\n", (int)ftStatus);
	}
    printf("\nReceived data[0] = 0x%x (%d)\t Received data[1]= 0x%x (%d)\n",dat[0],dat[0],dat[1],dat[1]) ;
	return(dwBytesRead);
}

FT_HANDLE init_usb_ex()
{
	FT_STATUS	ftStatus;
	FT_HANDLE	ftHandle;
	int	iNumDevs = 0;
	int	i;
	FT_DEVICE_LIST_INFO_NODE *devInfo;
  	char    cBufLD[1][64];
    char *  pcBufLD[2];
	
	ftStatus = FT_CreateDeviceInfoList((LPDWORD)(&iNumDevs));
	if (ftStatus == FT_OK) {
	   printf("Number of devices is %d\n",iNumDevs);
	}

	//
	// allocate storage for list based on numDevs
	//
	devInfo = (FT_DEVICE_LIST_INFO_NODE*)malloc(sizeof(FT_DEVICE_LIST_INFO_NODE)*iNumDevs);

	//
	// get the device information list
	//
	ftStatus = FT_GetDeviceInfoList(devInfo,(LPDWORD)(&iNumDevs));
	printf("Getting device info list %d\n",(int)ftStatus);
	
	for(i = 0; i < MAX_DEVICES; i++) {
		pcBufLD[i] = cBufLD[i];
	}
	pcBufLD[MAX_DEVICES] = NULL;

	ftStatus = FT_ListDevices(pcBufLD, &iNumDevs, FT_LIST_ALL | FT_OPEN_BY_SERIAL_NUMBER);
//	ftStatus = FT_ListDevices(pcBufLD, &iNumDevs, FT_LIST_ALL | FT_OPEN_BY_DESCRIPTION);
	printf("List devices %d\n",(int)ftStatus);
	
	if(ftStatus != FT_OK) {
		printf("Error: FT_ListDevices(%d)\n", (int)ftStatus);
		return NULL;
	}
	i=0;
	/* Setup */
	if((ftStatus = FT_OpenEx(cBufLD[i], FT_OPEN_BY_SERIAL_NUMBER, &ftHandle)) != FT_OK){
		/* 
			This can fail if the ftdi_sio driver is loaded
	 		use lsmod to check this and rmmod ftdi_sio to remove
			also rmmod usbserial
	 	*/
		printf("Error FT_Open%x(%d), device\n", (int)ftStatus, i);
		return NULL;
	}
	
	FT_SetDeadmanTimeout(ftHandle, 100000);
	
	FT_ResetDevice(ftHandle);
	// baud rate does not seem to matter at all!
	ftStatus = FT_SetBitMode(ftHandle,0xff,0); // reset bit mode
	if((ftStatus = FT_SetBaudRate(ftHandle, 19200)) != FT_OK) {
		printf("Error FT_SetBaudRate(%d), cBufLD[i] = %s\n", (int)ftStatus, cBufLD[i]);
		exit(-1);
	}
	printf("Almost done....\n");
	sleep(1);
	flush_data(ftHandle);
	return(ftHandle);
	}
	
FT_HANDLE init_usb()
{
	FT_STATUS	ftStatus;
	FT_HANDLE	ftHandle;

	if((ftStatus = FT_Open(0, &ftHandle)) != FT_OK){
		/* 
			This can fail if the ftdi_sio driver is loaded
	 		use lsmod to check this and rmmod ftdi_sio to remove
			also rmmod usbserial
	 	*/
		printf("Error FT_Open%x, device\n", (int)ftStatus);
		return NULL;
	}
	
	FT_SetDeadmanTimeout(ftHandle, 100000);
	
	FT_ResetDevice(ftHandle);
	flush_data(ftHandle);
	return(ftHandle);
	}	
	
void close_usb(FT_HANDLE ftHandle)
	{
		FT_Close(ftHandle);
	}
	
