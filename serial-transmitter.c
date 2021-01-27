#include <Windows.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <conio.h> 

void progressIndicator(char msg[]){
	printf("%s", msg);
	sleep(1);
	printf(".");
	sleep(1);
	printf(".");
	sleep(1);
	printf(".\n");
	sleep(1);
}

void print_intro(){
	system("color 0A");
	printf("    __  __                         ___             __      _                ____       _                \n");
	printf("   / / / /_  ______  ___  _____   /   |  _________/ /_  __(_)___  ____     / __ \\_____(_)   _____  _____\n");
	printf("  / /_/ / / / / __ \\/ _ \\/ ___/  / /| | / ___/ __  / / / / / __ \\/ __ \\   / / / / ___/ / | / / _ \\/ ___/\n");
	printf(" / __  / /_/ / /_/ /  __/ /     / ___ |/ /  / /_/ / /_/ / / / / / /_/ /  / /_/ / /  / /| |/ /  __/ /    \n");
	printf("/_/ /_/\\__, / .___/\\___/_/     /_/  |_/_/   \\__,_/\\__,_/_/_/ /_/\\____/  /_____/_/  /_/ |___/\\___/_/     \n");
	printf("      /____/_/                                                                                          \n");
	printf("                                                                                        Author: hybrayhem\n\n\n");

	printf("Please select from the following menu:\n\n");
	printf("(1) Turn on Led on The Arduino\n");
	printf("(2) Turn off Led on The Arduino\n");
	printf("(3) Flash Arduino Led 3 times\n");
	printf("(4) Send a number to Arduino for compute square in it\n");
	printf("(5) Button press counter with The Ultimate Joystick Visualizer\n");
	printf("(6) Led Blink counting game\n");
	printf("(0) Exit\n");
	
	//printf("Enter the selection: ");
}

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



void serial_read(HANDLE hComm, int waitData){
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
	    //printf("data: '%s' \n\n", buffer);
	    printf("'%s'\n", buffer);
	}else{
		//printf("Buffer is empty.\n");
		if(waitData == 1){
			goto read;	
		}
	}
}



int main(void) {
	print_intro();
	printf("\nConnecting to Arduino... ");
	int number;
	char command[64];
	char temp[64];
	
    HANDLE hComm;  // Handle to the Serial port
    BOOL   Status; // Status
    DCB dcbSerialParams = { 0 };  // Initializing DCB structure
    COMMTIMEOUTS timeouts = { 0 };  //Initializing timeouts structure
    char SerialBuffer[64] = { 0 }; //Buffer to send and receive data
    DWORD dwEventMask;     // Event mask to trigger
    
    ///Open the serial com port
    hComm = CreateFileA("COM7", //friendly name
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
    
    
    serial_read(hComm, 1);	//Reading ready message of arduino
    while(1){
    	/*printf_s("\n\nEnter the selection: ");
    	scanf_s("%s", SerialBuffer, (unsigned)_countof(SerialBuffer));
    	if(strcmp(SerialBuffer, "q") == 0){
    		goto Exit1;
		}
    	serial_write(hComm, SerialBuffer);
    	serial_write(hComm, "1");
    	serial_read(hComm);
    	
    	serial_write(hComm, "2");
    	serial_read(hComm);*/
    	
    	printf_s("\n\nEnter the selection: ");
    	scanf_s("%s", SerialBuffer, (unsigned)_countof(SerialBuffer));
    	printf("\n");
    	if(strcmp(SerialBuffer, "0") == 0){
    		break;
    		// print greetings
		}
    	else if(strcmp(SerialBuffer, "1") == 0){
    		printf("case1\n");
    		serial_write(hComm, SerialBuffer);
    		serial_read(hComm, 1);
		}
		else if(strcmp(SerialBuffer, "2") == 0){
			printf("case2\n");
    		serial_write(hComm, SerialBuffer);
    		serial_read(hComm, 1);
		}
		else if(strcmp(SerialBuffer, "3") == 0){
			printf("case3\n");
    		serial_write(hComm, SerialBuffer);
    		serial_read(hComm, 1);
		}
		else if(strcmp(SerialBuffer, "4") == 0){
			printf("case4\n");
    		printf("Enter the number you want to get square: ");
    		scanf("%s", temp);
    		SerialBuffer[0] = '4';
    		strncat(SerialBuffer, temp, sizeof(temp));
    		
    		printf("SerialBuffer: %s\n", SerialBuffer);
    		serial_write(hComm, SerialBuffer);
    		serial_read(hComm, 1);
    		//printf(incoming data);
		}
		else if(strcmp(SerialBuffer, "5") == 0){
			printf("case5 Press 0 to exit to menu.\n");
			serial_write(hComm, SerialBuffer);
			char ch;
			while(1){
				//printf("continue\n");
				if(kbhit()){
					// Stores the pressed key in ch 
		            ch = getch(); 
		            // Terminates the loop when escape is pressed 
		            if (ch == '0'){
		            	serial_write(hComm, "9");
		            	serial_read(hComm, 1);
					    break;
					}	    
				}
				serial_read(hComm, 0);
			}
		}
		else if(strcmp(SerialBuffer, "6") == 0){
			
			//send 65, 5 times blink
    		printf("case6\n");
    		srand(time(NULL));
			int x = rand() % 9 + 1;
			printf("x: %d\n",x);
			itoa(x, temp, 10);
			printf("temp: %s\n",temp);
			strncat(SerialBuffer, temp, sizeof(temp));
			printf("SerialBuffer: %s\n",SerialBuffer);
			
			printf("The Led on Arduino will blink randomly, please count\n");
    		progressIndicator("Starting");
			serial_write(hComm, SerialBuffer);
			serial_read(hComm, 1);
			printf("Enter the number of blinks: \n");
			scanf("%d", &number);
			if(number == x){
				printf("Correct!\n");
			}else{
				printf("Wrong! The answer was %d.", x);
			}
		}else{
			printf("Invalid value: '%s'\n",SerialBuffer);
		}
	}
    
    
Exit1:
    CloseHandle(hComm);//Closing the Serial Port
Exit2:
    system("pause");
    return 0;
}
