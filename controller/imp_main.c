#include <stdlib.h>
#include <stdio.h>

#include <limits.h>
#include <pthread.h>
#include <sched.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <signal.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <LabJackM.h> 

#include "include/imp_structures.h"
#include "include/LJM_Utilities.h"

#include "include/imp_variables.h"

#define DEBUG 1 //will print updates
#define CONNECT_TO_UI 0
#define GET_PARAMS_FROM_UI 0 //will get params from remote UI (set 0 for testing, 1 for production)
#define MAX_COUNT 50999 //maximum iterations before shutdown (only on debug) 

#define BUFFER_SIZE 10 //size of data sturcture array
#define STRUCTURE_ELEMENTS 25 //number of elements in data structure

#define USE_DEFINED_X_RANGE 0 //use the max range from imp_variables file (X_END)

/**********************************************************************
					   Global Variables
***********************************************************************/
int subject_id = 6;

int daqHandle; 
int listenfd_1 = 0, connfd_1 = 0, listenfd_2 = 0, connfd_2 = 0;
int environment = 2;
int exp_number = 0;
int game_number = 0;
int game_type = 0;
int exp_iteration = 0;

double custom_trajectory[2][100];

int finished_home = 0;
int finished_set = 0;
int finished_calibrate = 0; 
int record_traj = 0;

int terminate_program = 0;

pthread_mutex_t lock[BUFFER_SIZE]; 
struct impStruct * imp_cont;
struct impStruct * imp_cont_next;
struct impStruct * imp_log;
struct impStruct * imp_serve;
struct impStruct * imp_curr;
struct impStruct * imp_prev;


char recvBuff[1024];
char sendBuff[1024];

int DeviceScanBacklog = 0;
int LJMScanBacklog = 0;

int temp_counter = 0; 
double curr_pos = 0.0;
struct timespec last_time;
struct timespec temp_time;    

double aValues[5] = {0};
const char * aNames[5] = {"DAC0", "AIN0","FIO0", "FIO1", "DIO2_EF_READ_A_F_AND_RESET"};
int aNumValues[5] = {1,1,1,1,1};
int aWrites[5] = {1,0,0,0,0};
int errorAddress = 0;

double v_filt[FIR_ORDER_V + 1] = {0};
double f_filt[FIR_ORDER_F + 1] = {0};

int fir_order_v = FIR_ORDER_V;
int fir_order_f = FIR_ORDER_F;

double direction = 1.0; 
double ft_offset = 0.0;

double xa = 0.0;
double va = 0.0;
double fa = 0.0; 
double fk = 0.0;
double fa_1 = 0.0;

double k_gain = 0.0;
double b_gain = 0.0; 
int max_count = 0;

double xdes_old = 0.0;
double x_end = 0.0;
double v_max = 0.0;

double * traj_curr_step;
double * traj_start_time; 


double home_decrease = 0.0;

struct physics_ball physics_ball; 
struct gait_sim gait;

int game_wait_sec = 5.0;

FILE * fp_traj;

regex_t compiled;
struct regexMatch regex = {
		.SET = "SET",
		.P = "_P([0-9]*.[0-9]*)_",
		.D = "_D([0-9]*.[0-9]*)_",
		.xdes = "_xdes([0-9]*.[0-9]*)_",
		.K = "_K([0-9]*.[0-9]*)_",
		.B = "_B([0-9]*.[0-9]*)_",
		.M = "_M([0-9]*.[0-9]*)_",
		.xmax = "_xmax([0-9]*.[0-9]*)_",
		.vmax = "_vmax([0-9]*.[0-9]*)_",
		.game = "_game([0-9]*)_",
		.exp = "_exp([0-9]*)_",
		.traj = "_traj([0-9]*)_"
	} ; //regex matches
regmatch_t matches[2];
char matchBuffer[100];

/***********************************************************************
***********************************************************************/

int main(int argc, char* argv[]) {

    if(DEBUG) printf("Begin ... \n");

    int start_controller = 0;
    struct impStruct imp[BUFFER_SIZE + 1] = {0};

	double Ad[4] = {0};
	double Bd[2] = {0};

	double Ad2[4] = {0};
	double Bd2[2] = {0};

	double Adf[4] = {0};
	double Bdf[2] = {0};

	physics_ball.in_play = 0;
	physics_ball.contact = 0;
	physics_ball.dx = 10.0;
	physics_ball.x_mass = 0.0;
	physics_ball.v_mass = 0.0;
	physics_ball.k = PHYSICS_K;
	physics_ball.m = PHYSICS_M;
	physics_ball.Fs = 0.0;
	physics_ball.dir = 1.0;
	physics_ball.game = 1;

    /**********************************************************************
					   Initialize TCP Socket
	***********************************************************************/
		
    struct sockaddr_in serv_addr_1, serv_addr_2; 

    listenfd_1 = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr_1, '0', sizeof(serv_addr_1));
    
    serv_addr_1.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1",  &serv_addr_1.sin_addr.s_addr);
    serv_addr_1.sin_port = htons(UI_PORT); 

    listenfd_2 = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr_2, '0', sizeof(serv_addr_2));
    
    serv_addr_2.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1",  &serv_addr_2.sin_addr.s_addr);
    serv_addr_2.sin_port = htons(GAME_PORT); 


    if(DEBUG) printf("Initialized Sockets\n"); 

    /**********************************************************************
					  		 Initialize DAQ
	***********************************************************************/

    daqHandle = init_daq(daqHandle);

    double value = 0;
    const char * NAME = {"SERIAL_NUMBER"};
    LJM_eReadName(daqHandle, NAME, &value);

    aValues[0] = MOTOR_ZERO; 
    LJM_eNames(daqHandle, 5, aNames, aWrites, aNumValues, aValues, &errorAddress);

    if(DEBUG) printf("Connected to LabJack %s = %f\n", NAME, value);


    /**********************************************************************
					  		 Initialize Data Log File 
	***********************************************************************/

    //get current date & time
    time_t rawtime;
	struct tm * timeinfo;

	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	char * data_file_name;
	data_file_name = asctime(timeinfo);
	int en = strlen(data_file_name);
	data_file_name[en-1] = '_';

	const char file_name[] = "data.txt";
	char folder[1000] = "data/";
	strcat(folder, data_file_name);

	//strcat(folder, file_ext);

	for(int i; i < strlen(folder) - 1; i++)
	{
		if (folder[i] == ' ') 
		    folder[i]='_';
		if (folder[i] == ':')
			folder[i]='-';
	}

	mkdir(folder,0);


   //create file name (date&time_data.txt)

	double freq = NSEC_IN_SEC / STEP_NSEC / 1000.0;
	char freq_buff[1000];
	

	sprintf(freq_buff, "Controller Frequency: %.2f kHz", freq);

    imp[0].fp = fopen (file_name,"w");
    fprintf (imp[0].fp, "Subject ID: %d\n", subject_id); 
    fprintf (imp[0].fp, "%s", asctime (timeinfo) ); 
    fprintf (imp[0].fp, "%s\n", freq_buff); 
    fprintf (imp[0].fp, "Time(s), Time(ns), x, xa, v, va, v_unfilt, f, f_unfilt, Fa, xdes, vdes, cmd, LSB, LSF\n"); //print header
    //fclose(imp[0].fp);
    
    if(DEBUG) printf("Created data file %s\n", folder);

    /**********************************************************************
					   Initialize Mutexes
	***********************************************************************/

	for(int i = 0; i < BUFFER_SIZE; i++)
	{
		pthread_mutex_init(&lock[i], NULL); //init mutex locks
		struct imp *point = &(imp[i]); //allocate memory 
		point = (struct imp*)calloc(STRUCTURE_ELEMENTS,sizeof(imp[i])); 
	}


	/**********************************************************************
					   Initialize Threads & Memory Lock
	***********************************************************************/

	struct sched_param param[3];
    pthread_attr_t attr[3];
    pthread_t thread[3];

    //initialize threads (do not start yet)
    for(int i = 0; i < 3; i++)
    {
    	init_thread(&attr[i], &param[i], 98-2*i);
    }

    //lock memory (no dynamic allocation beyond here)
    if(mlockall(MCL_CURRENT|MCL_FUTURE) == -1) {
       printf("mlockall failed: %m\n");
       return 0;
    }
	 

    /**********************************************************************
				 Initialize Controller for Homing/Recording 
	***********************************************************************/

	double A2[2][2] = {{0.0, 1.0},{0.0, -b_gain/imp[0].M}};
    double B2[2] = {0.0, 1.0/M_GAIN};

    matrix_exp(A2, Ad2);
    imp_calc_Bd(Ad2, A2, B2, Bd2);

    for(int i = 0; i < BUFFER_SIZE; i++)
    {
    	imp[i].Ad2 = Ad2;
    	imp[i].Bd2 = Bd2;
    }

    printf("Ad: %.4f, %.4f, %.4f, %.4f\n", Ad2[0], Ad2[1], Ad2[2], Ad2[3]);
    printf("Bd: %.4f, %.4f\n", Bd2[0], Bd2[1]);


    /**********************************************************************
					   Wait for input 
	***********************************************************************/

    //start tcp socket
    bind(listenfd_1, (struct sockaddr*)&serv_addr_1, sizeof(serv_addr_1)); 
    listen(listenfd_1, 100);

    bind(listenfd_2, (struct sockaddr*)&serv_addr_2, sizeof(serv_addr_2)); 
    listen(listenfd_2, 100);

	//Start UI Process *make sure to use npm run build before  
	//system("gnome-terminal --working-directory=Documents/RehabRobot/server -e 'sudo NODE_ENV='production' node server.js'");
	system("gnome-terminal --working-directory=Documents/RehabRobotV2/server -e 'sudo node server.js'");
	system("gnome-terminal --working-directory=Documents/RehabRobotV2/server/game -e 'sudo ./Linux_build01.x86_64'");

	
	connfd_2 = accept(listenfd_2, (struct sockaddr*)NULL, NULL);

	printf("connfd_2: %d\n", connfd_2);

	//start top level while loop 
	while(1)
	{

		//TODO: reset after run 
		connfd_1 = accept(listenfd_1, (struct sockaddr*)NULL, NULL);

		while(1)
		{
			
			if(read(connfd_1, recvBuff, sizeof(recvBuff)))
			{
				if(recvBuff[0] == 'S'){
					
					//recieved settings 
					recvBuff[0] == 'X';
					get_parameters(imp);

					sprintf(sendBuff,"SET");
					send(connfd_1, sendBuff, strlen(sendBuff), 0);

				}else if(recvBuff[0] == 'H'){
					
					//received homing command 
					recvBuff[0] == 'X';
					home(imp);

					sprintf(sendBuff,"HOME_%d",x_end);
					send(connfd_1, sendBuff, strlen(sendBuff), 0);

				}else if(recvBuff[0] == 'C'){
					
					//received homing command 
					recvBuff[0] == 'X';
					calibrate();

					sprintf(sendBuff,"CAL_%d",ft_offset);
					send(connfd_1, sendBuff, strlen(sendBuff), 0);

				}else if(recvBuff[0] == 'T'){
					
					//recieved trajectory record command 
					recvBuff[0] == 'X';
					fp_traj = fopen('trajectory/custom_trajectory.txt',"w");
					record_trajectory(imp); 

					sprintf(sendBuff,"TRAJ");
					send(connfd_1, sendBuff, strlen(sendBuff), 0);

				}else if(recvBuff[0] == 'R'){
					
					//end loop, exit program
					recvBuff[0] == 'X';
					//if() break;

					sprintf(sendBuff,"RUN");
					send(connfd_1, sendBuff, strlen(sendBuff), 0); 

					break;	

				}else if(recvBuff[0] == 'E'){
					
					//end loop, exit program
					//finished sessions, begin shutdown
					LJM_eStreamStop(daqHandle);  
					LJM_Close(daqHandle);
					fclose(imp[0].fp);
					shutdown(connfd_1, 2);
				    if(DEBUG) printf("Finished, terminating program... \n");

				    return;
				}		
			}

			sleep(0.01);
		}


	    /**********************************************************************
						   	Calculate Ad, Bd
		***********************************************************************/

	    //descrete state space for admittance control (x(k+1) = Ad*x(k) + Bd*u(k))
	   	
	    double A[2][2] = {{0.0, 1.0},{-k_gain/imp[0].M, -b_gain/imp[0].M}};
	    double B[2] = {0.0, 1.0/M_GAIN};

	    matrix_exp(A, Ad);

	    printf("a to invert 1: %.2f,%.2f,%.2f,%.2f\n", A[0][0], A[0][1], A[1][0], A[1][1]);

	    imp_calc_Bd(Ad, A, B, Bd);

	    for(int i = 0; i < BUFFER_SIZE; i++)
	    {
	    	imp[i].Ad = Ad;
	    	imp[i].Bd = Bd;
	    }

	    printf("Ad: %.4f, %.4f, %.4f, %.4f\n", Ad[0], Ad[1], Ad[2], Ad[3]);
	    printf("Bd: %.4f, %.4f\n", Bd[0], Bd[1]);

	    sprintf(sendBuff,"SET");
		send(connfd_1, sendBuff, strlen(sendBuff), 0);

	  	

		/**********************************************************************
							   	Wait for RUN command from UI
		***********************************************************************/

	    sprintf(sendBuff,"RUN");
		send(connfd_1, sendBuff, strlen(sendBuff), 0);


		/**********************************************************************
							   	Create and join threads
		***********************************************************************/


		sleep(game_wait_sec);

	    if(DEBUG) printf("Joining Threads ...\n"); 
	   	// sleep(10);
		//create and join threads 
		pthread_create(&thread[0], &attr[0], controller, (void *)imp);
		pthread_create(&thread[1], &attr[1], server, (void *)imp);
		pthread_create(&thread[2], &attr[2], logger, (void *)imp);
	    pthread_join(thread[0], NULL);
		pthread_join(thread[1], NULL);
		pthread_join(thread[2], NULL);
		
	}

	return 0;
}


/**********************************************************************
						THREADS  
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
					switch(imp_cont->traj)
					{
						case 1:
							//standard trajectory
							imp_traj2(imp_cont, &direction, &xdes_old, &x_end);
							break;
						case 2:
							//custom trajectory from file
							imp_traj_custom(imp_cont, traj_curr_step, traj_start_time, custom_trajectory); //void imp_traj_custom(struct impStruct * imp, double * curr_step, double * start_time, double * custom_trajectory)
							break;
					}

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
	send(connfd_1, sendBuff, strlen(sendBuff), 0);

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
				send(connfd_2, sendBuff, strlen(sendBuff), 0);
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

void home(struct impStruct * imp)
{

    sleep(2);
    if(DEBUG) printf("Homing...\n" );  

    //home to back
    /*
    aValues[0] = MOTOR_ZERO; 
    LJM_eNames(daqHandle, 5, aNames, aWrites, aNumValues, aValues, &errorAddress);
    imp[9].LSB[0] = aValues[3];

    printf("%f\n", aValues[3]);
    curr_pos = 0.0;
    home_decrease = 0.0;

    while(imp[9].LSB[0] == 0)
    {
    	curr_pos = curr_pos + ENC_TO_MM * (double)aValues[4];
    	aValues[0] = MOTOR_ZERO_BWD + 0.02 - home_decrease;
    	if(aValues[0] > MOTOR_ZERO_BWD + 0.005) home_decrease += 0.0001; //decrease home command to prevent acceleration

    	LJM_eNames(daqHandle, 5, aNames, aWrites, aNumValues, aValues, &errorAddress);
    	imp[9].LSB[0] = aValues[3];
    }

    curr_pos = 0.0;

    //home to front
    
    aValues[0] = MOTOR_ZERO; 
    LJM_eNames(daqHandle, 5, aNames, aWrites, aNumValues, aValues, &errorAddress);
    imp[9].LSB[0] = aValues[2];

    printf("%f\n", aValues[2]);

    while(imp[9].LSF[0] == 0)
    {
    	curr_pos = curr_pos + ENC_TO_MM * (double)aValues[4];
    	aValues[0] = MOTOR_ZERO_FWD - 0.05 + home_decrease;
    	if(aValues[0] > MOTOR_ZERO_FWD - 0.005) home_decrease += 0.0001; //decrease home command to prevent acceleration

    	LJM_eNames(daqHandle, 5, aNames, aWrites, aNumValues, aValues, &errorAddress);
    	imp[9].LSF[0] = aValues[2];
    }

    if(exp_iteration == 1)
    {
	    //set x_end to curr_pos somehow
	   	if(!USE_DEFINED_X_RANGE) x_end = curr_pos;
	   	else x_end = X_END;

	    if(DEBUG) printf("X MAX (mm): %.2f\n", x_end);
	}
    
    aValues[0] = MOTOR_ZERO; 

    for(int i = 0; i < BUFFER_SIZE; i++)
	{
		if(game_type == 2 && environment == 1) 
		{
			imp[i].xdes = BALANCE_POINT; //set balance game set point
		}else{
			imp[i].xdes = x_end;
		}
	}
    //Robot should not be homed, reading the encoder will zero the position here.
    
    LJM_eNames(daqHandle, 5, aNames, aWrites, aNumValues, aValues, &errorAddress);
    LJM_eNames(daqHandle, 5, aNames, aWrites, aNumValues, aValues, &errorAddress);
    LJM_eNames(daqHandle, 5, aNames, aWrites, aNumValues, aValues, &errorAddress);

    sprintf(sendBuff,"INFO,%.2f,%.2f", ft_offset, x_end);
	send(connfd_1, sendBuff, strlen(sendBuff), 0);

	finished_home = 1;
	*/
	return NULL;

}


void record_trajectory(struct impStruct * imp)
{

	if(DEBUG) printf("Beginning recording of trajectory...\n");

	xa = 0.0; // X_DES*1000;
	va = 0.0;
	curr_pos = 0.0; //track movement since homing? 
	x_end = 0.0;

	*imp_curr = imp[0];
	*imp_prev = imp[1];
	imp_curr->start_time = temp_time;

	while(1)
	{

		clock_gettime(CLOCK_MONOTONIC, &temp_time); 
		LJM_eNames(daqHandle, 5, aNames, aWrites, aNumValues, aValues, &errorAddress);
				
        imp_curr->fk = FT_GAIN*aValues[1] - ft_offset;
        imp_prev->LSF[0] = imp_curr->LSF[0];
        imp_prev->LSB[0] = imp_curr->LSB[0];
        imp_curr->LSF[0] = aValues[2];
        imp_curr->LSB[0] = aValues[3];
        curr_pos = curr_pos + ENC_TO_MM * (double)aValues[4]; 
        imp_curr->xk = curr_pos;

        imp_StepTime(&imp_curr->start_time, &last_time, &imp_curr->step_time);
		imp_curr->vk = ENC_TO_MM * aValues[4] / ((double)imp_curr->step_time.tv_sec + (double)imp_curr->step_time.tv_nsec/NSEC_IN_SEC);
		imp_curr->v_unfilt = imp_curr->vk;
		imp_FIR(v_filt, &imp_curr->vk, &fir_order_v);

		imp_Adm(imp_curr, &xa, &va, &x_end);

		if(imp_curr->cmd != imp_curr->cmd) imp_curr->cmd = 0; //check if command is nan
		if(imp_curr->cmd > MAX_COMMAND) imp_curr->cmd = MAX_COMMAND;
		if((-1)*imp_curr->cmd > MAX_COMMAND) imp_curr->cmd = (-1)*MAX_COMMAND;
		if(imp_curr->cmd > 0) imp_curr->cmd = MOTOR_ZERO_FWD - imp_curr->cmd;
		if(imp_curr->cmd < 0) imp_curr->cmd = MOTOR_ZERO_BWD - imp_curr->cmd;
		if(imp_curr->cmd == 0) imp_curr->cmd = MOTOR_ZERO;

		if(imp_curr->LSF[0])
		{
		  	if(imp_curr->cmd < MOTOR_ZERO) imp_curr->cmd = MOTOR_ZERO; 
		  	//direction = -1.0;		
		}
		if(imp_curr->LSB[0])  
		{
			if(imp_curr->cmd > MOTOR_ZERO) imp_curr->cmd = MOTOR_ZERO;  
			//direction = 1.0;
		}
		
		if(imp_curr->LSB[0] && !imp_prev->LSB[0] && record_traj == 0)
		{
			//begin recording 
			record_traj = 1;
			xa = 0.0; // X_DES*1000;
			va = 0.0;
			curr_pos = 0.0;
		}

		if(imp_curr->LSF[0] && !imp_prev->LSF[0] && record_traj == 1)
		{
			//halfway through trajectory 
			record_traj = 2;
		}

		if(imp_curr->LSB[0] && !imp_prev->LSB[0] && record_traj == 2)
		{
			//stop recording 
			record_traj = 0;
			fclose(fp_traj);
			break;
		}

		if(imp_curr->fk > MAX_FORCE) imp_curr->cmd = MOTOR_ZERO; 

		//set motor command (written at beginning of next step on eNames())
		aValues[0] = imp_curr->cmd;
		//if(DEBUG  & i==0) printf("Motor Command: %.2f\n", aValues[0]);

		if(record_traj > 0)
		{
			fprintf (fp_traj, "%.2f,%.2f\n", imp_curr->step_time, imp_curr->xk);
		}

        clock_gettime(CLOCK_MONOTONIC, &(imp_curr->end_time));
        
        imp_curr->wait_time = imp_curr->end_time;
        last_time = imp_curr->start_time;
        
        imp_StepTime(&imp_curr->end_time, &imp_curr->start_time, &imp_curr->step_time);
        imp_WaitTime(&imp_curr->step_time, &imp_curr->wait_time);

        //unlock current, lock next mutex
        
		if(++temp_counter > MAX_COUNT) {
			//pthread_mutex_unlock(&lock[0]);	
			//printf("i %d count %d \n", i, temp_counter);
			//LJM_eWriteName(daqHandle, "DAC0", MOTOR_ZERO);
			//return;
		}
       
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &imp_curr->wait_time, NULL);

	}


	return;
}

void get_parameters(struct impStruct * imp)
{
	if(DEBUG) printf("recieved data: %s\n", recvBuff);
				
	regcomp(&compiled, regex.exp, REG_EXTENDED);
	if(regexec(&compiled, recvBuff, 2, matches, 0)==0)
	{
		sprintf(matchBuffer, "%.*s\n", matches[1].rm_eo-matches[1].rm_so,  recvBuff+matches[1].rm_so );
		sscanf(matchBuffer, "%d", &exp_number);
	    if(DEBUG) { printf("Experiment set to: %d\n", exp_number); }
	}
	if(exp_number == 3)
	{
		regcomp(&compiled, regex.game, REG_EXTENDED);
			if(regexec(&compiled, recvBuff, 2, matches, 0)==0){
			sprintf(matchBuffer, "%.*s\n", matches[1].rm_eo-matches[1].rm_so,  recvBuff+matches[1].rm_so );
			sscanf(matchBuffer, "%d", &game_number);
		    if(DEBUG) { printf("Game set to: %d\n", game_number); }
		}
	}

	/*
	if(DEBUG) printf("Waiting for run signal from UI ... \n");

	//wait for game settings
	//connfd_1 = accept(listenfd_1, (struct sockaddr*)NULL, NULL); 
	if(read(connfd_1, recvBuff, sizeof(recvBuff)) && recvBuff[0] == 'S')
	{
		//recieved settings 
		if(DEBUG) printf("recieved data: %s\n", recvBuff);
		//start_controller = 1;

		//find set parameters in message using regular expreesions 
		regcomp(&compiled, regex.P, REG_EXTENDED);
		if(regexec(&compiled, recvBuff, 2, matches, 0)==0){
			sprintf(matchBuffer, "%.*s\n", matches[1].rm_eo-matches[1].rm_so,  recvBuff+matches[1].rm_so );
			sscanf(matchBuffer, "%lf", &imp[0].P);
		    if(DEBUG) { printf("P gain is: %lf\n", imp[0].P); }
		}

		regcomp(&compiled, regex.D, REG_EXTENDED);
		if(regexec(&compiled, recvBuff, 2, matches, 0)==0){
			sprintf(matchBuffer, "%.*s\n", matches[1].rm_eo-matches[1].rm_so,  recvBuff+matches[1].rm_so );
			sscanf(matchBuffer, "%lf", &imp[0].D);
		    if(DEBUG) { printf("D gain is: %f\n", imp[0].D); }
		}

		regcomp(&compiled, regex.xdes, REG_EXTENDED);
		if(regexec(&compiled, recvBuff, 2, matches, 0)==0){
			sprintf(matchBuffer, "%.*s\n", matches[1].rm_eo-matches[1].rm_so,  recvBuff+matches[1].rm_so );
			sscanf(matchBuffer, "%lf", &imp[0].xdes);
		    if(DEBUG) { printf("xdes is: %f\n", imp[0].xdes); }
		}

		regcomp(&compiled, regex.xmax, REG_EXTENDED);
		if(regexec(&compiled, recvBuff, 2, matches, 0)==0){
			sprintf(matchBuffer, "%.*s\n", matches[1].rm_eo-matches[1].rm_so,  recvBuff+matches[1].rm_so );
			sscanf(matchBuffer, "%lf", &imp[0].xmax);
		    if(DEBUG) { printf("Xmax is: %f\n", imp[0].xmax); }
		}

		regcomp(&compiled, regex.vmax, REG_EXTENDED);
		if(regexec(&compiled, recvBuff, 2, matches, 0)==0){
			sprintf(matchBuffer, "%.*s\n", matches[1].rm_eo-matches[1].rm_so,  recvBuff+matches[1].rm_so );
			sscanf(matchBuffer, "%lf", &imp[0].vmax);
		    if(DEBUG) { printf("Vmax is: %f\n", imp[0].vmax); }
		}

		regcomp(&compiled, regex.K, REG_EXTENDED);
		if(regexec(&compiled, recvBuff, 2, matches, 0)==0){
			sprintf(matchBuffer, "%.*s\n", matches[1].rm_eo-matches[1].rm_so,  recvBuff+matches[1].rm_so );
			sscanf(matchBuffer, "%lf", &imp[0].K);
		    if(DEBUG) { printf("K is: %f\n", imp[0].K); }
		}
		
		regcomp(&compiled, regex.B, REG_EXTENDED);
		if(regexec(&compiled, recvBuff, 2, matches, 0)==0){
			sprintf(matchBuffer, "%.*s\n", matches[1].rm_eo-matches[1].rm_so,  recvBuff+matches[1].rm_so );
			sscanf(matchBuffer, "%lf", &imp[0].B);
		    if(DEBUG) { printf("B is: %f\n", imp[0].B); }
		}
		
		regcomp(&compiled, regex.M, REG_EXTENDED);
		if(regexec(&compiled, recvBuff, 2, matches, 0)==0){
			sprintf(matchBuffer, "%.*s\n", matches[1].rm_eo-matches[1].rm_so,  recvBuff+matches[1].rm_so );
			sscanf(matchBuffer, "%lf", &imp[0].M);
		    if(DEBUG) { printf("M is: %f\n", imp[0].M); }
		}

		regcomp(&compiled, regex.game, REG_EXTENDED);
		if(regexec(&compiled, recvBuff, 2, matches, 0)==0){
			sprintf(matchBuffer, "%.*s\n", matches[1].rm_eo-matches[1].rm_so,  recvBuff+matches[1].rm_so );
			sscanf(matchBuffer, "%lf", &imp[0].game);
		    if(DEBUG) { printf("Game set to: %f\n", imp[0].game); }
		}

		regcomp(&compiled, regex.traj, REG_EXTENDED);
		if(regexec(&compiled, recvBuff, 2, matches, 0)==0){
			sprintf(matchBuffer, "%.*s\n", matches[1].rm_eo-matches[1].rm_so,  recvBuff+matches[1].rm_so );
			sscanf(matchBuffer, "%lf", &imp[0].traj);
		    if(DEBUG) { printf("Trajectory set to: %f\n", imp[0].traj); }

		    if(imp[0].traj == 2)
		    {
		    	//load trajectory 
		    	fp_traj = fopen('trajectory/trajectory.txt', "r");
		    	int it = 0;
		    	char *traj_data; 
		    	char * str;

		    	while (fgets(str, 10, fp_traj) != NULL)
		    		traj_data = strtok(str, ',');
			        custom_trajectory[it][0] = traj_data[0];
			    	custom_trajectory[it][1] = traj_data[1];
			    	it++;

			    fclose(fp_traj);
		    }
		}

	}
		
		

	for(int i = 1; i < BUFFER_SIZE; i++)
	{
		imp[i].P = imp[0].P;
		imp[i].D = imp[0].D;
		imp[i].K = imp[0].K;
		imp[i].B = imp[0].B;
		imp[i].M = imp[0].M;
		imp[i].xdes = imp[0].xdes;
		imp[i].fp = imp[0].fp;

		imp[i].xmax = imp[0].xmax;
		imp[i].vmax = imp[0].vmax;
		imp[i].game = imp[0].game;
				
	}

	if(DEBUG) printf("Set All Parameters...\n");
	finished_set = 1;



	switch(imp[0].game){
		case 1:
			system("gnome-terminal --working-directory=Documents/RehabRobot/server -e 'sudo ./game1'");
			break;
		case 2:
			break;
		case 3:
			break;
	}
	
	*/
	return;
}

void calibrate()
{

	/**********************************************************************
					   	Calibrate Force Sensor (get offset)
	***********************************************************************/

    if(DEBUG) printf("Calibrating Force Sensor, Keep motor enabled ...\n"); 

    /*
    LJM_eNames(daqHandle, 5, aNames, aWrites, aNumValues, aValues, &errorAddress);
    ft_offset = FT_GAIN*aValues[1]; 

    for(int i = 1; i < 20; i++)
    {
    	LJM_eNames(daqHandle, 5, aNames, aWrites, aNumValues, aValues, &errorAddress);
    	//printf("Force: %.3f\n", ft_offset);
    	ft_offset = ( (ft_offset*(double)i) + FT_GAIN*aValues[1] ) / ((double)i + 1.0);
    	usleep(1000); //sleep to space out measurements
    }
    ft_offset = 2.66;
    if(DEBUG) printf("Force sensor offset: %.3f\n", ft_offset);

    finished_calibrate = 1;
    */

	return;
}