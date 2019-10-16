#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include "include/imp_structures.h"
#include "include/imp_variables.h"

/**********************************************************************
				   	Thread 1: Controller 
***********************************************************************/

void *controller(void * d)
{
	if(DEBUG) printf("Thread 1 (controller) initializing ...\n");
	pthread_mutex_lock(&lock[0]);

	//reset 
	xa = x_end; // X_DES*1000;
	va = 0.0;
	curr_pos = x_end; //track movement since homing? 
	xdes_old = x_end;

	for(int i = 0; i < FIR_ORDER_V; i++)
	{
		v_filt[i] = 0.0;
		f_filt[i] = 0.0;
	}

	aValues[0] = MOTOR_ZERO; 
    LJM_eNames(daqHandle, 5, aNames, aWrites, aNumValues, aValues, &errorAddress);
    clock_gettime(CLOCK_MONOTONIC, &last_time); 

    //CONTROL LOOP -------------------------------------------------
	while(1){

		for(int i = 0; i < BUFFER_SIZE; i++)
		{
			clock_gettime(CLOCK_MONOTONIC, &temp_time); 

			imp_cont = &((struct impStruct*)d)[i];
			imp_cont->start_time = temp_time;

			//if(DEBUG & i == 0) printf("Thread 1 (controller) Executing ...\n");
			//Read & Write to DAQ ---------------------------------------
			LJM_eNames(daqHandle, 5, aNames, aWrites, aNumValues, aValues, &errorAddress);
				
	        imp_cont->fk = FT_GAIN*aValues[1] - ft_offset;
	        //printf("Force: %.2f\n", imp_cont->fk);
	        imp_cont->LSF[0] = imp_cont->LSF[1];
	        imp_cont->LSB[0] = imp_cont->LSB[1];
	        imp_cont->LSF[1] = aValues[2];
	        imp_cont->LSB[1] = aValues[3];
	        curr_pos = curr_pos + ENC_TO_MM * (double)aValues[4]; 
	        imp_cont->xk = curr_pos;

	        imp_cont->f_unfilt = imp_cont->fk;
	        imp_FIR(f_filt, &imp_cont->fk, &fir_order_f); //moving avg filter for force
			

			//if(DEBUG & i==0) printf("Force: %.3f\n", imp_cont->fk);
			//Calculate Velocity 
	        imp_StepTime(&imp_cont->start_time, &last_time, &imp_cont->step_time);
			imp_cont->vk = ENC_TO_MM * aValues[4] / ((double)imp_cont->step_time.tv_sec + (double)imp_cont->step_time.tv_nsec/NSEC_IN_SEC);
			imp_cont->v_unfilt = imp_cont->vk;
			imp_FIR(v_filt, &imp_cont->vk, &fir_order_v); //moving average filter for velocity 

			//Controller

			switch(game_type)
			{
				case 1:
					//assistive or resistive
					imp_traj2(imp_cont, &direction, &xdes_old, &x_end);
					imp_Adm(imp_cont, &xa, &va, &x_end);
					break;

				case 2:
					//balance or gait
					imp_Haptics_impedance(imp_cont, &physics_ball, &gait, &xa, &va, &fa, &fk, &fa_1, &environment, &x_end);
					break;
				
			}

			//imp_traj(imp_cont, &direction, &xdes_old, &x_end);
			//imp_PD(imp_cont);
			//imp_Adm(imp_cont, &xa, &va);
			//imp_Haptics_impedance(imp_cont, &physics_ball, &gait, &xa, &va, &fa, &fk, &fa_1, &environment, &x_end);
			//imp_Adm_free(imp_cont, &xa, &va);
			
			//Safety Checks
			//TODO : check direction of command
			//TODO : check IR
			if(imp_cont->cmd != imp_cont->cmd) imp_cont->cmd = 0; //check if command is nan
			if(imp_cont->cmd > MAX_COMMAND) imp_cont->cmd = MAX_COMMAND;
			if((-1)*imp_cont->cmd > MAX_COMMAND) imp_cont->cmd = (-1)*MAX_COMMAND;
			if(imp_cont->cmd > 0) imp_cont->cmd = MOTOR_ZERO_FWD - imp_cont->cmd;
			if(imp_cont->cmd < 0) imp_cont->cmd = MOTOR_ZERO_BWD - imp_cont->cmd;
			if(imp_cont->cmd == 0) imp_cont->cmd = MOTOR_ZERO;

			if(imp_cont->LSF[1] )
			{
			  	if(imp_cont->cmd < MOTOR_ZERO) imp_cont->cmd = MOTOR_ZERO; 
			  	//direction = -1.0;		
			}
			if(imp_cont->LSB[1])  
			{
				if(imp_cont->cmd > MOTOR_ZERO) imp_cont->cmd = MOTOR_ZERO;  
				//direction = 1.0;
			}
			if(imp_cont->fk > MAX_FORCE) imp_cont->cmd = MOTOR_ZERO; 

			//set motor command (written at beginning of next step on eNames())
			aValues[0] = imp_cont->cmd;
			//if(DEBUG  & i==0) printf("Motor Command: %.2f\n", aValues[0]);

	        clock_gettime(CLOCK_MONOTONIC, &(imp_cont->end_time));
	        
	        imp_cont->wait_time = imp_cont->end_time;
	        last_time = imp_cont->start_time;
	        
	        imp_StepTime(&imp_cont->end_time, &imp_cont->start_time, &imp_cont->step_time);
	        imp_WaitTime(&imp_cont->step_time, &imp_cont->wait_time);

	        //unlock current, lock next mutex
			if(i == BUFFER_SIZE - 1) { pthread_mutex_lock(&lock[0]); }
			else { pthread_mutex_lock(&lock[i+1]); }
			pthread_mutex_unlock(&lock[i]);	
	        
			if(++temp_counter > MAX_COUNT) {
				//pthread_mutex_unlock(&lock[0]);	
				//printf("i %d count %d \n", i, temp_counter);
				//LJM_eWriteName(daqHandle, "DAC0", MOTOR_ZERO);
				//return;
			}
	       
	        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &imp_cont->wait_time, NULL);

		}

		if(temp_counter > max_count) 
		{
			fprintf (imp_cont->fp, "End_stage\n"); //print header
			pthread_mutex_unlock(&lock[0]);
			break;
		}
	}

	LJM_eWriteName(daqHandle, "DAC0", MOTOR_ZERO);
	sprintf(sendBuff,"END_STAGE");
	send(connfd, sendBuff, strlen(sendBuff), 0);

	return NULL;
}


/**********************************************************************
				   	Thread 2: Comm Server 
***********************************************************************/

void *server(void* d)
{
	imp_serve = &((struct impStruct*)d)[BUFFER_SIZE]; //get socket data

	while(1)
	{
		for(int i = 0; i < BUFFER_SIZE; i++)
		{
			pthread_mutex_lock(&lock[i]);
			//if(DEBUG & i == 0) printf("Thread 2 (server) Executing ...\n");
			if(temp_counter%20 == 0) //refresh UI every _ seconds / 500 = every 0.5 seconds
			{
				imp_serve = &((struct impStruct*)d)[i];
				sprintf(sendBuff,"%.2f,%.2f,%.2f,%.2f,%.2f,%.2f", imp_serve->xk, imp_serve->xdes,imp_serve->vk,imp_serve->vdes,imp_serve->x_ball, x_end);
				send(connfd, sendBuff, strlen(sendBuff), 0);
			}
			

			pthread_mutex_unlock(&lock[i]);	

		}
			
		if(temp_counter > max_count) return;
		

	}

	return NULL;
}



/**********************************************************************
				   	Thread 3: Logging Server
***********************************************************************/

void *logger(void * d)
{
	while(1){
		for(int i = 0; i < BUFFER_SIZE; i++)
		{
			pthread_mutex_lock(&lock[i]);
			//if(DEBUG & i == 0) printf("Thread 3 (logging) Executing ...\n");
			imp_log = &((struct impStruct*)d)[i];

			fprintf (imp_log->fp, "%d, %d, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %d, %d \n", 
				imp_log->start_time.tv_sec, imp_log->start_time.tv_nsec, imp_log->xk, imp_log->xa, 
				imp_log->vk, imp_log->va, imp_log->v_unfilt, imp_log->fk, imp_log->f_unfilt, imp_log->Fa, imp_log->xdes, imp_log->vdes, imp_log->cmd, imp_log->LSB[0], imp_log->LSF[0]); 
			
			pthread_mutex_unlock(&lock[i]);

		}
			
		if(temp_counter > max_count) return;
	}
	
	return NULL;

}



//TODO: set up homing, position thread 

void *home(void * d)
{
	pthread_mutex_lock(&lock[9]);
   	imp_cont = &((struct impStruct*)d)[9];

   	if(DEBUG) printf("Homing ...\n"); 
    sleep(10);

    aValues[0] = MOTOR_ZERO; 
    LJM_eNames(daqHandle, 5, aNames, aWrites, aNumValues, aValues, &errorAddress);
    imp_cont->LSB[0] = aValues[3];

    while(imp_cont->LSB[0] == 0)
    {
    	aValues[0] = 2.43; 
    	LJM_eNames(daqHandle, 5, aNames, aWrites, aNumValues, aValues, &errorAddress);
    	imp_cont->LSB[0] = aValues[3];

    }

    aValues[0] = MOTOR_ZERO; 
    //Robot should not be homed, reading the encoder will zero the position here.
    LJM_eNames(daqHandle, 5, aNames, aWrites, aNumValues, aValues, &errorAddress);
	
	if(DEBUG) printf("Device is home.\n"); 
	pthread_mutex_unlock(&lock[9]);

	return NULL;

}

void *goto_position(void * d)
{
	if(DEBUG) printf("Going to desired position ...\n");     
   
    sleep(10);
	
	return NULL;

}


