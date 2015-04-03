#define IMPLEMENT_API
#define NEKO_COMPATIBLE

#include <hx/CFFI.h>

/*
	Modified from ofSerial from OpenFrameworks.
	Date: 20100505
	Author: Andy Li andy@onthewings.net
*/

#if defined( TARGET_OSX ) || defined( TARGET_LINUX )
	#include <termios.h>
	#include <sys/ioctl.h>
	#include <getopt.h>
	#include <unistd.h>
	#include <dirent.h>
#else
	#pragma comment(lib, "setupapi.lib")
	#include <winbase.h>
	#include <tchar.h>
	#include <iostream>
	#include <string.h>
	#include <setupapi.h>
	#include <regstr.h>
	#define MAX_SERIAL_PORTS 256
	#include <winioctl.h>
	#ifdef __MINGW32__
		#define INITGUID
		#include <initguid.h> // needed for dev-c++ & DEFINE_GUID
	#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>


//---------------------------------------------
#ifdef TARGET_WIN32
//---------------------------------------------

char**		portNamesShort = new char * [MAX_SERIAL_PORTS];
char**		portNamesFriendly = new char * [MAX_SERIAL_PORTS];
int	 		nPorts = 0;
bool 		bPortsEnumerated = false;

//------------------------------------
// needed for serial bus enumeration:
//4d36e978-e325-11ce-bfc1-08002be10318}
DEFINE_GUID (GUID_SERENUM_BUS_ENUMERATOR, 0x4D36E978, 0xE325, 0x11CE, 0xBF, 0xC1, 0x08, 0x00, 0x2B, 0xE1, 0x03, 0x18);
//------------------------------------

void enumerateWin32Ports(){
	if (bPortsEnumerated == true) return;

	for (int i = 0; i < MAX_SERIAL_PORTS; i++){
		portNamesShort[i] = new char[10];
		portNamesFriendly[i] = new char[MAX_PATH];
	}

	HDEVINFO hDevInfo = NULL;
	SP_DEVINFO_DATA DeviceInterfaceData;
	int i = 0;
	DWORD dataType, actualSize = 0;
	unsigned char dataBuf[MAX_PATH + 1];

	// Reset Port List
	nPorts = 0;
	// Search device set
	hDevInfo = SetupDiGetClassDevs((struct _GUID *)&GUID_SERENUM_BUS_ENUMERATOR,0,0,DIGCF_PRESENT);
	if ( hDevInfo ){
		while (TRUE){
			ZeroMemory(&DeviceInterfaceData, sizeof(DeviceInterfaceData));
			DeviceInterfaceData.cbSize = sizeof(DeviceInterfaceData);
			if (!SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInterfaceData)){
				// SetupDiEnumDeviceInfo failed
				break;
			}

			if (SetupDiGetDeviceRegistryProperty(hDevInfo,
				&DeviceInterfaceData,
				SPDRP_FRIENDLYNAME,
				&dataType,
				dataBuf,
				sizeof(dataBuf),
				&actualSize)){

				sprintf(portNamesFriendly[nPorts], "%s", dataBuf);
				portNamesShort[nPorts][0] = 0;

				// turn blahblahblah(COM4) into COM4

				char * begin = NULL;
				char * end = NULL;
				begin = strstr((char *)dataBuf, "(COM");


				if (begin) {
					end = strstr(begin, ")");
					if (end) {
						*end = 0;   // get rid of the )...
						strcpy(portNamesShort[nPorts], begin + 1);
					}
					if (nPorts++ > MAX_SERIAL_PORTS)
						break;
				}
			}
			i++;
		}
	}
	SetupDiDestroyDeviceInfoList(hDevInfo);

	bPortsEnumerated = false;
}
//---------------------------------------------
#endif
//---------------------------------------------

value enumerateDevices() {
	char empty = 0;
	char* str = &empty;
	
	//---------------------------------------------
	#if defined( TARGET_OSX )
	//---------------------------------------------

		//----------------------------------------------------
		//We will find serial devices by listing the directory
		DIR *dir;
		struct dirent *entry;
		dir = opendir("/dev");

		if (dir == NULL){
			return alloc_null();
		} else {
			while ((entry = readdir(dir)) != NULL){
				char* name = entry->d_name;
				if (strncmp(name,"cu.",3) == 0 || strncmp(name,"tty.",4) == 0) {
					int len0 = strlen(str);
					int len1 = strlen(name);
					char* nstr = (char*)malloc(len0 + len1 + 7);
					strcpy(nstr,str);
					strcat(nstr,"/dev/");
					strcat(nstr,name);
					strcat(nstr,"\n");
					if (str != &empty) free(str);
					str = nstr;
				}
			}
		}
	//---------------------------------------------
	#endif
	//---------------------------------------------

	//---------------------------------------------
	#if defined( TARGET_LINUX )
	//---------------------------------------------

		//----------------------------------------------------
		//We will find serial devices by listing the directory
		DIR *dir;
		struct dirent *entry;
		dir = opendir("/dev");

		if (dir == NULL){
			return alloc_null();
		} else {
			while ((entry = readdir(dir)) != NULL){
				char* name = entry->d_name;
				if (strncmp(name,"ttyS",4) == 0 || strncmp(name,"ttyUSB",6) == 0 || strncmp(name,"ttyACM",6) == 0 || strncmp(name,"rfc",3) == 0) {
					int len0 = strlen(str);
					int len1 = strlen(name);
					char* nstr = (char*)malloc(len0 + len1 + 7);
					strcpy(nstr,str);
					strcat(nstr,"/dev/");
					strcat(nstr,name);
					strcat(nstr,"\n");
					if (str != &empty ) free(str);
					str = nstr;
				}
			}
		}

	//---------------------------------------------
	#endif
	//---------------------------------------------

	//---------------------------------------------
	#ifdef TARGET_WIN32
	//---------------------------------------------

		enumerateWin32Ports();
		for (int i = 0; i < nPorts; i++){
			char* name = portNamesShort[i];
			int len0 = strlen(str);
			int len1 = strlen(name);
			char* nstr = (char*)malloc(len0 + len1 + 7);
			strcpy(nstr,str);
			strcat(nstr,name);
			strcat(nstr,"\n");
			if (str != &empty) free(str);
			str = nstr;
		} 

	//---------------------------------------------
	#endif
	//---------------------------------------------


	if (strlen(str) > 0) str[strlen(str)-1] = '\0';
	value ret = alloc_string(str);
	if (str != &empty) free(str);
	return ret;
}
DEFINE_PRIM(enumerateDevices,0);

value setup(value a, value b) {
	const char* portName = val_string(a);
	int baud = val_int(b);
	
	//---------------------------------------------
	#if defined( TARGET_OSX ) || defined( TARGET_LINUX )
	//---------------------------------------------
	int fd = open(portName, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if(fd == -1){
		return alloc_null();
	}

	struct termios options;
	struct termios oldoptions;
	tcgetattr(fd,&oldoptions);
	options = oldoptions;
	switch(baud){
		case 300:
			cfsetispeed(&options,B300);
			cfsetospeed(&options,B300);
			break;
		case 1200:
			cfsetispeed(&options,B1200);
			cfsetospeed(&options,B1200);
			break;
		case 2400:
			cfsetispeed(&options,B2400);
			cfsetospeed(&options,B2400);
			break;
		case 4800:
			cfsetispeed(&options,B4800);
			cfsetospeed(&options,B4800);
			break;
		case 9600:
			cfsetispeed(&options,B9600);
			cfsetospeed(&options,B9600);
			break;
		#ifdef TARGET_OSX
		case 14400:
			cfsetispeed(&options,B14400);
			cfsetospeed(&options,B14400);
			break;
		#endif
		case 19200:
			cfsetispeed(&options,B19200);
			cfsetospeed(&options,B19200);
			break;
		#ifdef TARGET_OSX
		case 28800:
			cfsetispeed(&options,B28800);
			cfsetospeed(&options,B28800);
			break;
		#endif
		case 38400:
			cfsetispeed(&options,B38400);
			cfsetospeed(&options,B38400);
			break;
		case 57600:
			cfsetispeed(&options,B57600);
			cfsetospeed(&options,B57600);
			break;
		case 115200:
			cfsetispeed(&options,B115200);
			cfsetospeed(&options,B115200);
			break;
		default:
			cfsetispeed(&options,B9600);
			cfsetospeed(&options,B9600);
			break;
	}

	options.c_cflag |= (CLOCAL | CREAD);
	options.c_cflag &= ~(PARENB | PARODD);
	options.c_cflag &= ~CSTOPB;
	options.c_cflag &= ~CRTSCTS;
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS8;

	if (tcsetattr(fd,TCSANOW,&options) != 0)
	{
		return alloc_null();
	}

	return alloc_int(fd);

	//---------------------------------------------
	#endif
	//---------------------------------------------


	//---------------------------------------------
	#ifdef TARGET_WIN32
	//---------------------------------------------

	// open the serial port:
	// "COM4", etc...

	HANDLE hComm = CreateFile(
		portName,
		GENERIC_READ|GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL
	);

	if(hComm==INVALID_HANDLE_VALUE){
		return alloc_null();
	}

	// now try the settings:
	COMMCONFIG cfg;
	DWORD cfgSize;
	char  buf[80];

	cfgSize=sizeof(cfg);
	GetCommConfig(hComm,&cfg,&cfgSize);
	int bps = baud;
	sprintf(buf,"baud=%d parity=N data=8 stop=1",bps);

	#if (_MSC_VER)       // microsoft visual studio
		// msvc doesn't like BuildCommDCB,
		//so we need to use this version: BuildCommDCBA
		if(!BuildCommDCBA(buf,&cfg.dcb)){
			//ofLog(OF_LOG_ERROR,"ofSerial: unable to build comm dcb; (%s)",buf);
		}
	#else
		if(!BuildCommDCB(buf,&cfg.dcb)){
			//ofLog(OF_LOG_ERROR,"ofSerial: Can't build comm dcb; %s",buf);
		}
	#endif


	// Set baudrate and bits etc.
	// Note that BuildCommDCB() clears XON/XOFF and hardware control by default

	if(!SetCommState(hComm,&cfg.dcb)){
		//ofLog(OF_LOG_ERROR,"ofSerial: Can't set comm state");
	}
	//ofLog(OF_LOG_NOTICE,buf,"bps=%d, xio=%d/%d",cfg.dcb.BaudRate,cfg.dcb.fOutX,cfg.dcb.fInX);

	// Set communication timeouts (NT)
	COMMTIMEOUTS tOut;
	COMMTIMEOUTS oldTimeout;
	GetCommTimeouts(hComm,&oldTimeout);
	tOut = oldTimeout;
	// Make timeout so that:
	// - return immediately with buffered characters
	tOut.ReadIntervalTimeout=MAXDWORD;
	tOut.ReadTotalTimeoutMultiplier=0;
	tOut.ReadTotalTimeoutConstant=0;
	SetCommTimeouts(hComm,&tOut);
	
	return alloc_int((int)hComm);
	//---------------------------------------------
	#endif
	//---------------------------------------------
}
DEFINE_PRIM(setup,2);

value writeBytes(value a, value b, value c) {
	int fd = val_int(a);
	unsigned char * buffer = (unsigned char *) val_string(b);
	int length = val_int(c);

	//---------------------------------------------
	#if defined( TARGET_OSX ) || defined( TARGET_LINUX )
		return alloc_int(write(fd, buffer, length));
	#endif
	//---------------------------------------------

	//---------------------------------------------
	#ifdef TARGET_WIN32
		DWORD written;
		WriteFile((HANDLE)fd, buffer, length, &written,0);
		return alloc_int((int)written);
	#endif
	//---------------------------------------------
}
DEFINE_PRIM(writeBytes,3);

value readBytes(value a, value b) {
	int fd = val_int(a);
	int length = val_int(b);
	char * buffer = (char*) malloc(length+1);
	
	//---------------------------------------------
	#if defined( TARGET_OSX ) || defined( TARGET_LINUX )
		if(read(fd, buffer, length) < 0){
			return alloc_null();
		}
	#endif
	//---------------------------------------------

	//---------------------------------------------
	#ifdef TARGET_WIN32
		DWORD nRead = 0;
		if (!ReadFile((HANDLE)fd,buffer,length,&nRead,0)){
			return alloc_null();
		}
	#endif
	//---------------------------------------------

	buffer[length] = '\0';
	value ret = alloc_string(buffer);
	free(buffer);
	return ret;
}
DEFINE_PRIM(readBytes,2);


value writeByte(value a, value b) {
	int fd = val_int(a);
	unsigned char buffer = (unsigned char) val_int(b);

	//---------------------------------------------
	#if defined( TARGET_OSX ) || defined( TARGET_LINUX )
		return alloc_bool(write(fd, &buffer, 1) > 0);
	#endif
	//---------------------------------------------

	//---------------------------------------------
	#ifdef TARGET_WIN32
		DWORD written = 0;
		if(!WriteFile((HANDLE)fd, &buffer, 1, &written,0)){
			return alloc_null();
		}

		return alloc_bool((int)written > 0);
	#endif
	//---------------------------------------------
}
DEFINE_PRIM(writeByte,2);

value readByte(value a) {
	int fd = val_int(a);
	char buffer[2];

	//---------------------------------------------
	#if defined( TARGET_OSX ) || defined( TARGET_LINUX )
		if(read(fd, buffer, 1) < 0){
			return alloc_null();
		}
	#endif
	//---------------------------------------------

	//---------------------------------------------
	#ifdef TARGET_WIN32
		DWORD nRead;
		if (!ReadFile((HANDLE)fd, buffer, 1, &nRead, 0)){
			return alloc_null();
		}
	#endif
	//---------------------------------------------
	
	buffer[1] = '\0';
	return alloc_int(buffer[0]);
}
DEFINE_PRIM(readByte,1);

value flush(value a,value b,value c){
	int fd = val_int(a);
	bool flushIn = val_bool(b);
	bool flushOut = val_bool(c);
	int flushType;

	//---------------------------------------------
	#if defined( TARGET_OSX ) || defined( TARGET_LINUX )
		if( flushIn && flushOut) flushType = TCIOFLUSH;
		else if(flushIn) flushType = TCIFLUSH;
		else if(flushOut) flushType = TCOFLUSH;
		else return alloc_null();

		tcflush(fd, flushType);
	#endif
	//---------------------------------------------

	//---------------------------------------------
	#ifdef TARGET_WIN32
		if( flushIn && flushOut) flushType = PURGE_TXCLEAR | PURGE_RXCLEAR;
		else if(flushIn) flushType = PURGE_RXCLEAR;
		else if(flushOut) flushType = PURGE_TXCLEAR;
		else return alloc_null();

		PurgeComm((HANDLE)fd, flushType);
	#endif
	//---------------------------------------------
	
	return alloc_null();
}
DEFINE_PRIM(flush,3);

value available(value a){
	int fd = val_int(a);
	int numBytes = 0;
	
	//---------------------------------------------
	#if defined( TARGET_OSX ) || defined( TARGET_LINUX )
		ioctl(fd,FIONREAD,&numBytes);
	#endif
	//---------------------------------------------

	//---------------------------------------------
	#ifdef TARGET_WIN32
		COMSTAT stat;
		DWORD err;
		if(fd!=(int)INVALID_HANDLE_VALUE){
			if(!ClearCommError((HANDLE)fd, &err, &stat)){
				numBytes = 0;
			} else {
				numBytes = stat.cbInQue;
			}
		} else {
			numBytes = 0;
		}
	#endif
	//---------------------------------------------
	
	return alloc_int(numBytes);
}
DEFINE_PRIM(available,1);

value breakdown(value a){
	int fd = val_int(a);
	//---------------------------------------------
	#ifdef TARGET_WIN32
	//---------------------------------------------
		CloseHandle((HANDLE)fd);
	//---------------------------------------------
	#else
	//---------------------------------------------
		::close(fd);
		//---------------------------------------------
	#endif
	//---------------------------------------------
	return alloc_null();
}
DEFINE_PRIM(breakdown,1);
