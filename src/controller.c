/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: controller.c
 *
 * Code generated for Simulink model 'slow'.
 *
 * Model version                  : 1.273
 * Simulink Coder version         : 9.2 (R2019b) 18-Jul-2019
 * C/C++ source code generated on : Mon Jan  6 15:18:05 2020
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: Intel->x86-64 (Linux 64)
 * Code generation objectives:
 *    1. Execution efficiency
 *    2. Debugging
 *    3. Traceability
 * Validation result: All passed
 */

#include "controller.h"

/* Named constants for Chart: '<S1>/Hybrid_controller' */
#define slow_IN_Idle                   ((uint8_T)1U)
#define slow_IN_NO_ACTIVE_CHILD        ((uint8_T)0U)
#define slow_IN_PD_controller_down     ((uint8_T)1U)
#define slow_IN_PD_controller_up       ((uint8_T)2U)
#define slow_IN_Sliding_mode_controller ((uint8_T)3U)
#define slow_IN_Swing_up               ((uint8_T)2U)

/* Forward declaration for local functions */
static real32_T slow_energy(const real32_T state[4]);
static void slow_contr_pd(const real32_T *deg_to_rad, const real32_T
  TmpSignalConversionAtSFunctionI[4], real32_T *alpha_ref, real32_T *alpha,
  int8_T *reset, real32_T *DiscreteTransferFcn);
static void slow_contr_su(const real32_T TmpSignalConversionAtSFunctionI[4]);

/* System initialize for function-call system: '<S4>/ref_gen' */
void slow_ref_gen_Init(DW_ref_gen_slow_T *localDW)
{
  /* InitializeConditions for DiscreteTransferFcn: '<S7>/Discrete Transfer Fcn' */
  localDW->DiscreteTransferFcn_icLoad = 1U;
}

/* Output and update for function-call system: '<S4>/ref_gen' */
void slow_ref_gen(real32_T rtu_alpha_ref, real32_T rtu_alpha, int8_T rtu_reset,
                  real32_T *rty_out, DW_ref_gen_slow_T *localDW,
                  ZCE_ref_gen_slow_T *localZCE)
{
  real32_T DiscreteTransferFcn_tmp;

  /* DiscreteTransferFcn: '<S7>/Discrete Transfer Fcn' incorporates:
   *  MATLAB Function: '<S7>/initial_cond'
   */
  /* MATLAB Function 'Controller/Hybrid_controller/ref_gen/initial_cond': '<S8>:1' */
  /* '<S8>:1:3' x0 = 1/par_ctrl.dpole_ref*(alpha/(1-par_ctrl.dpole_ref)-alpha_ref); */
  if (rt_I32ZCFcn(FALLING_ZERO_CROSSING,&localZCE->DiscreteTransferFcn_Reset_ZCE,
                  (rtu_reset)) != NO_ZCEVENT) {
    localDW->DiscreteTransferFcn_icLoad = 1U;
  }

  if (localDW->DiscreteTransferFcn_icLoad != 0) {
    localDW->DiscreteTransferFcn_states = (rtu_alpha / (1.0F -
      par_ctrl.dpole_ref) - rtu_alpha_ref) * (1.0F / par_ctrl.dpole_ref);
  }

  DiscreteTransferFcn_tmp = (rtu_alpha_ref - par_ctrl.ref_gen_den[1] *
    localDW->DiscreteTransferFcn_states) / par_ctrl.ref_gen_den[0];
  *rty_out = par_ctrl.ref_gen_num[0] * DiscreteTransferFcn_tmp +
    par_ctrl.ref_gen_num[1] * localDW->DiscreteTransferFcn_states;

  /* End of DiscreteTransferFcn: '<S7>/Discrete Transfer Fcn' */

  /* Update for DiscreteTransferFcn: '<S7>/Discrete Transfer Fcn' */
  localDW->DiscreteTransferFcn_icLoad = 0U;
  localDW->DiscreteTransferFcn_states = DiscreteTransferFcn_tmp;
}

/*
 * Function for Chart: '<S1>/Hybrid_controller'
 * function ME = energy(state)
 */
static real32_T slow_energy(const real32_T state[4])
{
  real32_T x_tmp;
  real32_T ME_tmp;
  real32_T ME_tmp_0;

  /* MATLAB Function 'energy': '<S4>:86' */
  /*  Mechanical energy calculation (kinetic + potential) */
  /* '<S4>:86:5' th = state(2); */
  /* '<S4>:86:6' al_d = state(3); */
  /* '<S4>:86:7' th_d = state(4); */
  /* '<S4>:86:9' ME = (Jp*al_d^2)/2 + (Jp*th_d^2)/2 + (Larm^2*al_d^2*mp)/2 - g*lp*mp - (Jp*al_d^2*cos(th)^2)/2 + g*lp*mp*cos(th) + Larm*al_d*lp*mp*th_d*cos(th); */
  x_tmp = cosf(state[1]);
  ME_tmp = state[2] * state[2];
  ME_tmp_0 = ME_tmp * 0.00429067202F;
  return (((((ME_tmp_0 / 2.0F + state[3] * state[3] * 0.00429067202F / 2.0F) +
             ME_tmp * 0.046656F * 0.127F / 2.0F) - 0.194355741F) - ME_tmp_0 *
           (x_tmp * x_tmp) / 2.0F) + 0.194355741F * x_tmp) + 0.216F * state[2] *
    0.156F * 0.127F * state[3] * x_tmp;
}

/*
 * Function for Chart: '<S1>/Hybrid_controller'
 * function contr_pd
 */
static void slow_contr_pd(const real32_T *deg_to_rad, const real32_T
  TmpSignalConversionAtSFunctionI[4], real32_T *alpha_ref, real32_T *alpha,
  int8_T *reset, real32_T *DiscreteTransferFcn)
{
  /* MATLAB Function 'contr_pd': '<S4>:73' */
  /*  PD controller with reference generator */
  /* '<S4>:73:5' q_ref = [ref_gen(alpha_ref,q(1),int8(0)); theta_ref; 0; 0]; */
  /* Simulink Function 'ref_gen': '<S4>:153' */
  *alpha_ref = *deg_to_rad;
  *alpha = TmpSignalConversionAtSFunctionI[0];
  *reset = 0;

  /* Outputs for Function Call SubSystem: '<S4>/ref_gen' */
  slow_ref_gen(*alpha_ref, *alpha, *reset, DiscreteTransferFcn, &slow_DW.ref_gen,
               &slow_PrevZCX.ref_gen);

  /* End of Outputs for SubSystem: '<S4>/ref_gen' */

  /* '<S4>:73:6' error = q_ref - q; */
  /* '<S4>:73:7' volt = K * error; */
  slow_DW.volt = (((*DiscreteTransferFcn - TmpSignalConversionAtSFunctionI[0]) *
                   slow_DW.K[0] + (slow_DW.theta_ref -
    TmpSignalConversionAtSFunctionI[1]) * slow_DW.K[1]) + (0.0F -
    TmpSignalConversionAtSFunctionI[2]) * slow_DW.K[2]) + (0.0F -
    TmpSignalConversionAtSFunctionI[3]) * slow_DW.K[3];
}

/*
 * Function for Chart: '<S1>/Hybrid_controller'
 * function contr_su
 */
static void slow_contr_su(const real32_T TmpSignalConversionAtSFunctionI[4])
{
  real32_T sgn;

  /* MATLAB Function 'contr_su': '<S4>:71' */
  /*  Swing-up controller */
  /* '<S4>:71:5' th = q(2); */
  /* '<S4>:71:6' th_d = q(4); */
  /* '<S4>:71:8' if th_d*cos(th) == 0 */
  sgn = TmpSignalConversionAtSFunctionI[3] * cosf
    (TmpSignalConversionAtSFunctionI[1]);
  if (sgn == 0.0F) {
    /* '<S4>:71:9' sgn = sign(th); */
    if (TmpSignalConversionAtSFunctionI[1] < 0.0F) {
      sgn = -1.0F;
    } else if (TmpSignalConversionAtSFunctionI[1] > 0.0F) {
      sgn = 1.0F;
    } else {
      sgn = TmpSignalConversionAtSFunctionI[1];
    }
  } else {
    /* '<S4>:71:10' else */
    /* '<S4>:71:11' sgn = sign(th_d*cos(th)); */
    if (sgn < 0.0F) {
      sgn = -1.0F;
    } else {
      if (sgn > 0.0F) {
        sgn = 1.0F;
      }
    }
  }

  /* '<S4>:71:14' volt = -volt_max*sgn; */
  slow_DW.volt = -6.0F * sgn;
}

/* System initialize for atomic system: '<Root>/Controller' */
void controller_Init(void)
{
  int32_T i;

  /* InitializeConditions for Delay: '<S5>/Delay' */
  slow_DW.Delay_DSTATE[0] = 0.0F;
  slow_DW.Delay_DSTATE[1] = 3.14159274F;

  /* InitializeConditions for DiscreteStateSpace: '<S5>/vel_estim' */
  slow_DW.vel_estim_DSTATE[0] = (0.0F);
  slow_DW.vel_estim_DSTATE[1] = (17.3310871F);

  /* InitializeConditions for Delay: '<S1>/Delay' */
  for (i = 0; i < 255; i++) {
    slow_DW.Delay_DSTATE_n[i] = 2625U;
  }

  /* End of InitializeConditions for Delay: '<S1>/Delay' */
  slow_DW.theta_ref = 3.14159274F;

  /* SystemInitialize for Chart: '<S1>/Hybrid_controller' incorporates:
   *  SubSystem: '<S4>/ref_gen'
   */
  slow_ref_gen_Init(&slow_DW.ref_gen);
}

/* Output and update for atomic system: '<Root>/Controller' */
void controller(real32_T rtu_alpha_ref, uint8_T rtu_swingup, uint16_T
                rtu_CNT_alpha, uint16_T rtu_CNT_theta, uint8_T rtu_delay,
                real32_T *rty_theta_ref, real32_T rty_alpha[2], real32_T
                *rty_voltage, uint16_T *rty_CCR)
{
  /* local block i/o variables */
  real32_T rtb_vel_estim[2];
  real32_T rtb_pos_meas[2];
  int32_T b_k;
  boolean_T rEQ0;
  real32_T q;
  uint16_T rtb_CCR;
  real32_T deg_to_rad;
  real32_T TmpSignalConversionAtSFunctionI[4];
  real32_T alpha;
  int8_T reset;
  real32_T DiscreteTransferFcn;
  boolean_T guard1 = false;

  /* DataTypeConversion: '<S3>/Cast4' */
  rty_alpha[0] = rtu_CNT_alpha;
  rty_alpha[1] = rtu_CNT_theta;

  /* Gain: '<S3>/CNT_to_rad' */
  /* MATLAB Function 'Controller/State_estimator/pos_estim': '<S9>:1' */
  /* '<S9>:1:3' sect = @(th)mod(int8(floor(th/single(pi/2))),int8(4)); */
  /* '<S9>:1:5' pos_meas = floor(pos_old/single(2*pi))*single(2*pi) +... */
  /* '<S9>:1:6'            mod(rad,single(2*pi)); */
  /* '<S9>:1:8' for k=1:2 */
  rty_alpha[0] *= 0.00153398083F;

  /* MATLAB Function: '<S5>/pos_estim' incorporates:
   *  Delay: '<S5>/Delay'
   */
  rtb_pos_meas[0] = slow_DW.Delay_DSTATE[0] / 6.28318548F;
  rtb_pos_meas[0] = floorf(rtb_pos_meas[0]);
  if (rty_alpha[0] == 0.0F) {
    deg_to_rad = 0.0F;
  } else {
    deg_to_rad = fmodf(rty_alpha[0], 6.28318548F);
    rEQ0 = (deg_to_rad == 0.0F);
    if (!rEQ0) {
      q = fabsf(rty_alpha[0] / 6.28318548F);
      rEQ0 = (fabsf(q - floorf(q + 0.5F)) <= 1.1920929E-7F * q);
    }

    if (rEQ0) {
      deg_to_rad = 0.0F;
    } else {
      if (rty_alpha[0] < 0.0F) {
        deg_to_rad += 6.28318548F;
      }
    }
  }

  rtb_pos_meas[0] = rtb_pos_meas[0] * 6.28318548F + deg_to_rad;

  /* '<S9>:1:9' if sect(pos_old(k)) == int8(3) && sect(rad(k)) == int8(0) */
  deg_to_rad = floorf(slow_DW.Delay_DSTATE[0] / 1.57079637F);
  if (deg_to_rad < 128.0F) {
    if (deg_to_rad >= -128.0F) {
      reset = (int8_T)deg_to_rad;
    } else {
      reset = MIN_int8_T;
    }
  } else {
    reset = MAX_int8_T;
  }

  guard1 = false;
  if ((int8_T)(reset - ((reset >> 2) << 2)) == 3) {
    q = floorf(rty_alpha[0] / 1.57079637F);
    if (q < 128.0F) {
      if (q >= -128.0F) {
        reset = (int8_T)q;
      } else {
        reset = MIN_int8_T;
      }
    } else {
      reset = MAX_int8_T;
    }

    if ((int8_T)(reset - ((reset >> 2) << 2)) == 0) {
      /* '<S9>:1:10' pos_meas(k) = pos_meas(k) + single(2*pi); */
      rtb_pos_meas[0] += 6.28318548F;
    } else {
      guard1 = true;
    }
  } else {
    guard1 = true;
  }

  if (guard1) {
    if (deg_to_rad < 128.0F) {
      if (deg_to_rad >= -128.0F) {
        reset = (int8_T)deg_to_rad;
      } else {
        reset = MIN_int8_T;
      }
    } else {
      reset = MAX_int8_T;
    }

    if ((int8_T)(reset - ((reset >> 2) << 2)) == 0) {
      deg_to_rad = floorf(rty_alpha[0] / 1.57079637F);
      if (deg_to_rad < 128.0F) {
        if (deg_to_rad >= -128.0F) {
          reset = (int8_T)deg_to_rad;
        } else {
          reset = MIN_int8_T;
        }
      } else {
        reset = MAX_int8_T;
      }

      if ((int8_T)(reset - ((reset >> 2) << 2)) == 3) {
        /* '<S9>:1:11' elseif sect(pos_old(k)) == int8(0) && sect(rad(k)) == int8(3) */
        /* '<S9>:1:12' pos_meas(k) = pos_meas(k) - single(2*pi); */
        rtb_pos_meas[0] -= 6.28318548F;
      }
    }
  }

  /* Gain: '<S3>/CNT_to_rad' */
  rty_alpha[1] *= 0.00153398083F;

  /* MATLAB Function: '<S5>/pos_estim' incorporates:
   *  Delay: '<S5>/Delay'
   */
  rtb_pos_meas[1] = slow_DW.Delay_DSTATE[1] / 6.28318548F;
  rtb_pos_meas[1] = floorf(rtb_pos_meas[1]);
  if (rty_alpha[1] == 0.0F) {
    deg_to_rad = 0.0F;
  } else {
    deg_to_rad = fmodf(rty_alpha[1], 6.28318548F);
    rEQ0 = (deg_to_rad == 0.0F);
    if (!rEQ0) {
      q = fabsf(rty_alpha[1] / 6.28318548F);
      rEQ0 = (fabsf(q - floorf(q + 0.5F)) <= 1.1920929E-7F * q);
    }

    if (rEQ0) {
      deg_to_rad = 0.0F;
    } else {
      if (rty_alpha[1] < 0.0F) {
        deg_to_rad += 6.28318548F;
      }
    }
  }

  rtb_pos_meas[1] = rtb_pos_meas[1] * 6.28318548F + deg_to_rad;

  /* '<S9>:1:9' if sect(pos_old(k)) == int8(3) && sect(rad(k)) == int8(0) */
  deg_to_rad = floorf(slow_DW.Delay_DSTATE[1] / 1.57079637F);
  if (deg_to_rad < 128.0F) {
    if (deg_to_rad >= -128.0F) {
      reset = (int8_T)deg_to_rad;
    } else {
      reset = MIN_int8_T;
    }
  } else {
    reset = MAX_int8_T;
  }

  guard1 = false;
  if ((int8_T)(reset - ((reset >> 2) << 2)) == 3) {
    q = floorf(rty_alpha[1] / 1.57079637F);
    if (q < 128.0F) {
      if (q >= -128.0F) {
        reset = (int8_T)q;
      } else {
        reset = MIN_int8_T;
      }
    } else {
      reset = MAX_int8_T;
    }

    if ((int8_T)(reset - ((reset >> 2) << 2)) == 0) {
      /* '<S9>:1:10' pos_meas(k) = pos_meas(k) + single(2*pi); */
      rtb_pos_meas[1] += 6.28318548F;
    } else {
      guard1 = true;
    }
  } else {
    guard1 = true;
  }

  if (guard1) {
    if (deg_to_rad < 128.0F) {
      if (deg_to_rad >= -128.0F) {
        reset = (int8_T)deg_to_rad;
      } else {
        reset = MIN_int8_T;
      }
    } else {
      reset = MAX_int8_T;
    }

    if ((int8_T)(reset - ((reset >> 2) << 2)) == 0) {
      deg_to_rad = floorf(rty_alpha[1] / 1.57079637F);
      if (deg_to_rad < 128.0F) {
        if (deg_to_rad >= -128.0F) {
          reset = (int8_T)deg_to_rad;
        } else {
          reset = MIN_int8_T;
        }
      } else {
        reset = MAX_int8_T;
      }

      if ((int8_T)(reset - ((reset >> 2) << 2)) == 3) {
        /* '<S9>:1:11' elseif sect(pos_old(k)) == int8(0) && sect(rad(k)) == int8(3) */
        /* '<S9>:1:12' pos_meas(k) = pos_meas(k) - single(2*pi); */
        rtb_pos_meas[1] -= 6.28318548F;
      }
    }
  }

  /* DiscreteStateSpace: '<S5>/vel_estim' */
  {
    rtb_vel_estim[0] = (-6.5717063F)*slow_DW.vel_estim_DSTATE[0];
    rtb_vel_estim[0] += (36.2538452F)*rtb_pos_meas[0];
    rtb_vel_estim[1] = (-6.5717063F)*slow_DW.vel_estim_DSTATE[1];
    rtb_vel_estim[1] += (36.2538452F)*rtb_pos_meas[1];
  }

  /* Gain: '<S1>/deg_to_rad' */
  deg_to_rad = 0.0174532924F * rtu_alpha_ref;

  /* Gain: '<S1>/rad_to_deg1' */
  rty_alpha[0] = 57.2957802F * rtb_pos_meas[0];

  /* SignalConversion generated from: '<S4>/ SFunction ' incorporates:
   *  Chart: '<S1>/Hybrid_controller'
   */
  TmpSignalConversionAtSFunctionI[0] = rtb_pos_meas[0];
  TmpSignalConversionAtSFunctionI[2] = rtb_vel_estim[0];

  /* Gain: '<S1>/rad_to_deg1' */
  rty_alpha[1] = 57.2957802F * rtb_pos_meas[1];

  /* SignalConversion generated from: '<S4>/ SFunction ' incorporates:
   *  Chart: '<S1>/Hybrid_controller'
   */
  TmpSignalConversionAtSFunctionI[1] = rtb_pos_meas[1];
  TmpSignalConversionAtSFunctionI[3] = rtb_vel_estim[1];

  /* Chart: '<S1>/Hybrid_controller' incorporates:
   *  SignalConversion generated from: '<S4>/ SFunction '
   */
  /* Gateway: Controller/Hybrid_controller */
  /* During: Controller/Hybrid_controller */
  if (slow_DW.is_active_c5_slow == 0U) {
    /* Entry: Controller/Hybrid_controller */
    slow_DW.is_active_c5_slow = 1U;

    /* Entry Internal: Controller/Hybrid_controller */
    /* Transition: '<S4>:142' */
    slow_DW.is_c5_slow = slow_IN_Sliding_mode_controller;

    /* Entry 'Sliding_mode_controller': '<S4>:138' */
    /* Entry Internal 'Sliding_mode_controller': '<S4>:138' */
    /* Transition: '<S4>:109' */
    slow_DW.is_Sliding_mode_controller = slow_IN_Idle;

    /* Entry 'Idle': '<S4>:28' */
  } else {
    switch (slow_DW.is_c5_slow) {
     case slow_IN_PD_controller_down:
      /* During 'PD_controller_down': '<S4>:4' */
      /* '<S4>:7:1' sf_internal_predicateOutput = swingup == 1; */
      if (rtu_swingup == 1) {
        /* Transition: '<S4>:7' */
        slow_DW.is_c5_slow = slow_IN_Sliding_mode_controller;

        /* Entry 'Sliding_mode_controller': '<S4>:138' */
        /* Entry Internal 'Sliding_mode_controller': '<S4>:138' */
        /* Transition: '<S4>:109' */
        slow_DW.is_Sliding_mode_controller = slow_IN_Idle;

        /* Entry 'Idle': '<S4>:28' */
      } else {
        /* '<S4>:4:5' theta_ref = (round(q(2)/single(2*pi)+0.5)-0.5)*single(2*pi); */
        slow_DW.theta_ref = (roundf(rtb_pos_meas[1] / 6.28318548F + 0.5F) - 0.5F)
          * 6.28318548F;

        /* '<S4>:4:6' K = [par_ctrl.down_kp_alpha,... */
        /* '<S4>:4:7'     single(0),... */
        /* '<S4>:4:8'     par_ctrl.down_kd_alpha,... */
        /* '<S4>:4:9'     single(0)]; */
        slow_DW.K[0] = par_ctrl.down_kp_alpha;
        slow_DW.K[1] = 0.0F;
        slow_DW.K[2] = par_ctrl.down_kd_alpha;
        slow_DW.K[3] = 0.0F;

        /* '<S4>:4:10' contr_pd; */
        slow_contr_pd(&deg_to_rad, TmpSignalConversionAtSFunctionI, &q, &alpha,
                      &reset, &DiscreteTransferFcn);
      }
      break;

     case slow_IN_PD_controller_up:
      /* During 'PD_controller_up': '<S4>:5' */
      /* '<S4>:16:1' sf_internal_predicateOutput = swingup == 0 ||... */
      /* '<S4>:16:2' abs(q(2)-round(q(2)/(2*pi))*2*pi) > th_lim; */
      if ((rtu_swingup == 0) || (fabsf(rtb_pos_meas[1] - roundf(rtb_pos_meas[1] /
             6.28318548F) * 2.0F * 3.14159274F) > 0.52359879F)) {
        /* Transition: '<S4>:16' */
        slow_DW.is_c5_slow = slow_IN_Sliding_mode_controller;

        /* Entry 'Sliding_mode_controller': '<S4>:138' */
        /* Entry Internal 'Sliding_mode_controller': '<S4>:138' */
        /* Transition: '<S4>:109' */
        slow_DW.is_Sliding_mode_controller = slow_IN_Idle;

        /* Entry 'Idle': '<S4>:28' */
      } else {
        /* '<S4>:5:6' K = [par_ctrl.up_kp_alpha,... */
        /* '<S4>:5:7'     par_ctrl.up_kp_theta,... */
        /* '<S4>:5:8'     par_ctrl.up_kd_alpha,... */
        /* '<S4>:5:9'     par_ctrl.up_kd_theta]; */
        slow_DW.K[0] = par_ctrl.up_kp_alpha;
        slow_DW.K[1] = par_ctrl.up_kp_theta;
        slow_DW.K[2] = par_ctrl.up_kd_alpha;
        slow_DW.K[3] = par_ctrl.up_kd_theta;

        /* '<S4>:5:10' contr_pd; */
        slow_contr_pd(&deg_to_rad, TmpSignalConversionAtSFunctionI, &q, &alpha,
                      &reset, &DiscreteTransferFcn);
      }
      break;

     default:
      /* During 'Sliding_mode_controller': '<S4>:138' */
      /* '<S4>:96:1' sf_internal_predicateOutput = swingup == 1 &&... */
      /* '<S4>:96:2' abs(q(2)-round(q(2)/(2*pi))*2*pi) <= th_lim; */
      q = roundf(rtb_pos_meas[1] / 6.28318548F);
      if ((rtu_swingup == 1) && (fabsf(rtb_pos_meas[1] - q * 2.0F * 3.14159274F)
           <= 0.52359879F)) {
        /* Transition: '<S4>:96' */
        /* Exit Internal 'Sliding_mode_controller': '<S4>:138' */
        slow_DW.is_Sliding_mode_controller = slow_IN_NO_ACTIVE_CHILD;
        slow_DW.is_c5_slow = slow_IN_PD_controller_up;

        /* Entry 'PD_controller_up': '<S4>:5' */
        /* '<S4>:5:3' ref_gen(alpha_ref,q(1),int8(1)); */
        /* Simulink Function 'ref_gen': '<S4>:153' */
        q = deg_to_rad;
        alpha = rtb_pos_meas[0];
        reset = 1;

        /* Outputs for Function Call SubSystem: '<S4>/ref_gen' */
        slow_ref_gen(deg_to_rad, rtb_pos_meas[0], 1, &DiscreteTransferFcn,
                     &slow_DW.ref_gen, &slow_PrevZCX.ref_gen);

        /* End of Outputs for SubSystem: '<S4>/ref_gen' */
        /* '<S4>:5:4' theta_ref = round(q(2)/single(2*pi))*single(2*pi); */
        slow_DW.theta_ref = roundf(rtb_pos_meas[1] / 6.28318548F) * 6.28318548F;

        /* '<S4>:5:6' K = [par_ctrl.up_kp_alpha,... */
        /* '<S4>:5:7'     par_ctrl.up_kp_theta,... */
        /* '<S4>:5:8'     par_ctrl.up_kd_alpha,... */
        /* '<S4>:5:9'     par_ctrl.up_kd_theta]; */
        slow_DW.K[0] = par_ctrl.up_kp_alpha;
        slow_DW.K[1] = par_ctrl.up_kp_theta;
        slow_DW.K[2] = par_ctrl.up_kd_alpha;
        slow_DW.K[3] = par_ctrl.up_kd_theta;

        /* '<S4>:5:10' contr_pd; */
        slow_contr_pd(&deg_to_rad, TmpSignalConversionAtSFunctionI, &q, &alpha,
                      &reset, &DiscreteTransferFcn);
      } else {
        /* '<S4>:29:1' sf_internal_predicateOutput = swingup == 0; */
        if (rtu_swingup == 0) {
          /* Transition: '<S4>:29' */
          /* Exit Internal 'Sliding_mode_controller': '<S4>:138' */
          slow_DW.is_Sliding_mode_controller = slow_IN_NO_ACTIVE_CHILD;
          slow_DW.is_c5_slow = slow_IN_PD_controller_down;

          /* Entry 'PD_controller_down': '<S4>:4' */
          /* '<S4>:4:3' ref_gen(alpha_ref,q(1),int8(1)); */
          /* Simulink Function 'ref_gen': '<S4>:153' */
          q = deg_to_rad;
          alpha = rtb_pos_meas[0];
          reset = 1;

          /* Outputs for Function Call SubSystem: '<S4>/ref_gen' */
          slow_ref_gen(deg_to_rad, rtb_pos_meas[0], 1, &DiscreteTransferFcn,
                       &slow_DW.ref_gen, &slow_PrevZCX.ref_gen);

          /* End of Outputs for SubSystem: '<S4>/ref_gen' */
          /* '<S4>:4:5' theta_ref = (round(q(2)/single(2*pi)+0.5)-0.5)*single(2*pi); */
          slow_DW.theta_ref = (roundf(rtb_pos_meas[1] / 6.28318548F + 0.5F) -
                               0.5F) * 6.28318548F;

          /* '<S4>:4:6' K = [par_ctrl.down_kp_alpha,... */
          /* '<S4>:4:7'     single(0),... */
          /* '<S4>:4:8'     par_ctrl.down_kd_alpha,... */
          /* '<S4>:4:9'     single(0)]; */
          slow_DW.K[0] = par_ctrl.down_kp_alpha;
          slow_DW.K[1] = 0.0F;
          slow_DW.K[2] = par_ctrl.down_kd_alpha;
          slow_DW.K[3] = 0.0F;

          /* '<S4>:4:10' contr_pd; */
          slow_contr_pd(&deg_to_rad, TmpSignalConversionAtSFunctionI, &q, &alpha,
                        &reset, &DiscreteTransferFcn);
        } else {
          /* '<S4>:138:3' theta_ref = round(q(2)/single(2*pi))*single(2*pi); */
          slow_DW.theta_ref = q * 6.28318548F;
          if (slow_DW.is_Sliding_mode_controller == slow_IN_Idle) {
            /* During 'Idle': '<S4>:28' */
            /* '<S4>:95:1' sf_internal_predicateOutput = energy(q) < 0; */
            if (slow_energy(TmpSignalConversionAtSFunctionI) < 0.0F) {
              /* Transition: '<S4>:95' */
              slow_DW.is_Sliding_mode_controller = slow_IN_Swing_up;

              /* Entry 'Swing_up': '<S4>:1' */
              /* '<S4>:1:3' contr_su; */
              slow_contr_su(TmpSignalConversionAtSFunctionI);
            } else {
              /* '<S4>:28:3' volt = 0; */
              slow_DW.volt = 0.0F;
            }
          } else {
            /* During 'Swing_up': '<S4>:1' */
            /* '<S4>:17:1' sf_internal_predicateOutput = energy(q) >= 0; */
            if (slow_energy(TmpSignalConversionAtSFunctionI) >= 0.0F) {
              /* Transition: '<S4>:17' */
              slow_DW.is_Sliding_mode_controller = slow_IN_Idle;

              /* Entry 'Idle': '<S4>:28' */
            } else {
              /* '<S4>:1:3' contr_su; */
              slow_contr_su(TmpSignalConversionAtSFunctionI);
            }
          }
        }
      }
      break;
    }
  }

  /* Gain: '<S1>/rad_to_deg2' */
  *rty_theta_ref = 57.2957802F * slow_DW.theta_ref;

  /* Saturate: '<S1>/Saturation' */
  if (slow_DW.volt > 6.0F) {
    *rty_voltage = 6.0F;
  } else if (slow_DW.volt < -6.0F) {
    *rty_voltage = -6.0F;
  } else {
    *rty_voltage = slow_DW.volt;
  }

  /* End of Saturate: '<S1>/Saturation' */

  /* DataTypeConversion: '<S6>/Cast' incorporates:
   *  Fcn: '<S6>/Fcn'
   */
  rtb_CCR = (uint16_T)roundf((*rty_voltage + 6.0F) / 12.0F * 5250.0F);

  /* Delay: '<S1>/Delay' */
  if (rtu_delay <= 0) {
    *rty_CCR = rtb_CCR;
  } else {
    *rty_CCR = slow_DW.Delay_DSTATE_n[(uint8_T)(255U - rtu_delay)];
  }

  /* End of Delay: '<S1>/Delay' */

  /* Update for Delay: '<S5>/Delay' */
  slow_DW.Delay_DSTATE[0] = rtb_pos_meas[0];
  slow_DW.Delay_DSTATE[1] = rtb_pos_meas[1];

  /* Update for DiscreteStateSpace: '<S5>/vel_estim' */
  {
    real32_T xnew[2];
    xnew[0] = (0.818730772F)*slow_DW.vel_estim_DSTATE[0];
    xnew[0] += (1.0F)*rtb_pos_meas[0];
    xnew[1] = (0.818730772F)*slow_DW.vel_estim_DSTATE[1];
    xnew[1] += (1.0F)*rtb_pos_meas[1];
    (void) memcpy(&slow_DW.vel_estim_DSTATE[0], xnew,
                  sizeof(real32_T)*2);
  }

  /* Update for Delay: '<S1>/Delay' */
  for (b_k = 0; b_k < 254; b_k++) {
    slow_DW.Delay_DSTATE_n[b_k] = slow_DW.Delay_DSTATE_n[b_k + 1];
  }

  slow_DW.Delay_DSTATE_n[254] = rtb_CCR;

  /* End of Update for Delay: '<S1>/Delay' */
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
