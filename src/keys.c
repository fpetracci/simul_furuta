#include "keys.h"

// Variabili extern
extern ref_t				ref_pc;
extern pthread_mutex_t 		mux_ref_pc;
extern state_pc_t 			state_pc;
extern state_board_t 		state_board;
extern state_board_t 		state_board_reset;
extern pthread_mutex_t 		mux_state_pc;
extern pthread_mutex_t 		mux_state_board;
extern view_t 				view;
extern pthread_mutex_t 		mux_view;
extern par_ctrl_t 			par_control_pc;
extern par_ctrl_t 			par_control_reset;
extern pthread_mutex_t 		mux_parcontr_pc;
extern dn_t dn;
extern pthread_mutex_t		mux_dn;
extern par_dn_t par_dn;
extern int dl_miss_keys;
extern int end;
extern float pole_ref;
extern unsigned int period_control;
#ifdef EXTIME
extern int ex_time[6];
extern struct timespec monotime_i[6], monotime_f[6];
extern int ex_cnt[6];
extern long ex_sum[6];
#endif

// variabili usate da keys
char ascii, scan;

//------- task keys
void* keys(void* arg){
    cpu_set(0);
	int id;							// task index
	id = get_task_index(arg);		// retrieve the task index
	set_activation(id);
	
	
	while(!end){
#ifdef EXTIME
        start_extime(5, PERIOD_KEYS);
#endif
		if (keypressed()) {
			get_keycodes(&scan, &ascii);
			key_action();
            key_par_control();
		} else {
			//reset kick
			pthread_mutex_lock(&mux_dn);
				dn.kick = 0;
			pthread_mutex_unlock(&mux_dn);
	} // end keypressed

	// end of task
	if(deadline_miss(id)){
			dl_miss_keys+=1;
	}
#ifdef EXTIME
        stop_extime(5);
#endif
	wait_for_period(id);	// wait to next period
	
	}
	return 0;
}

// key_action, secondo il tasto premuto compie un'azione
void key_action(){

    // ESC, end Threads e conseguente chiusura del programma
    if (scan == KEY_ESC) {
        end = 1;
    }

    // 1, reset state_pc e ref_pc
    if (scan == KEY_1) {
        pthread_mutex_lock(&mux_ref_pc);
        ref_pc.alpha = ALPHA_0;
        ref_pc.theta = THETA_0;
        pthread_mutex_unlock(&mux_ref_pc);
    }

    // W, kick
    if (scan == KEY_D){
        pthread_mutex_lock(&mux_dn);
        dn.kick = 1;
        pthread_mutex_unlock(&mux_dn);
    }

    // Q, regola swingup
    if (scan == KEY_Q){
        pthread_mutex_lock(&mux_ref_pc);
        if(ref_pc.swingup){
            ref_pc.swingup = 0;
        }else{
            ref_pc.swingup = 1;
        }
        pthread_mutex_unlock(&mux_ref_pc);
    }

    /*
        * 	DELAY
        */
    if(scan == KEY_W){
        pthread_mutex_lock(&mux_dn);
        dn.delay += 1;
        pthread_mutex_unlock(&mux_dn);
    }
    if(scan == KEY_E && dn.delay > 0){
        pthread_mutex_lock(&mux_dn);
        dn.delay += -1;
        pthread_mutex_unlock(&mux_dn);
    }
    if(scan == KEY_5){
        pthread_mutex_lock(&mux_dn);
        dn.delay = 0;
        pthread_mutex_unlock(&mux_dn);
    }

    /*
    *	RIFERIMENTO
    */
    // A, incrementa riferimento di alpha
    if (scan==KEY_A) {
        pthread_mutex_lock(&mux_ref_pc);
        ref_pc.alpha += INCR_ANG;
        pthread_mutex_unlock(&mux_ref_pc);
    }
    // S, diminuisce riferimento di alpha
    if (scan==KEY_S) {
        pthread_mutex_lock(&mux_ref_pc);
        ref_pc.alpha += -INCR_ANG;
        pthread_mutex_unlock(&mux_ref_pc);
    }

    // 9, incrementa il periodo del task di controllo
    if(scan == KEY_9) {
        period_control += 1;
        set_period(ID_CONTROL, period_control);
    }
    // 0, diminuisce il periodo del task di controllo
    if(scan == KEY_0 && period_control > 1) {
        period_control -= 1;
        set_period(ID_CONTROL, period_control);
    }

    /*
    *      VIEW
    */
    // reset vista
    if (scan==KEY_2) {
        pthread_mutex_lock(&mux_view);
        view.lon = LON_0;
        view.lat = LAT_0;
        pthread_mutex_unlock(&mux_view);
    }

    //VIEW_LAT up down
    if (scan==KEY_UP) {
        pthread_mutex_lock(&mux_view);
        if (view.lat + INCR_ANG <= LAT_MAX) {
            view.lat += INCR_ANG;
        }
        pthread_mutex_unlock(&mux_view);
    }
    if (scan==KEY_DOWN) {
        pthread_mutex_lock(&mux_view);
        if (view.lat + -INCR_ANG >= -LAT_MAX) {
            view.lat += -INCR_ANG;
        }
        pthread_mutex_unlock(&mux_view);
    }
    // VIEW_LON left right
    if (scan==KEY_LEFT) {
        pthread_mutex_lock(&mux_view);
        view.lon += -INCR_ANG;
        pthread_mutex_unlock(&mux_view);
    }
    if (scan==KEY_RIGHT) {
        pthread_mutex_lock(&mux_view);
        view.lon += +INCR_ANG;
        pthread_mutex_unlock(&mux_view);
    }
}

// key_par_control, parte dei tasti che vanno a modificare i parametri del controllore
void key_par_control(){
	/*
	* Par_control
	*/
	// controllore UP
	// alpha kp
	if(scan == KEY_R){
		pthread_mutex_lock(&mux_parcontr_pc);
			par_control_pc.up_kp_alpha += INCR_K;
		pthread_mutex_unlock(&mux_parcontr_pc);
	}
	if(scan == KEY_T){
		pthread_mutex_lock(&mux_parcontr_pc);
			par_control_pc.up_kp_alpha += -INCR_K;
		pthread_mutex_unlock(&mux_parcontr_pc);
	}
	// alpha kd
	if(scan == KEY_Y){
		pthread_mutex_lock(&mux_parcontr_pc);
			par_control_pc.up_kd_alpha += INCR_K;
		pthread_mutex_unlock(&mux_parcontr_pc);
	}
	if(scan == KEY_U){
		pthread_mutex_lock(&mux_parcontr_pc);
			par_control_pc.up_kd_alpha += -INCR_K;
		pthread_mutex_unlock(&mux_parcontr_pc);
	}
	
	// theta kp
	if(scan == KEY_H){
		pthread_mutex_lock(&mux_parcontr_pc);
			par_control_pc.up_kp_theta += INCR_K;
		pthread_mutex_unlock(&mux_parcontr_pc);
	}
	if(scan == KEY_J){
		pthread_mutex_lock(&mux_parcontr_pc);
			par_control_pc.up_kp_theta += -INCR_K;
		pthread_mutex_unlock(&mux_parcontr_pc);
	}
	// theta kd
	if(scan == KEY_K){
		pthread_mutex_lock(&mux_parcontr_pc);
			par_control_pc.up_kd_theta += INCR_K;
		pthread_mutex_unlock(&mux_parcontr_pc);
	}
	if(scan == KEY_L){
		pthread_mutex_lock(&mux_parcontr_pc);
			par_control_pc.up_kd_theta += -INCR_K;
		pthread_mutex_unlock(&mux_parcontr_pc);
	}
	
	// controllore DOWN alpha
	// down_kp_alpha
	if(scan == KEY_V){
		pthread_mutex_lock(&mux_parcontr_pc);
			par_control_pc.down_kp_alpha += INCR_K;
		pthread_mutex_unlock(&mux_parcontr_pc);
	}
	if(scan == KEY_B){
		pthread_mutex_lock(&mux_parcontr_pc);
			par_control_pc.down_kp_alpha += -INCR_K;
		pthread_mutex_unlock(&mux_parcontr_pc);
	}
	
	// down_kd_alpha
	if(scan == KEY_N){
		pthread_mutex_lock(&mux_parcontr_pc);
			par_control_pc.down_kd_alpha += INCR_K;
		pthread_mutex_unlock(&mux_parcontr_pc);
	}
	if(scan == KEY_M){
		pthread_mutex_lock(&mux_parcontr_pc);
			par_control_pc.down_kd_alpha += -INCR_K;
		pthread_mutex_unlock(&mux_parcontr_pc);
	}
	
	// 3, reset par_control_pc
	if(scan == KEY_3){
		pole_ref = POLE_REF_DEF;
		pthread_mutex_lock(&mux_parcontr_pc);
			par_control_pc = par_control_reset;
		pthread_mutex_unlock(&mux_parcontr_pc);
	}
	
	// ref_gen_online
	if(scan == KEY_I){
		pole_ref += INCR_K;
		pthread_mutex_lock(&mux_parcontr_pc);
			par_control_pc.dpole_ref = expf(- pole_ref * 0.005F);
			par_control_pc.ref_gen_num[0] = 1.0F - par_control_pc.dpole_ref;
			par_control_pc.ref_gen_num[1] = 0.0F;
			par_control_pc.ref_gen_den[0] = 1.0F;
			par_control_pc.ref_gen_den[1] = - par_control_pc.dpole_ref;
		pthread_mutex_unlock(&mux_parcontr_pc);
		
	}
	if(scan == KEY_O && pole_ref > 0.5F){
		pole_ref += -INCR_K;
		pthread_mutex_lock(&mux_parcontr_pc);
			par_control_pc.dpole_ref = expf(- pole_ref * 0.005F);
			par_control_pc.ref_gen_num[0] = 1.0F - par_control_pc.dpole_ref;
			par_control_pc.ref_gen_num[1] = 0.0F;
			par_control_pc.ref_gen_den[0] = 1.0F;
			par_control_pc.ref_gen_den[1] = - par_control_pc.dpole_ref;
		pthread_mutex_unlock(&mux_parcontr_pc);
		
	}

	/*
	*	par_dn
	*/
	if(scan == KEY_F){
		par_dn.dist_amp += INCR_K/2;
	}
	if(scan == KEY_G){
		par_dn.dist_amp += -INCR_K/2;
	}
	if(scan == KEY_X){
		par_dn.noise_amp += 1;
	}
	if(scan == KEY_C && par_dn.noise_amp > 0){
		par_dn.noise_amp += -1;
	}
	if(scan == KEY_4){
		par_dn.dist_amp = DIST_AMP_DEF;
		par_dn.noise_amp = NOISE_AMP_DEF;
		
		pthread_mutex_lock(&mux_dn);
		dn.delay = 0;
		pthread_mutex_unlock(&mux_dn);
	}
	

	
}

// get_keycodes
void get_keycodes(char *scan, char *ascii)
{
    // The following function waits for a key pressed and extracts
    // the corresponding ASCII code and scan code:
    int k;
    k = readkey(); // block until a key is pressed
    *ascii = k; // get ASCII code
    *scan = k >> 8; // get scan code
}
