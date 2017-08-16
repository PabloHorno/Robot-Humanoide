// Windows version
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include "../dynamixel.h"

#pragma comment(lib, "dynamixel.lib")


// Control table address
#define P_GOAL_POSITION_L		30
#define P_GOAL_POSITION_H		31
#define P_PRESENT_POSITION_L	36
#define P_PRESENT_POSITION_H	37
#define P_MOVING				46



int main()
{
	int id = 1;
	int baudnum = 1;
	int GoalPos[2] = {0, 1023};
	int index = 0;
	int Moving, PresentPos;

	///////// Open USB2Dynamixel ////////////
	if( dxl_initialize() == 0 )
	{
		printf( "Failed to open USB2Dynamixel!\n" );
		goto END_MAIN;
	}
	printf( "Succeed to open USB2Dynamixel!\n" );

	dxl_set_baud( baudnum );
	printf( "Quit to press ESC key!\n\n" );
	while(1)
	{
		Moving = dxl_read_byte( id, P_MOVING );
		if( dxl_get_result() == COMM_RXSUCCESS )
		{
			if( Moving == 0 )
			{
				if( index == 0 )
					index = 1;
				else
					index = 0;

				dxl_write_word( id, P_GOAL_POSITION_L, GoalPos[index] );
			}
		}

		PresentPos = dxl_read_word( id, P_PRESENT_POSITION_L );
		printf( "\r                                              " );
		printf( "\rGoal position: %d  Present position: %d",GoalPos[index], PresentPos );

		// key check
		if( _kbhit() > 0 )
		{
			if( _getch() == 0x1b )
			{
				printf( "\n" );
				goto END_MAIN;
			}
		}
	}

END_MAIN:
	dxl_terminate();
	printf( "Press any key to terminate...\n" );
	_getch();

	return 0;
}
