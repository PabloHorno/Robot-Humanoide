// Windows version
#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <conio.h>
#include "../dynamixel.h"

#pragma comment(lib, "dynamixel.lib")

#define PI	3.141592f
#define NUM_ACTUATOR		3

// Control table address
#define P_GOAL_POSITION_L	30
#define P_GOAL_POSITION_H	31
#define P_GOAL_SPEED_L		32
#define P_GOAL_SPEED_H		33



int main()
{
	int id[NUM_ACTUATOR];
	int baudnum = 1;
	float phase[NUM_ACTUATOR];
	float theta = 0;
	float step_theta = PI / 100.0f;
	int GoalPos;
	int i, a;

	///////// Open USB2Dynamixel ////////////
	if( dxl_initialize() == 0 )
	{
		printf( "Failed to open USB2Dynamixel!\n" );
		goto END_MAIN;
	}
	printf( "Succeed to open USB2Dynamixel!\n" );

	dxl_set_baud( baudnum );
	printf( "Quit to press ESC key!\n\n" );
	
	for( i=0; i<NUM_ACTUATOR; i++ )
	{
		id[i] = i+1;
		phase[i] = 2*PI * (float)i / (float)NUM_ACTUATOR;
	}
	
	dxl_write_word( BROADCAST_ID, P_GOAL_SPEED_L, 0 );
	dxl_write_word( BROADCAST_ID, P_GOAL_POSITION_L, 512 );
	Sleep(2000);
	while(1)
	{
		// Make syncwrite packet
		// Method 1.
		gbInstructionPacket[ID] = BROADCAST_ID;
		gbInstructionPacket[INSTRUCTION] = INST_SYNC_WRITE;
		gbInstructionPacket[PARAMETER] = P_GOAL_POSITION_L;
		gbInstructionPacket[PARAMETER+1] = 2;
		for( i=0; i<NUM_ACTUATOR; i++ )
		{
			gbInstructionPacket[PARAMETER+2+3*i] = id[i];
			GoalPos = (int)((sin(theta+phase[i]) + 1.0) * 512.0);
			printf( "[%d]:%d ", id[i], GoalPos );
			gbInstructionPacket[PARAMETER+2+3*i+1] = dxl_get_lowbyte(GoalPos);
			gbInstructionPacket[PARAMETER+2+3*i+2] = dxl_get_highbyte(GoalPos);
			a = dxl_get_lowbyte(GoalPos);
			a = dxl_get_highbyte(GoalPos);
		}
		printf( "\r" );
		gbInstructionPacket[LENGTH] = (2+1)*NUM_ACTUATOR+4;
		dxl_txrx_packet();

		
		/* Method 2.
		dxl_set_txpacket_id(BROADCAST_ID);
		dxl_set_txpacket_instruction(INST_SYNC_WRITE);
		dxl_set_txpacket_parameter(0, P_GOAL_POSITION_L);
		dxl_set_txpacket_parameter(1, 2);
		for( i=0; i<NUM_ACTUATOR; i++ )
		{
			dxl_set_txpacket_parameter(2+3*i, id[i]);
			GoalPos = (int)((sin(theta+phase[i]) + 1.0) * 512.0);
			printf( "[%d]:%d ", id[i], GoalPos );
			dxl_set_txpacket_parameter(2+3*i+1, dxl_get_lowbyte(GoalPos));
			dxl_set_txpacket_parameter(2+3*i+2, dxl_get_highbyte(GoalPos));
		}
		printf( "\r" );
		dxl_set_txpacket_length((2+1)*NUM_ACTUATOR+4);
		dxl_txrx_packet();
		*/
		
		// key check
		if( _kbhit() > 0 )
		{
			if( _getch() == 0x1b )
			{
				printf( "\n" );
				goto END_MAIN;
			}
		}

		theta += step_theta;
		if( theta > 2*PI )
			theta -= 2*PI;
		Sleep(10);
	}

END_MAIN:
	dxl_terminate();
	printf( "Press any key to terminate...\n" );
	_getch();

	return 0;
}
