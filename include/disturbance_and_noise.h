/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: disturbance_and_noise.h
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

#ifndef RTW_HEADER_disturbance_and_noise_h_
#define RTW_HEADER_disturbance_and_noise_h_
#ifndef slow_COMMON_INCLUDES_
# define slow_COMMON_INCLUDES_
#include "rtwtypes.h"
#include "zero_crossing_types.h"
#endif                                 /* slow_COMMON_INCLUDES_ */

#include "condiviso.h"

/* Block signals and states (default storage) */
extern DW_slow_T slow_DW;

/*
 * Exported Global Parameters
 *
 * Note: Exported global parameters are tunable parameters with an exported
 * global storage class designation.  Code generation will declare the memory for
 * these parameters and exports their symbols.
 *
 */
extern par_dn_t par_dn;                /* Variable: par_dn
                                        * Referenced by:
                                        *   '<S2>/Disturbance_generator'
                                        *   '<S10>/Gain1'
                                        */

extern void disturbance_and_noise_Init(real32_T *rty_dist);
extern void disturbance_and_noise(uint8_T rtu_kick, real32_T *rty_dist, real32_T
  rty_noise[2]);

#endif                                 /* RTW_HEADER_disturbance_and_noise_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
