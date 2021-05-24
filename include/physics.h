/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: physics.h
 *
 * Code generated for Simulink model 'fast'.
 *
 * Model version                  : 1.247
 * Simulink Coder version         : 9.2 (R2019b) 18-Jul-2019
 * C/C++ source code generated on : Fri Jan  3 22:18:41 2020
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: Intel->x86-64 (Linux 64)
 * Code generation objectives:
 *    1. Execution efficiency
 *    2. Debugging
 *    3. Traceability
 * Validation result: All passed
 */

#ifndef RTW_HEADER_physics_h_
#define RTW_HEADER_physics_h_
#include <string.h>
#include <math.h>
#ifndef fast_COMMON_INCLUDES_
# define fast_COMMON_INCLUDES_
#include "rtwtypes.h"
#endif                                 /* fast_COMMON_INCLUDES_ */

#include "condiviso.h"

/* Block signals and states (default storage) */
extern DW_fast_T fast_DW;

/*
 * Exported States
 *
 * Note: Exported states are block states with an exported global
 * storage class designation.  Code generation will declare the memory for these
 * states and exports their symbols.
 *
 */
extern dn_t dn;                        /* Simulink.Signal object 'dn' */
extern state_board_t state_board;     /* Simulink.Signal object 'state_board' */

extern void physics_Init(void);
extern void physics(uint16_T rtu_CCR, real32_T rtu_dist, const real32_T
                    rtu_noise[2], uint16_T rtyy_CNT_alphaCNT_theta[2]);

#endif                                 /* RTW_HEADER_physics_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
