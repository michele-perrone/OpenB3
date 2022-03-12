#include "global_definitions.h"

double SampleRateD = 0; // Initialized to some value for Apple CLang compatibility.
                        // Actual sample rate assigned by BeatrixCPP's constructor

unsigned int defaultPresetUpperManual[9] = { 8, 8, 6, 0, 0, 0, 0, 0, 0 };
unsigned int defaultPresetLowerManual[9] = { 8, 8, 8, 8, 0, 0, 0, 0, 8 };
unsigned int defaultPresetPedalBoard[9] =  { 8, 0, 0, 0, 0, 0, 0, 0, 0 };
