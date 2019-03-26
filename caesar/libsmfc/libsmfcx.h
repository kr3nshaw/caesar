/**
 * libsmfcx.h: libsmfc extra functions by loveemu
 */


#ifndef LIBSMFCX_H
#define LIBSMFCX_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "libsmfc.h"

#define SMF_CONTROL_BANKSELM        0
#define SMF_CONTROL_MODULATION      1
#define SMF_CONTROL_PORTAMENTOTIME  5
#define SMF_CONTROL_DATAENTRYM      6
#define SMF_CONTROL_VOLUME          7
#define SMF_CONTROL_PANPOT          10
#define SMF_CONTROL_EXPRESSION      11
#define SMF_CONTROL_BANKSELL        32
#define SMF_CONTROL_DATAENTRYL      38
#define SMF_CONTROL_PORTAMENTO      65
#define SMF_CONTROL_PORTAMENTOCTRL  84
#define SMF_CONTROL_TIMBRE          71
#define SMF_CONTROL_RELEASETIME     72
#define SMF_CONTROL_ATTACKTIME      73
#define SMF_CONTROL_BRIGHTNESS      74
#define SMF_CONTROL_DECAYTIME       75
#define SMF_CONTROL_VIBRATORATE     76
#define SMF_CONTROL_VIBRATODEPTH    77
#define SMF_CONTROL_VIBRATODELAY    78
#define SMF_CONTROL_REVERB          91
#define SMF_CONTROL_CHORUS          93
#define SMF_CONTROL_NRPNL           98
#define SMF_CONTROL_NRPNM           99
#define SMF_CONTROL_RPNL            100
#define SMF_CONTROL_RPNM            101
#define SMF_CONTROL_MONO            126
#define SMF_CONTROL_POLY            127

#define SMF_META_TEXT               0x01
#define SMF_META_COPYRIGHT          0x02
#define SMF_META_TRACKNAME          0x03
#define SMF_META_SEQUENCENAME       0x03
#define SMF_META_SETTEMPO           0x51

bool smfWriteFile(Smf* seq, const char* filename);
bool smfInsertNoteOff(Smf* seq, int time, int channel, int track, int key, int velocity);
bool smfInsertNoteOn(Smf* seq, int time, int channel, int track, int key, int velocity);
bool smfInsertNote(Smf* seq, int time, int channel, int track, int key, int velocity, int duration);
bool smfInsertKeyPress(Smf* seq, int time, int channel, int track, int key, int amount);
bool smfInsertControl(Smf* seq, int time, int channel, int track, int controlNumber, int value);
bool smfInsertProgram(Smf* seq, int time, int channel, int track, int programNumber);
bool smfInsertChanPress(Smf* seq, int time, int channel, int track, int key, int amount);
bool smfInsertPitchBend(Smf* seq, int time, int channel, int track, int value);
bool smfInsertSysex(Smf* seq, int time, int port, int track, const byte* data, size_t dataSize);
bool smfInsertMetaEvent(Smf* seq, int time, int track, int metaType, const byte* data, size_t dataSize);
bool smfInsertMetaText(Smf* seq, int time, int track, int metaType, const char* text);

bool smfInsertGM1SystemOn(Smf* seq, int time, int port, int track);
bool smfInsertMasterVolume(Smf* seq, int time, int port, int track, int volume);
bool smfInsertTempo(Smf* seq, int time, int track, int microSeconds);
bool smfInsertTempoBPM(Smf* seq, int time, int track, double bpm);

#ifdef __cplusplus
}
#endif

#endif /* !LIBSMFCX_H */
