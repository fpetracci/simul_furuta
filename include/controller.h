/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: controller.h
 *
 * Code generated for Simulink model 'slow'.
 *
 * Model version                  : 1.265
 * Simulink Coder version         : 9.2 (R2019b) 18-Jul-2019
 * C/C++ source code generated on : Fri Jan  3 22:32:25 2020
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: Intel->x86-64 (Linux 64)
 * Code generation objectives:
 *    1. Execution efficiency
 *    2. Debugging
 *    3. Traceability
 * Validation result: All passed
 */

#ifndef RTW_HEADER_controller_h_
#define RTW_HEADER_controller_h_
#include <string.h>
#include <math.h>
#ifndef slow_COMMON_INCLUDES_
# define slow_COMMON_INCLUDES_
#include "rtwtypes.h"
#include "zero_crossing_types.h"
#endif                                 /* slow_COMMON_INCLUDES_ */

#include "condiviso.h"
#include "rt_i32zcfcn.h"

/* Block signals and states (default storage) */
extern DW_slow_T slow_DW;

/* Zero-crossing (trigger) state */
extern PrevZCX_slow_T slow_PrevZCX;

/*
 * Exported Global Parameters
 *
 * Note: Exported global parameters are tunable parameters with an exported
 * global storage class designation.  Code generation will declare the memory for
 * these parameters and exports their symbols.
 *
 */
extern par_ctrl_t par_ctrl;            /* Variable: par_ctrl
                                        * Referenced by:
                                        *   '<S1>/Hybrid_controller'
                                        *   '<S7>/Discrete Transfer Fcn'
                                        */

extern void slow_ref_gen_Init(DW_ref_gen_slow_T *localDW);
extern void slow_ref_gen(real32_T rtu_alpha_ref, real32_T rtu_alpha, int8_T
  rtu_reset, real32_T *rty_out, DW_ref_gen_slow_T *localDW, ZCE_ref_gen_slow_T
  *localZCE);
extern void controller_Init(void);
extern void controller(real32_T rtu_alpha_ref, uint8_T rtu_swingup, uint16_T
  rtu_CNT_alpha, uint16_T rtu_CNT_theta, uint8_T rtu_delay, real32_T
  *rty_theta_ref, real32_T rty_alpha[2], real32_T *rty_voltage, uint16_T
  *rty_CCR);

#endif                                 /* RTW_HEADER_controller_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
