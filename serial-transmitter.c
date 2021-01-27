#include <Windows.h>
#include <stdio.h>
#include <string.h>


void serial_write(HANDLE hComm, char *SerialBuffer){
	DWORD BytesWritten = 0;
	//Writing data to Serial Port
    BOOL Status = WriteFile(hComm,// Handle to the Serialport
                       SerialBuffer,            // Data to be written to the port
                       sizeof(SerialBuffer),   // No of bytes to write into the port
                       &BytesWritten,  // No of bytes written to the port
                       NULL);
    if (Status == FALSE)
    {
        printf_s("\nFail to Written");
        CloseHandle(hComm);//Closing the Serial Port
    }
    if (BytesWritten != sizeof(SerialBuffer))
	{
	    printf_s("Failed to write all bytes to port");
	    CloseHandle(hComm);//Closing the Serial Port
	}
    //print numbers of byte written to the serial port
    //printf_s("\nNumber of bytes written to the serial port = %d\n\n", BytesWritten);
}



void serial_read(HANDLE hComm){
	char buffer[64] = { 0 };
    char  ReadData;        //temperory Character
    DWORD NoBytesRead;     // Bytes read by ReadFile()
    unsigned char loop = 0;
    BOOL Status;
	
    //Read data and store in a buffer
read:    
    do
    {
        Status = ReadFile(hComm, &ReadData, sizeof(ReadData), &NoBytesRead, NULL);
        buffer[loop] = ReadData;
        ++loop;
    } while (NoBytesRead > 0);
    --loop; //Get Actual length of received data
    if(loop > 0){
    	/*printf_s("\nNumber of bytes received = %d\n\n", loop);
	    print receive data on console
	    printf_s("\n\n");
	    printf("data: '");
	    int index = 0;
	    for (index = 0; index < loop; ++index)
	    {
			printf_s("%c", buffer[index]);
	    }
	    printf("'\n");*/
	    printf("data: '%s' \n\n", buffer);
	}else{
		//printf("Buffer is empty.\n");
		goto read;
	}
}



int main(void) {
	
    HANDLE hComm;  // Handle to the Serial port
    BOOL   Status; // Status
    DCB dcbSerialParams = { 0 };  // Initializing DCB structure
    COMMTIMEOUTS timeouts = { 0 };  //Initializing timeouts structure
    char SerialBuffer[64] = { 0 }; //Buffer to send and receive data
    //DWORD BytesWritten = 0;          // No of bytes written to the port
    DWORD dwEventMask;     // Event mask to trigger
    //char  ReadData;        //temperory Character
    //DWORD NoBytesRead;     // Bytes read by ReadFile()
    //unsigned char loop = 0;
    
    ///Open the serial com port
    hComm = CreateFileA("COM5", //friendly name
                       GENERIC_READ | GENERIC_WRITE,      // Read/Write Access
                       0,                                 // No Sharing, ports cant be shared
                       NULL,                              // No Security
                       OPEN_EXISTING,                     // Open existing port only
                       0,                                 // Non Overlapped I/O
                       NULL);                             // Null for Comm Devices
    if (hComm == INVALID_HANDLE_VALUE)
    {
        printf_s("\n Port can't be opened\n\n");
        goto Exit2;
    }
    //Setting the Parameters for the SerialPort
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    Status = GetCommState(hComm, &dcbSerialParams); //retreives  the current settings
    if (Status == FALSE)
    {
        printf_s("\nError to Get the Com state\n\n");
        goto Exit1;
    }
    dcbSerialParams.BaudRate = CBR_9600;      //BaudRate = 9600
    dcbSerialParams.ByteSize = 8;             //ByteSize = 8
    dcbSerialParams.StopBits = ONESTOPBIT;    //StopBits = 1
    dcbSerialParams.Parity = NOPARITY;      //Parity = None
    Status = SetCommState(hComm, &dcbSerialParams);
    if (Status == FALSE)
    {
        printf_s("\nError to Setting DCB Structure\n\n");
        goto Exit1;
    }
    //Setting Timeouts
    timeouts.ReadIntervalTimeout = 3;
    timeouts.ReadTotalTimeoutConstant = 3;
    timeouts.ReadTotalTimeoutMultiplier = 2;
    timeouts.WriteTotalTimeoutConstant = 3;
    timeouts.WriteTotalTimeoutMultiplier = 2;
    if (SetCommTimeouts(hComm, &timeouts) == FALSE)
    {
        printf_s("\nError to Setting Time outs");
        goto Exit1;
    }
    
    //Setting Receive Mask
    Status = SetCommMask(hComm, EV_RXCHAR);
    if (Status == FALSE)
    {
        printf_s("\nError to in Setting CommMask\n\n");
        CloseHandle(hComm);
    }
    //Setting WaitComm() Event
    Status = WaitCommEvent(hComm, &dwEventMask, NULL); //Wait for the character to be received
    if (Status == FALSE)
    {
        printf_s("\nError! in Setting WaitCommEvent()\n\n");
        CloseHandle(hComm);
    }
    
    serial_read(hComm);
    while(1){
    	printf_s("\n\nEnter your message: ");
    	scanf_s("%s", SerialBuffer, (unsigned)_countof(SerialBuffer));
    	if(strcmp(SerialBuffer, "q") == 0){
    		goto Exit1;
		}
    	serial_write(hComm, SerialBuffer);
    	
    	serial_read(hComm);
	}
    
    
Exit1:
    CloseHandle(hComm);//Closing the Serial Port
Exit2:
    system("pause");
    return 0;
}
