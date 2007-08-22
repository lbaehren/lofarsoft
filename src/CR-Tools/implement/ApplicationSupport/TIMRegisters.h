
#ifndef TIMREGISTERS_H
#define TIMREGISTERS_H

#include <linux/types.h>

/*!
  \class TIMRegisters

  \ingroup ApplicationSupport

  \brief Registers of the TIM device.

  <h3>Address map TIM ACEX</h3>
  
  \verbatim
  // Control: 
  // 0x00000000  0:7 Mode Register           8:31 unused  
  // 0x00000004  0:7 Control Register        8:31 unused
  // 0x00000008  0:7 Segment Register        8:31 unused
  // 0x0000000C  0:7 Clock Register          8:31 unused
  // 0x00000010  0:7 Addresscounter Byte 0   8:31 unused
  // 0x00000014  0:7 Addresscounter Byte 1   8:31 unused
  // 0x00000018  0:7 Addresscounter Byte 2   8:31 unused
  // 0x0000001C  0:7 Addresscounter Byte 3   8:31 unused
  // 0x00000020  0:7 Version Register        8:31 unused
  // 0x00000024  0:7 Startdelay Register     8:31 unused
  
  // Memory segment:
  // 0x01000000 - 0x01fffffc
  \endverbatim
  
  <h3>Modes</h3>
  
  \verbatim
  mode(0:1)= 00 Capture mode starts as a given address
  mode(2)  = 0   DQM mode
  mode(3)  = 0  Quiet mode
  mode(4:7)= 0000 Block length  1B
  \endverbatim
*/

#define TIM_MODEREG 0x0
#define TIM_CTRLREG 0x4
#define TIM_SEGREG  0x8
#define TIM_CLKREG  0xC
#define TIM_ADRREG0 0x10
#define TIM_ADRREG1 0x14
#define TIM_ADRREG2 0x18
#define TIM_ADRREG3 0x1C
#define TIM_VERSREG 0x20
#define TIM_SDELAYREG 0x24

class TIMRegisters {

  int fd;
  unsigned segmentreg, modereg, clockreg, versionreg;
  unsigned segmentsize, base_address;
  unsigned long prefetchcounter;
  
 public:
  enum blocklength {B32=0, B64, B256, B2K, B4K, B32K, 
		    B64K, B1M, B2M, B4M, B32M, B64M, 
		    B256M, B512M, B1G, B2G};  
  static const unsigned int  blocksize[];
  
  //! Argumented constructor
  TIMRegisters (const char *dev = "/dev/tim0");
  //! Argumented constructor
  TIMRegisters (int open_fd);
  //! Destructor
  ~TIMRegisters ();
  
  int cwrite(__u32 address, void* buf, unsigned size);
  int cread(__u32 address, void* buf, unsigned size);
  int write(__u32 address, void* buf, unsigned size);
  int read(__u32 address, void* buf, unsigned size);

  void setfd(int open_fd);
  
  /*!
    This method reads from TIM assuming the startaddress is aligned
    to a 64 byte boundary. This means we can (nearly always) use DMA
    reads, which should be faster than a normal TIM::read(). It also
    means that the userspace program has to provide an address which
    is aligned to the 64 byte boundary and a size which is a
    multiple of 64 bytes.
  */
  int read_aligned(__u32 address, void* buf, unsigned size);
  int _read(__u32 address, void* buf, unsigned size);
  int Segmentreg(__u32 seg);
  int Segmentreg(void);
  int Modereg(__u32 mode);
  int Modereg(void);
  int Clockreg(__u32 reg);
  int Clockreg(void);
  int Controlreg(__u32 reg);
  int Controlreg(void);
  int Startdelayreg(__u32 reg);
  int Startdelayreg(void);
  
  //! Set Blocklength
  int Blocklength(enum blocklength b);
  //! Get Blocklength
  int Blocklength(void);
  int Initram(void);
  void prefcnt(int cnt);
  int intcsr();
  void startbist();
  int  bist();
  
  int Versionreg(void);
  int SDelayreg(void);
  //! Show TIM Register settings
  void ShowRegs(void);
  unsigned long Addressreg(void);
  unsigned long Syncaddress(void);
  
  int getfd(void) { return fd; }
  
 private:
  void init();
};

#endif
