/**
 * libsmfc.h: simple standard midi writer by loveemu
 */


#ifndef LIBSMFC_H
#define LIBSMFC_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stddef.h>

#if !defined(bool) && !defined(__cplusplus)
  typedef int bool;
  #define true    1
  #define false   0
#endif /* !bool */

#ifndef byte
  typedef unsigned char byte;
#endif /* !byte */
#ifndef sbyte
  typedef signed char sbyte;
#endif /* !sbyte */

unsigned int smfReadVarLength(byte* buffer, size_t bufferSize);
size_t smfWriteByte(size_t sizeToTransfer, unsigned int value, byte* buffer, size_t bufferSize);
size_t smfGetVarLengthSize(unsigned int value);
size_t smfWriteVarLength(unsigned int value, byte* buffer, size_t bufferSize);


typedef struct TagSmfEvent SmfEvent;
struct TagSmfEvent
{
  byte*       data;
  size_t      size;
  int         time;
  int         port;
  SmfEvent*   prevEvent;
  SmfEvent*   nextEvent;
};

SmfEvent* smfEventCreate(int time, int port, const byte* data, size_t dataSize);
void smfEventDelete(SmfEvent* event);
SmfEvent* smfEventCopy(SmfEvent* event);
size_t smfEventGetSize(SmfEvent* event);
size_t smfEventWrite(SmfEvent* event, byte* buffer, size_t bufferSize);
int smfEventCompare(SmfEvent* event, SmfEvent* targetEvent);


typedef struct TagSmfTrack
{
  SmfEvent*   firstEvent;
  SmfEvent*   lastEvent;
} SmfTrack;

SmfTrack* smfTrackCreate(void);
void smfTrackDelete(SmfTrack* track);
SmfTrack* smfTrackCopy(SmfTrack* track);
bool smfTrackInsertEvent(SmfTrack* track, int time, int port, const byte* data, size_t dataSize);
size_t smfTrackGetSize(SmfTrack* track);
size_t smfTrackWrite(SmfTrack* track, byte* buffer, size_t bufferSize);
int smfTrackGetEndTiming(SmfTrack* track);
int smfTrackSetEndTiming(SmfTrack* track, int newEndTiming);


typedef struct TagSmf
{
  int numTracks;
  int timebase;
  SmfTrack** track;
} Smf;

Smf* smfCreate(void);
void smfDelete(Smf* seq);
Smf* smfCopy(Smf* seq);
bool smfInsertEvent(Smf* seq, int time, int port, int track, const byte* data, size_t dataSize);
size_t smfGetSize(Smf* seq);
size_t smfWrite(Smf* seq, byte* buffer, size_t bufferSize);
int smfSetTimebase(Smf* seq, int newTimebase);
int smfSetEndTimingOfTrack(Smf* seq, int track, int newEndTiming);

#ifdef __cplusplus
}
#endif

#endif /* !LIBSMFC_H */
