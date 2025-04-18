#include "finger.hpp"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define RX_LEN 2048
#define TX_LEN 2048
u32 AS608Addr = 0XFFFFFFFF;  // default address
char str2[6] = {0};

/// @brief ///
uint8_t key_num = 3;
static u8 USART_RX_BUF[RX_LEN] = {0};
static u8 USART_RX_STA = 0;
static char USART_TX_BUF[1] = {0};

static fingerEventInterFace *fingerEventInterFace_ = nullptr;

fingerEventInterFace::fingerEventInterFace() { fingerEventInterFace_ = this; }

// Send one byte over serial port
static void MYUSART_SendData(u8 data) {
  // while((USART3->SR & 0X40) == 0);
  // USART3->DR = data;
  USART_TX_BUF[0] = data;
  fingerEventInterFace_->UartSendData_(USART_TX_BUF, 1);
}

// Send packet header
static void SendHead(void) {
  MYUSART_SendData(0xEF);
  MYUSART_SendData(0x01);
}

// Send module address
static void SendAddr(void) {
  MYUSART_SendData(AS608Addr >> 24);
  MYUSART_SendData(AS608Addr >> 16);
  MYUSART_SendData(AS608Addr >> 8);
  MYUSART_SendData(AS608Addr);
}

// Send packet flag
static void SendFlag(u8 flag) { MYUSART_SendData(flag); }

// Send packet length
static void SendLength(int length) {
  MYUSART_SendData(length >> 8);
  MYUSART_SendData(length);
}

// Send command code
static void Sendcmd(u8 cmd) { MYUSART_SendData(cmd); }

// Send checksum
static void SendCheck(u16 check) {
  MYUSART_SendData(check >> 8);
  MYUSART_SendData(check);
}

// Check if response packet is received from serial interrupt buffer
// waittime: timeout in milliseconds
// Return value: pointer to packet start address
static u8 *JudgeStr(u16 waittime) {
  char *data;
  u8 str[8];
  str[0] = 0xef;
  str[1] = 0x01;
  str[2] = AS608Addr >> 24;
  str[3] = AS608Addr >> 16;
  str[4] = AS608Addr >> 8;
  str[5] = AS608Addr;
  str[6] = 0x07;
  str[7] = '\0';
  // USART_RX_STA = 0;
  while (--waittime) {
    usleep(1000);
    // if(USART_RX_STA & 0X8000) // data received
    // {
    // USART_RX_STA = 0;
    if (USART_RX_STA == 1) {
      USART_RX_STA = 0;
      data = strstr((char *)USART_RX_BUF, (const char *)str);
      if (data) return (u8 *)data;
    }
    // }
  }
  return 0;
}

// Callback function for handling serial events
void fingerEventInterFace::hasEvent(
    dataFingerImpl::serial_with_params_event &e) {
  switch (e.event_type) {
    case dataFingerImpl::SERIAL_RECEIVE_EVENT: {
      char *buf = (char *)e.params;
      memset(USART_RX_BUF, 0, RX_LEN);
      memcpy(USART_RX_BUF, buf, RX_LEN);
      USART_RX_STA = 1;
    } break;

    default:
      break;
  }
}

// PS_GetImage: capture fingerprint image
// Function: Detect if finger is placed and capture fingerprint image to ImageBuffer.
// Returns confirmation code from module
u8 fingerEventInterFace::PS_GetImage(void) {
  u16 temp;
  u8 ensure;
  u8 *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);  // command packet flag
  SendLength(0x03);
  Sendcmd(0x01);
  temp = 0x01 + 0x03 + 0x01;
  SendCheck(temp);
  data = JudgeStr(2000);
  if (data)
    ensure = data[9];
  else
    ensure = 0xff;
  return ensure;
}

// PS_GenChar: generate feature from captured image
// Function: Convert raw image in ImageBuffer into fingerprint feature and store in CharBuffer1 or CharBuffer2
// Parameter: BufferID --> CharBuffer1: 0x01, CharBuffer2: 0x02
// Returns confirmation code from module
u8 fingerEventInterFace::PS_GenChar(u8 BufferID) {
  u16 temp;
  u8 ensure;
  u8 *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);  // command packet flag
  SendLength(0x04);
  Sendcmd(0x02);
  MYUSART_SendData(BufferID);
  temp = 0x01 + 0x04 + 0x02 + BufferID;
  SendCheck(temp);
  data = JudgeStr(2000);
  if (data)
    ensure = data[9];
  else
    ensure = 0xff;
  return ensure;
}

// PS_Match: match two fingerprint features
// Function: Compare feature data in CharBuffer1 and CharBuffer2 for a match
// Returns confirmation code
u8 fingerEventInterFace::PS_Match(void) {
  u16 temp;
  u8 ensure;
  u8 *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);  // command packet flag
  SendLength(0x03);
  Sendcmd(0x03);
  temp = 0x01 + 0x03 + 0x03;
  SendCheck(temp);
  data = JudgeStr(2000);
  if (data)
    ensure = data[9];
  else
    ensure = 0xff;
  return ensure;
}

// PS_Search: search fingerprint template
// Function: Search entire or partial database using feature in CharBuffer1 or CharBuffer2. Returns match page ID if found.
// Parameter: BufferID (CharBuffer1/2), StartPage, PageNum, SearchResult* p
// Returns confirmation code and match result (pageID, match score)
u8 fingerEventInterFace::PS_Search(u8 BufferID, u16 StartPage, u16 PageNum,
                                   SearchResult *p) {
  u16 temp;
  u8 ensure;
  u8 *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);  // command packet flag
  SendLength(0x08);
  Sendcmd(0x04);
  MYUSART_SendData(BufferID);
  MYUSART_SendData(StartPage >> 8);
  MYUSART_SendData(StartPage);
  MYUSART_SendData(PageNum >> 8);
  MYUSART_SendData(PageNum);
  temp = 0x01 + 0x08 + 0x04 + BufferID + (StartPage >> 8) + (u8)StartPage +
         (PageNum >> 8) + (u8)PageNum;
  SendCheck(temp);
  data = JudgeStr(2000);
  if (data) {
    ensure = data[9];
    p->pageID = (data[10] << 8) + data[11];
    p->mathscore = (data[12] << 8) + data[13];
  } else
    ensure = 0xff;
  return ensure;
}
// Merge features to generate fingerprint template - PS_RegModel
// Function: Merge fingerprint features in CharBuffer1 and CharBuffer2
// The result is stored in both CharBuffer1 and CharBuffer2
// Return: confirmation code from the module
u8 fingerEventInterFace::PS_RegModel(void) {
  u16 temp;
  u8 ensure;
  u8 *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);  // command packet identifier
  SendLength(0x03);
  Sendcmd(0x05);
  temp = 0x01 + 0x03 + 0x05;
  SendCheck(temp);
  data = JudgeStr(2000);
  if (data)
    ensure = data[9];
  else
    ensure = 0xff;
  return ensure;
}

// Store template - PS_StoreChar
// Function: Store the template in CharBuffer1 or CharBuffer2 into the specified flash PageID
// Parameters: BufferID @ref CharBuffer1: 0x01, CharBuffer2: 0x02
//             PageID: template location index in fingerprint library
// Return: confirmation code from the module
u8 fingerEventInterFace::PS_StoreChar(u8 BufferID, u16 PageID) {
  u16 temp;
  u8 ensure;
  u8 *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);  // command packet identifier
  SendLength(0x06);
  Sendcmd(0x06);
  MYUSART_SendData(BufferID);
  MYUSART_SendData(PageID >> 8);
  MYUSART_SendData(PageID);
  temp = 0x01 + 0x06 + 0x06 + BufferID + (PageID >> 8) + (u8)PageID;
  SendCheck(temp);
  data = JudgeStr(2000);
  if (data)
    ensure = data[9];
  else
    ensure = 0xff;
  return ensure;
}

// Delete template - PS_DeletChar
// Function: Delete N fingerprint templates starting from PageID in flash database
// Parameters: PageID - template start index; N - number of templates to delete
// Return: confirmation code from the module
u8 fingerEventInterFace::PS_DeletChar(u16 PageID, u16 N) {
  u16 temp;
  u8 ensure;
  u8 *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);  // command packet identifier
  SendLength(0x07);
  Sendcmd(0x0C);
  MYUSART_SendData(PageID >> 8);
  MYUSART_SendData(PageID);
  MYUSART_SendData(N >> 8);
  MYUSART_SendData(N);
  temp = 0x01 + 0x07 + 0x0C + (PageID >> 8) + (u8)PageID + (N >> 8) + (u8)N;
  SendCheck(temp);
  data = JudgeStr(2000);
  if (data)
    ensure = data[9];
  else
    ensure = 0xff;
  return ensure;
}

// Clear fingerprint database - PS_Empty
// Function: Delete all fingerprint templates in flash database
// Parameters: None
// Return: confirmation code from the module
u8 fingerEventInterFace::PS_Empty(void) {
  u16 temp;
  u8 ensure;
  u8 *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);  // command packet identifier
  SendLength(0x03);
  Sendcmd(0x0D);
  temp = 0x01 + 0x03 + 0x0D;
  SendCheck(temp);
  data = JudgeStr(2000);
  if (data)
    ensure = data[9];
  else
    ensure = 0xff;
  return ensure;
}

// Write system register - PS_WriteReg
// Function: Write to module's system register
// Parameters: RegNum - register number (4/5/6), DATA - value to write
// Return: confirmation code from the module
u8 fingerEventInterFace::PS_WriteReg(u8 RegNum, u8 DATA) {
  u16 temp;
  u8 ensure;
  u8 *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);  // command packet identifier
  SendLength(0x05);
  Sendcmd(0x0E);
  MYUSART_SendData(RegNum);
  MYUSART_SendData(DATA);
  temp = RegNum + DATA + 0x01 + 0x05 + 0x0E;
  SendCheck(temp);
  data = JudgeStr(2000);
  if (data)
    ensure = data[9];
  else
    ensure = 0xff;
  if (ensure == 0)
    printf("\r\nSet parameter success!");
  else
    printf("\r\n%s", EnsureMessage(ensure));
  return ensure;
}

// Read system basic parameters - PS_ReadSysPara
// Function: Read basic parameters from the module (baud rate, package size, etc.)
// Parameters: None
// Return: confirmation code + parameter content (16 bytes)
u8 fingerEventInterFace::PS_ReadSysPara(SysPara *p) {
  u16 temp;
  u8 ensure;
  u8 *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);  // command packet identifier
  SendLength(0x03);
  Sendcmd(0x0F);
  temp = 0x01 + 0x03 + 0x0F;
  SendCheck(temp);
  data = JudgeStr(1000);
  if (data) {
    ensure = data[9];
    p->PS_max = (data[14] << 8) + data[15];
    p->PS_level = data[17];
    p->PS_addr =
        (data[18] << 24) + (data[19] << 16) + (data[20] << 8) + data[21];
    p->PS_size = data[23];
    p->PS_N = data[25];
  } else
    ensure = 0xff;
  if (ensure == 0x00) {
    printf("\r\nMax fingerprint capacity = %d", p->PS_max);
    printf("\r\nMatch level = %d", p->PS_level);
    printf("\r\nAddress = %x", p->PS_addr);
    printf("\r\nBaud rate = %d", p->PS_N * 9600);
  } else
    printf("\r\n%s", EnsureMessage(ensure));
  return ensure;
}

// Set module address - PS_SetAddr
// Function: Set the address of the fingerprint module
// Parameters: PS_addr - target address
// Return: confirmation code from the module
u8 fingerEventInterFace::PS_SetAddr(u32 PS_addr) {
  u16 temp;
  u8 ensure;
  u8 *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);  // command packet identifier
  SendLength(0x07);
  Sendcmd(0x15);
  MYUSART_SendData(PS_addr >> 24);
  MYUSART_SendData(PS_addr >> 16);
  MYUSART_SendData(PS_addr >> 8);
  MYUSART_SendData(PS_addr);
  temp = 0x01 + 0x07 + 0x15 + (u8)(PS_addr >> 24) + (u8)(PS_addr >> 16) +
         (u8)(PS_addr >> 8) + (u8)PS_addr;
  SendCheck(temp);
  AS608Addr = PS_addr;  // Update address after command sent
  data = JudgeStr(2000);
  if (data)
    ensure = data[9];
  else
    ensure = 0xff;
  AS608Addr = PS_addr;
  if (ensure == 0x00)
    printf("\r\nSet address success!");
  else
    printf("\r\n%s", EnsureMessage(ensure));
  return ensure;
}

// PS_WriteNotepad: Write to user notepad (flash memory)
// Function: The module provides 256 bytes of flash memory for user notes,
// logically divided into 16 pages
// Parameters: NotePageNum (0~15), Byte32 (32 bytes of content to write)
// Return: confirmation code from the module
u8 fingerEventInterFace::PS_WriteNotepad(u8 NotePageNum, u8 *Byte32) {
  u16 temp;
  u8 ensure, i;
  u8 *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);  // command packet identifier
  SendLength(36);
  Sendcmd(0x18);
  MYUSART_SendData(NotePageNum);
  for (i = 0; i < 32; i++) {
    MYUSART_SendData(Byte32[i]);
    temp += Byte32[i];
  }
  temp = 0x01 + 36 + 0x18 + NotePageNum + temp;
  SendCheck(temp);
  data = JudgeStr(2000);
  if (data)
    ensure = data[9];
  else
    ensure = 0xff;
  return ensure;
}
// Read notepad - PS_ReadNotepad
// Function: Read 128 bytes of user data from the FLASH notepad area
// Parameter: NotePageNum (0~15)
// Return: confirmation code + user data
u8 fingerEventInterFace::PS_ReadNotepad(u8 NotePageNum, u8 *Byte32) {
  u16 temp;
  u8 ensure, i;
  u8 *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);  // command packet flag
  SendLength(0x04);
  Sendcmd(0x19);
  MYUSART_SendData(NotePageNum);
  temp = 0x01 + 0x04 + 0x19 + NotePageNum;
  SendCheck(temp);
  data = JudgeStr(2000);
  if (data) {
    ensure = data[9];
    for (i = 0; i < 32; i++) {
      Byte32[i] = data[10 + i];
    }
  } else
    ensure = 0xff;
  return ensure;
}

// High-speed search - PS_HighSpeedSearch
// Function: Use features in CharBuffer1 or CharBuffer2 to quickly search the fingerprint library.
// If a match is found, returns the matched page ID. This command is effective when the fingerprint
// is well registered in the library.
// Parameters: BufferID, StartPage, PageNum
// Return: confirmation code + match page ID
u8 fingerEventInterFace::PS_HighSpeedSearch(u8 BufferID, u16 StartPage,
                                            u16 PageNum, SearchResult *p) {
  u16 temp;
  u8 ensure;
  u8 *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);  // command packet flag
  SendLength(0x08);
  Sendcmd(0x1b);
  MYUSART_SendData(BufferID);
  MYUSART_SendData(StartPage >> 8);
  MYUSART_SendData(StartPage);
  MYUSART_SendData(PageNum >> 8);
  MYUSART_SendData(PageNum);
  temp = 0x01 + 0x08 + 0x1b + BufferID + (StartPage >> 8) + (u8)StartPage +
         (PageNum >> 8) + (u8)PageNum;
  SendCheck(temp);
  data = JudgeStr(2000);
  if (data) {
    ensure = data[9];
    p->pageID = (data[10] << 8) + data[11];
    p->mathscore = (data[12] << 8) + data[13];
  } else
    ensure = 0xff;
  return ensure;
}

// Read number of valid fingerprint templates - PS_ValidTempleteNum
// Function: Read the number of valid fingerprint templates
// Parameter: None
// Return: confirmation code + number of valid templates (ValidN)
u8 fingerEventInterFace::PS_ValidTempleteNum(u16 *ValidN) {
  u16 temp;
  u8 ensure;
  u8 *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);  // command packet flag
  SendLength(0x03);
  Sendcmd(0x1d);
  temp = 0x01 + 0x03 + 0x1d;
  SendCheck(temp);
  data = JudgeStr(2000);
  if (data) {
    ensure = data[9];
    *ValidN = (data[10] << 8) + data[11];
  } else
    ensure = 0xff;

  if (ensure == 0x00) {
    printf("\r\nValid fingerprint count = %d", (data[10] << 8) + data[11]);
  } else
    printf("\r\n%s", EnsureMessage(ensure));
  return ensure;
}

// Handshake with AS608 - PS_HandShake
// Parameter: PS_Addr (pointer to address)
// Description: The module returns its current (correct) address
u8 fingerEventInterFace::PS_HandShake(u32 *PS_Addr) {
  SendHead();
  SendAddr();
  MYUSART_SendData(0X01);
  MYUSART_SendData(0X00);
  MYUSART_SendData(0X00);
  usleep(200 * 1000);
  // if (USART_RX_STA & 0X8000) // data received
  if (USART_RX_STA == 1) {
    USART_RX_STA = 0;
    // Check if this is a response packet from the module
    if (USART_RX_BUF[0] == 0XEF && USART_RX_BUF[1] == 0X01 &&
        USART_RX_BUF[6] == 0X07) {
      *PS_Addr = (USART_RX_BUF[2] << 24) + (USART_RX_BUF[3] << 16) +
                 (USART_RX_BUF[4] << 8) + (USART_RX_BUF[5]);
      return 0;
    }
  }
  return 1;
}

// Parse response confirmation code - EnsureMessage
// Function: Translate confirmation code to readable error message
// Parameter: ensure
const char *fingerEventInterFace::EnsureMessage(u8 ensure) {
  const char *p;
  switch (ensure) {
    case 0x00:
      p = "OK";
      break;
    case 0x01:
      p = "data package receive error";
      break;
    case 0x02:
      p = "no finger";
      break;
    case 0x03:
      p = "store finger failed";
      break;
    case 0x04:
      p = "finger is too shallow or dry";
      break;
    case 0x05:
      p = "finger is too deep or moist";
      break;
    case 0x06:
      p = "finger image is too chaos";
      break;
    case 0x07:
      p = "There are too few fingerprint features";
      break;
    case 0x08:
      p = "Fingerprints don't match";
      break;
    case 0x09:
      p = "No fingerprints were found";
      break;
    case 0x0a:
      p = "Feature merge failed";
      break;
    case 0x0b:
      p = "The address number is out of range";
      break;
    case 0x10:
      p = "Failed to delete the template";
      break;
    case 0x11:
      p = "Failed to clear the fingerprint database";
      break;
    case 0x15:
      p = "There is no valid map in the buffer";
      break;
    case 0x18:
      p = "Error reading and writing FLASH";
      break;
    case 0x19:
      p = "Undefined errors";
      break;
    case 0x1a:
      p = "Invalid register number";
      break;
    case 0x1b:
      p = "The contents of the register are incorrect";
      break;
    case 0x1c:
      p = "Notepad page numbering is incorrect";
      break;
    case 0x1f:
      p = "The fingerprint bank is full";
      break;
    case 0x20:
      p = "The address is incorrect";
      break;
    default:
      p = "The return confirmation code is incorrect";
      break;
  }
  return p;
}
// Display confirmation code error message
void fingerEventInterFace::ShowErrMessage(u8 ensure) {
  printf("show ensure message : %d\r\n", ensure);
}

// Add fingerprint
void fingerEventInterFace::Add_FR(void) {
  u8 i, ensure, processnum = 0;
  u8 ID_NUM = 0;
  while (1) {
    switch (processnum) {
      case 0:
        i++;
        printf("please press finger\r\n");
        ensure = PS_GetImage();
        if (ensure == 0x00) {
          ensure = PS_GenChar(CharBuffer1);  // Generate feature
          if (ensure == 0x00) {
            printf("finger is right\r\n");
            i = 0;
            processnum = 1;  // Proceed to step 2
          } else
            ShowErrMessage(ensure);
        } else
          ShowErrMessage(ensure);
        break;

      case 1:
        i++;
        printf("please press again\r\n");
        ensure = PS_GetImage();
        if (ensure == 0x00) {
          ensure = PS_GenChar(CharBuffer2);  // Generate second feature
          if (ensure == 0x00) {
            printf("finger is right, second\r\n");
            i = 0;
            processnum = 2;  // Proceed to step 3
          } else
            ShowErrMessage(ensure);
        } else
          ShowErrMessage(ensure);
        break;

      case 2:
        printf("compare two finger\r\n");
        ensure = PS_Match();
        if (ensure == 0x00) {
          printf("compare success\r\n");
          processnum = 3;  // Proceed to step 4
        } else {
          printf("compare failed\r\n");
          ShowErrMessage(ensure);
          i = 0;
          processnum = 0;  // Go back to step 1
        }
        usleep(500 * 1000);
        break;

      case 3:
        printf("generate finger template ...\r\n");
        usleep(500 * 1000);
        ensure = PS_RegModel();
        if (ensure == 0x00) {
          printf("generate finger template success\r\n");
          processnum = 4;  // Proceed to step 5
        } else {
          processnum = 0;
          ShowErrMessage(ensure);
        }
        usleep(1000 * 1000);
        break;

      case 4:
        // OLED_ShowCH(0, 0, " Press K4 to increase, K2 to decrease ");
        // OLED_ShowCH(0, 2, "    Press K3 to save    ");
        // OLED_ShowCH(0, 4, "  0 <= ID <= 99   ");
        while (key_num != 3) {
          // key_num = KEY_Scan(0);
          if (key_num == 2) {
            key_num = 0;
            if (ID_NUM > 0) ID_NUM--;
          }
          if (key_num == 4) {
            key_num = 0;
            if (ID_NUM < 99) ID_NUM++;
          }
          // OLED_ShowCH(40, 6, "ID=");
          // OLED_ShowNum(65, 6, ID_NUM, 2, 1);
        }
        key_num = 0;
        ensure = PS_StoreChar(CharBuffer2, ID_NUM);  // Store template
        if (ensure == 0x00) {
          printf("Add finger success\r\n");
          usleep(1500 * 1000);
          showMenu();
          return;
        } else {
          // OLED_Clear();
          processnum = 0;
          ShowErrMessage(ensure);
        }
        break;
    }
    usleep(400 * 1000);
    if (i == 10)  // Exit if finger not pressed after 5 attempts
    {
      break;
    }
  }
}

// Verify fingerprint
void fingerEventInterFace::press_FR(void) {
  SearchResult seach;
  u8 ensure;
  char str[20];
  while (key_num != 1) {
    // key_num = KEY_Scan(0);
    ensure = PS_GetImage();
    if (ensure == 0x00)  // Successfully captured image
    {
      ensure = PS_GenChar(CharBuffer1);
      if (ensure == 0x00)  // Successfully generated feature
      {
        ensure = PS_HighSpeedSearch(CharBuffer1, 0, 99, &seach);
        if (ensure == 0x00)  // Successfully found match
        {
          printf("verification success\r\n");
          sprintf(str, " ID:%d score:%d ", seach.pageID, seach.mathscore);
          printf("%s\r\n", str);
          usleep(1500 * 1000);
          usleep(1500 * 1000);
        } else {
          printf("verification failed\r\n");
          usleep(1500 * 1000);
        }
      } else {
      };
      // OLED_Clear();
      // OLED_ShowCH(32, 2, "please press finger");
      printf("please press finger\r\n");
    }
  }
  showMenu();
}

// Delete fingerprint
void fingerEventInterFace::Del_FR(void) {
  u8 ensure;
  u16 ID_NUM = 0;
  // OLED_ShowCH(0, 0, "K4 +  K2 -  K3 Confirm");
  // OLED_ShowCH(0, 2, "  K5 Clear all  ");
  // OLED_ShowCH(0, 4, "K1 Back  0 <= ID <= 99");
  while (key_num != 3) {
    // key_num = KEY_Scan(0);
    if (key_num == 2) {
      key_num = 0;
      if (ID_NUM > 0) ID_NUM--;
    }
    if (key_num == 4) {
      key_num = 0;
      if (ID_NUM < 99) ID_NUM++;
    }
    if (key_num == 1) goto MENU;  // Return to main menu
    if (key_num == 5) {
      key_num = 0;
      ensure = PS_Empty();  // Clear fingerprint database
      if (ensure == 0) {
        // OLED_Clear();
        // OLED_ShowCH(0, 2, " Clear success ");
      } else
        ShowErrMessage(ensure);
      usleep(1500 * 1000);
      goto MENU;  // Return to main menu
    }
    // OLED_ShowCH(40, 6, "ID=");
    // OLED_ShowNum(65, 6, ID_NUM, 2, 1);
  }
  ensure = PS_DeletChar(ID_NUM, 1);  // Delete single fingerprint
  if (ensure == 0) {
    // OLED_Clear();
    // OLED_ShowCH(0, 2, " Delete success ");
  } else
    ShowErrMessage(ensure);
  usleep(1500 * 1000);
MENU:
  showMenu();
  key_num = 0;
}

// Display function menu
void fingerEventInterFace::showMenu() {
  printf("===========================================\r\n");
  printf("14 : verification finger\r\n");
  printf("15 : delete finger \r\n");
  printf("16 : add finger \r\n");
}
