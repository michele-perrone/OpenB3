/* setBfree - DSP tonewheel organ
 *
 * Copyright (C) 2013-2018 Robin Gareus <robin@gareus.org>
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

#ifndef GLOBAL_INST_H
#define GLOBAL_INST_H

#ifdef __cplusplus
extern "C" {
#endif

#include "midi.h"
#include "overdrive.h"
#include "program.h"
#include "reverb.h"
#include "../tonegen/tonegen.h"
#include "vibrato.h"
#include "whirl.h"

#include "cfgParser.h"
#include "main.h"
#include "midi.h"
#include "pgmParser.h"
#include "state.h"

typedef struct b_instance {
	struct b_reverb*    reverb;
	struct b_whirl*     whirl;
	struct b_tonegen*   synth;
	struct b_programme* progs;
	void*               midicfg;
	void*               preamp;
	void*               state;
} b_instance;

/* clang-format off */
#define LOCALEGUARD_START                                        \
        char* oldlocale = strdup (setlocale (LC_NUMERIC, NULL)); \
        if (strcmp (oldlocale, "C")) {                           \
                setlocale (LC_NUMERIC, "C");                     \
        } else {                                                 \
                free (oldlocale);                                \
                oldlocale = NULL;                                \
        }

#define LOCALEGUARD_END                            \
        if (oldlocale) {                           \
                setlocale (LC_NUMERIC, oldlocale); \
                free (oldlocale);                  \
        }
/* clang-format on */

#ifdef __cplusplus
}
#endif

#endif
