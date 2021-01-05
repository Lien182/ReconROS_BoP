#ifndef __AXIVDMA_H__
#define __AXIVDMA_H__

#include <stdint.h>
#include "io.h"

#define XAXIVDMA_BD_STRIDE_OFFSET      0x18  /**< Stride size */

#define XAXIVDMA_FRMDLY_MASK      0x0F000000 /**< Frame delay */

#define XAXIVDMA_REGINDEX_MASK		0x00000001 /**< Register Index */


#define XAXIVDMA_VSIZE_MASK       0x00001FFF /**< Vertical size */
#define XAXIVDMA_HSIZE_MASK       0x0000FFFF /**< Horizontal size */
#define XAXIVDMA_STRIDE_MASK      0x0000FFFF /**< Stride size */
#define XAXIVDMA_FRMDLY_MASK      0x0F000000 /**< Frame delay */

#define XAXIVDMA_BD_NDESC_OFFSET       0x00  /**< Next descriptor pointer */
#define XAXIVDMA_BD_START_ADDR_OFFSET  0x08  /**< Start address */
#define XAXIVDMA_BD_VSIZE_OFFSET       0x10  /**< Vertical size */
#define XAXIVDMA_BD_HSIZE_OFFSET       0x14  /**< Horizontal size */
#define XAXIVDMA_BD_STRIDE_OFFSET      0x18  /**< Stride size */


#define XAXIVDMA_MAX_VSIZE  0x1FFF  /* Max vertical size, 8K */
#define XAXIVDMA_MAX_HSIZE  0xFFFF  /* Max horizontal size, 64K */
#define XAXIVDMA_MAX_STRIDE 0xFFFF  /* Max stride size, 64K */
#define XAXIVDMA_FRMDLY_MAX 0xF     /**< Maximum frame delay */
/*@}*/

#define XAXIVDMA_PARKPTR_READREF_MASK 0x0000001F /**< Read frame to park on */
#define XAXIVDMA_PARKPTR_WRTREF_MASK  0x00001F00 /**< Write frame to park on */
#define XAXIVDMA_PARKPTR_READSTR_MASK 0x001F0000 /**< Current read frame */
#define XAXIVDMA_PARKPTR_WRTSTR_MASK  0x1F000000 /**< Current write frame */
#define XAXIVDMA_READREF_SHIFT        0
#define XAXIVDMA_WRTREF_SHIFT         8
#define XAXIVDMA_READSTR_SHIFT        16
#define XAXIVDMA_WRTSTR_SHIFT         24
#define XAXIVDMA_FRM_MAX              0xF       /**< At most 16 frames */
#define XAXIVDMA_FRMDLY_SHIFT        24     /**< Shift for frame delay */


#define XAXIVDMA_TX_OFFSET      0x00000000	/**< TX channel registers base */
#define XAXIVDMA_RX_OFFSET      0x00000030	/**< RX channel registers base */
#define XAXIVDMA_PARKPTR_OFFSET 0x00000028  /**< Park Pointer Register */
#define XAXIVDMA_VERSION_OFFSET 0x0000002C  /**< Version register */

/* This set of registers are applicable for both channels. Use
 * XAXIVDMA_TX_OFFSET for the TX channel, and XAXIVDMA_RX_OFFSET for the
 * RX channel
 */
#define XAXIVDMA_CR_OFFSET    	0x00000000	 /**< Channel control */
#define XAXIVDMA_SR_OFFSET    	0x00000004	 /**< Status */
#define XAXIVDMA_CDESC_OFFSET 	0x00000008	 /**< Current descriptor pointer */
#define XAXIVDMA_TDESC_OFFSET	0x00000010	 /**< Tail descriptor pointer */
#define XAXIVDMA_HI_FRMBUF_OFFSET	0x00000014 	 /**< 32 FrameBuf Sel*/
#define XAXIVDMA_FRMSTORE_OFFSET	0x00000018	 /**< Frame Store */
#define XAXIVDMA_BUFTHRES_OFFSET	0x0000001C	 /**< Line Buffer Thres */
#define XAXIVDMA_MM2S_ADDR_OFFSET 0x00000050 /**< MM2S channel Addr */
#define XAXIVDMA_S2MM_ADDR_OFFSET 0x000000A0 /**< S2MM channel Addr */

/* This register is specific to S2MM channel
 */
#define XAXIVDMA_S2MM_DMA_IRQ_MASK_OFFSET	0x0000003C /**< S2MM Err IRQ Mask*/
/*@}*/

/** @name Start Addresses Register Array for a Channel
 *
 * Base offset is set in each channel
 * This set of registers are write only, they can be read when
 * C_ENABLE_VIDPRMTR_READS is 1.
 * @{
 */
#define XAXIVDMA_VSIZE_OFFSET         0x00000000  /**< Vertical size */
#define XAXIVDMA_HSIZE_OFFSET         0x00000004  /**< Horizontal size */
#define XAXIVDMA_STRD_FRMDLY_OFFSET   0x00000008  /**< Horizontal size */
#define XAXIVDMA_START_ADDR_OFFSET    0x0000000C  /**< Start of address */
#define XAXIVDMA_START_ADDR_LEN       0x00000004  /**< Each entry is 4 bytes */
#define XAXIVDMA_START_ADDR_MSB_OFFSET 0x00000010  /**< Start of address */

/*@}*/

/** @name Bitmasks of the XAXIVDMA_CR_OFFSET register
 * @{
 */
#define XAXIVDMA_CR_RUNSTOP_MASK    0x00000001 /**< Start/stop DMA channel */
#define XAXIVDMA_CR_TAIL_EN_MASK    0x00000002 /**< Tail ptr enable or Park */
#define XAXIVDMA_CR_RESET_MASK      0x00000004 /**< Reset channel */
#define XAXIVDMA_CR_SYNC_EN_MASK    0x00000008 /**< Gen-lock enable */
#define XAXIVDMA_CR_FRMCNT_EN_MASK  0x00000010 /**< Frame count enable */
#define XAXIVDMA_CR_FSYNC_SRC_MASK  0x00000060 /**< Fsync Source Select */
#define XAXIVDMA_CR_GENLCK_SRC_MASK 0x00000080 /**< Genlock Source Select */
#define XAXIVDMA_CR_RD_PTR_MASK     0x00000F00 /**< Read pointer number */
#define XAXIVDMA_CR_GENLCK_RPT_MASK 0x00008000 /**< GenLock Repeat */

#define XAXIVDMA_CR_RD_PTR_SHIFT    8   /**< Shift for read pointer number */
/*@}*/

/** @name Bitmasks of the XAXIVDMA_SR_OFFSET register
 * This register reports status of a DMA channel, including
 * run/stop/idle state, errors, and interrupts
 * @{
 */
#define XAXIVDMA_SR_HALTED_MASK       0x00000001  /**< DMA channel halted */
#define XAXIVDMA_SR_IDLE_MASK         0x00000002  /**< DMA channel idle */
#define XAXIVDMA_SR_ERR_INTERNAL_MASK 0x00000010  /**< Datamover internal err */
#define XAXIVDMA_SR_ERR_SLAVE_MASK    0x00000020  /**< Datamover slave err */
#define XAXIVDMA_SR_ERR_DECODE_MASK   0x00000040  /**< Datamover decode err */
#define XAXIVDMA_SR_ERR_FSZ_LESS_MASK 0x00000080  /**< FSize Less Mismatch err */
#define XAXIVDMA_SR_ERR_LSZ_LESS_MASK 0x00000100  /**< LSize Less Mismatch err */
#define XAXIVDMA_SR_ERR_SG_SLV_MASK   0x00000200  /**< SG slave err */
#define XAXIVDMA_SR_ERR_SG_DEC_MASK   0x00000400  /**< SG decode err */
#define XAXIVDMA_SR_ERR_FSZ_MORE_MASK 0x00000800  /**< FSize More Mismatch err */
#define XAXIVDMA_SR_ERR_ALL_MASK      0x00000FF0  /**< All errors */

/** @name Bitmask for interrupts
 * These masks are shared by the XAXIVDMA_CR_OFFSET register and
 * the XAXIVDMA_SR_OFFSET register
 * @{
 */
#define XAXIVDMA_IXR_FRMCNT_MASK      0x00001000 /**< Frame count intr */
#define XAXIVDMA_IXR_DELAYCNT_MASK    0x00002000 /**< Delay interrupt */
#define XAXIVDMA_IXR_ERROR_MASK       0x00004000 /**< Error interrupt */
#define XAXIVDMA_IXR_COMPLETION_MASK  0x00003000 /**< Completion interrupts */
#define XAXIVDMA_IXR_ALL_MASK         0x00007000 /**< All interrupts */






#define XAXIVDMA_MAX_FRAMESTORE    32  /**< Maximum # of the frame store */

#define XAXIVDMA_MAX_FRAMESTORE_64 16  /**< Maximum # of the frame store for 64 bit*/




#define XAXIVDMA_WRITE       1        /**< DMA transfer into memory */
#define XAXIVDMA_READ        2        /**< DMA transfer from memory */

#define XAXIVDMA_CHAN_FSYNC		0
#define XAXIVDMA_CHAN_OTHER_FSYNC	1
#define XAXIVDMA_S2MM_TUSER_FSYNC	2

/**
 * GenLock Source Selection
 */
#define XAXIVDMA_EXTERNAL_GENLOCK	0
#define XAXIVDMA_INTERNAL_GENLOCK	1

/**
 * GenLock Mode Constants
 */
#define XAXIVDMA_GENLOCK_MASTER		0
#define XAXIVDMA_GENLOCK_SLAVE		1
#define XAXIVDMA_DYN_GENLOCK_MASTER	2
#define XAXIVDMA_DYN_GENLOCK_SLAVE	3


#define XAXIVDMA_ENABLE_DBG_THRESHOLD_REG	0x01
#define XAXIVDMA_ENABLE_DBG_FRMSTORE_REG	0x02
#define XAXIVDMA_ENABLE_DBG_FRM_CNTR	0x04
#define XAXIVDMA_ENABLE_DBG_DLY_CNTR	0x08
#define XAXIVDMA_ENABLE_DBG_ALL_FEATURES	0x0F



typedef void (*XAxiVdma_CallBack) (void *CallBackRef, uint32_t InterruptTypes);
typedef void (*XAxiVdma_ErrorCallBack) (void *CallBackRef, uint32_t ErrorMask);

#define XAXIVDMA_BD_MINIMUM_ALIGNMENT_WD 0x8
typedef uint32_t XAxiVdma_Bd[XAXIVDMA_BD_MINIMUM_ALIGNMENT_WD];



typedef struct {
    uint32_t* ChanBase;       /* Base address for this channel */
    uint32_t* InstanceBase;   /* Base address for the whole device */
    uint32_t* StartAddrBase;  /* Start address register array base */

    int IsValid;        /* Whether the channel has been initialized */
    int FlushonFsync;	/* VDMA Transactions are flushed & channel states
			   reset on Frame Sync */
    int HasSG;          /* Whether hardware has SG engine */
    int IsRead;         /* Read or write channel */
    int HasDRE;         /* Whether support unaligned transfer */
    int LineBufDepth;	/* Depth of Channel Line Buffer FIFO */
    int LineBufThreshold;	/* Threshold point at which Channel Line
				 *  almost empty flag asserts high */
    int WordLength;     /* Word length */
    int NumFrames;	/* Number of frames to work on */

    uint32_t* HeadBdPhysAddr; /* Physical address of the first BD */
    uint32_t** HeadBdAddr;     /* Virtual address of the first BD */
    uint32_t* TailBdPhysAddr; /* Physical address of the last BD */
    uint32_t* TailBdAddr;     /* Virtual address of the last BD */
    int Hsize;          /* Horizontal size */
    int Vsize;          /* Vertical size saved for no-sg mode hw start */

    int AllCnt;         /* Total number of BDs */

    int GenLock;	/* Mm2s Gen Lock Mode */
    int S2MmSOF;	/* S2MM Start of Flag */
    int StreamWidth;     /* Stream Width */
    XAxiVdma_Bd BDs[32] __attribute__((__aligned__(32)));
                        /*Statically allocated BDs */
    uint32_t DbgFeatureFlags; /* Debug Parameter Flags */
	int AddrWidth;
	int direction;	/* Determines whether Read or write channel */
}XAxiVdma_Channel;

typedef struct {
    XAxiVdma_CallBack CompletionCallBack; /**< Call back for completion intr */
    void *CompletionRef;                  /**< Call back ref */

    XAxiVdma_ErrorCallBack ErrCallBack;   /**< Call back for error intr */
    void *ErrRef;                         /**< Call back ref */
} XAxiVdma_ChannelCallBack;

typedef struct {
    uint32_t* BaseAddr;                   /**< Memory address for this device */
    int HasSG;                      /**< Whether hardware has SG engine */
    int IsReady;                    /**< Whether driver is initialized */

    int MaxNumFrames;                /**< Number of frames to work on */
    int HasMm2S;                    /**< Whether hw build has read channel */
    int HasMm2SDRE;                 /**< Whether read channel has DRE */
    int HasS2Mm;                    /**< Whether hw build has write channel */
    int HasS2MmDRE;                 /**< Whether write channel has DRE */
    int EnableVIDParamRead;	    /**< Read Enable for video parameters in
				      *  direct register mode */
    int UseFsync;       	    /**< DMA operations synchronized to
				      * Frame Sync */
    int InternalGenLock;  	    /**< Internal Gen Lock */
    XAxiVdma_ChannelCallBack ReadCallBack;  /**< Call back for read channel */
    XAxiVdma_ChannelCallBack WriteCallBack; /**< Call back for write channel */

    XAxiVdma_Channel ReadChannel;  /**< Channel to read from memory */
    XAxiVdma_Channel WriteChannel; /**< Channel to write to memory */
	int AddrWidth;		  /**< Address Width */
} XAxiVdma;

typedef struct {
    int VertSizeInput;      /**< Vertical size input */
    int HoriSizeInput;      /**< Horizontal size input */
    int Stride;             /**< Stride */
    int FrameDelay;         /**< Frame Delay */

    int EnableCircularBuf;  /**< Circular Buffer Mode? */
    int EnableSync;         /**< Gen-Lock Mode? */
    int PointNum;           /**< Master we synchronize with */
    int EnableFrameCounter; /**< Frame Counter Enable */
    uint32_t FrameStoreStartAddr[32];
                            /**< Start Addresses of Frame Store Buffers. */
    int FixedFrameStoreAddr;/**< Fixed Frame Store Address index */
    int GenLockRepeat;      /**< Gen-Lock Repeat? */
} XAxiVdma_DmaSetup;

typedef struct {
    uint16_t DeviceId;         /**< DeviceId is the unique ID  of the device */
    uint32_t* BaseAddress;      /**< BaseAddress is the physical base address of the
                            *  device's registers */
    uint16_t MaxFrameStoreNum; /**< The maximum number of Frame Stores */
    int HasMm2S;          /**< Whether hw build has read channel */
    int HasMm2SDRE;       /**< Read channel supports unaligned transfer */
    int Mm2SWordLen;      /**< Read channel word length */
    int HasS2Mm;          /**< Whether hw build has write channel */
    int HasS2MmDRE;       /**< Write channel supports unaligned transfer */
    int S2MmWordLen;      /**< Write channel word length */
    int HasSG;            /**< Whether hardware has SG engine */
    int EnableVIDParamRead;
			  /**< Read Enable for video parameters in direct
			    *  register mode */
    int UseFsync;	  /**< DMA operations synchronized to Frame Sync */
    int FlushonFsync;	  /**< VDMA Transactions are flushed & channel states
			    *	reset on Frame Sync */
    int Mm2SBufDepth;	  /**< Depth of Read Channel Line Buffer FIFO */
    int S2MmBufDepth;	  /**< Depth of Write Channel Line Buffer FIFO */
    int Mm2SGenLock;	  /**< Mm2s Gen Lock Mode */
    int S2MmGenLock;	  /**< S2Mm Gen Lock Mode */
    int InternalGenLock;  /**< Internal Gen Lock */
    int S2MmSOF;	  /**< S2MM Start of Flag Enable */
    int Mm2SStreamWidth;  /**< MM2S TData Width */
    int S2MmStreamWidth;  /**< S2MM TData Width */
    int Mm2SThresRegEn;   /**< MM2S Threshold Register Enable Flag
							   This corresponds to C_ENABLE_DEBUG_INFO_1
							   configuration parameter */
    int Mm2SFrmStoreRegEn;/**< MM2S Frame Store Register Enable Flag
							   This corresponds to C_ENABLE_DEBUG_INFO_5
							   configuration parameter */
    int Mm2SDlyCntrEn;	  /**< MM2S Delay Counter (Control Reg) Enable Flag
							   This corresponds to C_ENABLE_DEBUG_INFO_6
							   configuration parameter */
    int Mm2SFrmCntrEn;    /**< MM2S Frame Counter (Control Reg) Enable Flag
							   This corresponds to C_ENABLE_DEBUG_INFO_7
							   configuration parameter */
    int S2MmThresRegEn;   /**< S2MM Threshold Register Enable Flag
							   This corresponds to C_ENABLE_DEBUG_INFO_9
							   configuration parameter */
    int S2MmFrmStoreRegEn;/**< S2MM Frame Store Register Enable Flag
							   This corresponds to C_ENABLE_DEBUG_INFO_13
							   configuration parameter */
    int S2MmDlyCntrEn;	  /**< S2MM Delay Counter (Control Reg) Enable Flag
							   This corresponds to C_ENABLE_DEBUG_INFO_14
							   configuration parameter */
    int S2MmFrmCntrEn;	  /**< S2MM Frame Counter (Control Reg) Enable Flag
						       This corresponds to C_ENABLE_DEBUG_INFO_15
							   configuration parameter */
    int EnableAllDbgFeatures;/**< Enable all Debug features
						       This corresponds to C_ENABLE_DEBUG_ALL
							   configuration parameter */
	int AddrWidth;		  /**< Address Width */
} XAxiVdma_Config;


typedef struct {
    uint8_t ReadFrameCount;      /**< Interrupt threshold for Receive */
    uint8_t ReadDelayTimerCount; /**< Delay timer threshold for receive */
    uint8_t WriteFrameCount;     /**< Interrupt threshold for transmit */
    uint8_t WriteDelayTimerCount;/**< Delay timer threshold for transmit */
} XAxiVdma_FrameCounter;


typedef struct {
    int VertSizeInput;      /**< Vertical size input */
    int HoriSizeInput;      /**< Horizontal size input */
    int Stride;             /**< Stride */
    int FrameDelay;         /**< Frame Delay */

    int EnableCircularBuf;  /**< Circular Buffer Mode? */
    int EnableSync;         /**< Gen-Lock Mode? */
    int PointNum;           /**< Master we synchronize with */
    int EnableFrameCounter; /**< Frame Counter Enable */
    uint32_t* FrameStoreStartAddr[32];
                            /**< Start Addresses of Frame Store Buffers. */
    int FixedFrameStoreAddr;/**< Fixed Frame Store Address index */
    int GenLockRepeat;      /**< Gen-Lock Repeat? */
}XAxiVdma_ChannelSetup;


#define XAxiVdma_WriteReg(BaseAddress, RegOffset, Data) \
    Out32((uint32_t*)((BaseAddress) + (RegOffset/4)), (uint32_t)(Data))

#define XAxiVdma_ReadReg(BaseAddress, RegOffset) \
    In32((uint32_t*)((BaseAddress) + (RegOffset/4)))


#define XAxiVdma_ChannelHiFrmAddrEnable(Channel) \
{ \
	XAxiVdma_WriteReg(Channel->ChanBase, \
			XAXIVDMA_HI_FRMBUF_OFFSET, XAXIVDMA_REGINDEX_MASK); \
}



int XAxiVdma_CfgInitialize(XAxiVdma *InstancePtr, XAxiVdma_Config *CfgPtr,
					uint32_t* EffectiveAddr);

XAxiVdma_Config *XAxiVdma_LookupConfig(uint16_t DeviceId);

int XAxiVdma_FsyncSrcSelect(XAxiVdma *InstancePtr, uint32_t Source,
				uint16_t Direction);

int XAxiVdma_DmaConfig(XAxiVdma *InstancePtr, uint16_t Direction,
        XAxiVdma_DmaSetup *DmaConfigPtr);


int XAxiVdma_DmaSetBufferAddr(XAxiVdma *InstancePtr, uint16_t Direction,
        uint32_t *BufferAddrSet);


int XAxiVdma_DmaStart(XAxiVdma *InstancePtr, uint16_t Direction);

void XAxiVdma_DmaStop(XAxiVdma *InstancePtr, uint16_t Direction);


uint32_t XAxiVdma_ChannelGetStatus(XAxiVdma_Channel *Channel);



void XAxiVdma_BdSetAddr(XAxiVdma_Bd *BdPtr, uint32_t Addr);

int XAxiVdma_BdSetFrmDly(XAxiVdma_Bd *BdPtr, int FrmDly);

int XAxiVdma_BdSetVsize(XAxiVdma_Bd *BdPtr, int Vsize);

int XAxiVdma_BdSetHsize(XAxiVdma_Bd *BdPtr, int Hsize);


int XAxiVdma_BdSetStride(XAxiVdma_Bd *BdPtr, int Stride);

uint32_t XAxiVdma_BdRead(XAxiVdma_Bd *BdPtr, uint32_t Offset);

void XAxiVdma_BdWrite(XAxiVdma_Bd *BdPtr, uint32_t Offset, uint32_t Value);

int XAxiVdma_ChannelIsRunning(XAxiVdma_Channel *Channel);

int XAxiVdma_ChannelIsBusy(XAxiVdma_Channel *Channel);

 void XAxiVdma_BdSetNextPtr(XAxiVdma_Bd *BdPtr, uint32_t NextPtr);




int XAxiVdma_ChannelIsBusy(XAxiVdma_Channel *Channel);

void XAxiVdma_BdSetNextPtr(XAxiVdma_Bd *BdPtr, uint32_t NextPtr);

int XAxiVdma_ChannelStart(XAxiVdma_Channel *Channel);

void XAxiVdma_ChannelStop(XAxiVdma_Channel *Channel);




uint32_t XAxiVdma_BdRead(XAxiVdma_Bd *BdPtr, uint32_t Offset);
void XAxiVdma_BdWrite(XAxiVdma_Bd *BdPtr, uint32_t Offset, uint32_t Value);





#endif