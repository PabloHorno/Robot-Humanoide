// Windows version
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include "../dynamixel.h"

#pragma comment(lib, "dynamixel.lib")


// Control table address
#define P_MODEL_NUMBER_L	0	// Model number low byte
#define P_MODEL_NUMBER_H	1	// Model number high byte
#define P_VERSION			2	// Firmware version

void ErrorDisplay();


int main()
{
	int i, j, n;
	int bValue;
	int wValue;

	///////// Open USB2Dynamixel ////////////
	if( dxl_initialize() == 0 )
	{
		printf( "Failed to open USB2Dynamixel!\n" );
		goto END_MAIN;
	}
	printf( "Succeed to open USB2Dynamixel!\n" );


	//////////// Scan Dynamixel //////////////
	printf( "Scan start!(Quit to press ESC key)\n" );
	for( i=1; i<255; i++ )
	{
		dxl_set_baud( i );
		printf( "==== Baudrate number: %d =====\n", i );
		
		for( j=0, n=0; j<BROADCAST_ID; j++ )
		{
			dxl_ping( j );
			if( dxl_get_result() == COMM_RXSUCCESS )
			{
				n++;
				printf( "-ID:%d (", j );
				wValue = dxl_read_word( j, P_MODEL_NUMBER_L );
				if( dxl_get_result() == COMM_RXSUCCESS )
					printf( "Model number:%d, ", wValue );
				else
				{
					ErrorDisplay();
					printf( ", " );
				}

				bValue = dxl_read_byte( j, P_VERSION );
				if( dxl_get_result() == COMM_RXSUCCESS )
					printf( "Version:%d", bValue );
				else
					ErrorDisplay();
				
				printf( ")\n" );
			}

			// key check
			if( _kbhit() > 0 )
			{
				if( _getch() == 0x1b )
				{
					printf( "Scan break!\n" );
					goto END_MAIN;
				}
			}
		}

		printf( "Total found number: %d\n", n );
	}


END_MAIN:
	dxl_terminate();
	printf( "Press any key to terminate...\n" );
	_getch();

	return 0;
}

void ErrorDisplay()
{
	switch( dxl_get_result() )
	{
	case COMM_TXFAIL:
		printf( "COMM_TXFAIL" );
		break;
	
	case COMM_TXERROR:
		printf( "COMM_TXERROR" );
		break;

	case COMM_RXFAIL:
		printf( "COMM_RXFAIL" );
		break;

	case COMM_RXWAITING:
		printf( "COMM_RXWAITING" );
		break;

	case COMM_RXTIMEOUT:
		printf( "COMM_RXTIMEOUT" );
		break;

	case COMM_RXCORRUPT:
		printf( "COMM_RXCORRUPT" );
		break;
	}
}