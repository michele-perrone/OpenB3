#ifndef GLOBAL_DEFINITIONS_H
#define GLOBAL_DEFINITIONS_H

#ifdef __cplusplus
extern "C" {
#endif

#define MIN(A, B) (((A) < (B)) ? (A) : (B))

#define UPPER_MANUAL 0
#define LOWER_MANUAL 1
#define PEDAL_BOARD  2

extern double SampleRateD;

#ifdef __cplusplus
}
#endif

#endif
