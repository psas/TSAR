// tsar_post_fire.c
// Portland State Aerospace Society
// TSAR Capstone 2019-2020
// TODO UPDATE THIS
// Change Log
// 	-Creation of Document 5/17/2020 [APJ]
//
//	This file contains methods for the POST_FIRE
//  state.
//  PostFire has two verification Methods
//  It first checks that both current and previous states are valid
//  It then checks if the expected state is the state passed
//  At this point the valves are opened and a log entry is created
//
//	References:
//
//
//  System Risk Factor = 0.33 (Catastrophic, Unlikely)
#include "post_fire.h"

uint32_t PostFire(enum StateName state, enum StateName lastState)
{
	uint32_t success = FALSE;
	uint32_t valve_configuration = 0;
	uint32_t valve_target = 0;
	char message[256];
	char *msgPtr = message;
    if(VerifyState(state) && VerifyState(lastState))
    {
    	if((state & POST_FIRE) == POST_FIRE){
    		// PV1 PV2 PV3 VV1 VV2 IV1 IV2 MV1 MV2
    		// | 0| 0|  0|  1|  1|  0|  0|  0|  0

    		// Set Valve States
    		valve_target |= ((uint16_t)VV1 	\
						 |(uint16_t)VV2);
    		// Timestamp and Log
    		valve_configuration = StateConfiguration();

    		// Change State conditions
    		lastState=state;
    		state = POST_FIRE;
    		success = (valve_configuration == valve_target ? TRUE : FALSE);
    		// Create Message and Transmit
    		Get_Valve_State_Status_Msg(msgPtr,valve_configuration,success);
    		UART_SendMessage(&hlpuart1, msgPtr);
    	}else{
    		//Log Expected State != Passed State
    		Get_State_Disagree_Error_Msg(msgPtr, POST_FIRE, state);
    		UART_SendMessage(&hlpuart1,msgPtr);

    	}
    }else{
    	    	// Log Invalid State
    	Get_Invalid_State_Error_Msg(msgPtr, state, lastState);
    	UART_SendMessage(&hlpuart1, msgPtr);
    }
	return success;
}
