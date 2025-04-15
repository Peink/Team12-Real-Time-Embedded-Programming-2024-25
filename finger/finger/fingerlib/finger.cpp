#include "finger.hpp"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define RX_LEN 2048
#define TX_LEN 2048
u32 AS608Addr = 0XFFFFFFFF;  // setting
char str2[6] = {0};

/// @brief ///
uint8_t key_num = 3;
static u8 USART_RX_BUF[RX_LEN] = {0};
static u8 USART_RX_STA = 0;
static char USART_TX_BUF[1] = {0};

static fingerEventInterFace *fingerEventInterFace_ = nullptr;

fingerEventInterFace::fingerEventInterFace() { fingerEventInterFace_ = this; }

// send a bite to the serial
static void MYUSART_SendData(u8 data) {
  // while((USART3->SR & 0X40) == 0);
  // USART3->DR = data;
  USART_TX_BUF[0] = data;
  fingerEventInterFace_->UartSendData_(USART_TX_BUF, 1);
}
// send to frame header
static void SendHead(void) {
  MYUSART_SendData(0xEF);
  MYUSART_SendData(0x01);
}
// send address
static void SendAddr(void) {
  MYUSART_SendData(AS608Addr >> 24);
  MYUSART_SendData(AS608Addr >> 16);
  MYUSART_SendData(AS608Addr >> 8);
  MYUSART_SendData(AS608Addr);
}
// send Packet Identifier
static void SendFlag(u8 flag) { MYUSART_SendData(flag); }
// send frame header length
static void SendLength(int length) {
  MYUSART_SendData(length >> 8);
  MYUSART_SendData(length);
}
// send command code 
static void Sendcmd(u8 cmd) { MYUSART_SendData(cmd); }
// send checksum
static void SendCheck(u16 check) {
  MYUSART_SendData(check >> 8);
  MYUSART_SendData(check);
}
// Determine whether the interrupt-received buffer has a response packet
// waittime is the time to wait for data reception (in ms)
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
    // if(USART_RX_STA & 0X8000) //Receiving once data
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

// Enroll fingerprint image PS_GetImage
// Function: Detect if finger is pressed, and save fingerprint image to ImageBuffer
// Return confirmation code from moduleu8 fingerEventInterFace::PS_GetImage(void) {
  u16 temp;
  u8 ensure;
  u8 *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);  // command Packet Identifier
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
// Generate fingerprint feature PS_GenChar
// Function: Convert image in ImageBuffer into feature file, store in CharBuffer1 or CharBuffer2
// Param: BufferID ¡ú CharBuffer1: 0x01 or CharBuffer2: 0x02
// Return confirmation code
u8 fingerEventInterFace::PS_GenChar(u8 BufferID) {
  u16 temp;
  u8 ensure;
  u8 *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);  // command packet identifier
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
// Match two fingerprint features PS_Match
// Function: Precisely compare features in CharBuffer1 and CharBuffer2
// Return confirmation code
u8 fingerEventInterFace::PS_Match(void) {
  u16 temp;
  u8 ensure;
  u8 *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);  // command indentifier
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
// Search fingerprint PS_Search
// Function: Search the fingerprint library using features from CharBuffer1 or CharBuffer2
// Return: confirmation code and matched page ID if found
u8 fingerEventInterFace::PS_Search(u8 BufferID, u16 StartPage, u16 PageNum,
                                   SearchResult *p) {
  u16 temp;
  u8 ensure;
  u8 *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);  // command identifier
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
// Merge fingerprint features into template PS_RegModel
// Function: Merge features in CharBuffer1 and CharBuffer2 into a template
// Return confirmation code
u8 fingerEventInterFace::PS_RegModel(void) {
  u16 temp;
  u8 ensure;
  u8 *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);  // command identifier
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
// Store template PS_StoreChar
// Function: Store template from CharBuffer1 or CharBuffer2 into flash at PageID
// Param: BufferID and PageID
// Return confirmation code
u8 fingerEventInterFace::PS_StoreChar(u8 BufferID, u16 PageID) {
  u16 temp;
  u8 ensure;
  u8 *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);  // command identifier
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
// Delete template PS_DeletChar
// Function: Delete N templates starting from PageID
// Return confirmation code
u8 fingerEventInterFace::PS_DeletChar(u16 PageID, u16 N) {
  u16 temp;
  u8 ensure;
  u8 *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);  // command identifier
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
// Clear fingerprint database PS_Empty
// Function: Delete all fingerprint templates from flash
// Return confirmation code
u8 fingerEventInterFace::PS_Empty(void) {
  u16 temp;
  u8 ensure;
  u8 *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);  // command identifier
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
// Write system register PS_WriteReg
// Function: Write module register
// Param: RegNum: 4/5/6
// Return confirmation code
u8 fingerEventInterFace::PS_WriteReg(u8 RegNum, u8 DATA) {
  u16 temp;
  u8 ensure;
  u8 *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);  // command identifier
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
    printf("\r\n setting successful£¡");
  else
    printf("\r\n%s", EnsureMessage(ensure));
  return ensure;
}
// Read system parameters PS_ReadSysPara
// Function: Read module¡¯s basic parameters (baud rate, packet size, etc.)
// Return confirmation code and 16 bytes of system info
u8 fingerEventInterFace::PS_ReadSysPara(SysPara *p) {
  u16 temp;
  u8 ensure;
  u8 *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);  // command identifier
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
    printf("\r\n  maximum volumm of the finger module=%d", p->PS_max);
    printf("\r\n  comparing levels=%d", p->PS_level);
    printf("\r\n address=%x", p->PS_addr);
    printf("\r\n bord rate=%d", p->PS_N * 9600);
  } else
    printf("\r\n%s", EnsureMessage(ensure));
  return ensure;
}
// Set module address PS_SetAddr
// Function: Set fingerprint module address
// Param: PS_addr
// Return confirmation code
u8 fingerEventInterFace::PS_SetAddr(u32 PS_addr) {
  u16 temp;
  u8 ensure;
  u8 *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);  // command identifier
  SendLength(0x07);
  Sendcmd(0x15);
  MYUSART_SendData(PS_addr >> 24);
  MYUSART_SendData(PS_addr >> 16);
  MYUSART_SendData(PS_addr >> 8);
  MYUSART_SendData(PS_addr);
  temp = 0x01 + 0x07 + 0x15 + (u8)(PS_addr >> 24) + (u8)(PS_addr >> 16) +
         (u8)(PS_addr >> 8) + (u8)PS_addr;
  SendCheck(temp);
  AS608Addr = PS_addr;  // sendig command finished, change address
  data = JudgeStr(2000);
  if (data)
    ensure = data[9];
  else
    ensure = 0xff;
  AS608Addr = PS_addr;
  if (ensure == 0x00)
    printf("\r\n address setting successful£¡");
  else
    printf("\r\n%s", EnsureMessage(ensure));
  return ensure;
}
// PS_WriteNotepad
// Function: Write 32 bytes of data into 1 of 16 user note pages (total 256 bytes flash)
// Param: Page number (0~15), Byte32 (32 bytes of data)
// Return confirmation code
u8 fingerEventInterFace::PS_WriteNotepad(u8 NotePageNum, u8 *Byte32) {
  u16 temp;
  u8 ensure, i;
  u8 *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);  // command identifier
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
// Read notepad PS_ReadNotepad
// Function: Read 32 bytes of user data from specified notepad page
// Param: NotePageNum (0~15)
// Return confirmation code and user data
u8 fingerEventInterFace::PS_ReadNotepad(u8 NotePageNum, u8 *Byte32) {
  u16 temp;
  u8 ensure, i;
  u8 *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);  // command identifier
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
// High-speed search PS_HighSpeedSearch
// Function: Search entire fingerprint library quickly using CharBuffer1/2
// Return confirmation code and matched page ID
// BufferID£¬
// StartPage£¬PageNum illustrate:  module retern confirm bite+Page num£¨matching finger module £©
u8 fingerEventInterFace::PS_HighSpeedSearch(u8 BufferID, u16 StartPage,
                                            u16 PageNum, SearchResult *p) {
  u16 temp;
  u8 ensure;
  u8 *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);  // command identifier
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
// Read number of valid templates PS_ValidTempleteNum
// Function: Get number of valid fingerprint templates
// Return confirmation code and number of valid templates
u8 fingerEventInterFace::PS_ValidTempleteNum(u16 *ValidN) {
  u16 temp;
  u8 ensure;
  u8 *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);  // command identifier
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
    printf("\r\n numbers of valid finger prints=%d", (data[10] << 8) + data[11]);
  } else
    printf("\r\n%s", EnsureMessage(ensure));
  return ensure;
}
// Handshake with AS608 PS_HandShake
// Param: pointer to PS_Addr
// Return: confirmed address
u8 fingerEventInterFace::PS_HandShake(u32 *PS_Addr) {
  SendHead();
  SendAddr();
  MYUSART_SendData(0X01);
  MYUSART_SendData(0X00);
  MYUSART_SendData(0X00);
  usleep(200 * 1000);
  // if(USART_RX_STA & 0X8000) //receiving data
  // {
  if (USART_RX_STA == 1) {
    USART_RX_STA = 0;
    if (  // identify if it is the correct responding packet
        USART_RX_BUF[0] == 0XEF && USART_RX_BUF[1] == 0X01 &&
        USART_RX_BUF[6] == 0X07) {
      *PS_Addr = (USART_RX_BUF[2] << 24) + (USART_RX_BUF[3] << 16) +
                 (USART_RX_BUF[4] << 8) + (USART_RX_BUF[5]);
      // USART_RX_STA = 0;
      return 0;
    }
  }
  // USART_RX_STA = 0;
  // }
  return 1;
}
// Parse confirmation code PS_EnsureMessage
// Function: Return human-readable error message based on confirmation code
const char *fingerEventInterFace::EnsureMessage(u8 ensure) {
  const char *p;
  switch (ensure) {
    case 0x00:
      p = "       OK       ";
      break;
    case 0x01:
      p = " erros of data sending package ";
      break;
    case 0x02:
      p = "no finger on the sensor";
      break;
    case 0x03:
      p = "finger print wrong";
      break;
    case 0x04:
      p = " finger too dry or too wet ";
      break;
    case 0x05:
      p = " finger too wet or not clear ";
      break;
    case 0x06:
      p = "  finger too mess  ";
      break;
    case 0x07:
      p = " finger characters not clear ";
      break;
    case 0x08:
      p = "  finger not match    ";
      break;
    case 0x09:
      p = " no finger searched ";
      break;
    case 0x0a:
      p = "   failed finger combine ";
      break;
    case 0x0b:
      p = "adress numbers beyonded";
    case 0x10:
      p = "  failed deleted module  ";
      break;
    case 0x11:
      p = " failed clear finger print data ";
      break;
    case 0x15:
      p = "no valid pictures in saturated part";
      break;
    case 0x18:
      p = " FLASH wrong  ";
      break;
    case 0x19:
      p = "   no identified wrong   ";
      break;
    case 0x1a:
      p = "  invalid storage  ";
      break;
    case 0x1b:
      p = " storage content wrong ";
      break;
    case 0x1c:
      p = " page of note wrong ";
      break;
    case 0x1f:
      p = "    full finger print data    ";
      break;
    case 0x20:
      p = "    wrong address    ";
      break;
    default:
      p = " wrong return confirm ";
      break;
  }
  return p;
}

//Display error message
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
        ensure = PS_GetImage();
        if (ensure == 0x00) {
          ensure = PS_GenChar(CharBuffer1);  // create feature
          if (ensure == 0x00) {
            i = 0;
            processnum = 1;  // jump into second step
          } else
            ShowErrMessage(ensure);
        } else
          ShowErrMessage(ensure);
        break;

      case 1:
        i++;
        // OLED_ShowCH(0, 2, "   try agian   ");
        // OLED_ShowCH(0, 4, "                ");
        ensure = PS_GetImage();
        if (ensure == 0x00) {
          ensure = PS_GenChar(CharBuffer2);  // create feature
          if (ensure == 0x00) {
            // OLED_ShowCH(0, 2, "    normal finger print    ");
            // OLED_ShowCH(0, 4, "                ");
            i = 0;
            processnum = 2;  // jump into step three
          } else
            ShowErrMessage(ensure);
        } else
          ShowErrMessage(ensure);
        break;

      case 2:
        // OLED_ShowCH(0, 2, "  compare two finger print  ");
        // OLED_ShowCH(0, 4, "                ");
        ensure = PS_Match();
        if (ensure == 0x00) {
          // OLED_ShowCH(0, 2, "    match successful    ");
          // OLED_ShowCH(0, 4, "                ");
          processnum = 3;  // jump into step four
        } else {
          // OLED_ShowCH(0, 2, "    wrong match    ");
          // OLED_ShowCH(0, 4, "                ");
          ShowErrMessage(ensure);
          i = 0;
          processnum = 0;  // return to step one
        }
        usleep(500 * 1000);
        break;

      case 3:
        // OLED_ShowCH(0, 2, "  create finger module  ");
        // OLED_ShowCH(0, 4, "                ");
        usleep(500 * 1000);
        ensure = PS_RegModel();
        if (ensure == 0x00) {
          // OLED_ShowCH(0, 2, "create finger module successful");
          // OLED_ShowCH(0, 4, "                ");
          processnum = 4;  // jump to step five
        } else {
          processnum = 0;
          ShowErrMessage(ensure);
        }
        usleep(1000 * 1000);
        break;

      case 4:
        // OLED_ShowCH(0, 0, " pressK4plus,pressK2minus ");
        // OLED_ShowCH(0, 2, "    pressK3reserve    ");
        // OLED_ShowCH(0, 4, "  0=< ID <=99   ");
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
        ensure = PS_StoreChar(CharBuffer2, ID_NUM);  // storage module
        if (ensure == 0x00) {
          // OLED_Clear();
          // OLED_ShowCH(0, 2, "  finger print successful  ");
          // OLED_ShowCH(0, 4, "                ");
          usleep(1500 * 1000);
          // OLED_Clear();
          // OLED_ShowCH(0, 0, "finger print module test");
          // OLED_ShowCH(16, 2, "K1 add finger");
          // OLED_ShowCH(16, 4, "K3 delete finger");
          // OLED_ShowCH(16, 6, "K5 identify finger");
          return;
        } else {
          // OLED_Clear();
          processnum = 0;
          ShowErrMessage(ensure);
        }
        break;
    }
    usleep(400 * 1000);
    if (i == 10)  // beyond5times no finger then exit
    {
      break;
    }
  }
}

// new finger
void fingerEventInterFace::press_FR(void) {
  SearchResult seach;
  u8 ensure;
  char str[20];
  while (key_num != 1) {
    // key_num = KEY_Scan(0);
    ensure = PS_GetImage();
    if (ensure == 0x00)  // capture figure successful
    {
      ensure = PS_GenChar(CharBuffer1);
      if (ensure == 0x00)  // create feature successful
      {
        ensure = PS_HighSpeedSearch(CharBuffer1, 0, 99, &seach);
        if (ensure == 0x00)  // searching successful
        {
          // OLED_ShowCH(0, 2, "  finger print successful  ");
          sprintf(str, " ID:%d score :%d ", seach.pageID, seach.mathscore);
          // OLED_ShowCH(0, 4, (u8 *)str);
          usleep(1500 * 1000);
          usleep(1500 * 1000);
        } else {
          // OLED_ShowCH(32, 2, "failed identify");
          usleep(1500 * 1000);
        }
      } else {
      };
      // OLED_Clear();
      // OLED_ShowCH(32, 2, "press finger");
    }
  }
  // OLED_Clear();
  // OLED_ShowCH(0, 0, "finger print test");
  // OLED_ShowCH(16, 2, "K1 add finger");
  // OLED_ShowCH(16, 4, "K3 delete finger");
  // OLED_ShowCH(16, 6, "K5 identify finger");
}

// delete finger
void fingerEventInterFace::Del_FR(void) {
  u8 ensure;
  u16 ID_NUM = 0;
  // OLED_ShowCH(0, 0, "K4plus K2minus K3confirm");
  // OLED_ShowCH(0, 2, "  K5clear finger database  ");
  // OLED_ShowCH(0, 4, "K1return 0=<ID<=99");
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
    if (key_num == 1) goto MENU;  // return to home
    if (key_num == 5) {
      key_num = 0;
      ensure = PS_Empty();  // clear finger database
      if (ensure == 0) {
        // OLED_Clear();
        // OLED_ShowCH(0, 2, " clear finger data successful ");
      } else
        ShowErrMessage(ensure);
      usleep(1500 * 1000);
      goto MENU;  // return to homepage
    }
    // OLED_ShowCH(40, 6, "ID=");
    // OLED_ShowNum(65, 6, ID_NUM, 2, 1);
  }
  ensure = PS_DeletChar(ID_NUM, 1);  // delete single print
  if (ensure == 0) {
    // OLED_Clear();
    // OLED_ShowCH(0, 2, "  delete successful  ");
  } else
    ShowErrMessage(ensure);
  usleep(1500 * 1000);
MENU:
  // OLED_Clear();
  // OLED_ShowCH(0, 0, "finger print module successful");
  // OLED_ShowCH(16, 2, "K1add finger");
  // OLED_ShowCH(16, 4, "K3delet finger");
  // OLED_ShowCH(16, 6, "K5indetify finger");
  key_num = 0;
}
