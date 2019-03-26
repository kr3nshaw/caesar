/**
 * libsmfc.c: simple standard midi writer by loveemu
 */


#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <memory.h>
#include "libsmfc.h"

#define SMF_VARLEN_MAX          4
#define SMF_TIMEBASE_MAX        0x7fff
#define SMF_CHANNEL_MAX         0x0f
#define SMF_PORT_MAX            0xff
#define SMF_VCHANNEL_MAX        (SMF_CHANNEL_MAX * SMF_PORT_MAX)

#define SMF_EVENT_MASK_CHANNEL  0x0f
#define SMF_EVENT_MASK_MESSAGE  0xf0

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

#define SMF_MTHD_SIZE           14
#define SMF_MTRK_SIZE           8

unsigned int smfReadVarLength(byte* buffer, size_t bufferSize)
{
  unsigned int value;
  size_t transferedSize = 0;
  size_t maxSizeToTransfer = (SMF_VARLEN_MAX < bufferSize) 
    ? SMF_VARLEN_MAX : bufferSize;

  value = buffer[transferedSize] & 0x7f;
  while((transferedSize < maxSizeToTransfer) && (buffer[transferedSize] & 0x80))
  {
    transferedSize++;
    value = value << 7;
    value |= buffer[transferedSize] & 0x7f;
  }
  return value;
}

size_t smfWriteByte(size_t sizeToTransfer, unsigned int value, byte* buffer, size_t bufferSize)
{
  size_t transferedSize = 0;
  size_t realSizeToTransfer = (sizeToTransfer < bufferSize) 
    ? sizeToTransfer : bufferSize;

  if(buffer && realSizeToTransfer)
  {
    size_t byteCount = sizeToTransfer - 1;

    for(transferedSize = 0; transferedSize < realSizeToTransfer; transferedSize++)
    {
      size_t shiftCount = byteCount * 8;

      buffer[transferedSize] = (byte) ((value >> shiftCount) & 0xff);
      byteCount--;
    }
  }
  return transferedSize;
}

size_t smfGetVarLengthSize(unsigned int value)
{
  size_t varLengthSize = 1;
  unsigned int leftValue = value;

  while((leftValue > 0x7f) && (varLengthSize < SMF_VARLEN_MAX))
  {
    varLengthSize++;
    leftValue = leftValue >> 7;
  }
  return varLengthSize;
}

size_t smfWriteVarLength(unsigned int value, byte* buffer, size_t bufferSize)
{
  size_t transferedSize = 0;
  size_t varLengthSize = smfGetVarLengthSize(value);
  size_t sizeToTransfer = (varLengthSize < bufferSize) 
    ? varLengthSize : bufferSize;

  if(buffer && sizeToTransfer)
  {
    size_t shiftCount = (varLengthSize - 1) * 7;

    for(transferedSize = 0; transferedSize < (sizeToTransfer - 1); transferedSize++)
    {
      buffer[transferedSize] = (byte) ((value >> shiftCount) & 0x7f) | 0x80;
      shiftCount -= 7;
    }
    buffer[transferedSize] = (byte) ((value >> shiftCount) & 0x7f);
    transferedSize++;
  }
  return transferedSize;
}


bool smfEventIsNoteOff(SmfEvent* event);

SmfEvent* smfEventCreate(int time, int port, const byte* data, size_t dataSize)
{
  SmfEvent* newEvent = NULL;

  if(data && dataSize && (time >= 0) && (port >= 0) 
      && (port < SMF_PORT_MAX))
  {
    newEvent = (SmfEvent*) calloc(1, sizeof(SmfEvent));
    if(newEvent)
    {
      newEvent->data = (byte*) malloc(dataSize);
      if(newEvent->data)
      {
        memcpy(newEvent->data, data, dataSize);
        newEvent->size = dataSize;
        newEvent->time = time;
        newEvent->port = port;
      }
      else
      {
        free(newEvent);
        newEvent = NULL;
      }
    }
  }
  return newEvent;
}

void smfEventDelete(SmfEvent* event)
{
  if(event)
  {
    free(event->data);
    free(event);
  }
}

SmfEvent* smfEventCopy(SmfEvent* event)
{
  SmfEvent* newEvent = NULL;

  if(event)
  {
    newEvent = smfEventCreate(event->time, event->port, event->data, event->size);
  }
  return newEvent;
}

size_t smfEventGetSize(SmfEvent* event)
{
  return event ? event->size : 0;
}

size_t smfEventWrite(SmfEvent* event, byte* buffer, size_t bufferSize)
{
  size_t transferedSize = 0;

  if(event && buffer && bufferSize)
  {
    size_t sizeToTransfer = (event->size < bufferSize) 
      ? event->size : bufferSize;

    memcpy(buffer, event->data, sizeToTransfer);
    transferedSize += sizeToTransfer;
  }

  return transferedSize;
}

int smfEventCompare(SmfEvent* event, SmfEvent* targetEvent)
{
  int result = 0;

  if(event && targetEvent)
  {
    result = event->time - targetEvent->time;
    if(result == 0)
    {
      bool eventIsNoteOff = smfEventIsNoteOff(event);
      bool targetEventIsNoteOff = smfEventIsNoteOff(targetEvent);

      if(!eventIsNoteOff && targetEventIsNoteOff)
      {
        result = 1;
      }
      else if(eventIsNoteOff && !targetEventIsNoteOff)
      {
        result = -1;
      }
      else
      {
        result = 0;
      }
    }
  }
  return result;
}

bool smfEventIsNoteOff(SmfEvent* event)
{
  bool eventIsNoteOff = false;

  if(event)
  {
    byte eventMessage = (event->data[0] & SMF_EVENT_MASK_MESSAGE);

    switch(eventMessage)
    {
    case SMF_EVENT_NOTEOFF:
      eventIsNoteOff = true;
      break;

    case SMF_EVENT_NOTEON:
      if(3 <= event->size)
      {
        int velocity = event->data[2];

        eventIsNoteOff = (velocity == 0);
      }
      break;

    default:
      break;
    }
  }
  return eventIsNoteOff;
}


typedef bool (SmfTrackEnumEventsProc)(SmfEvent*, void*);
bool smfTrackEnumEvents(SmfTrack* track, SmfTrackEnumEventsProc* eventProc, void* customData);

typedef struct TagSmfTrackGetSizeProcInfo
{
  int prevEventTime;
  size_t trackSize;
} SmfTrackGetSizeProcInfo;
bool smfTrackGetSizeProc(SmfEvent* event, void* customData);

typedef struct TagSmfTrackWriteProcInfo
{
  int prevEventTime;
  byte* buffer;
  size_t bufferSize;
  size_t transferedSize;
} SmfTrackWriteProcInfo;
bool smfTrackWriteProc(SmfEvent* event, void* customData);

SmfTrack* smfTrackCreate(void)
{
  SmfTrack* newTrack;

  newTrack = (SmfTrack*) calloc(1, sizeof(SmfTrack));
  if(newTrack)
  {
    const byte endOfTrackData[] = { 0xff, 0x2f, 0x00 };
    SmfEvent* endOfTrack;

    endOfTrack = smfEventCreate(0, 0, endOfTrackData, sizeof(endOfTrackData));
    if(endOfTrack)
    {
      newTrack->firstEvent = endOfTrack;
      newTrack->lastEvent = endOfTrack;
    }
    else
    {
      free(newTrack);
      newTrack = NULL;
    }
  }
  return newTrack;
}

void smfTrackDelete(SmfTrack* track)
{
  if(track)
  {
    SmfEvent* event = track->firstEvent;

    while(event)
    {
      SmfEvent* nextEvent = event->nextEvent;
      smfEventDelete(event);
      event = nextEvent;
    }
  }
}

SmfTrack* smfTrackCopy(SmfTrack* track)
{
  SmfTrack* newTrack = NULL;

  if(track)
  {
    newTrack = smfTrackCreate();
    if(newTrack)
    {
      SmfEvent* event = track->firstEvent;

      while(event != track->lastEvent)
      {
        smfTrackInsertEvent(newTrack, event->time, event->port, event->data, event->size);
        event = event->nextEvent;
      }
      smfTrackSetEndTiming(newTrack, smfTrackGetEndTiming(track));
    }
  }
  return newTrack;
}

bool smfTrackInsertEvent(SmfTrack* track, int time, int port, const byte* data, size_t dataSize)
{
  SmfEvent* newEvent = smfEventCreate(time, port, data, dataSize);

  if(newEvent)
  {
    SmfEvent* nextEvent;
    SmfEvent* prevEvent;

    if(newEvent->time > smfTrackGetEndTiming(track))
    {
      smfTrackSetEndTiming(track, newEvent->time);
    }

    prevEvent = track->lastEvent->prevEvent;
    while(prevEvent && (smfEventCompare(newEvent, prevEvent) < 0))
    {
      prevEvent = prevEvent->prevEvent;
    }
    nextEvent = prevEvent ? prevEvent->nextEvent : track->firstEvent;

    newEvent->prevEvent = prevEvent;
    newEvent->nextEvent = nextEvent;
    if(nextEvent)
    {
      nextEvent->prevEvent = newEvent;
    }
    else
    {
      track->lastEvent = newEvent;
    }
    if(prevEvent)
    {
      prevEvent->nextEvent = newEvent;
    }
    else
    {
      track->firstEvent = newEvent;
    }
  }
  return (bool) (newEvent != NULL);
}

size_t smfTrackGetSize(SmfTrack* track)
{
  size_t trackSize = 0;

  if(track)
  {
    SmfTrackGetSizeProcInfo info;

    info.prevEventTime = 0;
    info.trackSize = SMF_MTRK_SIZE;
    smfTrackEnumEvents(track, smfTrackGetSizeProc, &info);
    trackSize = info.trackSize;
  }
  return trackSize;
}

bool smfTrackGetSizeProc(SmfEvent* event, void* customData)
{
  SmfTrackGetSizeProcInfo* info = (SmfTrackGetSizeProcInfo*) customData;
  int deltaTime = event->time - info->prevEventTime;
  size_t deltaTimeSize = smfGetVarLengthSize(deltaTime);

  info->trackSize += deltaTimeSize;
  info->trackSize += event->size;
  info->prevEventTime = event->time;
  return true;
}

size_t smfTrackWrite(SmfTrack* track, byte* buffer, size_t bufferSize)
{
  size_t transferedSize = 0;

  if(track && buffer && bufferSize)
  {
    byte MTrkData[SMF_MTRK_SIZE] = { 'M', 'T', 'r', 'k', 0, 0, 0, 0 };
    size_t trackSize = smfTrackGetSize(track);

    smfWriteByte(4, (unsigned int) (trackSize - SMF_MTRK_SIZE), &MTrkData[4], 4);
    if(bufferSize >= SMF_MTRK_SIZE)
    {
      SmfTrackWriteProcInfo info;

      memcpy(&buffer[transferedSize], MTrkData, SMF_MTRK_SIZE);
      transferedSize += SMF_MTRK_SIZE;

      info.prevEventTime = 0;
      info.buffer = buffer;
      info.bufferSize = bufferSize;
      info.transferedSize = transferedSize;
      smfTrackEnumEvents(track, smfTrackWriteProc, &info);
      transferedSize = info.transferedSize;
    }
    else
    {
      memcpy(&buffer[transferedSize], MTrkData, bufferSize - transferedSize);
      transferedSize = bufferSize;
    }
  }
  return transferedSize;
}

bool smfTrackWriteProc(SmfEvent* event, void* customData)
{
  bool result = false;
  SmfTrackWriteProcInfo* info = (SmfTrackWriteProcInfo*) customData;
  byte* buffer = info->buffer;
  size_t bufferSize = info->bufferSize;
  size_t transferedSize = info->transferedSize;
  int deltaTime = event->time - info->prevEventTime;
  size_t deltaTimeSize = smfGetVarLengthSize(deltaTime);

  if(bufferSize >= (transferedSize + deltaTimeSize))
  {
    smfWriteVarLength(deltaTime, &buffer[transferedSize], deltaTimeSize);
    transferedSize += deltaTimeSize;

    if(bufferSize >= (transferedSize + event->size))
    {
      memcpy(&buffer[transferedSize], event->data, event->size);
      transferedSize += event->size;
      result = true;
    }
    else
    {
      memcpy(&buffer[transferedSize], event->data, bufferSize - transferedSize);
      transferedSize = bufferSize;
    }
  }
  else
  {
    smfWriteVarLength(deltaTime, &buffer[transferedSize], bufferSize - transferedSize);
    transferedSize = bufferSize;
  }

  info->prevEventTime = event->time;
  info->transferedSize = transferedSize;
  return result;
}

bool smfTrackEnumEvents(SmfTrack* track, SmfTrackEnumEventsProc* eventProc, void* customData)
{
  bool result = false;

  if(track && eventProc)
  {
    int prevEventPort = 0;
    SmfEvent* event = track->firstEvent;

    result = true;
    while(event)
    {
      if((event->port != prevEventPort) && (event->data[0] != SMF_EVENT_META))
      {
        byte portChangeMessage[] = { 0xff, 0x21, 0x01, 0 };
        SmfEvent* portChangeEvent;

        portChangeMessage[3] = (byte) event->port;
        portChangeEvent = smfEventCreate(event->time, event->port, 
        portChangeMessage, sizeof(portChangeMessage));
        if(!portChangeEvent)
        {
          result = false;
          break;
        }
        if(!eventProc(portChangeEvent, customData))
        {
          result = false;
          break;
        }
        smfEventDelete(portChangeEvent);
        prevEventPort = event->port;
      }

      if(!eventProc(event, customData))
      {
        result = false;
        break;
      }
      event = event->nextEvent;
    }
  }
  return result;
}

int smfTrackGetEndTiming(SmfTrack* track)
{
  int endTiming = 0;

  if(track)
  {
    SmfEvent* endOfTrack = track->lastEvent;

    endTiming = endOfTrack->time;
  }
  return endTiming;
}

int smfTrackSetEndTiming(SmfTrack* track, int newEndTiming)
{
  int oldEndTiming = 0;

  if(track)
  {
    SmfEvent* endOfTrack = track->lastEvent;
    int lastEventTiming = endOfTrack->prevEvent 
      ? endOfTrack->prevEvent->time : 0;

    if(newEndTiming >= lastEventTiming)
    {
      endOfTrack->time = newEndTiming;
    }
  }
  return oldEndTiming;
}


bool smfReallocTrack(Smf* seq, int newNumTracks);

Smf* smfCreate(void)
{
  Smf* newSeq = (Smf*) calloc(1, sizeof(Smf));

  if(newSeq)
  {
    newSeq->track = (SmfTrack**) malloc(sizeof(SmfTrack*));
    if(newSeq->track)
    {
      newSeq->track[0] = smfTrackCreate();
      if(newSeq->track[0])
      {
        newSeq->numTracks++;
      }
      else
      {
        free(newSeq->track);
        free(newSeq);
        newSeq = NULL;
      }
    }
    else
    {
      free(newSeq);
      newSeq = NULL;
    }
  }

  return newSeq;
}

void smfDelete(Smf* seq)
{
  if(seq)
  {
    int trackIndex;

    for(trackIndex = 0; trackIndex < seq->numTracks; trackIndex++)
    {
      smfTrackDelete(seq->track[trackIndex]);
    }
    free(seq);
  }
}

Smf* smfCopy(Smf* seq)
{
  Smf* newSeq = smfCreate();

  if(newSeq)
  {
    if(smfReallocTrack(newSeq, seq->numTracks))
    {
      int trackIndex;

      for(trackIndex = 0; trackIndex < seq->numTracks; trackIndex++)
      {
        SmfTrack* newTrack = smfTrackCopy(seq->track[trackIndex]);
        if(!newTrack)
        {
          smfDelete(newSeq);
          newSeq = NULL;
          break;
        }
        smfTrackDelete(newSeq->track[trackIndex]);
        newSeq->track[trackIndex] = newTrack;
      }

      smfSetTimebase(newSeq, seq->timebase);
    }
    else
    {
      smfDelete(newSeq);
      newSeq = NULL;
    }
  }
  return newSeq;
}

bool smfInsertEvent(Smf* seq, int time, int port, int track, const byte* data, size_t dataSize)
{
  bool result = false;

  if(seq)
  {
    bool allocResult = true;

    if(track >= seq->numTracks)
    {
      allocResult = smfReallocTrack(seq, track + 1);
    }
    if(allocResult)
    {
      result = smfTrackInsertEvent(seq->track[track], time, port, data, dataSize);
    }
  }
  return result;
}

size_t smfGetSize(Smf* seq)
{
  size_t seqSize = 0;

  if(seq)
  {
    int trackIndex;

    seqSize = SMF_MTHD_SIZE;
    for(trackIndex = 0; trackIndex < seq->numTracks; trackIndex++)
    {
      seqSize += smfTrackGetSize(seq->track[trackIndex]);
    }
  }
  return seqSize;
}

size_t smfWrite(Smf* seq, byte* buffer, size_t bufferSize)
{
  size_t transferedSize = 0;

  if(seq && buffer && bufferSize)
  {
    byte MThdData[SMF_MTHD_SIZE] = { 'M', 'T', 'h', 'd', 0, 0, 0, 6, 0, 1, 0, 0, 0, 0 };

    smfWriteByte(2, seq->numTracks, &MThdData[10], 2);
    smfWriteByte(2, seq->timebase, &MThdData[12], 2);
    if(bufferSize >= SMF_MTHD_SIZE)
    {
      int trackIndex;

      memcpy(&buffer[transferedSize], MThdData, SMF_MTHD_SIZE);
      transferedSize += SMF_MTHD_SIZE;

      for(trackIndex = 0; trackIndex < seq->numTracks; trackIndex++)
      {
        transferedSize += smfTrackWrite(seq->track[trackIndex], 
        &buffer[transferedSize], bufferSize - transferedSize);
        if(transferedSize == bufferSize)
        {
          break;
        }
      }
    }
    else
    {
      memcpy(&buffer[transferedSize], MThdData, bufferSize - transferedSize);
      transferedSize = bufferSize;
    }
  }
  return transferedSize;
}

int smfSetTimebase(Smf* seq, int newTimebase)
{
  int oldTimebase = 0;

  if(seq && (newTimebase >= 0) && (newTimebase <= SMF_TIMEBASE_MAX))
  {
    oldTimebase = seq->timebase;
    seq->timebase = newTimebase;
  }
  return oldTimebase;
}

int smfSetEndTimingOfTrack(Smf* seq, int track, int newEndTiming)
{
  int oldEndTiming = 0;

  if(seq)
  {
    bool allocResult = true;

    if(track >= seq->numTracks)
    {
      allocResult = smfReallocTrack(seq, track + 1);
    }
    if(allocResult)
    {
      oldEndTiming = smfTrackSetEndTiming(seq->track[track], newEndTiming);
    }
  }
  return oldEndTiming;
}

bool smfReallocTrack(Smf* seq, int newNumTracks)
{
  bool result = false;

  if(seq)
  {
    result = true;
    if(newNumTracks > seq->numTracks)
    {
      SmfTrack** newTracks = (SmfTrack**) realloc(seq->track, sizeof(SmfTrack*) * newNumTracks);

      if(newTracks)
      {
        int trackIndex;

        seq->track = newTracks;
        for(trackIndex = seq->numTracks; trackIndex < newNumTracks; trackIndex++)
        {
          seq->track[trackIndex] = smfTrackCreate();
          seq->numTracks++;
          if(!seq->track[trackIndex])
          {
            for(; trackIndex >= seq->numTracks; trackIndex--)
            {
              smfTrackDelete(seq->track[trackIndex]);
              seq->numTracks--;
            }
            result = false;
            break;
          }
        }
      }
      else
      {
        result = false;
      }
    }
  }
  return result;
}
