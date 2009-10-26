/*
 * plx9080.h -- definitions for the PLX9080 device
 *
 * Copyright (C) 2002 Gideon W. Kant
 *
 */

/* 	$Id$	 */


#ifndef _PLX9080_H_
#define _PLX9080_H_

#define PLX_VENDORID     0x10b5
#define PLX9080_DEVICEID 0x9080


/****************************************************************************/
/* PLX Local config Register Offsets - PCI                                  */
/****************************************************************************/

#define PLX_LCONFREG_MARBR          0x08
#define PLX_LCONFREG_BIGEND         0x0c
#define PLX_LCONFREG_LBRD0          0x18
#define PLX_LCONFREG_INTCSR         0x68
#define PLX_LCONFREG_CNTRL          0x6c
#define PLX_LCONFREG_DMAMODE0       0x80
#define PLX_LCONFREG_DMAPADR0       0x84
#define PLX_LCONFREG_DMALADR0       0x88
#define PLX_LCONFREG_DMASIZ0        0x8c
#define PLX_LCONFREG_DMADPR0        0x90
#define PLX_LCONFREG_DMACSR0        0xa8
#define PLX_LCONFREG_DMATHR         0xB0

/* #define PLX_LCONFREG_DMAMODE1       0x94 */ //only use one DMA-Controler
/* #define PLX_LCONFREG_DMAPADR1       0x98 */
/* #define PLX_LCONFREG_DMALADR1       0x9c */
/* #define PLX_LCONFREG_DMASIZ1        0xa0 */
/* #define PLX_LCONFREG_DMADPR1        0xa4 */
/* #define PLX_LCONFREG_DMACSR1        0xac */

/****************************************************************************/
/* PLX Operation Registers Size - PCI                                      */
/****************************************************************************/

#define PLX_LCONFREG_SIZE	    84	/* in bytes */

/****************************************************************************/
/* PLX Local config reg INTCSR states                                       */
/****************************************************************************/

#define PLX_INTCSR_ENABLE           0x5b
#define PLX_INTCSR_DISABLE          0x12
#define PLX_INTCSR_STATCHECK        0xa4
#define PLX_INTCSR_SRC1             0x4
#define PLX_INTCSR_SRC2             0x20
#define PLX_INTCSR_SRCT             0x80

/****************************************************************************/
/* Interrupt sources                                                        */
/****************************************************************************/

#define PLX_INT_PCIDOOR    0x00002000  // Local to PCI doorbell interrupt 
#define PLX_INT_PCIABRT    0x00004000  // PCI abort interrupt
#define PLX_INT_LINT       0x00008000  // Local interrupt
#define PLX_INT_LOCDOOR    0x00100000  // PCI to local interrupt
#define PLX_INT_DMA0       0x00200000  // DMA Channel 0 interrupt
#define PLX_INT_DMA1       0x00400000  // DMA Channel 1 interrupt
#define PLX_INT_ALLINTS    0x0070e000  // all interrupts, used to check if IRQ is from us

/****************************************************************************/
/* DMA-Register Values/Bits                                                 */
/****************************************************************************/

/****************************************************************************/
/* DMA-Mode Register:                                                       */
/*      1:0     11       Local bus width (11 = 32bit)      \                */
/*      5:2      0       internal wait states (set to 0)    \ like          */
/*        6      1       Ready Input Enable (enabled)       / LBDR0         */
/*        7      1       BTERM Input Enable (enabled)      /                */
/*        8      1       Local Burst Enable (enabled)                       */
/*        9      0       Chaining (disabled)                                */
/*       10      0       Done Interrupt Enable (disabled)                   */
/*       11      0       Local Addressing Mode (1=increment address)        */
/*       12      0       Demand Mode (disabled)                             */
/*       13      1       Write and Invalidate Mode (enabled)                */
/*       14      0       DMA EOT Enable (0= EOT[1:0]# pin disabled)         */
/*       15      0       DMA Stop Transfer Mode                             */
/*       16      0       DMA Clear Count Mode                               */
/*       17      1       DMA Interrupt Destination (1=interrupt on PCI)     */
/*    31:18      0       Reserved                                           */
/****************************************************************************/

#define PLX_DMA_DEFMODE 0x000221c3  // Default DMA-Mode
#define PLX_DMA_READ    0x00000008  // Set to read 
#define PLX_DMA_WRITE   0x00000000  // Set to WRITE
#define PLX_DMA_DEFTHR  0x33337777  // Default DMA-Thresholds

#endif /* _PLX9080_H_ */
