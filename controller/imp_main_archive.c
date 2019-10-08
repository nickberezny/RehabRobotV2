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

/**********************************************************************
					   Global Variables
***********************************************************************/
int daqHandle; 

int listenfd = 0, connfd = 0;

int environment = 2;

pthread_mutex_t lock[BUFFER_SIZE]; 
struct impStruct * imp_cont;
struct impStruct * imp_cont_next;
struct impStruct * imp_log;
struct impStruct * imp_serve;

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

double xdes_old = 0.0;

double home_decrease = 0.0;

struct physics_ball physics_ball; 
struct gait_sim gait;


/***********************************************************************
***********************************************************************/

int main(int argc, char* argv[]) {

    if(DEBUG) printf("Begin ... \n");

    int start_controller = 0;
    struct impStruct imp[BUFFER_SIZE + 1] = {0};

    struct regexMatch regex =
	{
		.SET = "SET",
		.P = "_P([0-9]*.[0-9]*)_",
		.D = "_D([0-9]*.[0-9]*)_",
		.xdes = "_xdes([0-9]*.[0-9]*)_",
		.K = "_K([0-9]*.[0-9]*)_",
		.B = "_B([0-9]*.[0-9]*)_",
		.M = "_M([0-9]*.[0-9]*)_",
		.xmax = "_xmax([0-9]*.[0-9]*)_",
		.vmax = "_vmax([0-9]*.[0-9]*)_",
		.game = "_game([0-9]*)_"
	} ; //regex matches

	regex_t compiled;
	regmatch_t matches[2];
	char matchBuffer[100];

	double Ad[4] = {0};
	double Bd[2] = {0};

	double Adf[4] = {0};
	double Bdf[2] = {0};

	physics_ball.in_play = 0;
	physics_ball.contact = 0;
	physics_ball.dx =0.0;
	physics_ball.x_mass = 0.0;
	physics_ball.v_mass = 0.0;
	physics_ball.k = PHYSICS_K;
	physics_ball.m = PHYSICS_M;
	physics_ball.Fs = 0.0;
	physics_ball.dir = 1.0;

    /**********************************************************************
					   Initialize TCP Socket
	***********************************************************************/
		
    struct sockaddr_in serv_addr; 

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    
    serv_addr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1",  &serv_addr.sin_addr.s_addr);
    serv_addr.sin_port = htons(TCP_PORT); 

    if(DEBUG) printf("Initialized Socket\n"); 

    /**********************************************************************
					  		 Initialize DAQ
	***********************************************************************/

    daqHandle = init_daq(daqHandle);

    double value = 0;
    const char * NAME = {"SERIAL_NUMBER"};
    LJM_eReadName(daqHandle, NAME, &value);

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

	//printf("%s\n", data_file_name);
	const char file_ext[] = "data2.txt";
	char folder[1000] = "data/";
	//strcat(data_file_name, file_ext);
	//strcat(folder, data_file_name);
	strcat(folder, file_ext);

	for(int i; i < strlen(folder) - 1; i++)
	{
		if (folder[i] == ' ') 
		    folder[i]='_';
		if (folder[i] == ':')
			folder[i]='-';
	}


   //create file name (date&time_data.txt)

	double freq = NSEC_IN_SEC / STEP_NSEC / 1000.0;
	char freq_buff[1000];
	sprintf(freq_buff, "Controller Frequency: %.2f kHz", freq);

    imp[0].fp = fopen (folder,"w");
    fprintf (imp[0].fp, "%s", asctime (timeinfo) ); 
    fprintf (imp[0].fp, "%s\n", freq_buff); 
    fprintf (imp[0].fp, "Time(s), Time(ns), x, xa, v, va, v_unfilt, f, f_unfilt, xdes, vdes, cmd, LSB, LSF\n"); //print header
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

    //lock memory (no dynamic allocation beyond here)
    if(mlockall(MCL_CURRENT|MCL_FUTURE) == -1) {
       printf("mlockall failed: %m\n");
       return 0;
    }

    //initialize threads (do not start yet)
    for(int i = 0; i < 3; i++)
    {
    	init_thread(&attr[i], &param[i], 98-2*i);
    }

    /**********************************************************************
					   Wait for input 
	***********************************************************************/

    if(CONNECT_TO_UI){

	    //start tcp socket
	    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 
	    listen(listenfd, 100);

		//Start UI Process 
		system("gnome-terminal --working-directory=Documents/RehabRobot/server -e 'sudo node server.js'");
		printf("TEST01\n");
		connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 

		printf("TEST01\n");
		
		if(GET_PARAMS_FROM_UI){
			while(1)
		    {
				if(DEBUG) printf("Waiting for run signal from UI ... \n");

				//wait for game settings
				connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 
				if(read(connfd, recvBuff, sizeof(recvBuff)) && recvBuff[0] == 'S')
				{
					//recieved settings 
					if(DEBUG) printf("recieved data: %s\n", recvBuff);
					start_controller = 1;

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

				if(DEBUG) printf("Set All Parameters (From UI)...\n");

				//wait for run signal before starting controller
				if(read(connfd, recvBuff, sizeof(recvBuff)) && recvBuff[0] == 'R' && start_controller == 1)
				{
					//everything set, begin therapy 
					if(DEBUG) printf("Start signal recieved \n");
					break;
				}

				close(connfd);
				printf("TEST01\n");
				sleep(0.01);
				printf("TEST01\n");
			}

				
	    }
	}
   		
	if(! GET_PARAMS_FROM_UI) {
   		//set default values if not connecting to UI (for testing)
	    for(int i = 0; i < BUFFER_SIZE; i++)
		{
			imp[i].P = P_GAIN;
			imp[i].D = D_GAIN;
			imp[i].K = K_GAIN;
			imp[i].B = B_GAIN;
			imp[i].M = M_GAIN;
			imp[i].b = B_GAIN;
			imp[i].m = M_GAIN;
			imp[i].xdes = X_DES*1000;
			imp[i].vdes = 0.0;
			imp[i].fdes = 0.0;
			imp[i].fp = imp[0].fp;
			imp[i].vmax = V_MAX;
			imp[i].F_Gain = F_GAIN;
			imp[i].xa = 0.0;
			imp[i].T = STEP_NSEC/NSEC_IN_SEC;
			imp[i].va_1 = 0.0;
			imp[i].va = 0.0;
			imp[i].Fk_1= 0.0;
			imp[i].Fa_1 = 0.0;
			imp[i].Fa = 0.0;				
		}

		gait.k_assist = K_GAIN;
		gait.k_gravity = K_GAIN - 0.15;
		gait.k_floor = K_GAIN + 0.3;
		gait.x_floor = 300.0;
		gait.x_thresh = 20.0;
		gait.f_thresh = 7.0;
		gait.phase = 1;
		gait.v_traj = 25.0;

		if(DEBUG) printf("Set All Parameters...\n");
	}

	//check if params are within acceptable range
	if( PMAX < imp[0].P || PMIN > imp[0].P  )
	{ 
		printf("P Gain out of range\n");
		aValues[0] = MOTOR_ZERO; 
    	LJM_eNames(daqHandle, 5, aNames, aWrites, aNumValues, aValues, &errorAddress);
    	return NULL;
	}
	if( DMAX < imp[0].D || DMIN > imp[0].D  )
	{ 
		printf("D Gain out of range\n");
		aValues[0] = MOTOR_ZERO; 
    	LJM_eNames(daqHandle, 5, aNames, aWrites, aNumValues, aValues, &errorAddress);
    	return NULL;
	}
	if( KMAX < imp[0].K || KMIN > imp[0].K  )
	{ 
		printf("K Gain out of range\n");
		aValues[0] = MOTOR_ZERO; 
    	LJM_eNames(daqHandle, 5, aNames, aWrites, aNumValues, aValues, &errorAddress);
    	return NULL;
	}
	if( BMAX < imp[0].B || BMIN > imp[0].B  )
	{ 
		printf("B Gain out of range\n");
		aValues[0] = MOTOR_ZERO; 
    	LJM_eNames(daqHandle, 5, aNames, aWrites, aNumValues, aValues, &errorAddress);
    	return NULL;
	}
	if( MMAX < imp[0].M || MMIN > imp[0].M  )
	{ 
		printf("M Gain out of range\n");
		aValues[0] = MOTOR_ZERO; 
    	LJM_eNames(daqHandle, 5, aNames, aWrites, aNumValues, aValues, &errorAddress);
    	return NULL;
	}
	if( V_MAX_MAX < imp[0].vmax || V_MAX_MIN > imp[0].vmax )
	{ 
		printf("Max velocity out of range\n");
		aValues[0] = MOTOR_ZERO; 
    	LJM_eNames(daqHandle, 5, aNames, aWrites, aNumValues, aValues, &errorAddress);
    	return NULL;
	}
	if( X_DES_MAX < imp[0].xdes || X_DES_MIN > imp[0].xdes  )
	{ 
		printf("Desired X Gain out of range\n");
		aValues[0] = MOTOR_ZERO; 
    	LJM_eNames(daqHandle, 5, aNames, aWrites, aNumValues, aValues, &errorAddress);
    	return NULL;
	}





    /**********************************************************************
					   	Calculate Ad, Bd
	***********************************************************************/

    //descrete state space for admittance control (x(k+1) = Ad*x(k) + Bd*u(k))
   	
    double A[2][2] = {{0.0, 1.0},{-imp[0].K/imp[0].M, -imp[0].B/imp[0].M}};
    double B[2] = {0.0, 1.0/imp[0].M};

    matrix_exp(A, Ad);
    imp_calc_Bd(Ad, A, B, Bd);

    for(int i = 0; i < BUFFER_SIZE; i++)
    {
    	imp[i].Ad = Ad;
    	imp[i].Bd = Bd;
    }

    printf("Ad: %.4f, %.4f, %.4f, %.4f\n", Ad[0], Ad[1], Ad[2], Ad[3]);
    printf("Bd: %.4f, %.4f\n", Bd[0], Bd[1]);

    //free motion matrices 

    double A2[2][2] = {{0.0, 1.0},{0.0, -(imp[0].B)/(imp[0].M)}};
    double B2[2] = {0.0, 1.0/(0.4*imp[0].M)};

    matrix_exp(A2, Adf);

    B2[1] = B2[1] * 0.001;
    //imp_calc_Bd(Adf, A2, B2, Bdf);

    for(int i = 0; i < BUFFER_SIZE; i++)
    {
    	imp[i].Adf = Adf;
    	imp[i].Bdf = B2; //inverted A is singular
    }

    printf("Adf: %.4f, %.4f, %.4f, %.4f\n", Adf[0], Adf[1], Adf[2], Adf[3]);
    printf("Bdf: %.4f, %.4f\n", B2[0], B2[1]);


	
/**********************************************************************
					   	Home to front then to back
***********************************************************************/

    sleep(2);
    
    if(DEBUG) printf("Homing ...\n");  

    //home to front
    /*
    aValues[0] = MOTOR_ZERO; 
    LJM_eNames(daqHandle, 5, aNames, aWrites, aNumValues, aValues, &errorAddress);
    imp[9].LSB[0] = aValues[3];

    printf("%f\n", aValues[3]);

    while(imp[9].LSF[0] == 0)
    {
    	aValues[0] = MOTOR_ZERO_FWD - 0.02 + home_decrease;
    	if(aValues[0] > MOTOR_ZERO_FWD - 0.005) home_decrease += 0.0001; //decrease home command to prevent acceleration

    	LJM_eNames(daqHandle, 5, aNames, aWrites, aNumValues, aValues, &errorAddress);
    	imp[9].LSF[0] = aValues[3];
    }
*/
    //home to back
    aValues[0] = MOTOR_ZERO; 
    LJM_eNames(daqHandle, 5, aNames, aWrites, aNumValues, aValues, &errorAddress);
    imp[9].LSB[0] = aValues[3];

    printf("%f\n", aValues[3]);
    curr_pos = 0.0;

    while(imp[9].LSB[0] == 0)
    {
    	curr_pos = curr_pos + ENC_TO_MM * (double)aValues[4];
    	aValues[0] = MOTOR_ZERO_BWD + 0.02 - home_decrease;
    	if(aValues[0] > MOTOR_ZERO_BWD + 0.005) home_decrease += 0.0001; //decrease home command to prevent acceleration

    	LJM_eNames(daqHandle, 5, aNames, aWrites, aNumValues, aValues, &errorAddress);
    	imp[9].LSB[0] = aValues[3];
    }

    //TODO set XEND to curr_pos somehow
    aValues[0] = MOTOR_ZERO; 
    //Robot should not be homed, reading the encoder will zero the position here.
    
    LJM_eNames(daqHandle, 5, aNames, aWrites, aNumValues, aValues, &errorAddress);
    LJM_eNames(daqHandle, 5, aNames, aWrites, aNumValues, aValues, &errorAddress);
    LJM_eNames(daqHandle, 5, aNames, aWrites, aNumValues, aValues, &errorAddress);

/**********************************************************************
					   	Calibrate Force Sensor (get offset)
***********************************************************************/

    if(DEBUG) printf("Calibrating Force Sensor, Keep motor enabled ...\n"); 

    LJM_eNames(daqHandle, 5, aNames, aWrites, aNumValues, aValues, &errorAddress);
    ft_offset = FT_GAIN*aValues[1]; 

    for(int i = 1; i < 20; i++)
    {
    	LJM_eNames(daqHandle, 5, aNames, aWrites, aNumValues, aValues, &errorAddress);
    	printf("Force: %.3f\n", ft_offset);
    	ft_offset = ( (ft_offset*(double)i) + FT_GAIN*aValues[1] ) / ((double)i + 1.0);
    	usleep(1000); //sleep to space out measurements
    }

    if(DEBUG) printf("Force sensor offset: %.3f\n", ft_offset);

    sleep(5);

/**********************************************************************
					   	Create and join threads
***********************************************************************/

    if(DEBUG) printf("Joining Threads ...\n"); 
   // sleep(10);
	//create and join threads 
	pthread_create(&thread[0], &attr[0], controller, (void *)imp);
	pthread_create(&thread[1], &attr[1], server, (void *)imp);
	pthread_create(&thread[2], &attr[2], logger, (void *)imp);
    pthread_join(thread[0], NULL);
	pthread_join(thread[1], NULL);
	pthread_join(thread[2], NULL);
	
	//finished sessions, begin shutdown
	LJM_eStreamStop(daqHandle);
	LJM_Close(daqHandle);
	fclose(imp[0].fp);
	shutdown(connfd, 2);
    if(DEBUG) printf("Finished, terminating program... \n");

    char data_command[] = "gnome-terminal --working-directory=Documents/RehabRobot/controller/data -e 'sudo python data_test_plot.py data2.txt";
    char apostrophe[] = "'";
    //strcat(data_command, data_file_name);
    strcat(data_command, apostrophe);

    printf("%s\n", data_command);

    system(data_command); //automatically run python plotting script on data

	return 0;

}

/**********************************************************************
				   	Thread 1: Controller 
***********************************************************************/

void *controller(void * d)
{
	if(DEBUG) printf("Thread 1 (controller) initializing ...\n");
	pthread_mutex_lock(&lock[0]);

	xa = 0.0; // X_DES*1000;
	curr_pos = 0.0;

	aValues[0] = MOTOR_ZERO; 
    LJM_eNames(daqHandle, 5, aNames, aWrites, aNumValues, aValues, &errorAddress);

    //CONTROL LOOP -------------------------------------------------
	while(1){

		for(int i = 0; i < BUFFER_SIZE; i++)
		{
			clock_gettime(CLOCK_MONOTONIC, &temp_time); 

			imp_cont = &((struct impStruct*)d)[i];
			imp_cont->start_time = temp_time;

			if(DEBUG & i == 0) printf("Thread 1 (controller) Executing ...\n");
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
			

			if(i==0) printf("Force: %.3f\n", imp_cont->fk);
			//Calculate Velocity 
	        imp_StepTime(&imp_cont->start_time, &last_time, &imp_cont->step_time);
			imp_cont->vk = ENC_TO_MM * aValues[4] / ((double)imp_cont->step_time.tv_sec + (double)imp_cont->step_time.tv_nsec/NSEC_IN_SEC);
			imp_cont->v_unfilt = imp_cont->vk;
			imp_FIR(v_filt, &imp_cont->vk, &fir_order_v); //moving average filter for velocity 

			//Controller

			/*
			if(imp_cont->game == 1)
			{
				//assitive trajectory
				imp_traj(imp_cont, &direction);
				imp_Adm(imp_cont, &xa, &va);
			}
			else if(imp_cont->game == 2)
			{
				//follow velcocity (no assistance)
				imp_Adm_free(imp_cont, &xa, &va);
			}
			else if(imp_cont->game == 3)
			{
				//balance with haptic disturbances
				imp_Haptics_impedance(imp_cont, &physics_ball);
			}
			else if(imp_cont->game == 4)
			{
				//gait cycle simulation
				imp_gait(imp_cont);
			}
			else
			{
				if(DEBUG) printf("No game selected ...\n");
			}
			*/

			//imp_traj(imp_cont, &direction, &xdes_old);
			//imp_PD(imp_cont);
			//imp_Adm(imp_cont, &xa, &va);
			imp_Haptics_impedance(imp_cont, &physics_ball, &gait, &xa, &va, &fa, &fk, &fa_1, &environment);
			//imp_Adm_free(imp_cont, &xa, &va);
			
			//Safety Checks
			//TODO : check direction of command
			//TODO : check IR
			if(imp_cont->cmd > MAX_COMMAND) imp_cont->cmd = MAX_COMMAND;
			if((-1)*imp_cont->cmd > MAX_COMMAND) imp_cont->cmd = (-1)*MAX_COMMAND;
			if(imp_cont->cmd > 0) imp_cont->cmd = MOTOR_ZERO_FWD - imp_cont->cmd;
			if(imp_cont->cmd < 0) imp_cont->cmd = MOTOR_ZERO_BWD - imp_cont->cmd;
			if(imp_cont->cmd == 0) imp_cont->cmd = MOTOR_ZERO;

			if(imp_cont->LSF[1] )
			{
			  	if(imp_cont->cmd < MOTOR_ZERO) imp_cont->cmd = MOTOR_ZERO; 
			  	direction = -1.0;		
			}
			if(imp_cont->LSB[1])  
			{
				if(imp_cont->cmd > MOTOR_ZERO) imp_cont->cmd = MOTOR_ZERO;  
				direction = 1.0;
			}
			if(imp_cont->fk > MAX_FORCE) imp_cont->cmd = MOTOR_ZERO; 

			//set motor command (written at beginning of next step on eNames())
			aValues[0] = imp_cont->cmd;
			if(DEBUG  & i==0) printf("Motor Command: %.2f\n", aValues[0]);

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
				pthread_mutex_unlock(&lock[0]);	
				printf("i %d count %d \n", i, temp_counter);
				LJM_eWriteName(daqHandle, "DAC0", MOTOR_ZERO);
				return;
			}
	       
	        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &imp_cont->wait_time, NULL);

		}
	}

	LJM_eWriteName(daqHandle, "DAC0", MOTOR_ZERO);

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
			if(i == 0)
			{
				imp_serve = &((struct impStruct*)d)[i];
				sprintf(sendBuff,"%.2f,%.2f,%.2f,%.2f", imp_serve->xk,imp_serve->xdes,imp_serve->vk,imp_serve->vdes);
				printf("%d\n", send(connfd, sendBuff, strlen(sendBuff), 0));
			}
			

			pthread_mutex_unlock(&lock[i]);	

		}
			
		if(temp_counter > MAX_COUNT) return;
		

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

			fprintf (imp_log->fp, "%d, %d, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %d, %d \n", 
				imp_log->start_time.tv_sec, imp_log->start_time.tv_nsec, imp_log->xk, imp_log->xa, 
				imp_log->vk, imp_log->va, imp_log->v_unfilt, imp_log->fk, imp_log->f_unfilt, imp_log->xdes, imp_log->vdes, imp_log->cmd, imp_log->LSB[0], imp_log->LSF[0]); 
			
			pthread_mutex_unlock(&lock[i]);

		}
			
		if(temp_counter > MAX_COUNT) return;
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


