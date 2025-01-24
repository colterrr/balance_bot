/*
 * File: lqrrr.c
 *
 * MATLAB Coder version            : 5.5
 * C/C++ source code generated on  : 24-Sep-2024 12:28:05
 */

/* Include Files */
#include "lqrrr.h"

/* Function Definitions */
/*
 * Arguments    : const double states[6]
 *                double L0
 *                double refx
 *                double *T
 *                double *Tp
 * Return Type  : void
 */
void lqrrr(const double states[6], double L0, double refx, double *T,
           double *Tp)
{
  double y[12];
  double b_states[6];
  double out[2];
  int i;
  int i1;
  y[0] = -(L0 * (L0 * (L0 * 1508.9 - 247.22) + 8.323) - 2.9908);
  y[2] = -(L0 * (L0 * (L0 * 6819.9 - 1383.2) + 91.521) - 5.7974);
  y[4] = -(L0 * (L0 * (L0 * -6515.5 + 1844.8) - 204.46) - 3.5744);
  y[6] = -(L0 * (L0 * (L0 * -1499.2 + 421.61) - 44.938) - 1.6246);
  y[8] = -(L0 * (L0 * (L0 * -1166.6 + 1397.9) - 296.36) + 22.147);
  y[10] = -(L0 * (L0 * (L0 * 117.71 + 134.46) - 36.706) + 2.9581);
  y[1] = -(L0 * (L0 * (L0 * 8435.0 - 1469.4) + 51.367) + 1.9748);
  y[3] = -(L0 * (L0 * (L0 * 2882.3 - 114.77) - 64.835) + 5.8285);
  y[5] = -(L0 * (L0 * (L0 * 29447.0 - 5505.9) + 237.05) + 5.9892);
  y[7] = -(L0 * (L0 * (L0 * 894.72 + 126.07) - 63.552) + 5.6087);
  y[9] = -(L0 * (L0 * (L0 * 23468.0 - 6627.4) + 661.5) + 20.296);
  y[11] = -(L0 * (L0 * (L0 * 3096.5 - 877.66) + 88.014) + 3.0456);
  b_states[0] = states[0] - refx;
  b_states[1] = states[1];
  b_states[2] = states[2];
  b_states[3] = states[3];
  b_states[4] = states[4];
  b_states[5] = states[5];
  for (i = 0; i < 2; i++) {
    double d;
    d = 0.0;
    for (i1 = 0; i1 < 6; i1++) {
      d += y[i + (i1 << 1)] * b_states[i1];
    }
    out[i] = d;
  }
  *T = out[0];
  *Tp = out[1];
}

/*
 * File trailer for lqrrr.c
 *
 * [EOF]
 */
