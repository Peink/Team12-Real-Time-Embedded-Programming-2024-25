#ifndef _FINGER_EVENT_INTERFACE_H_
#define _FINGER_EVENT_INTERFACE_H_
#include <stdint.h>
#include <stdio.h>

#include "data_finger_impl.hpp"

#define CharBuffer1 0x01
#define CharBuffer2 0x02
#define u32 uint32_t
#define u16 uint16_t
#define u8 uint8_t

class dataFingerImpl;
class fingerEventInterFace
    : public dataFingerImpl::SerialEventCallbackInterface {
 public:
  typedef struct {
    u16 pageID;     // Finger ID
    u16 mathscore;  // Matching score
  } SearchResult;

  typedef struct {
    u16 PS_max;   // maximum finger print volumm
    u8 PS_level;  // security level
    u32 PS_addr;
    u8 PS_size;  // capacity of communication data packet
    u8 PS_N;     // Baud rate N
  } SysPara;

  fingerEventInterFace();
  ~fingerEventInterFace() = default;

  void hasEvent(dataFingerImpl::serial_with_params_event &e) override;

  void Add_FR(void);

  void press_FR(void);

  void Del_FR(void);

  void setUARTSendData(void (*UartSendData)(const char *, uint16_t)) {
    UartSendData_ = UartSendData;
  }

  void (*UartSendData_)(const char *data, uint16_t data_len) = nullptr;

 private:
  u8 PS_GetImage(void);  // recording figure

  u8 PS_GenChar(u8 BufferID);  // create feature

  u8 PS_Match(void);  // compare the finger feature

  u8 PS_Search(u8 BufferID, u16 StartPage, u16 PageNum,
               SearchResult *p);  // searching finger

  u8 PS_RegModel(void);  // combine feature£¨create module£©

  u8 PS_StoreChar(u8 BufferID, u16 PageID);  // storage module

  u8 PS_DeletChar(u16 PageID, u16 N);  // delete module

  u8 PS_Empty(void);  // clear database

  u8 PS_WriteReg(u8 RegNum, u8 DATA);  // write in system storage

  u8 PS_ReadSysPara(SysPara *p);  // read system parameters

  u8 PS_SetAddr(u32 addr);  // setting module address

  u8 PS_WriteNotepad(u8 NotePageNum, u8 *content);  // write into note

  u8 PS_ReadNotepad(u8 NotePageNum, u8 *note);  // read note

  u8 PS_HighSpeedSearch(u8 BufferID, u16 StartPage, u16 PageNum,
                        SearchResult *p);  // high speed search

  u8 PS_ValidTempleteNum(u16 *ValidN);  // read valid module

  u8 PS_HandShake(u32 *PS_Addr);  // Shake hands with AS608 module

  const char *EnsureMessage(u8 ensure);  // confirm wrong data resolve

  void ShowErrMessage(u8 ensure);
};

#endif
