cmake_minimum_required(VERSION 3.21)

project(BeatrixCPP LANGUAGES CXX C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(Source)
include_directories(Source/tonegen)
include_directories(Source/config)
include_directories(Source/program)
include_directories(Source/state)
include_directories(Source/midi)
#include_directories(Source/memstream)

#include_directories(Source/convolution)
include_directories(Source/overdrive)
include_directories(Source/reverb)
include_directories(Source/whirl)
include_directories(Source/vibrato)

add_executable(BeatrixCPP
#    Source/convolution/convolution.h
#    Source/convolution/convolution.cc

    Source/overdrive/overdrive.h
    Source/overdrive/overdrive.c
    Source/overdrive/filterTools.h
    Source/overdrive/filterTools.c

    Source/reverb/reverb.h
    Source/reverb/reverb.c

    Source/whirl/whirl.h
    Source/whirl/whirl.c
    Source/whirl/eqcomp.h
    Source/whirl/eqcomp.c

    Source/vibrato/vibrato.h
    Source/vibrato/vibrato.c    

    Source/config/cfgParser.h
    Source/config/cfgParser.c

    Source/program/defaultpgm.h
    Source/program/pgmParser.h
    Source/program/pgmParser.c
    Source/program/program.h
    Source/program/program.c

    Source/midi/midi.h
    Source/midi/midi.c
    Source/midi/midi_aseq.h
    Source/midi/midi_aseq.c
    Source/midi/midi_types.h
    Source/midi/midnam.c

    Source/state/state.h
    Source/state/state.c

#    Source/memstream/memstream.h
#    Source/memstream/memstream.c

    Source/tonegen/tonegen.h
    Source/tonegen/tonegen.c

    Source/global_inst.h
    Source/global_definitions.h
    Source/global_definitions.c

    Source/beatrix.hpp

    Source/main.h
    Source/main.cpp
    )

IF (NOT WIN32)
  target_link_libraries(BeatrixCPP m)
ENDIF()
