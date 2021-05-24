#include "simulation.h"


//----------- Variabili extern 
extern int 					end;
extern state_pc_t			state_pc;
extern pthread_mutex_t		mux_state_pc;
extern ref_t				ref_pc;
extern pthread_mutex_t		mux_ref_pc;
extern par_ctrl_t 			par_control_pc;
extern pthread_mutex_t		mux_parcontr_pc;
extern int 					dl_miss_control;
extern int 					dl_miss_state_update;
extern int 					dl_miss_comboard;
extern int 					dl_miss_compc;
extern dn_t                 dn;
extern pthread_mutex_t		mux_dn;
extern par_dn_t             par_dn;
extern unsigned int         period_control;
#ifdef EXTIME
extern int ex_time[6];
extern int wc_extime[6];
extern struct timespec monotime_i[6], monotime_f[6];
extern int ex_cnt[6];
extern long ex_sum[6];
#endif

//---------------- Variabili Board
state_board_t state_board=
	{CNT_ALPHA_0,
	CNT_THETA_0,
	CCR_0,
	};
pthread_mutex_t 	mux_state_board = PTHREAD_MUTEX_INITIALIZER;
state_board_t 	state_board_reset =
	{CNT_ALPHA_0,
    CNT_THETA_0,
    CCR_0,
	};
par_ctrl_t par_control_board =
{KP_UP_ALPHA_DEF,
	KP_UP_THETA_DEF,
	KD_UP_ALPHA_DEF,
	KD_UP_THETA_DEF,
	KP_DOWN_ALPHA_DEF,
	KD_DOWN_ALPHA_DEF,
	DPOLE_REF_DEF,
	REF_GEN_NUM_DEF,
	REF_GEN_DEN_DEF}; // parametri per il controllo scheda
pthread_mutex_t 	mux_parcontr_board = PTHREAD_MUTEX_INITIALIZER;		// mutual exclusion per par_control
ref_t ref_board = {ALPHA_REF, THETA_REF, SWINGUP_DEF};		// struct riferimento 
pthread_mutex_t 	mux_ref_board = PTHREAD_MUTEX_INITIALIZER;			// mutual exclusion per ref

//---------- Variabili Buffer
state_pc_t state_buffer=
{	ALPHA_0,
	THETA_0,
	VOLTAGE_0
}; 	
pthread_mutex_t 	mux_state_buffer = PTHREAD_MUTEX_INITIALIZER;
par_ctrl_t par_control_buffer =
{KP_UP_ALPHA_DEF,
	KP_UP_THETA_DEF,
	KD_UP_ALPHA_DEF,
	KD_UP_THETA_DEF,
	KP_DOWN_ALPHA_DEF,
	KD_DOWN_ALPHA_DEF,
	DPOLE_REF_DEF,
	REF_GEN_NUM_DEF,
	REF_GEN_DEN_DEF};
pthread_mutex_t 	mux_parcontr_buffer = PTHREAD_MUTEX_INITIALIZER;	// mutual exclusion per par_control
ref_t ref_buffer = {ALPHA_REF, THETA_REF, SWINGUP_DEF};					// struct riferimento 
pthread_mutex_t 	mux_ref_buffer = PTHREAD_MUTEX_INITIALIZER;			// mutual exclusion per ref

//----------- Variabili locali
ref_t ref= {ALPHA_REF, THETA_REF, SWINGUP_DEF};
real32_T rtb_rad_to_deg1[2];	// variabile locale che condiene il valore di alpha e theta in gradi.
real32_T voltage;
par_ctrl_t par_ctrl;
dn_t dn_su;		// dn locale nel task state_update
dn_t dn_ctrl;	// dn locale nel task control

DW_fast_T fast_DW;
DW_slow_T slow_DW;
PrevZCX_slow_T slow_PrevZCX;

//----------- state_update
void* state_update(void* arg){
    cpu_set(2);
	int id;							// task index
	id = get_task_index(arg);		// retrieve the task index
	set_activation(id);
	
	// init
	physics_Init();
	uint16_T rtb_Cast2[2];
	uint16_T CCR_local;
	
	while(!end){
#ifdef EXTIME
        start_extime(0, PERIOD_STATE_UPDATE);
#endif
		pthread_mutex_lock(&mux_ref_board);
			CCR_local = state_board.CCR;
		pthread_mutex_unlock(&mux_ref_board);
		pthread_mutex_lock(&mux_dn);
			dn_su = dn;
		pthread_mutex_unlock(&mux_dn);
		
		physics(CCR_local, dn_su.dist, dn_su.noise, rtb_Cast2);
		
		pthread_mutex_lock(&mux_ref_board);
			state_board.CNT_alpha = rtb_Cast2[0];
			state_board.CNT_theta = rtb_Cast2[1];
		pthread_mutex_unlock(&mux_ref_board);
		
		// end task
		if(deadline_miss(id)){
			dl_miss_state_update+=1;
		}
#ifdef EXTIME
        stop_extime(0);
#endif
		wait_for_period(id);		// wait to next period
	}
	return 0;
}

//---------- control
void* control(void* arg){
    cpu_set(1);
	int id;							// task index
	id = get_task_index(arg);		// retrieve the task index
	set_activation(id);
	
	// init
	par_ctrl = par_control_board;
	controller_Init();
	disturbance_and_noise_Init(&slow_DW.dist);
	
	while(!end){
#ifdef EXTIME
        start_extime(1, period_control);
#endif
		pthread_mutex_lock(&mux_parcontr_board);
			par_ctrl = par_control_board;
		pthread_mutex_unlock(&mux_parcontr_board);
		pthread_mutex_lock(&mux_ref_board);
			ref.alpha = ref_board.alpha;
            ref.swingup = ref_board.swingup;
            ref_board.theta = ref.theta;
		pthread_mutex_unlock(&mux_ref_board);
		pthread_mutex_lock(&mux_dn);
			dn_ctrl = dn;
		pthread_mutex_unlock(&mux_dn);

		controller(ref.alpha, ref.swingup, state_board.CNT_alpha,
             state_board.CNT_theta, dn_ctrl.delay, &ref.theta, rtb_rad_to_deg1,
             &voltage, &state_board.CCR);
		
		disturbance_and_noise(dn_ctrl.kick, &slow_DW.dist, dn_ctrl.noise);
		dn_ctrl.dist = slow_DW.dist;
		
		pthread_mutex_lock(&mux_dn);
			dn= dn_ctrl;
		pthread_mutex_unlock(&mux_dn);
		
		// end task
		if(deadline_miss(id)){
			dl_miss_control+=1;
		}
#ifdef EXTIME
        stop_extime(1);
#endif
		wait_for_period(id);		// wait to next period
	}
	
	return 0;
}

// task di comunicazione pc, scrive ref_pc e par_control_pc su buffer, legge state da buffer
void* compc(void* arg){
    cpu_set(0);
	int id;							// task index
	id = get_task_index(arg);		// retrieve the task index
	set_activation(id);
	
	while(!end){
#ifdef EXTIME
        start_extime(2, PERIOD_COMPC);
#endif
		// scrittura di ref_pc su buffer
		pthread_mutex_lock(&mux_ref_buffer);
			pthread_mutex_lock(&mux_ref_pc);
				ref_buffer.alpha = ref_pc.alpha;
                ref_buffer.swingup = ref_pc.swingup;
                ref_pc.theta = ref_buffer.theta;
			pthread_mutex_unlock(&mux_ref_pc);
		pthread_mutex_unlock(&mux_ref_buffer);
		
		// scrittura di par_control_pc su buffer
		pthread_mutex_lock(&mux_parcontr_buffer);
			pthread_mutex_lock(&mux_parcontr_pc);
				par_control_buffer = par_control_pc;
			pthread_mutex_unlock(&mux_parcontr_pc);
		pthread_mutex_unlock(&mux_parcontr_buffer);
		
		// lettura di state_buffer e scrittura state_pc
		pthread_mutex_lock(&mux_state_buffer);
			pthread_mutex_lock(&mux_state_pc);
				state_pc = state_buffer;
			pthread_mutex_unlock(&mux_state_pc);
		pthread_mutex_unlock(&mux_state_buffer);
		
		
		// end task
		if(deadline_miss(id)){
			dl_miss_compc+=1;
		}
#ifdef EXTIME
        stop_extime(2);
#endif
		wait_for_period(id);		// wait to next period
	}
	return 0;
}

// task di comunicazione scheda, scrive state su buffer, legge par_control e ref da buffer
void* comboard(void* arg){
    cpu_set(1);
	int id;							// task index
	id = get_task_index(arg);		// retrieve the task index
	set_activation(id);
	
	
	while(!end){
#ifdef EXTIME
        start_extime(3, PERIOD_COMBOARD);
#endif
		// lettura di ref da buffer, scrittura di ref_board
		pthread_mutex_lock(&mux_ref_buffer);
			pthread_mutex_lock(&mux_ref_board);
				ref_board.alpha = ref_buffer.alpha;
				ref_board.swingup = ref_buffer.swingup;
                ref_buffer.theta = ref_board.theta;
			pthread_mutex_unlock(&mux_ref_board);
		pthread_mutex_unlock(&mux_ref_buffer);
		
		// lettura di par_control da buffer, scrittura di par_control_pc
		pthread_mutex_lock(&mux_parcontr_buffer);
			pthread_mutex_lock(&mux_parcontr_board);
				par_control_board = par_control_buffer;
			pthread_mutex_unlock(&mux_parcontr_board);
		pthread_mutex_unlock(&mux_parcontr_buffer);
		
		// scrittura di state_board su buffer
		pthread_mutex_lock(&mux_state_buffer);
			pthread_mutex_lock(&mux_state_board);
				state_buffer.alpha = rtb_rad_to_deg1[0];
				state_buffer.theta = rtb_rad_to_deg1[1];
				state_buffer.voltage = voltage;
			pthread_mutex_unlock(&mux_state_board);
		pthread_mutex_unlock(&mux_state_buffer);
		
		// end task
		if(deadline_miss(id)){
			dl_miss_comboard+=1;
		}
#ifdef EXTIME
        stop_extime(3);
#endif
		wait_for_period(id);		// wait to next period

	}
	return 0;
}

