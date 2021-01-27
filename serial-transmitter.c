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
	printf("(6) LED Blink Counting game\n");
	printf("(0) Exit\n");
	
	printf("\nConnecting to Arduino...");
	//printf("Enter the selection: ");
}


int map(int x, int in_min, int in_max, int out_min, int out_max){
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int joystick_index(int x, int y){
	return y*26 + x*2;
}

void move_cursor(char *joystick_str, char cursor, int old_index, int new_x, int new_y){
	joystick_str[old_index] = ' ';
	int new_index = joystick_index(new_x, new_y);
	joystick_str[new_index] = cursor;
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


void serial_read(HANDLE hComm, int waitData, char *buffer){
	//char buffer[64] = { 0 };
    char  ReadData;        //temperory Character
    DWORD NumberOfBytesRead;     // Bytes read by ReadFile()
    unsigned char i = 0;
    BOOL Status;
	
    //Read data and store in a buffer
read:    
    do
    {
        Status = ReadFile(hComm, &ReadData, sizeof(ReadData), &NumberOfBytesRead, NULL);
        buffer[i] = ReadData;
        ++i;
    } while (NumberOfBytesRead > 0);
    --i; //Get Actual length of received data
    //printf("i: %d, ", i);
    //printf("sizeof(buffer): %d\n", sizeof(buffer));
    if(i <= 0){
	    if(waitData == 1){
			goto read;	
		}  
	}
	//printf("buffer:'%s'\n", buffer);
	buffer[i-2] = '\0'; //trim unnecessary/wrong characters
}



int main(void) {
	print_intro();
	int number;
	int counter = 0;
	char temp[64];
	int old_index = 142; //center index
	int x = 1, y = 1; //joystick values
	char cursor = '*';
	char joystick_str[] = ". . . . . . . . . . . . .\n.                       .\n.                       .\n.                       .\n.                       .\n.           *           .\n.                       .\n.                       .\n.                       .\n.                       .\n. . . . . . . . . . . . .\n";	
	
	
    HANDLE hComm;  // Handle to the Serial port
    BOOL   Status; // Status
    DCB dcbSerialParams = { 0 };  // Initializing DCB structure
    COMMTIMEOUTS timeouts = { 0 };  //Initializing timeouts structure
    char SerialBuffer[64] = { 0 }; //Buffer to send and receive data
    DWORD dwEventMask;     // Event mask to trigger
    
    ///Open the serial com port
    hComm = CreateFileA("COM4", //friendly name
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
    
    
    serial_read(hComm, 1, SerialBuffer);	//Reading ready message of arduino
    printf(" %s\n", SerialBuffer);
    while(1){
    	/*printf_s("\n\nEnter the selection: ");
    	scanf_s("%s", SerialBuffer, (unsigned)_countof(SerialBuffer));
    	if(strcmp(SerialBuffer, "q") == 0){
    		goto Exit1;
		}
    	serial_write(hComm, SerialBuffer);
    	serial_read(hComm, 1, SerialBuffer);
    	printf("SerialBuffer: '%s'\n", SerialBuffer);*/
    	
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
    		serial_read(hComm, 1, SerialBuffer);
    		printf("'%s'\n", SerialBuffer);
		}
		else if(strcmp(SerialBuffer, "2") == 0){
			printf("case2\n");
    		serial_write(hComm, SerialBuffer);
    		serial_read(hComm, 1, SerialBuffer);
    		printf("'%s'\n", SerialBuffer);
		}
		else if(strcmp(SerialBuffer, "3") == 0){
			printf("case3\n");
    		serial_write(hComm, SerialBuffer);
    		serial_read(hComm, 1, SerialBuffer);
    		printf("'%s'\n", SerialBuffer);
		}
		else if(strcmp(SerialBuffer, "4") == 0){
			printf("case4\n");
    		printf("Enter the number you want to get square: ");
    		scanf("%s", temp);
    		SerialBuffer[0] = '4';
    		strncat(SerialBuffer, temp, sizeof(temp));
    		
    		serial_write(hComm, SerialBuffer);
    		serial_read(hComm, 1, SerialBuffer);
    		printf("Result = '%s'\n", SerialBuffer);
		}
		else if(strcmp(SerialBuffer, "5") == 0){
			printf("Press 0 to exit to menu.\n");
			serial_write(hComm, SerialBuffer);
			char ch;
			counter = 0;
			while(1){
				if(kbhit()){
					// Stores the pressed key in ch 
		            ch = getch(); 
		            // Terminates the loop when escape is pressed 
		            if (ch == '0'){
		            	serial_write(hComm, "9");
		            	serial_read(hComm, 1, SerialBuffer); //for waiting end message from arduino of joystick data stream
					    break;
					}	    
				}
				serial_read(hComm, 0, SerialBuffer);
				if(SerialBuffer[0] != '\0'){
					printf("SerialBuffer: '%s'\n", SerialBuffer);
					if(SerialBuffer[0] == '+'){
						counter++;
						cursor = '+';
					}
					if(SerialBuffer[0] == '*'){
						cursor = '*';
					}else{
						int number = atoi(SerialBuffer);
						x = map(number / 10, 1, 9, 1, 11);
						y = number % 10;
						printf("%d,%d\n",x,y);
					}
					move_cursor(joystick_str, cursor, old_index, x, y);
					old_index = joystick_index(x, y);
					printf("%s", joystick_str);
					
					printf("\nYou have pressed button %d times.\n", counter);
				}
			}
		}
		else if(strcmp(SerialBuffer, "6") == 0){
			printf("Welcome to the LED Blink Counting game :)\n");
			
    		srand(time(NULL));
			int x = rand() % 9 + 1;
			//printf("x: %d\n",x);
			itoa(x, temp, 10);
			//printf("temp: %s\n",temp);
			strncat(SerialBuffer, temp, sizeof(temp));
			//printf("SerialBuffer: %s\n",SerialBuffer);
			
    		progressIndicator("A random blink is coming");
			serial_write(hComm, SerialBuffer);
			serial_read(hComm, 1, SerialBuffer);
			printf("Enter the number of blinks: ");
			scanf("%d", &number);
			printf("\n");
			if(number == x){
				printf("Correct!\n");
			}else{
				printf("Wrong! The answer was %d.\n", x);
			}
			
		}else{
			printf("Invalid value: '%s'\n",SerialBuffer);
		}
	}
    
    
Exit1:
    CloseHandle(hComm);//Closing the Serial Port
Exit2:
    //system("pause");
    return 0;
}
