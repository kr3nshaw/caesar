/**
 * libsmfcx.c: libsmfc extra functions by loveemu
 */


#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include "libsmfc.h"
#include "libsmfcx.h"

#define SMF_EVENT_NOTEOFF       0x80
#define SMF_EVENT_NOTEON        0x90
#define SMF_EVENT_KEYPRESS      0xa0
#define SMF_EVENT_CONTROL       0xb0
#define SMF_EVENT_PROGRAM       0xc0
#define SMF_EVENT_CHANPRESS     0xd0
#define SMF_EVENT_PITCHBEND     0xe0
#define SMF_EVENT_SYSEX         0xf0
#define SMF_EVENT_SYSEXLITE     0xf7
#define SMF_EVENT_META          0xff

bool smfWriteFile(Smf* seq, const char* filename)
{
  bool result = false;
  size_t seqSize = smfGetSize(seq);
  FILE* fileWriter = fopen(filename, "wb");

  if(fileWriter)
  {
    byte* buffer = (byte*) malloc(seqSize);

    if(buffer)
    {
      smfWrite(seq, buffer, seqSize);
      result = (bool) fwrite(buffer, seqSize, 1, fileWriter);
      free(buffer);
    }
    fclose(fileWriter);
  }
  return result;
}

bool smfInsertNoteOff(Smf* seq, int time, int channel, int track, int key, int velocity)
{
  bool result = false;

  if((key >= 0) && (key <= 127) && (velocity >= 0) && (velocity <= 127))
  {
    byte noteOff[3];

    noteOff[0] = SMF_EVENT_NOTEOFF | (channel % 16);
    noteOff[1] = key;
    noteOff[2] = velocity;
    result = smfInsertEvent(seq, time, channel / 16, track, noteOff, sizeof(noteOff));
  }
  return result;
}

bool smfInsertNoteOn(Smf* seq, int time, int channel, int track, int key, int velocity)
{
  bool result = false;

  if((key >= 0) && (key <= 127) && (velocity >= 0) && (velocity <= 127))
  {
    byte noteOn[3];

    noteOn[0] = SMF_EVENT_NOTEON | (channel % 16);
    noteOn[1] = key;
    noteOn[2] = velocity;
    result = smfInsertEvent(seq, time, channel / 16, track, noteOn, sizeof(noteOn));
  }
  return result;
}

bool smfInsertNote(Smf* seq, int time, int channel, int track, int key, int velocity, int duration)
{
  bool result = false;

  if(velocity > 0)
  {
    result = smfInsertNoteOn(seq, time, channel, track, key, velocity) 
      && smfInsertNoteOn(seq, time + duration, channel, track, key, 0);
  }
  return result;
}

bool smfInsertKeyPress(Smf* seq, int time, int channel, int track, int key, int amount)
{
  bool result = false;

  if((key >= 0) && (key <= 127) && (amount >= 0) && (amount <= 127))
  {
    byte keyPress[3];

    keyPress[0] = SMF_EVENT_KEYPRESS | (channel % 16);
    keyPress[1] = key;
    keyPress[2] = amount;
    result = smfInsertEvent(seq, time, channel / 16, track, keyPress, sizeof(keyPress));
  }
  return result;
}

bool smfInsertControl(Smf* seq, int time, int channel, int track, int controlNumber, int value)
{
  bool result = false;

  if((controlNumber >= 0) && (controlNumber <= 127) && (value >= 0) && (value <= 127))
  {
    byte controlChange[3];

    controlChange[0] = SMF_EVENT_CONTROL | (channel % 16);
    controlChange[1] = controlNumber;
    controlChange[2] = value;
    result = smfInsertEvent(seq, time, channel / 16, track, controlChange, sizeof(controlChange));
  }
  return result;
}

bool smfInsertProgram(Smf* seq, int time, int channel, int track, int programNumber)
{
  bool result = false;

  if((programNumber >= 0) && (programNumber <= 127))
  {
    byte programChange[2];

    programChange[0] = SMF_EVENT_PROGRAM | (channel % 16);
    programChange[1] = programNumber;
    result = smfInsertEvent(seq, time, channel / 16, track, programChange, sizeof(programChange));
  }
  return result;
}

bool smfInsertChanPress(Smf* seq, int time, int channel, int track, int key, int amount)
{
  bool result = false;

  if((key >= 0) && (key <= 127) && (amount >= 0) && (amount <= 127))
  {
    byte chanPress[3];

    chanPress[0] = SMF_EVENT_CHANPRESS | (channel % 16);
    chanPress[1] = key;
    chanPress[2] = amount;
    result = smfInsertEvent(seq, time, channel / 16, track, chanPress, sizeof(chanPress));
  }
  return result;
}

bool smfInsertPitchBend(Smf* seq, int time, int channel, int track, int value)
{
  bool result = false;

  if((value >= -8192) && (value <= 8191))
  {
    byte pitchBend[3];

    value += 8192;
    pitchBend[0] = SMF_EVENT_PITCHBEND | (channel % 16);
    pitchBend[1] = (unsigned int) value & 0x7f;
    pitchBend[2] = (unsigned int) value >> 7;
    result = smfInsertEvent(seq, time, channel / 16, track, pitchBend, sizeof(pitchBend));
  }
  return result;
}

bool smfInsertSysex(Smf* seq, int time, int port, int track, const byte* data, size_t dataSize)
{
  bool result = false;

  if(data && dataSize && ((data[0] == SMF_EVENT_SYSEX) || (data[0] == SMF_EVENT_SYSEXLITE)))
  {
    size_t sysexLength = dataSize - 1;
    size_t sysexLengthSize = smfGetVarLengthSize((unsigned int) sysexLength);
    size_t sysexDataSize = 1 + sysexLengthSize + sysexLength;
    byte* sysexData = (byte*) malloc(sysexDataSize);

    if(sysexData)
    {
      sysexData[0] = data[0];
      smfWriteVarLength((unsigned int) sysexLength, &sysexData[1], sysexLength);
      memcpy(&sysexData[1 + sysexLengthSize], &data[1], sysexLength);
      result = smfInsertEvent(seq, time, port, track, sysexData, sysexDataSize);
      free(sysexData);
    }
  }
  return result;
}

bool smfInsertMetaEvent(Smf* seq, int time, int track, int metaType, const byte* data, size_t dataSize)
{
  bool result = false;

  if((metaType >= 0) && (metaType <= 255) && data && dataSize)
  {
    size_t metaLength = dataSize;
    size_t metaLengthSize = smfGetVarLengthSize((unsigned int) metaLength);
    size_t metaDataSize = 2 + metaLengthSize + metaLength;
    byte* metaData = (byte*) malloc(metaDataSize);

    if(metaData)
    {
      metaData[0] = SMF_EVENT_META;
      metaData[1] = metaType;
      smfWriteVarLength((unsigned int) metaLength, &metaData[2], metaLength);
      memcpy(&metaData[2 + metaLengthSize], data, metaLength);
      result = smfInsertEvent(seq, time, 0, track, metaData, metaDataSize);
      free(metaData);
    }
  }
  return result;
}

bool smfInsertMetaText(Smf* seq, int time, int track, int metaType, const char* text)
{
  bool result = false;

  if(text)
  {
    result = smfInsertMetaEvent(seq, time, track, metaType, text, strlen(text));
  }
  return result;
}

bool smfInsertGM1SystemOn(Smf* seq, int time, int port, int track)
{
  byte sysexGM1SystemOn[] = { 0xF0, 0x7E, 0x7F, 0x09, 0x01, 0xF7 };

  return smfInsertSysex(seq, time, port, track, 
    sysexGM1SystemOn, sizeof(sysexGM1SystemOn));
}

bool smfInsertMasterVolume(Smf* seq, int time, int port, int track, int volume)
{
  bool result = false;

  if((volume >= 0) && (volume <= 127))
  {
    byte sysexMasterVolume[] = { 0xF0, 0x7F, 0x7F, 0x04, 0x01, 0x00, (byte) volume, 0xF7 };

    result = smfInsertSysex(seq, time, port, track, sysexMasterVolume, sizeof(sysexMasterVolume));
  }
  return result;
}

bool smfInsertTempo(Smf* seq, int time, int track, int microSeconds)
{
  bool result = false;

  if((microSeconds >= 0) && (microSeconds <= 0xffffff))
  {
    byte metaTempo[3];

    smfWriteByte(3, microSeconds, metaTempo, 3);
    result = smfInsertMetaEvent(seq, time, track, SMF_META_SETTEMPO, metaTempo, sizeof(metaTempo));
  }
  return result;
}

bool smfInsertTempoBPM(Smf* seq, int time, int track, double bpm)
{
  double microSeconds = 60000000 / bpm;

  return smfInsertTempo(seq, time, track, (int) microSeconds);
}
