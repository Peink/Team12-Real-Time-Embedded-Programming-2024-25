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
    u16 pageID;     // Fingerprint ID
    u16 mathscore;  // Matching score
  } SearchResult;

  typedef struct {
    u16 PS_max;   // Maximum fingerprint capacity
    u8 PS_level;  // Security level
    u32 PS_addr;  // Module address
    u8 PS_size;   // Data packet size
    u8 PS_N;      // Baud rate base N
  } SysPara;

  fingerEventInterFace();
  ~fingerEventInterFace() = default;

 void hasEvent(dataFingerImpl::serial_with_params_event &e) override;

  void deviceCheck(void);

  bool Add_FR(void);

  bool press_FR(void);

  bool Del_FR(void);

  void setUARTSendData(void (*UartSendData)(const char *, uint16_t)) {
    UartSendData_ = UartSendData;
  }

  void (*UartSendData_)(const char *data, uint16_t data_len) = nullptr;

 private:
  void showMenu();  // Display command menu

  u8 PS_GetImage(void);  // Capture fingerprint image

  u8 PS_GenChar(u8 BufferID);  // Generate fingerprint feature

  u8 PS_Match(void);  // Match two fingerprint features

  u8 PS_Search(u8 BufferID, u16 StartPage, u16 PageNum,
               SearchResult *p);  // Search fingerprint

  u8 PS_RegModel(void);  // Merge fingerprint features (generate template)

  u8 PS_StoreChar(u8 BufferID, u16 PageID);  // Store fingerprint template

  u8 PS_DeletChar(u16 PageID, u16 N);  // Delete fingerprint template

  u8 PS_Empty(void);  // Clear fingerprint database

  u8 PS_WriteReg(u8 RegNum, u8 DATA);  // Write system register

  u8 PS_ReadSysPara(SysPara *p);  // Read system parameters

  u8 PS_SetAddr(u32 addr);  // Set module address

  u8 PS_WriteNotepad(u8 NotePageNum, u8 *content);  // Write to notepad memory

  u8 PS_ReadNotepad(u8 NotePageNum, u8 *note);  // Read from notepad memory

  u8 PS_HighSpeedSearch(u8 BufferID, u16 StartPage, u16 PageNum,
                        SearchResult *p);  // High-speed fingerprint search

  u8 PS_ValidTempleteNum(u16 *ValidN);  // Get number of valid templates

  u8 PS_HandShake(u32 *PS_Addr);  // Handshake with AS608 module

  const char *EnsureMessage(u8 ensure);  // Interpret confirmation code

  void ShowErrMessage(u8 ensure);  // Display error message

  u8 finger_num = 0;
};


#endif

