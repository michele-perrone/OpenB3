/* This is a class that acts as a wrapper for the Beatrix C engine.
 * The goal is to make the usage of Beatric much simpler.
 */

#include <stdlib.h>
#include <string.h>

#include "global_inst.h"
#include "global_definitions.h"

struct Beatrix
{
    b_instance inst;

    float bufA[BUFFER_SIZE_SAMPLES];
    float bufB[BUFFER_SIZE_SAMPLES];
    float bufC[BUFFER_SIZE_SAMPLES];
    float bufD[2][BUFFER_SIZE_SAMPLES]; // drum, tmp.
    float bufL[2][BUFFER_SIZE_SAMPLES]; // leslie, out
    float bufJ[2][BUFFER_SIZE_SAMPLES];

    char* defaultConfigFile    = NULL;
    char* defaultProgrammeFile = NULL;

    double SampleRateD = 48000.0;
    int    SampleRateI = 48000;

    unsigned int defaultPreset[9] = { 8, 8, 6, 0, 0, 0, 0, 0, 0 };

    Beatrix(double sample_rate)
    {
        SampleRateI = (int)sample_rate;
        SampleRateD = sample_rate;

        memset (&inst, 0, sizeof (b_instance));

        alloc_all();

        init_all();
    }
    ~Beatrix()
    {
        free(defaultConfigFile);
        free(defaultProgrammeFile);

        freeReverb (inst.reverb);
        freeWhirl (inst.whirl);

        freeToneGenerator (inst.synth);
        freeMidiCfg (inst.midicfg);
        freePreamp (inst.preamp);
        freeProgs (inst.progs);
        freeRunningConfig (inst.state);

        fprintf (stderr, "bye\n");
    }
    void alloc_all()
    {
        inst.state = allocRunningConfig();
        inst.progs = allocProgs();
        inst.reverb = allocReverb();
        inst.whirl = allocWhirl();
        inst.synth = allocTonegen();
        inst.midicfg = allocMidiCfg(inst.state);
        inst.preamp = allocPreamp();
    }
    void init_all()
    {
        fprintf (stderr, "Oscillators : ");
        fflush (stderr);
        initToneGenerator (inst.synth, inst.midicfg);

        fprintf (stderr, "Scanner : ");
        fflush (stderr);
        initVibrato (inst.synth, inst.midicfg);

        fprintf (stderr, "Overdrive : ");
        fflush (stderr);
        initPreamp (inst.preamp, inst.midicfg);

        fprintf (stderr, "Reverb : ");
        fflush (stderr);
        initReverb (inst.reverb, inst.midicfg, SampleRateD);

        fprintf (stderr, "Whirl : ");
        fflush (stderr);
        initWhirl (inst.whirl, inst.midicfg, SampleRateD);

        fprintf (stderr, "RC : ");
        fflush (stderr);
        initRunningConfig (inst.state, inst.midicfg);

        fprintf (stderr, "Midi config : ");
        fflush (stderr);
        initMidiTables (inst.midicfg);

        fprintf (stderr, "Drawbars : ");
        fflush (stderr);
        setDrawBars (&inst, 0, defaultPreset);
        setDrawBars (&inst, 1, defaultPreset);
        setDrawBars (&inst, 2, defaultPreset);

        fprintf (stderr, "..done.\n");
        fflush (stderr);
    }
    void get_next_block(float* buffer_L, float* buffer_R, size_t n_channels, size_t nframes)
    {
        int boffset = BUFFER_SIZE_SAMPLES;

        size_t written = 0;

        while (written < nframes)
        {
            int nremain = nframes - written;

            if (boffset >= BUFFER_SIZE_SAMPLES)
            {
                boffset = 0;
                oscGenerateFragment (inst.synth, bufA, BUFFER_SIZE_SAMPLES);
                preamp (inst.preamp, bufA, bufB, BUFFER_SIZE_SAMPLES);
                reverb (inst.reverb, bufB, bufC, BUFFER_SIZE_SAMPLES);
                whirlProc3 (inst.whirl, bufC, bufL[0], bufL[1], bufD[0], bufD[1], BUFFER_SIZE_SAMPLES);
            }

            int nread = MIN (nremain, (BUFFER_SIZE_SAMPLES - boffset));

            memcpy (&buffer_L[written], &bufL[0][boffset], nread * sizeof (float));
            memcpy (&buffer_R[written], &bufL[0][boffset], nread * sizeof (float));

            written += nread;
            boffset += nread;
        }
    }
    /* Keys are numbered as such:
     *   0-- 63, upper manual (  0-- 60 in use)
     *  64--127, lower manual ( 64--124 in use)
     * 128--160, pedal        (128--159 in use)
     */
    void note_on(uint8_t midi_note)
    {
        oscKeyOn (this->inst.synth, midi_note-36, midi_note-36);
    }
    void note_off(uint8_t midi_note)
    {
        oscKeyOff (this->inst.synth, midi_note-36, midi_note-36);
    }
    void set_drawbars(unsigned int manual, unsigned int setting[])
    {
        setDrawBars (&this->inst, manual, setting);
    }
};
