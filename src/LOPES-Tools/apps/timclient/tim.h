/*
 * tim.h -- definitions for the tim device
 *
 * Copyright (C) 2002 Gideon W. Kant
 *
 */

/* 	$Id: tim.h,v 1.2 2005/05/09 09:27:41 bahren Exp $	 */


#ifndef _TIM_H_
#define _TIM_H_

/*!
  \file tim.h

  \brief Definitions for the tim device
 */

/* version dependencies have been confined to a separate file */
#include <plx9080.h>
#include <semaphore.h>

/*
 * Macros to help debugging
 */

#undef PDEBUG             /* undef it, just in case */
#ifdef TIM_DEBUG
#  ifdef __KERNEL__
     /* This one if debugging is on, and kernel space */
#    define PDEBUG(fmt, args...) printk( KERN_DEBUG "tim: " fmt, ## args)
#  else
     /* This one for user space */
#    define PDEBUG(fmt, args...) fprintf(stderr, fmt, ## args)
#  endif
#else
#  define PDEBUG(fmt, args...) /* not debugging: nothing */
#endif

#undef PDEBUGG
#define PDEBUGG(fmt, args...) /* nothing: it's a placeholder */

#ifndef TIM_MAJOR
#define TIM_MAJOR 0   /* dynamic major by default */
#endif

#define MAX_CARDS 5

#define TIM_NAME        "tim"
#define TIM_MAGIC       0x39303830      /* "9080" */
#define TIM_RW_TIMEOUT  (HZ * 5)        /* 5 sec. */


#ifdef CONFIG_DEVFS_FS /* only if enabled, to avoid errors in 2.0 */
#include <linux/devfs_fs_kernel.h>
#else
  typedef void * devfs_handle_t;  /* avoid #ifdef inside the structure */
#endif

/*
 * This is somehow a hack: avoid ifdefs in the cleanup code by declaring
 * an empty procedure as a placeholder for devfs_unregister. This is
 * only done *unless* <linux/devfs_fs_kernel.h> was included, as that
 * header already implements placeholder for all the devfs functions
 */
/*............................................... degin-tag devfs-ifdef */
#ifndef DEVFS_FL_DEFAULT
extern inline void devfs_unregister(devfs_handle_t de) {}
#endif

extern devfs_handle_t tim_devfs_dir;

/**************** ioctl *****************/

/*
 * Use 'k' as magic number 
 */
#define TIM_IOC_MAGIC           'k'
#define TIM_IOC_MAXNR           0xD0

#define TIM_IOCTL_BASE		0x00
#define TIM_UART_IOCTL_BASE	0xC0

/* general (from 0x00) */

#define TIM_IOPREFCNT  0x200
#define TIM_IOINTCSR   0x201
#define TIM_IORDBIST   0x202
#define TIM_IOSETBIST  0x203

#define TIM_IO9050RST _IO('k',(TIM_IOCTL_BASE + 0))
#define TIM_IOGETINTCSR _IOR('k',(TIM_IOCTL_BASE + 1),__u32)
#define TIM_IOGETCNTRL  _IOR('k',(TIM_IOCTL_BASE + 2),__u32)
#define TIMIOGETIRQTIME _IOR('k',(TIM_IOCTL_BASE + 3),plx_irq_time_stats)
#define TIMIOREAD       _IOR('k',(TIM_IOCTL_BASE + 4),struct plx_adpair)
#define TIMIOWRITE      _IOWR('k',(TIM_IOCTL_BASE + 5),struct plx_adpair)
#define TIMIOGETCONF    _IOR('k',(TIM_IOCTL_BASE + 6),struct plx_adpair)
#define TIMIOSETCONF    _IOWR('k',(TIM_IOCTL_BASE + 7),struct plx_adpair)
#define TIMIOGETNVRAM   _IOR('k',(TIM_IOCTL_BASE + 8),struct plx_byte_adpair)
#define TIMIOSETNVRAM   _IOWR('k',(TIM_IOCTL_BASE + 9),struct plx_byte_adpair)
#define TIM_IOCXREADLCONFREG  _IOWR('k',(TIM_IOCTL_BASE + 10), __u32)
#define TIM_IOSETWRINTMODE _IOW('k',(TIM_IOCTL_BASE + 11), __u32)
#define TIM_IOGETWRINTMODE _IOR('k',(TIM_IOCTL_BASE + 12), __u32)
#define TIM_IOSETRDINTMODE _IOW('k',(TIM_IOCTL_BASE + 13), __u32)
#define TIM_IOGETRDINTMODE _IOR('k',(TIM_IOCTL_BASE + 14), __u32)
#define TIMIOSETTIMEINT _IOW('k',(TIM_IOCTL_BASE + 15), __u32)
#define TIM_IOTESTPLXINT   _IO('k',(TIM_IOCTL_BASE + 16))
#define TIM_IOUARTSTATRESET _IO('k',(TIM_IOCTL_BASE + 17))

/* Local bus UART (from 0xC0) */

#define TIM_IOSETUARTFORM _IOW('k',(TIM_UART_IOCTL_BASE + 0),__u32)
#define TIM_IOSETUARTBAUD _IOW('k',(TIM_UART_IOCTL_BASE + 1),__u32)
#define TIM_IOSETUARTFIFO _IOW('k',(TIM_UART_IOCTL_BASE + 2),__u32)
#define TIM_IOSETUARTINT  _IOW('k',(TIM_UART_IOCTL_BASE + 3),__u32)
#define TIM_IOSETUARTMCR  _IOW('k',(TIM_UART_IOCTL_BASE + 4),__u32)



/*
 * Split minors in two parts
 */
#define TYPE(dev)   (MINOR(dev) >> 4)  /* high nibble */
#define NUM(dev)    (MINOR(dev) & 0xf) /* low  nibble */

extern struct file_operations tim_fops;

/*
 * The different configurable parameters
 */
extern int tim_major;


/*!
  \fn ssize_t tim_read (struct file *, char *, size_t, loff_t *)
  \brief Read data from TIM board memory
 */
ssize_t tim_read (struct file *filp, char *buf, size_t count,
                    loff_t *f_pos);
/*!
  \fn ssize_t tim_write (struct file *, const char *, size_t, loff_t *);

  \brief Write data to TIM board memory

  \param filp  - Pointer to disk file
  \param buf   - 
  \param count - 
  \param f_pos - 
 */
ssize_t tim_write (struct file *filp, const char *buf, size_t count,
                     loff_t *f_pos);
/*!
  \fn loff_t tim_llseek (struct file *, loff_t, int);

  \param filp   - Pointer to disk file
  \param off    - 
  \param whence - 
 */
loff_t  tim_llseek (struct file *filp, loff_t off, int whence);
int     tim_ioctl (struct inode *inode, struct file *filp,
                     unsigned int cmd, unsigned long arg);

/*--------------------------------------------------------------------*
 * 
 *  KERNEL ONLY Section
 *
 *--------------------------------------------------------------------*/

#ifdef __KERNEL__

/****************** Structures ****************************************/
/*
 *  struct plx_timings.
 *  This structure holds user-level IRQ timing information.
 */
struct plx_timings {
    volatile long start;
    volatile long irq;
    volatile long bh;
    volatile long ioctl;
};

/*
 *  struct plx_timeinfo.
 *  This structure holds IRQ timing information.
 */
struct plx_timeinfo {
    struct timeval op;
    struct timeval irq;
    struct timeval bh;
};

/* 
 *Special structure for single transfer info 
 */
struct plx_adpair  
{
  __u32 addr;
  __u32 data;
};

struct plx_byte_adpair  
{
  __u32 addr;
  __u8 data;
};


/*!
  \brief PLX 9050 local address region structure.

  This struct holds information about a single address region.
 */
typedef struct {
  int         lreg_type;      /* MEM or I/O mapped region */
  u_int       size;           /* dimensions */
  u_int       io_addr;        /* I/O address when I/O mapped */
  u_int       phys_addr;      /* real address when Memory mapped */
  u_char      *virt_addr;     /* after vremap/ioremap call when Memory mapped */

  /* Read buffer, filled by ISR by reading from lreg emptied by user read() */
  unsigned int rbuf;
  volatile unsigned long rbuf_head;
  volatile unsigned long rbuf_tail;

  /* Write buffer, filled by user write() emptied by ISR to lreg */
  unsigned int wbuf;
  volatile unsigned long wbuf_head;
  volatile unsigned long wbuf_tail;

  struct wait_queue *read_queue;

} plx9050_lreg;


/**************** time statistics - structs *****************/

typedef struct {
  unsigned int min;
  unsigned int max;
} plx_irq_time_stats;


/*
 *  Card structure
 */
struct tim_device {

    unsigned int      magic;
    unsigned int      type;
    unsigned int      nr;
    struct pci_dev    *pcidev;
  

    u_long            reg0base,reg0len,regs0,
	              reg1base,reg1len,
                      reg2base,reg2len,regs2,
                      reg3base,reg3len,
                      reg4base,reg4len,
	              reg5base,reg5len;

    int               reg0_type,reg1_type,reg2_type,
	              reg3_type,reg4_type,reg5_type;


    u_char            irq;            /* interrupt number */
    u_char            lregnum;        /* number of local address regions */


    /* Several shared resources have to be protected while modified */
    spinlock_t lock;

    /* large (128kB) kernel-space buffer & attached semaphore */
  void              *iobuff;
  struct semaphore  iobuff_sem;

  /* a flag whether the HSL of this card are busy & attached waitqueue*/
  int                  busyflag;
  wait_queue_head_t    busyqueue;
  
};

#define TIM_MEMBASE   0x1000000U
#define TIM_MODEREG   0x0U
#define TIM_CTRLREG   0x4U
#define TIM_SEGREG    0x8U
#define TIM_CLKREG    0xCU
#define TIM_ADDRREG0  0x10U
#define TIM_ADDRREG1  0x14U
#define TIM_ADDRREG2  0x18U
#define TIM_ADDRREG3  0x1CU



#endif /* KERNEL */

#endif /* _TIM_H_ */
