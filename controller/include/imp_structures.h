#include <stdbool.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <signal.h>
#include <time.h> 
#include <regex.h>
#include <stdio.h>

//data for tcp socket
struct socket_data {
	int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt;
    int addrlen;
};

//data for robot (updated at each time step)
struct impStruct {

	//admittance control parameters 
	double xk, vk, fk;
	double xa, va, va_1;
	double Fk_1, Fa, Fa_1;
	double Fw;
	double fdes, xdes, vdes;
	double xmax, vmax, fmax;
	double P, D;
	double *Ad, *Bd;
	double *Adf, *Bdf;
	double K, B, M;
	double m, b, T; //haptic coupling
	double cmd;
	int game; 
	int exp;
	struct timespec start_time;
	struct timespec end_time;
	struct timespec wait_time;
    struct timespec step_time;

    double f_unfilt, v_unfilt;
    double F_Gain;

	//other sensors 
	int LSF[2];
	int LSB[2];
	int IR;

	int home; //boolean indicated if device has been homed

	double x_ball;

	//data log
	FILE * fp;

};

//holds regular expression searches
struct regexMatch {
	char *SET;
	char *P;
	char *D;
	char *xdes;
	char *K;
	char *B;
	char *M;
	char *xmax;
	char *vmax;
	char *game;
	char *exp;
	char *traj;
};

struct physics_ball {
	int in_play; //is ball in play?
	int contact; //is spring in contact with player?
	double dx; //resting length of contact spring
	double x_mass; //position of ball
	double v_mass; //velocity of ball
	double k; //stiffness of contact spring
	double m; //mass of ball
	double Fs; //interaction force
	double dir; //current direction of travel 
	double game; //is player still playing?
};

struct gait_sim {
	int phase; // 1 = hold above xthresh, 2 = trajectory down, 3 = hold at Fthresh against spring, 4 = move up against spring 
	double x_thresh, f_thresh; //thresholds 
	double x_floor; //location of floor 
	double Fs; //interaction force
	double x_traj; //current position in trajectory 
	double v_traj; //velocity of trajectory
	double k_assist, k_floor, k_gravity; //stiffness for downward assistance, floor, upwards resistance
	struct timespec t_start; //start time of reaching threshold 
	struct timespec t_curr; //current time after reaching threshold
};



//FUNCTION DECLARATIONS ----------------------
void *controller(void * d);
void *server(void * d);
void *logger(void * d);
int init_daq();

bool init_thread(pthread_attr_t * attr, struct sched_param * param, int priority);
bool init_sock(struct socket_data * sock);
void init_log(char * filename);

void imp_struct_to_json(struct impStruct * imp, double k, double b, char * json, size_t len);
size_t imp_length_json(struct impStruct * imp, double k, double b);

void imp_PD(struct impStruct * imp);
void imp_Adm(struct impStruct * imp, double * xa, double * va, double *x_end);
void imp_Haptics(struct impStruct * imp);
void imp_StepTime(struct timespec * start_time, struct timespec * end_time, struct timespec * step_time  );
void imp_WaitTime(struct timespec * step_time, struct timespec * curr_time);
void imp_FIR(double * array, double * output, int * order);
void imp_Force(struct impStruct * imp);
void imp_traj(struct impStruct * imp, double * dir, double * xdes_old, double * x_end);

void matrix_square(double A[2][2], double C[2][2]);
void factorial(double n, double * ans);
void matrix_exp(double A[2][2], double B[2][2]);
void invert_matrix(double A[2][2], double B[2][2]);
void imp_calc_Bd(double Ad[2][2], double A[2][2], double B[2], double Bd[2]);

void imp_regex_match(regex_t * compiled, char recvBuff[1024], regmatch_t matches[2], 
	char matchBuffer[100],  struct regexMatch * regex, double * param_loc );

void imp_Haptics_impedance(struct impStruct * imp, struct physics_ball * ball, struct gait_sim * gait,  double * xa, double * va, double *fa, double * fk, double * fa_1, int * environment, double * x_end);
void imp_physics(struct impStruct * imp, struct physics_ball * ball, double * x_end);
void imp_gait(struct impStruct * imp, struct gait_sim * gait);