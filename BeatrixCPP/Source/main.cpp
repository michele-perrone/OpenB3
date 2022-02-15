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

#include "beatrix.hpp"

#define _XOPEN_SOURCE 700

#include <ctype.h>
#include <fcntl.h>
#include <getopt.h>
#include <limits.h>
#include <math.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/time.h>
#include <unistd.h>

/*
#include <math.h>
#include <samplerate.h>
*/

#ifndef _WIN32
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#else
#include <windows.h>
#endif

#include "main.h"

/*
#include "global_inst.h"
#include "../program/pgmParser.h"
#include "../program/program.h"
#include "../state/state.h"
#include "../vibrato/vibrato.h"
*/



#ifndef SHAREDIR
#define SHAREDIR "."
#endif

static const char* templateConfigFile    = SHAREDIR "/cfg/default.cfg";
static const char* templateProgrammeFile = SHAREDIR "/pgm/default.pgm";

static void
parse_preset (unsigned int* p, const char* c)
{
	unsigned int i;
	for (i = 0; i < strlen (c); ++i) {
		if (c[i] < '0' || c[i] > '9') {
			p[i] = 0;
		} else {
			p[i] = c[i] - '0';
		}
	}
	for (; i < 9; ++i) {
		p[i] = 0;
	}
}

int main()
{
    Beatrix beatrix(48000);

    return 0;
}
