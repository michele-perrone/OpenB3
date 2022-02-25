/* setBfree - DSP tonewheel organ
 *
 * Copyright (C) 2003-2004 Fredrik Kilander <fk@dsv.su.se>
 * Copyright (C) 2008-2018 Robin Gareus <robin@gareus.org>
 * Copyright (C) 2012 Will Panther <pantherb@setbfree.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SCANNER_H
#define SCANNER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cfgParser.h"

#define VIB1 0x01
#define VIB2 0x02
#define VIB3 0x03
#define CHO_ 0x80 /* The bit that turns on chorus. */
#define CHO1 0x81
#define CHO2 0x82
#define CHO3 0x83

#define INCTBL_SIZE 2048

#define BUF_SIZE_BYTES 1024

struct b_vibrato {
	unsigned int offset1Table[INCTBL_SIZE];
	unsigned int offset2Table[INCTBL_SIZE];
	unsigned int offset3Table[INCTBL_SIZE];

	unsigned int* offsetTable;

	unsigned int stator;
	unsigned int statorIncrement;

	unsigned int outPos;

	float vibBuffer[BUF_SIZE_BYTES];

	/*
 * Amplitudes of phase shift for the three vibrato settings.
 */

	double vib1OffAmp;
	double vib2OffAmp;
	double vib3OffAmp;

	double vibFqHertz;

	int mixedBuffers;
    int effectEnabled;
};

extern void vibratoProc (struct b_vibrato* v, float const* inbuffer, float* outbuffer, size_t bufferLengthSamples);

/* for standalone use */
extern void reset_vibrato (struct b_vibrato* v);
extern void init_vibrato (struct b_vibrato* v);

/* tonegen integration */
extern void resetVibrato (void* tonegen);
extern void initVibrato (void* tonegen, void* m);

extern int scannerConfig (void* t, ConfigContext* cfg);
extern const ConfigDoc* scannerDoc ();

#ifdef __cplusplus
}
#endif

#endif /* SCANNER_H */
