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
#include <regex.h>

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

#define DEBUG 1 //will print updates
#define UI_CONNECT 0 //will get params from remote UI (set 0 for testing, 1 for production)

#define BUFFER_SIZE 10 //size of data sturcture array
#define STRUCTURE_ELEMENTS 25 //number of elements in data structure
#define NSEC_IN_SEC 1000000000
#define STEP_NSEC 2000000 //control step time (1ms)

#define MAX_FORCE 50 //Newtons  

//Conversion
#define ENC_TO_MM 0.00115
#define MOTOR_ZERO 2.35 
#define FT_GAIN 43.0
#define FT_OFFSET -0.156393

/**********************************************************************
					   Global Variables
***********************************************************************/
int daqHandle; 

int listenfd = 0, connfd = 0;

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

int ScansPerRead = 1;
int NumAddresses = 7;
int aScanList[7] = {4800, 0, 2, 4, 2000, 2001, 2002, 1000}; //DAC0, AIN0, AIN1, AIN2, DIO0, DIO1, DIO2 
double ScanRate = NSEC_IN_SEC / STEP_NSEC;


double aValues[7] = {0};

float writeArray[1] = {1};

double aCmd[3] = {0};
const char * aNames[3] = {"DAC0","AIN0", "AIN1"};
int aNumValues[3] = {1,1,1};
int aWrites[3] = {1,0,0};
int errorAddress = 0;

double timeEnd = 0;
double timeStart = 0;

/***********************************************************************
***********************************************************************/

int main(int argc, char* argv[]) {

    if(DEBUG) printf("Begin ... \n");

    printf("%f\n", ScanRate);
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
		.M = "_M([0-9]*.[0-9]*)_"
	} ; //regex matches

	regex_t compiled;
	regmatch_t matches[2];
	char matchBuffer[100];

	//int temp[6] = {LJM_WRITE, LJM_READ, LJM_READ, LJM_READ, LJM_READ, LJM_READ};
	//memcpy(aWrites, temp, 6*sizeof(int));


    /**********************************************************************
					   Initialize TCP Socket
	***********************************************************************/
		
    struct sockaddr_in serv_addr; 

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    
    serv_addr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1",  &serv_addr.sin_addr.s_addr);
    serv_addr.sin_port = htons(1337); 

    if(DEBUG) printf("Initialized Socket\n"); 

    /**********************************************************************
					  		 Initialize DAQ
	***********************************************************************/

    daqHandle = init_daq(daqHandle);
    double value = 0;
    const char * NAME = {"SERIAL_NUMBER"};
    LJM_eReadName(daqHandle, NAME, &value);

    LJM_eStreamStop(daqHandle); //stop any previous streams
    LJM_eWriteName(daqHandle, "DAC0", MOTOR_ZERO); //set motor to zero
    
    //start Quadrature counter on DIO2 and DIO3
    LJM_eWriteName(daqHandle, "DIO2_EF_ENABLE", 0);
    LJM_eWriteName(daqHandle, "DIO3_EF_ENABLE", 0);

    LJM_eWriteName(daqHandle, "DIO2_EF_INDEX", 10);
    LJM_eWriteName(daqHandle, "DIO3_EF_INDEX", 10);

    LJM_eWriteName(daqHandle, "DIO2_EF_ENABLE", 1);
    LJM_eWriteName(daqHandle, "DIO3_EF_ENABLE", 1);
	
	///
	LJM_eWriteName(daqHandle, "STREAM_OUT0_ENABLE", 0);
	LJM_eWriteName(daqHandle, "STREAM_OUT0_TARGET", 1000);
	LJM_eWriteName(daqHandle, "STREAM_OUT0_BUFFER_SIZE", 64);
	LJM_eWriteName(daqHandle, "STREAM_OUT0_ENABLE", 1);

	LJM_eWriteName(daqHandle, "STREAM_OUT0_BUFFER_F32", writeArray[0]);
	LJM_eWriteName(daqHandle, "STREAM_OUT0_LOOP_SIZE", 1);
	LJM_eWriteName(daqHandle, "STREAM_OUT0_SET_LOOP", 1);

    LJM_WriteLibraryConfigS(LJM_STREAM_TRANSFERS_PER_SECOND, 80);
   

    //LJM_eStreamStart(daqHandle, ScansPerRead, NumAddresses, aScanList, &ScanRate);

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
	const char file_ext[] = "data.txt";
	char folder[1000] = "data/";
	strcat(data_file_name, file_ext);
	strcat(folder, data_file_name);

	for(int i; i < strlen(folder) - 1; i++)
	{
		if (folder[i] == ' ') 
		    folder[i]='_';
		if (folder[i] == ':')
			folder[i]='-';
	}


   //create file name (date&time_data.txt)

    imp[0].fp = fopen (folder,"w");
    fprintf (imp[0].fp, "%s", asctime (timeinfo) ); 
    fprintf (imp[0].fp, "StepTime, x, v, f, xdes, vdes, fdes, cmd, IR, LSB, LSF\n"); //print header
    //fclose(imp[0].fp);
    
    if(DEBUG) printf("Created data file %s\n", data_file_name); 

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

    if(UI_CONNECT){

	    //start tcp socket
	    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 
	    listen(listenfd, 100);

		//Start UI Process 
		system("gnome-terminal --working-directory=Documents/RehabRobot/server -e 'sudo node server.js'");


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
				
				

				for(int i = 1; i < BUFFER_SIZE; i++)
				{
					imp[i].P = imp[0].P;
					imp[i].D = imp[0].D;
					imp[i].K = imp[0].K;
					imp[i].B = imp[0].B;
					imp[i].M = imp[0].M;
					imp[i].xdes = imp[0].xdes;
					imp[i].fp = imp[0].fp;
							
				}

				if(DEBUG) printf("Set All Parameters...\n");
			}

			//wait for run signal before starting controller
			if(read(connfd, recvBuff, sizeof(recvBuff)) && recvBuff[0] == 'R' && start_controller == 1)
			{
				//everything set, begin therapy 
				if(DEBUG) printf("Start signal recieved \n");
				break;
			}

			close(connfd);
			sleep(0.01);
	    }
	}
   	else {

   		//set default values if not connecting to UI (for testing)
	    for(int i = 0; i < BUFFER_SIZE; i++)
			{
				imp[i].P = 0.001;
				imp[i].D = 0.001;
				imp[i].K = 0.001;
				imp[i].B = 0.001;
				imp[i].M = 0.001;
				imp[i].xdes = 100.0;
				imp[i].vdes = 0.0;
				imp[i].fp = imp[0].fp;
						
			}
	}


    /**********************************************************************
					   	Calculate Ad, Bd
	***********************************************************************/

    //descrete state space for admittance control (x(k+1) = Ad*x(k) + Bd*u(k))
    double Ad[4] = {{1.0, STEP_NSEC/NSEC_IN_SEC},{-STEP_NSEC/NSEC_IN_SEC * imp[0].K/imp[0].M, 1.0 - STEP_NSEC/NSEC_IN_SEC * imp[0].B/imp[0].M}};
    double Bd[2] = {0.0, 1.0/imp[0].M};

    for(int i = 0; i < BUFFER_SIZE; i++)
    {
    	imp[i].Ad = Ad;
    	imp[i].Bd = Bd;
    }


    /**********************************************************************
					   	Create and join threads
	***********************************************************************/
    
    if(DEBUG) printf("Joining Threads ...\n"); 

    sleep(10);

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
    if(DEBUG) printf("Finished, terminating program... \n");

	return 0;

}



/**********************************************************************
				   	Thread 1: Controller 
***********************************************************************/

void *controller(void * d)
{
	if(DEBUG) printf("Thread 1 (controller) initializing ...\n");
	pthread_mutex_lock(&lock[0]);

	//printf("%d\n", LJM_eStreamStart(daqHandle, ScansPerRead, NumAddresses, aScanList, &ScanRate));
	
	//clock_gettime(CLOCK_MONOTONIC, &(imp_cont->start_time)); 
    //CONTROL LOOP -------------------------------------------------
	while(1){

		for(int i = 0; i < BUFFER_SIZE; i++)
		{
			if(DEBUG & i == 0) printf("Thread 1 (controller) Executing ...\n");

			imp_cont = &((struct impStruct*)d)[i];
			//LJM_eWriteName(daqHandle, "STREAM_OUT0_BUFFER_F32", aCmd[0]);
			//Read & Write to DAQ ---------------------------------------
			
			//printf("%d\n",LJM_eWriteName(daqHandle, "STREAM_OUT0_BUFFER_F32", writeArray[0]));
			//printf("%d\n",LJM_eStreamRead(daqHandle, aValues, &DeviceScanBacklog, &LJMScanBacklog));
			//LJM_eReadName(daqHandle, "AIN0", aValues);
			//TODO: check if backlog exist, empty it 
			printf("1\n");
			timeStart = LJM_GetHostTick();
			LJM_eNames(daqHandle, 1, aNames, aWrites, aNumValues, aCmd, &errorAddress);
			timeEnd = LJM_GetHostTick();
			printf("%f\n", timeEnd-timeStart);
			//printf("Backlogs: %d, %d\n", DeviceScanBacklog, LJMScanBacklog);
			clock_gettime(CLOCK_MONOTONIC, &(imp_cont->start_time)); 
			printf("2\n");
	        //imp_cont->fk = FT_GAIN*aValues[0] + FT_OFFSET;
	        imp_cont->IR = aValues[1];
	        imp_cont->LSF[0] = imp_cont->LSF[1];
	        imp_cont->LSB[0] = imp_cont->LSF[1];
	        imp_cont->LSF[1] = aValues[3];
	        imp_cont->LSB[1] = aValues[4];
	        curr_pos = curr_pos + ENC_TO_MM * aValues[5];
	        imp_cont->xk = DeviceScanBacklog;
	
			//Calculate Velocity 
	        imp_StepTime(&imp_cont->start_time, &imp_cont->end_time, &imp_cont->step_time);
			imp_cont->vk = ENC_TO_MM * aValues[5] / (imp_cont->step_time.tv_sec + NSEC_IN_SEC*imp_cont->step_time.tv_nsec);

			//printf("step: %d . %d \n",imp_cont->start_time.tv_sec - imp_cont->end_time.tv_sec, imp_cont->start_time.tv_nsec - imp_cont->end_time.tv_nsec);

			//Controller
			//imp_Adm(imp_cont);
			imp_PD(imp_cont);
			//printf("Cmd: %.2f\n", imp_cont->cmd);
			

			//Safety Checks
			//TODO : check direction of command
			//TODO : check IR
			if(abs(imp_cont->cmd) > 0.75) imp_cont->cmd = 0;
			imp_cont->cmd += MOTOR_ZERO;

			if(imp_cont->LSF[1] && !imp_cont->LSF[0] && imp_cont->cmd > 0)  imp_cont->cmd = MOTOR_ZERO; 
			if(imp_cont->LSF[1] && !imp_cont->LSF[0] && imp_cont->cmd < 0)  imp_cont->cmd = MOTOR_ZERO; 
			if(imp_cont->fk > MAX_FORCE) imp_cont->cmd = MOTOR_ZERO; 

			aCmd[0] = 0;

    		//LJM_eWriteName(daqHandle, "STREAM_OUT0_BUFFER_F32", aCmd[0]);
			
			//write motor command 
			//LJM_eWriteName(daqHandle, "DAC0", 0.1);
			
			//LJM_eNames(daqHandle, 1, aNames, aWrites, aNumValues, aCmd, &errorAddress);
			//LJM_eNames(daqHandle, 1, aNames, aWrites, aNumValues, aCmd, &errorAddress);
	        clock_gettime(CLOCK_MONOTONIC, &(imp_cont->end_time));

	        //unlock current, lock next mutex
			if(i == BUFFER_SIZE - 1) { pthread_mutex_lock(&lock[0]); }
			else { pthread_mutex_lock(&lock[i+1]); }
			pthread_mutex_unlock(&lock[i]);	
	        
	        
			}

		if(++temp_counter > 100) {
			pthread_mutex_unlock(&lock[0]);	
			break;
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
			if(DEBUG & i == 0) printf("Thread 2 (server) Executing ...\n");

			imp_serve = &((struct impStruct*)d)[i];
			sprintf(sendBuff,"%.2f", imp_serve->xk);
			
			
			//send(connfd, sendBuff, strlen(sendBuff), 0);

			pthread_mutex_unlock(&lock[i]);	
		}
		if(temp_counter > 100) break;

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

			fprintf (imp_log->fp, "%d, %d,%.2f, %.2f, %.2f, %.2f, %d, %d \n", 
				i, imp_log->step_time.tv_nsec, imp_log->xk, 
				imp_log->xdes, imp_log->vdes, imp_log->cmd,imp_log->LSB[0], imp_log->LSF[0]); 
			//if(DEBUG & i == 0) printf("Thread 3 (logging) Done ...\n");
			pthread_mutex_unlock(&lock[i]);
		}
		if(temp_counter > 100) break;
	}
	
	return NULL;

}


