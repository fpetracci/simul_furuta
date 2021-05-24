#ifndef CONDIVISO_H
	#define CONDIVISO_H

#include <stdbool.h>
#include <pthread.h>
#include "rtwtypes.h" //conversioni matlab
#include "zero_crossing_types.h"


#define EXTIME //opzione di debug per leggere info aggiuntive sui task


//----------- costanti, definizioni
// stato, _0 iniziale 
#define ALPHA_0				0
#define THETA_0				180
#define VOLTAGE_0			0
#define CCR_0				2625 // meta` del range 0-5250
#define CNT_ALPHA_0			0
#define CNT_THETA_0			0

// view
#define LON_0				45		// longitudine iniziale
#define LAT_0				35		// latitudine iniziale
#define LAT_MAX				90
#define INCR_ANG			5

// gui
#define GRAPHICS            GFX_AUTODETECT_WINDOWED
#define WSCREEN				1024	// larghezza finestra
#define HSCREEN				768		// altezza finestra
#define COLOR_DEPTH			32		// numero colori
#define DIST				20		// unita` di misura per distanziare oggetti all'interno della finestra
#define WRETT1				400		//
#define L1_ASSON			133		// lunghezza link 1 vista assonometrica
#define L2_ASSON			100		// lunghezza link 2 vista assonometrica
#define L1_ALTO		    	100		// lunghezza link 1 vista dall'alto
#define L2_LATO				100		// lunghezza link 2 vista di lato
#define LX_GRID				180		// lunghezza lungo x griglia nella vista assonometrica
#define LY_GRID				200		// lunghezza lungo y griglia nella vista assonometrica
#define NUM_GRID			5		// numero di linee da disegnare nella griglia
#define NUM_POINTS			800		// numero di punti in cui calcolare il disegno dei cerchi
#define THICK				1		// spessore linee spesse
#define COL_BCK				makecol(190, 190, 190) 	// colore background
#define COL_RETT			makecol(0, 0, 179)		// colore rettangoli
#define COL_SCR				makecol(0, 0, 0)		// colore scritte
#define COL_MDL				makecol(0, 153, 0)  	// colore link
#define COL_MDL2			makecol(255, 0, 0)  	// colore link 2
#define COL_VERT            makecol(0,0,0)			// colore base (sezione verticale)
#define COL_RIF				makecol(128, 128, 128)	// colore riferimenti

// par_control	
#define KP_UP_ALPHA_DEF 	-2.23606801F 	// Valori di default delle costanti per i controllori
#define KP_UP_THETA_DEF 	-20.0269184F 	// UP = controllore per pendolo su, DOWN = controllore per pendolo giu`
#define KD_UP_ALPHA_DEF 	-2.08667636F 	// P = proporzionale, D = derivativo
#define KD_UP_THETA_DEF 	-2.67355F
#define KP_DOWN_ALPHA_DEF	0.9F
#define KD_DOWN_ALPHA_DEF	0.0F
#define POLE_REF_DEF		2.0F
#define DPOLE_REF_DEF		expf(- POLE_REF_DEF * 0.005F)	// Generatore di riferimenti
#define REF_GEN_NUM_DEF		{ 1.0F - DPOLE_REF_DEF, 0.0F }
#define REF_GEN_DEN_DEF		{ 1.0F, - DPOLE_REF_DEF }
#define INCR_K				0.5 // incremento da tastiera per i parametri del controllore

// dn
#define KICK_DEF			0U
#define DIST_DEF			0.0F
#define NOISE_DEF			{ 0.0F, 0.0F }
#define DELAY_DEF			0U

// par_dn
#define DIST_AMP_DEF		1
#define NOISE_AMP_DEF		0

//ref
#define ALPHA_REF			0
#define THETA_REF			0
#define SWINGUP_DEF			0

//----------- tasks, in ordine di priorita`
// state_update
#define ID_STATE_UPDATE		10	// id task
#define PRIO_STATE_UPDATE	99	// priorita` task, 0-99
#define PERIOD_STATE_UPDATE	1	// periodo task in ms

// control
#define ID_CONTROL			300
#define PRIO_CONTROL		90
#define PERIOD_CONTROL		5

// compc
#define ID_COMPC			400
#define PRIO_COMPC			30
#define PERIOD_COMPC		5

// comboard
#define ID_COMBOARD			500
#define PRIO_COMBOARD		30
#define PERIOD_COMBOARD		5

// keys
#define ID_KEYS				200
#define PRIO_KEYS			20
#define PERIOD_KEYS			20

// gui
#define ID_GUI				100
#define PRIO_GUI			10
#define FPS					25 //period 40

//----------- parametri scheda
#define CCR_MAX				5250 //PWM CCR 0-5250, 0 corrisponde a -6 Volt, 5250 a +6 Volt
#define CNT_MAX				4096 //ENC CNT 0-4096, 0 e 4096 corrispondono a  0 e 2*pi (posizione up per theta)

	
//----------- types, definizioni

typedef struct
{
    uint16_T CNT_alpha;
    uint16_T CNT_theta;
    uint16_T CCR;
} state_board_t; // struttura dello stato lato scheda


typedef struct {
    real32_T alpha;
    real32_T theta;
    real32_T voltage;
} state_pc_t; // struttura dello stato lato pc

typedef struct {
    real32_T up_kp_alpha;
    real32_T up_kp_theta;
    real32_T up_kd_alpha;
    real32_T up_kd_theta;
    real32_T down_kp_alpha;
    real32_T down_kd_alpha;
    real32_T dpole_ref;
    real32_T ref_gen_num[2];
    real32_T ref_gen_den[2];
} par_ctrl_t; // struttura per i parametri dei controllori

typedef struct {
    uint8_T kick;
    real32_T dist;
    real32_T noise[2];
    uint8_T delay;
} dn_t; // struttura per disturb and noise

typedef struct {
    real32_T dist_amp;
    uint16_T noise_amp;
} par_dn_t; // struttura per i parametri del disturbo e del rumore

typedef struct {
    real32_T alpha;
    real32_T theta;
    uint8_T swingup;
} ref_t; // struttura per riferimenti

typedef struct {
    int lon;
    int lat;
} view_t; // struttura per la vista assonometrica

//------ structure generate da Matlab
/* Block signals and states (default storage) for system '<Root>' */
typedef struct {
    real32_T Delay_DSTATE[4];            /* '<S6>/Delay' */
    real32_T DiscreteTimeIntegrator_DSTATE[4];/* '<S6>/Discrete-Time Integrator' */
    real32_T Delay_DSTATE_j;             /* '<S3>/Delay' */
    real32_T Delay_DSTATE_c;             /* '<S2>/Delay' */
    real32_T DiscreteStateSpace_DSTATE;  /* '<S2>/Discrete State-Space' */
} DW_fast_T;

/* Block signals and states (default storage) for system '<S4>/ref_gen' */
typedef struct {
    real32_T DiscreteTransferFcn_states; /* '<S7>/Discrete Transfer Fcn' */
    uint8_T DiscreteTransferFcn_icLoad;  /* '<S7>/Discrete Transfer Fcn' */
} DW_ref_gen_slow_T;

/* Zero-crossing (trigger) state for system '<S4>/ref_gen' */
typedef struct {
    ZCSigState DiscreteTransferFcn_Reset_ZCE;/* '<S7>/Discrete Transfer Fcn' */
} ZCE_ref_gen_slow_T;

/* Block signals and states (default storage) for system '<Root>' */
typedef struct {
    DW_ref_gen_slow_T ref_gen;           /* '<S4>/ref_gen' */
    real_T Noise_generator_NextOutput;   /* '<S10>/Noise_generator' */
    real_T Noise_generator1_NextOutput;  /* '<S10>/Noise_generator1' */
    real32_T Delay_DSTATE[2];            /* '<S5>/Delay' */
    real32_T vel_estim_DSTATE[2];        /* '<S5>/vel_estim' */
    real32_T K[4];                       /* '<S1>/Hybrid_controller' */
    real32_T dist;                       /* '<S2>/Disturbance_generator' */
    real32_T volt;                       /* '<S1>/Hybrid_controller' */
    real32_T theta_ref;                  /* '<S1>/Hybrid_controller' */
    uint32_T RandSeed;                   /* '<S10>/Noise_generator' */
    uint32_T RandSeed_b;                 /* '<S10>/Noise_generator1' */
    uint16_T Delay_DSTATE_n[255];        /* '<S1>/Delay' */
    uint8_T is_active_c7_slow;           /* '<S2>/Disturbance_generator' */
    uint8_T is_c7_slow;                  /* '<S2>/Disturbance_generator' */
    uint8_T temporalCounter_i1;          /* '<S2>/Disturbance_generator' */
    uint8_T is_active_c5_slow;           /* '<S1>/Hybrid_controller' */
    uint8_T is_c5_slow;                  /* '<S1>/Hybrid_controller' */
    uint8_T is_Sliding_mode_controller;  /* '<S1>/Hybrid_controller' */
} DW_slow_T;

/* Zero-crossing (trigger) state */
typedef struct {
    ZCE_ref_gen_slow_T ref_gen;          /* '<S4>/ref_gen' */
} PrevZCX_slow_T;

#endif //CONDIVISO_H
