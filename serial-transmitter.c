// Version 10.0 
// Terminal has various color
// Counter and joystick are completely seperated


#include <Windows.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <conio.h> 

cPrint(char msg[], int k){
	HANDLE  hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, k);
    printf("%s", msg);
    SetConsoleTextAttribute(hConsole, 15);	//return to default: 15
}

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
	//system("color 0A");
	
	cPrint("    __  __                         ___             __      _                ____       _                \n", 10);
	cPrint("   / / / /_  ______  ___  _____   /   |  _________/ /_  __(_)___  ____     / __ \\_____(_)   _____  _____\n", 10);
	cPrint("  / /_/ / / / / __ \\/ _ \\/ ___/  / /| | / ___/ __  / / / / / __ \\/ __ \\   / / / / ___/ / | / / _ \\/ ___/\n", 10);
	cPrint(" / __  / /_/ / /_/ /  __/ /     / ___ |/ /  / /_/ / /_/ / / / / / /_/ /  / /_/ / /  / /| |/ /  __/ /    \n", 10);
	cPrint("/_/ /_/\\__, / .___/\\___/_/     /_/  |_/_/   \\__,_/\\__,_/_/_/ /_/\\____/  /_____/_/  /_/ |___/\\___/_/     \n", 10);
	cPrint("      /____/_/                                                                                          \n", 10);
	cPrint("                                                                                        Author: hybrayhem\n\n\n\n", 10);

	cPrint("Please select from the following Menu:\n\n", 14);
	cPrint("(1) ", 14);	cPrint("Turn ON Led on The Arduino\n", 11);
	cPrint("(2) ", 14); cPrint("Turn OFF Led on The Arduino\n", 11);
	cPrint("(3) ", 14); cPrint("Flash Arduino Led 3 times\n", 11);
	cPrint("(4) ", 14); cPrint("Send a number to Arduino for compute square in it\n", 11);
	cPrint("(5) ", 14); cPrint("Button Press Counter\n", 11);
	cPrint("(6) ", 14); cPrint("LED Blink Counting Game\n", 11);
	cPrint("(7) ", 14); cPrint("Paint Board\n", 11);
	cPrint("(8) ", 14); cPrint("The Ultimate Joystick Visualizer\n", 13);
	cPrint("(9) ", 14); cPrint("Laser Ruler\n", 13);
	cPrint("(0) ", 12); cPrint("Exit\n", 12);
	
	printf("\nConnecting to Arduino...");
}

void print_outro(){
	printf("\n\n");
	cPrint("Game over. \n", 15); 	       
}

print_ruler(int distance){ // 20mm to 1200mm, displays 1 to 98
	int i;
	printf("\r");
	for(i=0; i<distance/12; i++){
		printf("-");
	}
	printf("| %dmm",distance);
	
	int space = 70 - (space/12);	// for long range 107 - (space/12)
	for(i=0; i<space; i++){
		printf(" ");
	}
	//printf("*",distance);
}

/*	PRINT RULER - COLOR VERSION

print_ruler(int distance, int color){ // 20mm to 1200mm, displays 1 to 98
	int i;
	char d[8];
	printf("\r");
	for(i=0; i<distance/12; i++){
		cPrint("-", color);
	}
	//printf("| %dmm",distance);
	itoa(distance, d, 5);
	cPrint("| ", color); cPrint(d, color); cPrint("mm", color);
	
	int space = 70 - (space/12);	// for long range 107 - (space/12)
	for(i=0; i<space; i++){
		printf(" ");
	}
	//printf("*",distance);
}
*/

int map(int x, int in_min, int in_max, int out_min, int out_max){
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int joystick_index(int x, int y){
	return y*26 + x*2;
}

void move_cursor(char *joystick_str, char cursor, int old_index, int new_x, int new_y, int is_painting, char painter){
	if(is_painting == 0){
		joystick_str[old_index] = ' ';
	}else{
		joystick_str[old_index] = painter;	
	}
	int new_index = joystick_index(new_x, new_y);
	joystick_str[new_index] = cursor;
}

void reset_board(char *joystick_str){
	int x, y;
	for(x=1; x <= 11; x++){
		for(y=1; y<=9; y++){
			move_cursor(joystick_str, ' ', 142, x, y, 0, ' ');
		}
	}
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
    char  ReadData;        //temperory Character
    DWORD NumberOfBytesRead;     // Bytes read by ReadFile()
    unsigned char i = 0;
    BOOL Status;
	
read:   
	//Read data and store in a buffer 
    do
    {
        Status = ReadFile(hComm, &ReadData, sizeof(ReadData), &NumberOfBytesRead, NULL);
        buffer[i] = ReadData;
        ++i;
    } while (NumberOfBytesRead > 0);
    --i; //Get Actual length of received data
    if(i <= 0){
	    if(waitData == 1){
			goto read;	
		}  
	}
	buffer[i-2] = '\0'; //trim unnecessary/wrong characters, discharge buffer
}



int main(void) {
	
	//--------------------------Please enter port number before start--------------------------//
	char port[] = "COM3";
	//-----------------------------------------------------------------------------------------//
	
	print_intro();
	int number;
	int counter = 0;
	char temp[64];
	int old_index = 142; //joystick_str center index
	int x = 6, y = 5; //joystick center position
	char cursor = '*';
	char joystick_str[] = ". . . . . . . . . . . . .\n.                       .\n.                       .\n.                       .\n.                       .\n.           *           .\n.                       .\n.                       .\n.                       .\n.                       .\n. . . . . . . . . . . . .\n\n";	
	
	
    HANDLE hComm;  // Handle to the Serial port
    BOOL   Status; // Status
    DCB dcbSerialParams = { 0 };  // Initializing DCB structure
    COMMTIMEOUTS timeouts = { 0 };  //Initializing timeouts structure
    char SerialBuffer[64] = { 0 }; //Buffer to send and receive data
    DWORD dwEventMask;     // Event mask to trigger
    
    ///Open the serial com port
    hComm = CreateFileA(port, //friendly name
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
        goto Exit1;
    }
    //Setting WaitComm() Event
    Status = WaitCommEvent(hComm, &dwEventMask, NULL); //Wait for the character to be received
    if (Status == FALSE)
    {
        printf_s("\nError! in Setting WaitCommEvent()\n\n");
        goto Exit1;
    }
    
    
    serial_read(hComm, 1, SerialBuffer);	//Reading ready message from arduino
    
    printf(" %s\n", SerialBuffer);
    while(1){
    	//printf_s("\nEnter the selection: ");
    	cPrint("\nEnter the selection: ", 7);
    	scanf_s("%s", SerialBuffer, (unsigned)_countof(SerialBuffer));
    	if(strcmp(SerialBuffer, "0") == 0){
    		goto Exit1;
		}
    	else if(strcmp(SerialBuffer, "1") == 0){
    		serial_write(hComm, SerialBuffer);
    		serial_read(hComm, 1, SerialBuffer);
    		//printf("'%s'\n", SerialBuffer);
		}
		else if(strcmp(SerialBuffer, "2") == 0){
    		serial_write(hComm, SerialBuffer);
    		serial_read(hComm, 1, SerialBuffer);
    		//printf("'%s'\n", SerialBuffer);
		}
		else if(strcmp(SerialBuffer, "3") == 0){
    		serial_write(hComm, SerialBuffer);
    		serial_read(hComm, 1, SerialBuffer);
    		//printf("'%s'\n", SerialBuffer);
		}
		else if(strcmp(SerialBuffer, "4") == 0){
    		printf("Enter the number you want to get square: ");
    		scanf("%s", temp);
    		SerialBuffer[0] = '4';	//key of square function on arduino
    		strncat(SerialBuffer, temp, sizeof(temp));
    		
    		serial_write(hComm, SerialBuffer);
    		serial_read(hComm, 1, SerialBuffer);
    		printf("Result = %s\n", SerialBuffer);
		}
		else if(strcmp(SerialBuffer, "5") == 0){
			printf("# PRESS 0 TO RETURN MENU #\n");
			serial_write(hComm, SerialBuffer);
			char ch;
			counter = 0;
			printf("You have pressed button %d times.", counter);
			while(1){
				if(kbhit()){
					// Stores the pressed key in ch 
		            ch = getch(); 
		            // Terminates the loop when escape is pressed 
		            if (ch == '0'){
		            	serial_write(hComm, "9");
		            	serial_read(hComm, 1, SerialBuffer); //for waiting end message from joystick data stream of arduino
					    break;
					}	    
				}
				serial_read(hComm, 0, SerialBuffer);
				if(SerialBuffer[0] != '\0'){
					if(SerialBuffer[0] == '+'){
						counter++;
					}
					printf("\rYou have pressed button %d times.", counter);
				}
			}
			printf("\n");
		}
		else if(strcmp(SerialBuffer, "6") == 0){
			printf("Welcome to the LED Blink Counting game :)\n");
			
    		srand(time(NULL));
			int x = rand() % 9 + 1;
			itoa(x, temp, 10);
			SerialBuffer[0] = '6';	//key of blink function on arduino
			strncat(SerialBuffer, temp, sizeof(temp));
			
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
		}
		else if(strcmp(SerialBuffer, "7") == 0){
			printf("# PRESS 0 TO RETURN MENU #\n");
			serial_write(hComm, "5");  // command 5 because joystick and counter feature working on same time on arduino
			char ch;
			int is_painting = 0;
			cursor = '-';
			while(1){
				if(kbhit()){
					// Stores the pressed key in ch 
		            ch = getch(); 
		            // Terminates the loop when escape is pressed 
		            if (ch == '0'){
		            	serial_write(hComm, "9");
		            	serial_read(hComm, 1, SerialBuffer); //for waiting end message from joystick data stream of arduino
		            	reset_board(joystick_str);
					    break;
					}	    
				}
				serial_read(hComm, 0, SerialBuffer);
				if(SerialBuffer[0] != '\0'){
					if(SerialBuffer[0] == '+'){
						is_painting = is_painting ^ 1;
						if(is_painting == 1){
							cursor = '+';
						}else{
							cursor = '-';
						}
					}
					else{
						int number = atoi(SerialBuffer);
						if(number > 0){
							x = map(number / 10, 1, 9, 1, 11);
							y = number % 10;
						}
					}
					move_cursor(joystick_str, cursor, old_index, x, y, is_painting, '*');
					old_index = joystick_index(x, y);
					cPrint(joystick_str, 11);
				}
			}
		}
		else if(strcmp(SerialBuffer, "8") == 0){
			printf("# PRESS 0 TO RETURN MENU #\n");
			serial_write(hComm, "5");  // command 5 because joystick and counter feature working on same time on arduino
			char ch;
			cursor = '*';
			while(1){
				if(kbhit()){
					// Stores the pressed key in ch 
		            ch = getch(); 
		            // Terminates the loop when escape is pressed 
		            if (ch == '0'){
		            	serial_write(hComm, "9");
		            	serial_read(hComm, 1, SerialBuffer); //for waiting end message from joystick data stream of arduino
					    break;
					}	    
				}
				serial_read(hComm, 0, SerialBuffer);
				if(SerialBuffer[0] != '\0'){
					if(SerialBuffer[0] == '+'){
						cursor = '+';
					}
					else if(SerialBuffer[0] == '*'){
						cursor = '*';
					}else{
						int number = atoi(SerialBuffer);
						if(number > 0){
							x = map(number / 10, 1, 9, 1, 11);
							y = number % 10;
						}
					}
					move_cursor(joystick_str, cursor, old_index, x, y, 0, ' ');
					old_index = joystick_index(x, y);
					cPrint(joystick_str, 13);
					//printf("\n");
				}
			}
		}
		else if(strcmp(SerialBuffer, "9") == 0){
			printf("# PRESS 0 TO RETURN MENU #\n\n");
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
		            	serial_read(hComm, 1, SerialBuffer); //for waiting end message from joystick data stream of arduino
					    break;
					}	    
				}
				serial_read(hComm, 0, SerialBuffer);
				if(SerialBuffer[0] != '\0'){
					if(SerialBuffer[0] == '+'){
						printf("\rOut of range.                                                                                              ");	// for cleaning whole line
					}else{
						int distance = atoi(SerialBuffer);
						print_ruler(distance);
					}
				}
			}
			printf("\n");
		}
		else{
			printf("Invalid value: '%s'\n",SerialBuffer);
		}
	}
    
    
Exit1:
    CloseHandle(hComm);//Closing the Serial Port
Exit2:
    print_outro();
    getch();
    return 0;
}
