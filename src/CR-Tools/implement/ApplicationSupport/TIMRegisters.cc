
#include <unistd.h>
#include <sys/types.h>
#include <linux/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <iostream>     // <iostream.h>
#include <fstream>      // <stream.h>
#include <iomanip>      // <iomanip.h>
#include <sys/ioctl.h>

#include <ApplicationSupport/tim.h>
#include <ApplicationSupport/TIMRegisters.h>

#define TIM_DEBUG

using namespace std;

const unsigned int TIMRegisters::blocksize[] = {32,
						64,
						256,
						2*1024,
						4*1024,
						32*1024,
						64*1024,
						1024*1024,
						2*1048576,
						4*1048576,
						32*1048576,
						64*1048576,
						256*1048576,
						512*1048576,
						1024*1048576,
						2*1073741824};

// ==============================================================================
//
//  Construction
//
// ==============================================================================

TIMRegisters::TIMRegisters(const char *dev)
{
  if (dev)
  {
    fd = open(dev, O_RDWR);
    if (fd <= 0) {
	cerr << "Error opening device: " << dev << endl;
        exit(-1);
    }

    init();
  }
}

TIMRegisters::TIMRegisters(int open_fd)
  : segmentreg(0),
    modereg(0),
    clockreg(0),
    versionreg(0),
    segmentsize(0),
    base_address(0),
    prefetchcounter(0)
{
  if (open_fd)
  {
    fd = open_fd;
    init();
  }
}

// ==============================================================================
//
//  Destruction
//
// ==============================================================================

TIMRegisters::~TIMRegisters()
{
   close(fd);
}

// ==============================================================================
//
//  Parameter access
//
// ==============================================================================

int TIMRegisters::cwrite(__u32 addr,
			 void* buf,
			 unsigned size)
{
  // Configuration writes go to base address 0
  // ioctl(fd, TIM_IOPREFCNT, 1);
  // cout << "cwrite address=0x" << hex << addr << endl;
  lseek(fd,addr, SEEK_SET);
  ::write(fd, buf, size);
  // ioctl(fd, TIM_IOPREFCNT, prefetchcounter);
  return 0;
}

int TIMRegisters::cread(__u32 addr,
			void* buf,
			unsigned size)
{
  // Configuration reads go to base address 0
  ioctl(fd, TIM_IOPREFCNT, 1);
  // cout << "cread address=0x" << hex << addr << endl;
  lseek(fd,addr, SEEK_SET);
  ::read(fd, buf, size);
  ioctl(fd, TIM_IOPREFCNT, 0);//prefetchcounter);
  return 0;
}

int TIMRegisters::write(__u32 addr,
			void* buf,
			unsigned size)
{
  unsigned nsegment = addr / segmentsize;
  if (nsegment != segmentreg) {
    Segmentreg(nsegment);
#ifdef TIM_DEBUG
    cerr << "Write: Segmentreg=" << segmentreg << endl;
#endif
  }
  
  addr = (addr % segmentsize) + base_address;
  
  lseek(fd,addr, SEEK_SET);
  ::write(fd, buf, size);
  return size;
}

int TIMRegisters::_read(__u32 addr,
			void* buf,
			unsigned size)
{
  unsigned nsegment = addr / segmentsize;
  if (nsegment != segmentreg) {
    Segmentreg(nsegment);
#ifdef TIM_DEBUG
    cerr << "Read: Segmentreg=" << segmentreg << endl;
#endif
  }
  // Make sure that we are not going to read over a segment
  // boundary
  addr = (addr % segmentsize);
  if ( addr+size > 16*1024*1024 ) {
    size = 16*1024*1024 - addr;
    cerr << "Clipped size to size=" << size << endl;
  }
  
  addr += base_address;
  lseek(fd,addr, SEEK_SET);
  ::read(fd, buf, size);
  return size;
}

#if 0 
int TIMRegisters::read(__u32 addr,
		       void* buf,
		       unsigned size)
{
  unsigned int nsegment, tsize, xfered=0;
  __u8 tmpbuff[64]; 
  __u32 taddr, diff;
  
  // Make shore we continue reading at a 64-byte boundary
  diff = addr & 0x3f;
  nsegment = addr / segmentsize;
  if (nsegment != segmentreg) {
    Segmentreg(nsegment);
  }
  taddr = (addr % segmentsize);
  lseek(fd, (taddr+ base_address), SEEK_SET);
  ::read(fd, tmpbuff, 0x40); 
  xfered = (0x40-diff);
  memcpy(buf, ((char*)tmpbuff+diff), xfered);
  //read the data in blocks, that fit into a segment
  while (size>xfered) {
    nsegment = (addr+xfered) / segmentsize;
    if (nsegment != segmentreg) {
      Segmentreg(nsegment);
#ifdef TIM_DEBUG
      cerr << "Read: Segmentreg=" << segmentreg << endl;
#endif
    }
    
    taddr = ((addr+xfered) % segmentsize);
    lseek(fd, (taddr+ base_address), SEEK_SET);
    
    tsize = size-xfered;
    if ( (tsize+taddr) > segmentsize ) tsize = segmentsize-taddr;  
    ::read(fd, ((char*)buf+xfered), tsize ); 
    xfered += tsize;
  };
  return xfered;
}
#endif

int TIMRegisters::read(__u32 addr, void* buf, unsigned size)
{
  unsigned int nsegment, tsize, xfered = 0;
  __u8 tmpbuff[64]; 
  __u32 taddr, diff;
  
  // Make sure we continue reading at a 64-byte boundary
  
  diff = addr & 0x3f;                     // offset from 64 byte boundary
  
  nsegment = addr / segmentsize;          // segment number 
  if (nsegment != segmentreg) {
    Segmentreg(nsegment); 
  }
  
  taddr = (addr % segmentsize) & ~0x3f;   // determine the 64 byte boundary from which to read
  
  if ( (__u32)lseek(fd, (taddr + base_address), SEEK_SET) != (taddr + base_address) ) { 
    cerr << "Read: lseek failed to find address." << endl;
  }
  
  if ( ::read(fd, tmpbuff, 0x40) != 0x40 ) {
    cerr << "Read: read from tim device failed." << endl;
  } 
  
  xfered = (0x40 - diff);
  memcpy(buf, ( (char*)tmpbuff + diff) , xfered);
  
  //read the data in blocks, that fit into a segment
  while ( size > xfered ) {
    
    // segment number
    nsegment = (addr + xfered) / segmentsize;
    if ( nsegment != segmentreg ) {
      Segmentreg(nsegment);
#ifdef TIM_DEBUG
      cerr << "Read: Segmentreg=" << segmentreg << endl;
#endif
    }
    
      // addr + xfered is already aligned to a 64 byte boundary
      taddr = ( (addr+xfered) % segmentsize );

      if ( (__u32)lseek(fd, (taddr + base_address), SEEK_SET) != (taddr + base_address) ) {
	cerr << "Read: lseek failed." << endl;
      }

      tsize = size - xfered;              // remaining read size

      if ( (tsize+taddr) > segmentsize ) {
	// remaining read exceeds the segment. Reduce size to segment border.
	tsize = segmentsize-taddr;  
      }

      //reduce tsize to the size of the kernel-modules IOBUFFER
      if (tsize > 131072) { tsize = 131072; }; 

      if ( (__u32)::read(fd, ((char*)buf+xfered), tsize) != tsize ) {
	cerr << "Read: read from tim device failed." << endl; 
      }
      xfered += tsize;
    }
    
    return xfered;
}

int TIMRegisters::read_aligned(__u32 addr, void* buf, unsigned size)
  /* This method reads from TIM assuming the startaddress is aligned
     to a 64 byte boundary. This means we can (nearly always) use DMA
     reads, which should be faster than a normal TIMRegisters::read(). It also
     means that the userspace program has to provide an address which
     is aligned to the 64 byte boundary and a size which is a
     multiple of 64 bytes.
  */
{
  unsigned int nsegment, tsize, xfered = 0;
  __u32 taddr;


  if ( (addr & 0x3f) != 0 ) {
    cerr << "Read_alligned: Read address not alligned to 64 byte boundary." << endl;
    return -1;
  } 
  
  if ( (size % 64) != 0 ) {
    cerr << "Read_alligned: Read size is not a multiple of 64 bytes." << endl;
    return -1;
  }

  while ( size > xfered ) {
    
    // segment number 
    nsegment = ( addr + xfered ) / segmentsize;
    if ( nsegment != segmentreg ) {

      Segmentreg(nsegment);
#ifdef TIM_DEBUG
      cerr << "Read_alligned: Segmentreg = " << segmentreg << endl;
#endif

    }

    taddr = ( (addr+xfered) % segmentsize );     // offset in the current segment
    tsize = ( size - xfered );                   // remaining read size
      
    if ( (__u32)lseek(fd, (taddr + base_address), SEEK_SET) != (taddr + base_address) ) {
      cerr << "Read_alligned: lseek failed." << endl;
    }
    
    if ( (tsize + taddr) > segmentsize ) { 
      // remaining read exceeds the segment. Reduce size to segment border.
      tsize = segmentsize - taddr;
    }

    if ( (__u32)::read(fd, ((char*)buf+xfered), tsize) != tsize ) {
      cerr << "Read_alligned: read from tim device failed." << endl;
    }

    xfered += tsize;
  }
  return xfered;
}

int TIMRegisters::Modereg(__u32 val) 
{
   cwrite(TIM_MODEREG, &val, 4);
   return modereg = val;
}

int TIMRegisters::Modereg(void) 
{
   __u32 val;
   cread(TIM_MODEREG, (void*)&val, 4);
   modereg = int(val & 0xff);
   return modereg;
}

int TIMRegisters::Controlreg(__u32 val) 
{
   cwrite(TIM_CTRLREG, &val, 4);
   return 4;
}

int TIMRegisters::Controlreg(void) 
{
    __u32 controlreg;
   cread(TIM_CTRLREG, &controlreg, 4);
   return int(controlreg & 0xf);
}


int TIMRegisters::Segmentreg(__u32 seg) 
{
   cwrite(TIM_SEGREG, &seg, 4);
   return segmentreg = seg;
}

int TIMRegisters::Segmentreg(void) 
{
   __u32 val;
   cread(TIM_SEGREG, (void*)&val, 4);
   segmentreg = int(val & 0xff);
   return segmentreg;
}

int TIMRegisters::Clockreg(__u32 val) 
{
   cwrite(TIM_CLKREG, &val, 4);
   return clockreg = val;
}

int TIMRegisters::Clockreg() 
{
   __u32 val;
   cread(TIM_CLKREG, (void*)&val, 4);
   clockreg = int(val & 0xff);
   return clockreg;
}

int TIMRegisters::Startdelayreg(__u32 val) 
{
   cwrite(TIM_SDELAYREG, &val, 4);
   return 4;
}

int TIMRegisters::Startdelayreg(void) 
{
    __u32 startdelayreg;
   cread(TIM_SDELAYREG, &startdelayreg, 4);
   return int(startdelayreg & 0xff);
}

int TIMRegisters::Blocklength(enum blocklength b) 
{
   modereg = (modereg & 0xf) | (b << 4); 
   __u32 val = modereg;
   cwrite(TIM_MODEREG, &val, 4);
   return b;
}

int TIMRegisters::Blocklength() 
{
   return modereg >> 4;
}

int TIMRegisters::Initram() 
{
   /*
      init RAM 
   */
   unsigned long val = 0x3;
   // Bit 1 clears a possible pending interrupt.
   cwrite(TIM_CTRLREG, &val, 4);
   return 0;
}

void TIMRegisters::prefcnt(int cnt) 
{
   prefetchcounter = cnt;
   ioctl(fd, TIM_IOPREFCNT, prefetchcounter);
}

int TIMRegisters::intcsr() 
{
    unsigned long reg = 0;
    return ioctl(fd, TIM_IOINTCSR, reg);
}

void TIMRegisters::startbist() 
{
    ioctl(fd, TIM_IOSETBIST);
}


int TIMRegisters::bist() 
{
    return ioctl(fd, TIM_IORDBIST);
}

void TIMRegisters::setfd(int open_fd)
{
  fd = open_fd;
  init();
}

void TIMRegisters::init()
{    
   prefetchcounter = 1;
   ioctl(fd, TIM_IOPREFCNT, prefetchcounter);


   // Initialize device with 
   // Capture mode 0
   // DQM = 1
   // blocklength 1
   //
   // blocklength(TIMRegisters::B1);
   // capturemode(0);
   // setdqm();

   // Initialize Registers from board
   //modereg = Modereg();
   // Initram();
   //segmentreg = Segmentreg();
   /*
     Clock register 0xC
     0..1	  	0x00= 40 MHz, 0x01=20 MHz, 0x10=10MHz
     2..3		Not used
     4		 0 = HSL write, 1 is PCI write
     5..7		Not used
   */
   //clockreg=Clockreg();
    // Configuration reads go to base address 0
   //prefetchcounter = 1;
   //ioctl(fd, TIM_IOPREFCNT, prefetchcounter);

   base_address = segmentsize=0x1000000;
   
   cread(TIM_VERSREG, (void*)&versionreg, 4);
   versionreg &= 0xff;
   if (versionreg < 10) {  
#ifdef TIM_DEBUG
     cout <<"Please update your TIM-board, version >= 10 is available"<< endl;
#endif
     if (versionreg < 7) {
       cerr << "Syncaddress calculation not valid for TIM-version <7!!! \n"; 
     };
   };

#ifdef TIM_DEBUG
   cout << "Init done " << endl;
#endif
}

void TIMRegisters::ShowRegs(void)
{

 cout << "TIM Register settings:" << endl << hex
      << "Mode register    = 0x" << setfill('0') << setw(2) 
      << Modereg() << endl;
 cout << "Control register = 0x" << setfill('0') << setw(2) 
      << Controlreg() << endl;
 cout << "Segment register = 0x" << setfill('0') << setw(2) 
      << Segmentreg() << endl;
 cout << "Clock register   = 0x" << setfill('0') << setw(2) 
                                 << Clockreg() << endl;
}

unsigned long TIMRegisters::Addressreg(void) 
{
   unsigned long address, adr0, adr1, adr2, adr3;
   cread(TIM_ADRREG3, (void*)&adr3, 4);
   cread(TIM_ADRREG2, (void*)&adr2, 4);
   cread(TIM_ADRREG1, (void*)&adr1, 4);
   cread(TIM_ADRREG0, (void*)&adr0, 4);

   address=(adr0 & 0x000000ffL);

   address+= ((adr1 << 8) & 0x0000ff00L);

   address+= ((adr2 << 16) & 0x00ff0000L);

   address+= ((adr3 << 24) & 0xff000000L);

   return address;
}

int TIMRegisters::Versionreg(void) 
{
    unsigned long version;
    
    cread(TIM_VERSREG, (void*)&version,4);
    version &= (0xff);
    return version;

}

int TIMRegisters::SDelayreg(void) 
{
   __u32 val1, val2;
   cread(TIM_SDELAYREG, (void*)&val1, 4);
   val1 &= 0xff;
   cread(TIM_ADRREG3, (void*)&val2, 4);
   val1 += ((val2 << 4) & 0xf00);
   return val1;
}

unsigned long TIMRegisters::Syncaddress(void) 
{
   unsigned long address, adr0, adr1, adr2, adr3, sdel;
   unsigned long syncaddress=0, delay=0;
   cread(TIM_ADRREG3, (void*)&adr3, 4);
   cread(TIM_ADRREG2, (void*)&adr2, 4);
   cread(TIM_ADRREG1, (void*)&adr1, 4);
   cread(TIM_ADRREG0, (void*)&adr0, 4);
   cread(TIM_SDELAYREG, (void*)&sdel, 4);

   address=(adr0 & 0x000000ffL);
   address+= ((adr1 << 8) & 0x0000ff00L);
   address+= ((adr2 << 16) & 0x00ff0000L);
   // address+= ((adr3 << 24) & 0xff000000L);
   address+= ((adr3 << 24) & 0x0f000000L);

   sdel &= 0xff;
   sdel += ((adr3 << 4) & 0xf00);

   if ((Modereg()&0x3) == 2) {
     if ((Clockreg()&0x3) == 0) {
       delay = ((sdel+2)%8)*4+4;
     } else if ((Clockreg()&0x3) == 1) {
       delay = ((sdel+4)%16)*4-4;
     } else if ((Clockreg()&0x3) == 2) {
       delay = ((sdel+8)%32)*4-20;
     };
     syncaddress = (address<<5)-blocksize[Blocklength()]-delay;
                 
   } else {
     cerr << "Error calculating syncaddress, mode != 2\n";
   };

   return syncaddress;
}




