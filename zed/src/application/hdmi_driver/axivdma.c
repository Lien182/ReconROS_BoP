#include "axivdma.h"
#include "io.h"

#include <stdio.h>

#define INITIALIZATION_POLLING   100000

#define XAXIVDMA_DEVICE_READY      0x11111111


#define XAxiVdma_ChannelHiFrmAddrDisable(Channel) \
{ \
	XAxiVdma_WriteReg(Channel->ChanBase, \
		XAXIVDMA_HI_FRMBUF_OFFSET, (XAXIVDMA_REGINDEX_MASK >> 1)); \
}


XAxiVdma_Channel *XAxiVdma_GetChannel(XAxiVdma *InstancePtr,
        uint16_t Direction)
{

	if (Direction == XAXIVDMA_READ) {
		return &(InstancePtr->ReadChannel);
	}
	else if (Direction == XAXIVDMA_WRITE) {
		return &(InstancePtr->WriteChannel);
	}
	else {
		printf("Invalid direction %x\r\n", Direction);

		return 0;
	}
}


int XAxiVdma_CfgInitialize(XAxiVdma *InstancePtr, XAxiVdma_Config *CfgPtr,
				uint32_t* EffectiveAddr)
{
	XAxiVdma_Channel *RdChannel;
	XAxiVdma_Channel *WrChannel;
	int Polls;


	/* Initially, no interrupt callback functions
	 */
	InstancePtr->ReadCallBack.CompletionCallBack = 0x0;
	InstancePtr->ReadCallBack.ErrCallBack = 0x0;
	InstancePtr->WriteCallBack.CompletionCallBack = 0x0;
	InstancePtr->WriteCallBack.ErrCallBack = 0x0;

	InstancePtr->BaseAddr = EffectiveAddr;
	InstancePtr->MaxNumFrames = CfgPtr->MaxFrameStoreNum;
	InstancePtr->HasMm2S = CfgPtr->HasMm2S;
	InstancePtr->HasS2Mm = CfgPtr->HasS2Mm;
	InstancePtr->UseFsync = CfgPtr->UseFsync;
	InstancePtr->InternalGenLock = CfgPtr->InternalGenLock;
	InstancePtr->AddrWidth = CfgPtr->AddrWidth;

	printf("VDMA Major Number: %x \n", XAxiVdma_Major(InstancePtr));

	if (XAxiVdma_Major(InstancePtr) < 3) {
		InstancePtr->HasSG = 1;
	}
	else {
		InstancePtr->HasSG = CfgPtr->HasSG;
	}

	/* The channels are not valid until being initialized
	 */
	RdChannel = XAxiVdma_GetChannel(InstancePtr, XAXIVDMA_READ);
	RdChannel->IsValid = 0;

	WrChannel = XAxiVdma_GetChannel(InstancePtr, XAXIVDMA_WRITE);
	WrChannel->IsValid = 0;

	if (InstancePtr->HasMm2S) {
		printf("DEBUG=1\n");
		RdChannel->direction = XAXIVDMA_READ;
		RdChannel->ChanBase = InstancePtr->BaseAddr + XAXIVDMA_TX_OFFSET/4;
		RdChannel->InstanceBase = InstancePtr->BaseAddr;
		RdChannel->HasSG = InstancePtr->HasSG;
		RdChannel->IsRead = 1;
		RdChannel->StartAddrBase = InstancePtr->BaseAddr + XAXIVDMA_MM2S_ADDR_OFFSET/4;

		RdChannel->NumFrames = CfgPtr->MaxFrameStoreNum;

		/* Flush on Sync */
		RdChannel->FlushonFsync = CfgPtr->FlushonFsync;

		/* Dynamic Line Buffers Depth */
		RdChannel->LineBufDepth = CfgPtr->Mm2SBufDepth;
		if(RdChannel->LineBufDepth > 0) {
			RdChannel->LineBufThreshold = XAxiVdma_ReadReg(RdChannel->ChanBase, XAXIVDMA_BUFTHRES_OFFSET);
			printf("Read Channel Buffer Threshold %d bytes\n\r", RdChannel->LineBufThreshold);
		}
		RdChannel->HasDRE = CfgPtr->HasMm2SDRE;
		RdChannel->WordLength = CfgPtr->Mm2SWordLen >> 3;
		RdChannel->StreamWidth = CfgPtr->Mm2SStreamWidth >> 3;
		RdChannel->AddrWidth = InstancePtr->AddrWidth;

		/* Internal GenLock */
		RdChannel->GenLock = CfgPtr->Mm2SGenLock;

		/* Debug Info Parameter flags */
		if (!CfgPtr->EnableAllDbgFeatures) {
			if (CfgPtr->Mm2SThresRegEn) {
				RdChannel->DbgFeatureFlags |=
					XAXIVDMA_ENABLE_DBG_THRESHOLD_REG;
			}

			if (CfgPtr->Mm2SFrmStoreRegEn) {
				RdChannel->DbgFeatureFlags |=
					XAXIVDMA_ENABLE_DBG_FRMSTORE_REG;
			}

			if (CfgPtr->Mm2SDlyCntrEn) {
				RdChannel->DbgFeatureFlags |=
					XAXIVDMA_ENABLE_DBG_DLY_CNTR;
			}

			if (CfgPtr->Mm2SFrmCntrEn) {
				RdChannel->DbgFeatureFlags |=
					XAXIVDMA_ENABLE_DBG_FRM_CNTR;
			}

		} else {
			RdChannel->DbgFeatureFlags =
				XAXIVDMA_ENABLE_DBG_ALL_FEATURES;
		}

		XAxiVdma_ChannelInit(RdChannel);

		XAxiVdma_ChannelReset(RdChannel);

		/* At time of initialization, no transfers are going on,
		 * reset is expected to be quick
		 */
		Polls = INITIALIZATION_POLLING;

		while (Polls && XAxiVdma_ChannelResetNotDone(RdChannel)) {
			Polls -= 1;
		}

		if (!Polls) {
			printf("Read channel reset failed %x\n\r",
			    (unsigned int)XAxiVdma_ChannelGetStatus(RdChannel));

			return 1;
		}
	}

	if (InstancePtr->HasS2Mm) {		
		WrChannel->direction = XAXIVDMA_WRITE;
		WrChannel->ChanBase = InstancePtr->BaseAddr + XAXIVDMA_RX_OFFSET/4;
		WrChannel->InstanceBase = InstancePtr->BaseAddr;
		WrChannel->HasSG = InstancePtr->HasSG;
		WrChannel->IsRead = 0;
		WrChannel->StartAddrBase = InstancePtr->BaseAddr +
		                                 XAXIVDMA_S2MM_ADDR_OFFSET/4;
		WrChannel->NumFrames = CfgPtr->MaxFrameStoreNum;
		WrChannel->AddrWidth = InstancePtr->AddrWidth;

		/* Flush on Sync */
		WrChannel->FlushonFsync = CfgPtr->FlushonFsync;

		/* Dynamic Line Buffers Depth */
		WrChannel->LineBufDepth = CfgPtr->S2MmBufDepth;
		if(WrChannel->LineBufDepth > 0) {
			WrChannel->LineBufThreshold =
				XAxiVdma_ReadReg(WrChannel->ChanBase,
					XAXIVDMA_BUFTHRES_OFFSET);
			printf("Write Channel Buffer Threshold %d bytes\n\r",
				WrChannel->LineBufThreshold);
		}
		WrChannel->HasDRE = CfgPtr->HasS2MmDRE;
		WrChannel->WordLength = CfgPtr->S2MmWordLen >> 3;
		WrChannel->StreamWidth = CfgPtr->S2MmStreamWidth >> 3;

		/* Internal GenLock */
		WrChannel->GenLock = CfgPtr->S2MmGenLock;

		/* Frame Sync Source Selection*/
		WrChannel->S2MmSOF = CfgPtr->S2MmSOF;

		/* Debug Info Parameter flags */
		if (!CfgPtr->EnableAllDbgFeatures) {
			if (CfgPtr->S2MmThresRegEn) {
				WrChannel->DbgFeatureFlags |=
					 XAXIVDMA_ENABLE_DBG_THRESHOLD_REG;
			}

			if (CfgPtr->S2MmFrmStoreRegEn) {
				WrChannel->DbgFeatureFlags |=
					XAXIVDMA_ENABLE_DBG_FRMSTORE_REG;
			}

			if (CfgPtr->S2MmDlyCntrEn) {
				WrChannel->DbgFeatureFlags |=
					XAXIVDMA_ENABLE_DBG_DLY_CNTR;
			}

			if (CfgPtr->S2MmFrmCntrEn) {
				WrChannel->DbgFeatureFlags |=
					XAXIVDMA_ENABLE_DBG_FRM_CNTR;
			}

		} else {
			WrChannel->DbgFeatureFlags =
					XAXIVDMA_ENABLE_DBG_ALL_FEATURES;
		}

		XAxiVdma_ChannelInit(WrChannel);

		XAxiVdma_ChannelReset(WrChannel);

		/* At time of initialization, no transfers are going on,
		 * reset is expected to be quick
		 */
		Polls = INITIALIZATION_POLLING;

		while (Polls && XAxiVdma_ChannelResetNotDone(WrChannel)) {
			Polls -= 1;
		}

		if (!Polls) {
			printf("Write channel reset failed %x\n\r",
			    (unsigned int)XAxiVdma_ChannelGetStatus(WrChannel));

			return 1;
		}
	}

	InstancePtr->IsReady = XAXIVDMA_DEVICE_READY;

	return 0;
}


#define XPAR_XAXIVDMA_NUM_INSTANCES 1

/* Definitions for peripheral ZED_HDMI_DISPLAY_AXI_VDMA_0 */
#define XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_DEVICE_ID 0
#define XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_BASEADDR 0x43000000
#define XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_HIGHADDR 0x4300FFFF
#define XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_NUM_FSTORES 3
#define XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_INCLUDE_MM2S 1
#define XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_INCLUDE_MM2S_DRE 1
#define XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_M_AXI_MM2S_DATA_WIDTH 64
#define XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_INCLUDE_S2MM 0
#define XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_INCLUDE_S2MM_DRE 0
#define XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_M_AXI_S2MM_DATA_WIDTH 64
#define XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_AXI_MM2S_ACLK_FREQ_HZ 0
#define XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_AXI_S2MM_ACLK_FREQ_HZ 0
#define XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_MM2S_GENLOCK_MODE 0
#define XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_MM2S_GENLOCK_NUM_MASTERS 1
#define XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_S2MM_GENLOCK_MODE 0
#define XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_S2MM_GENLOCK_NUM_MASTERS 1
#define XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_INCLUDE_SG 0
#define XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_ENABLE_VIDPRMTR_READS 1
#define XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_USE_FSYNC 1
#define XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_FLUSH_ON_FSYNC 1
#define XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_MM2S_LINEBUFFER_DEPTH 4096
#define XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_S2MM_LINEBUFFER_DEPTH 512
#define XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_INCLUDE_INTERNAL_GENLOCK 1
#define XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_S2MM_SOF_ENABLE 1
#define XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_M_AXIS_MM2S_TDATA_WIDTH 32
#define XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_S_AXIS_S2MM_TDATA_WIDTH 32
#define XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_ENABLE_DEBUG_INFO_1 0
#define XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_ENABLE_DEBUG_INFO_5 0
#define XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_ENABLE_DEBUG_INFO_6 0
#define XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_ENABLE_DEBUG_INFO_7 0
#define XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_ENABLE_DEBUG_INFO_9 0
#define XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_ENABLE_DEBUG_INFO_13 0
#define XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_ENABLE_DEBUG_INFO_14 0
#define XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_ENABLE_DEBUG_INFO_15 0
#define XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_ENABLE_DEBUG_ALL 0
#define XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_ADDR_WIDTH 32


/******************************************************************/

/* Canonical definitions for peripheral ZED_HDMI_DISPLAY_AXI_VDMA_0 */
#define XPAR_AXIVDMA_0_DEVICE_ID XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_DEVICE_ID
#define XPAR_AXIVDMA_0_BASEADDR 0x43000000
#define XPAR_AXIVDMA_0_HIGHADDR 0x4300FFFF
#define XPAR_AXIVDMA_0_NUM_FSTORES 3
#define XPAR_AXIVDMA_0_INCLUDE_MM2S 1
#define XPAR_AXIVDMA_0_INCLUDE_MM2S_DRE 1
#define XPAR_AXIVDMA_0_M_AXI_MM2S_DATA_WIDTH 64
#define XPAR_AXIVDMA_0_INCLUDE_S2MM 0
#define XPAR_AXIVDMA_0_INCLUDE_S2MM_DRE 0
#define XPAR_AXIVDMA_0_M_AXI_S2MM_DATA_WIDTH 64
#define XPAR_AXIVDMA_0_AXI_MM2S_ACLK_FREQ_HZ 0
#define XPAR_AXIVDMA_0_AXI_S2MM_ACLK_FREQ_HZ 0
#define XPAR_AXIVDMA_0_MM2S_GENLOCK_MODE 0
#define XPAR_AXIVDMA_0_MM2S_GENLOCK_NUM_MASTERS 1
#define XPAR_AXIVDMA_0_S2MM_GENLOCK_MODE 0
#define XPAR_AXIVDMA_0_S2MM_GENLOCK_NUM_MASTERS 1
#define XPAR_AXIVDMA_0_INCLUDE_SG 0
#define XPAR_AXIVDMA_0_ENABLE_VIDPRMTR_READS 1
#define XPAR_AXIVDMA_0_USE_FSYNC 1
#define XPAR_AXIVDMA_0_FLUSH_ON_FSYNC 1
#define XPAR_AXIVDMA_0_MM2S_LINEBUFFER_DEPTH 4096
#define XPAR_AXIVDMA_0_S2MM_LINEBUFFER_DEPTH 512
#define XPAR_AXIVDMA_0_INCLUDE_INTERNAL_GENLOCK 1
#define XPAR_AXIVDMA_0_S2MM_SOF_ENABLE 1
#define XPAR_AXIVDMA_0_M_AXIS_MM2S_TDATA_WIDTH 32
#define XPAR_AXIVDMA_0_S_AXIS_S2MM_TDATA_WIDTH 32
#define XPAR_AXIVDMA_0_ENABLE_DEBUG_INFO_1 0
#define XPAR_AXIVDMA_0_ENABLE_DEBUG_INFO_5 0
#define XPAR_AXIVDMA_0_ENABLE_DEBUG_INFO_6 0
#define XPAR_AXIVDMA_0_ENABLE_DEBUG_INFO_7 0
#define XPAR_AXIVDMA_0_ENABLE_DEBUG_INFO_9 0
#define XPAR_AXIVDMA_0_ENABLE_DEBUG_INFO_13 0
#define XPAR_AXIVDMA_0_ENABLE_DEBUG_INFO_14 0
#define XPAR_AXIVDMA_0_ENABLE_DEBUG_INFO_15 0
#define XPAR_AXIVDMA_0_ENABLE_DEBUG_ALL 0
#define XPAR_AXIVDMA_0_c_addr_width 32


XAxiVdma_Config XAxiVdma_ConfigTable[XPAR_XAXIVDMA_NUM_INSTANCES] =
{
	{
		XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_DEVICE_ID,
		XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_BASEADDR,
		XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_NUM_FSTORES,
		XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_INCLUDE_MM2S,
		XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_INCLUDE_MM2S_DRE,
		XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_M_AXI_MM2S_DATA_WIDTH,
		XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_INCLUDE_S2MM,
		XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_INCLUDE_S2MM_DRE,
		XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_M_AXI_S2MM_DATA_WIDTH,
		XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_INCLUDE_SG,
		XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_ENABLE_VIDPRMTR_READS,
		XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_USE_FSYNC,
		XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_FLUSH_ON_FSYNC,
		XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_MM2S_LINEBUFFER_DEPTH,
		XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_S2MM_LINEBUFFER_DEPTH,
		XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_MM2S_GENLOCK_MODE,
		XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_S2MM_GENLOCK_MODE,
		XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_INCLUDE_INTERNAL_GENLOCK,
		XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_S2MM_SOF_ENABLE,
		XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_M_AXIS_MM2S_TDATA_WIDTH,
		XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_S_AXIS_S2MM_TDATA_WIDTH,
		XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_ENABLE_DEBUG_INFO_1,
		XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_ENABLE_DEBUG_INFO_5,
		XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_ENABLE_DEBUG_INFO_6,
		XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_ENABLE_DEBUG_INFO_7,
		XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_ENABLE_DEBUG_INFO_9,
		XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_ENABLE_DEBUG_INFO_13,
		XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_ENABLE_DEBUG_INFO_14,
		XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_ENABLE_DEBUG_INFO_15,
		XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_ENABLE_DEBUG_ALL,
		XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_ADDR_WIDTH
	}
};


XAxiVdma_Config *XAxiVdma_LookupConfig(uint16_t DeviceId)
{
	extern XAxiVdma_Config XAxiVdma_ConfigTable[];
	XAxiVdma_Config *CfgPtr = 0;
	int i;

	for (i = 0; i < XPAR_XAXIVDMA_NUM_INSTANCES; i++) {
		if (XAxiVdma_ConfigTable[i].DeviceId == DeviceId) {
			CfgPtr = &XAxiVdma_ConfigTable[i];
			break;
		}
	}

	return CfgPtr;
}

int XAxiVdma_FsyncSrcSelect(XAxiVdma *InstancePtr, uint32_t Source,
				uint16_t Direction)
{
	XAxiVdma_Channel *Channel;
	uint32_t CrBits;
	uint32_t UseFsync;

	Channel = XAxiVdma_GetChannel(InstancePtr, Direction);

	if (Direction == XAXIVDMA_WRITE) {
		UseFsync = ((InstancePtr->UseFsync == 1) ||
				(InstancePtr->UseFsync == 3)) ? 1 : 0;
	} else {
		UseFsync = ((InstancePtr->UseFsync == 1) ||
				(InstancePtr->UseFsync == 2)) ? 1 : 0;
	}

	if (UseFsync) {
		CrBits = XAxiVdma_ReadReg(Channel->ChanBase,
				XAXIVDMA_CR_OFFSET);

		switch (Source) {
		case XAXIVDMA_CHAN_FSYNC:
			/* Same Channel Frame Sync */
			CrBits &= ~(XAXIVDMA_CR_FSYNC_SRC_MASK);
			break;

		case XAXIVDMA_CHAN_OTHER_FSYNC:
			/* The other Channel Frame Sync */
			CrBits |= (XAXIVDMA_CR_FSYNC_SRC_MASK & ~(1 << 6));
			break;

		case XAXIVDMA_S2MM_TUSER_FSYNC:
			/* S2MM TUser Sync */
			if (Channel->S2MmSOF) {
				CrBits |= (XAXIVDMA_CR_FSYNC_SRC_MASK
						 & ~(1 << 5));
			}
			else
				return 1;
			break;
		}

		XAxiVdma_WriteReg(Channel->ChanBase,
			XAXIVDMA_CR_OFFSET, CrBits);

		return 0;
	}

	printf("This bit is not valid for this configuration\n\r");
	return 1;
}

int XAxiVdma_DmaConfig(XAxiVdma *InstancePtr, uint16_t Direction,
        XAxiVdma_DmaSetup *DmaConfigPtr)
{
	XAxiVdma_Channel *Channel;

	Channel = XAxiVdma_GetChannel(InstancePtr, Direction);

	if (!Channel) {
		return 15;
	}


	if (Channel->IsValid) {

		return XAxiVdma_ChannelConfig(Channel,
		    (XAxiVdma_ChannelSetup *)DmaConfigPtr);
	}
	else {
		return 2;
	}
}

int XAxiVdma_ChannelSetBufferAddr(XAxiVdma_Channel *Channel,
        uint32_t *BufferAddrSet, int NumFrames)
{
	int i;
	uint32_t WordLenBits;
	int HiFrmAddr = 0;
	int FrmBound;
	if (Channel->AddrWidth > 32) {
		FrmBound = (XAXIVDMA_MAX_FRAMESTORE_64)/2 - 1;
	} else {
		FrmBound = (XAXIVDMA_MAX_FRAMESTORE)/2 - 1;
	}
	int Loop16 = 0;

	if (!Channel->IsValid) {
		printf("Channel not initialized\r\n");

		return 2;
	}

	WordLenBits = (uint32_t)(Channel->WordLength - 1);

	/* If hardware has no DRE, then buffer addresses must
	 * be word-aligned
	 */
	for (i = 0; i < NumFrames; i++) {
		if (!Channel->HasDRE) {
			if (BufferAddrSet[i] & WordLenBits) {
				printf("Unaligned address %d: %x without DRE\r\n",
				    i, BufferAddrSet[i]);

				return 15;
			}
		}
	}

	for (i = 0; i < NumFrames; i++, Loop16++) {
		XAxiVdma_Bd *BdPtr = (XAxiVdma_Bd *)(Channel->HeadBdAddr +
		         i * sizeof(XAxiVdma_Bd));

		if (Channel->HasSG) {
			XAxiVdma_BdSetAddr(BdPtr, BufferAddrSet[i]);
		}
		else {
			if ((i > FrmBound) && !HiFrmAddr) {
				XAxiVdma_ChannelHiFrmAddrEnable(Channel);
				HiFrmAddr = 1;
				Loop16 = 0;
			}

			if (Channel->AddrWidth > 32) {
				/* For a 40-bit address XAXIVDMA_MAX_FRAMESTORE
				 * value should be set to 16 */
				XAxiVdma_WriteReg(Channel->StartAddrBase,
					XAXIVDMA_START_ADDR_OFFSET +
					Loop16 * XAXIVDMA_START_ADDR_LEN + i*4,
					LOWER_32_BITS(BufferAddrSet[i]));

				XAxiVdma_WriteReg(Channel->StartAddrBase,
					XAXIVDMA_START_ADDR_MSB_OFFSET +
					Loop16 * XAXIVDMA_START_ADDR_LEN + i*4,
					UPPER_32_BITS((uint64_t)BufferAddrSet[i]));
			} else {
				XAxiVdma_WriteReg(Channel->StartAddrBase,
					XAXIVDMA_START_ADDR_OFFSET +
					Loop16 * XAXIVDMA_START_ADDR_LEN,
					BufferAddrSet[i]);
			}


			if ((NumFrames > FrmBound) && (i == (NumFrames - 1)))
				XAxiVdma_ChannelHiFrmAddrDisable(Channel);
		}
	}

	return 0;
}



int XAxiVdma_DmaSetBufferAddr(XAxiVdma *InstancePtr, uint16_t Direction,
        uint32_t *BufferAddrSet)
{
	XAxiVdma_Channel *Channel;

	Channel = XAxiVdma_GetChannel(InstancePtr, Direction);

	if (!Channel) {
		return 15;
	}

	if (Channel->IsValid) {
		return XAxiVdma_ChannelSetBufferAddr(Channel, BufferAddrSet,
		    Channel->NumFrames);
	}
	else {
		return 2;
	}
}

int XAxiVdma_DmaStart(XAxiVdma *InstancePtr, uint16_t Direction)
{
	XAxiVdma_Channel *Channel;

	Channel = XAxiVdma_GetChannel(InstancePtr, Direction);

	if (!Channel) {
		return 15;
	}

	if (Channel->IsValid) {
		return XAxiVdma_ChannelStart(Channel);
	}
	else {
		return 2;
	}
}

void XAxiVdma_DmaStop(XAxiVdma *InstancePtr, uint16_t Direction)
{
	XAxiVdma_Channel *Channel;

	Channel = XAxiVdma_GetChannel(InstancePtr, Direction);

	if (!Channel) {
		return;
	}

	if (Channel->IsValid) {
		XAxiVdma_ChannelStop(Channel);
	}

	return;
}



/******************************************************************************/

void XAxiVdma_ChannelInit(XAxiVdma_Channel *Channel)
{
	int i;
	int NumFrames;
	XAxiVdma_Bd *FirstBdPtr = &(Channel->BDs[0]);
	XAxiVdma_Bd *LastBdPtr;

	/* Initialize the BD variables, so proper memory management
	 * can be done
	 */
	NumFrames = Channel->NumFrames;
	Channel->IsValid = 0;
	Channel->HeadBdPhysAddr = 0;
	Channel->HeadBdAddr = 0;
	Channel->TailBdPhysAddr = 0;
	Channel->TailBdAddr = 0;

	LastBdPtr = &(Channel->BDs[NumFrames - 1]);

	/* Setup the BD ring
	 */
	memset((void *)FirstBdPtr, 0, NumFrames * sizeof(XAxiVdma_Bd));

	for (i = 0; i < NumFrames; i++) {
		XAxiVdma_Bd *BdPtr;
		XAxiVdma_Bd *NextBdPtr;

		BdPtr = &(Channel->BDs[i]);

		/* The last BD connects to the first BD
		 */
		if (i == (NumFrames - 1)) {
			NextBdPtr = FirstBdPtr;
		}
		else {
			NextBdPtr = &(Channel->BDs[i + 1]);
		}

		XAxiVdma_BdSetNextPtr(BdPtr,
				((uint32_t)vtop(NextBdPtr)));
	}

	Channel->AllCnt = NumFrames;

	/* Setup the BD addresses so that access the head/tail BDs fast
	 *
	 */
	Channel->HeadBdAddr = (uint32_t*)FirstBdPtr;
	Channel->HeadBdPhysAddr = ((uint32_t)vtop(FirstBdPtr));

	Channel->TailBdAddr = (uint32_t)LastBdPtr;
	Channel->TailBdPhysAddr = ((uint32_t)vtop(LastBdPtr));


	Channel->IsValid = 1;

	return;
}

int XAxiVdma_ChannelResetNotDone(XAxiVdma_Channel *Channel)
{
	return (XAxiVdma_ReadReg(Channel->ChanBase, XAXIVDMA_CR_OFFSET) &
	        XAXIVDMA_CR_RESET_MASK);
}

/*****************************************************************************/
/**
 * This function resets one DMA channel
 *
 * The registers will be default values after the reset
 *
 * @param Channel is the pointer to the DMA channel to work on
 *
 * @return
 *  None
 *
 *****************************************************************************/
void XAxiVdma_ChannelReset(XAxiVdma_Channel *Channel)
{
	XAxiVdma_WriteReg(Channel->ChanBase, XAXIVDMA_CR_OFFSET,
	    XAXIVDMA_CR_RESET_MASK);

	return;
}

uint32_t XAxiVdma_ChannelGetStatus(XAxiVdma_Channel *Channel)
{
	return XAxiVdma_ReadReg(Channel->ChanBase, XAXIVDMA_SR_OFFSET);
}

#define XST_INVALID_PARAM               15L

int XAxiVdma_ChannelConfig(XAxiVdma_Channel *Channel,
        XAxiVdma_ChannelSetup *ChannelCfgPtr)
{
	uint32_t CrBits;
	int i;
	int NumBds;
	int Status;
	uint32_t hsize_align;
	uint32_t stride_align;

	if (!Channel->IsValid) {
		printf("Channel not initialized\r\n");

		return XST_FAILURE;
	}

	if (Channel->HasSG && XAxiVdma_ChannelIsBusy(Channel)) {
		printf("Channel is busy, cannot config!\r\n");

		return XST_DEVICE_BUSY;
	}

	Channel->Vsize = ChannelCfgPtr->VertSizeInput;

	/* Check whether Hsize is properly aligned */
	if (Channel->direction == XAXIVDMA_WRITE) {
		if (ChannelCfgPtr->HoriSizeInput < Channel->WordLength) {
			hsize_align = (uint32_t)Channel->WordLength;
		} else {
			hsize_align =
				(uint32_t)(ChannelCfgPtr->HoriSizeInput % Channel->WordLength);
			if (hsize_align > 0)
				hsize_align = (Channel->WordLength - hsize_align);
		}
	} else {
		if (ChannelCfgPtr->HoriSizeInput < Channel->WordLength) {
			hsize_align = (uint32_t)Channel->WordLength;
		} else {
			hsize_align =
				(uint32_t)(ChannelCfgPtr->HoriSizeInput % Channel->StreamWidth);
			if (hsize_align > 0)
				hsize_align = (Channel->StreamWidth - hsize_align);
		}
	}

	/* Check whether Stride is properly aligned */
	if (ChannelCfgPtr->Stride < Channel->WordLength) {
		stride_align = (uint32_t)Channel->WordLength;
	} else {
		stride_align = (uint32_t)(ChannelCfgPtr->Stride % Channel->WordLength);
		if (stride_align > 0)
			stride_align = (Channel->WordLength - stride_align);
	}

	/* If hardware has no DRE, then Hsize and Stride must
	 * be word-aligned
	 */
	if (!Channel->HasDRE) {
		if (hsize_align != 0) {
			/* Adjust hsize to multiples of stream/mm data width*/
			ChannelCfgPtr->HoriSizeInput += hsize_align;
		}
		if (stride_align != 0) {
			/* Adjust stride to multiples of stream/mm data width*/
			ChannelCfgPtr->Stride += stride_align;
		}
	}

	Channel->Hsize = ChannelCfgPtr->HoriSizeInput;

	CrBits = XAxiVdma_ReadReg(Channel->ChanBase,
	     XAXIVDMA_CR_OFFSET);

	CrBits = XAxiVdma_ReadReg(Channel->ChanBase, XAXIVDMA_CR_OFFSET) &
	    ~(XAXIVDMA_CR_TAIL_EN_MASK | XAXIVDMA_CR_SYNC_EN_MASK |
	      XAXIVDMA_CR_FRMCNT_EN_MASK | XAXIVDMA_CR_RD_PTR_MASK);

	if (ChannelCfgPtr->EnableCircularBuf) {
		CrBits |= XAXIVDMA_CR_TAIL_EN_MASK;
	}
	else {
		/* Park mode */
		uint32_t FrmBits;
		uint32_t RegValue;

		if ((!XAxiVdma_ChannelIsRunning(Channel)) &&
		    Channel->HasSG) {
			printf("Channel is not running, cannot set park mode\r\n");

			return XST_INVALID_PARAM;
		}

		if (ChannelCfgPtr->FixedFrameStoreAddr > XAXIVDMA_FRM_MAX) {
			printf("Invalid frame to park on %d\r\n",
			    ChannelCfgPtr->FixedFrameStoreAddr);

			return XST_INVALID_PARAM;
		}

		if (Channel->IsRead) {
			FrmBits = ChannelCfgPtr->FixedFrameStoreAddr &
			              XAXIVDMA_PARKPTR_READREF_MASK;

			RegValue = XAxiVdma_ReadReg(Channel->InstanceBase,
			              XAXIVDMA_PARKPTR_OFFSET);

			RegValue &= ~XAXIVDMA_PARKPTR_READREF_MASK;

			RegValue |= FrmBits;

			XAxiVdma_WriteReg(Channel->InstanceBase,
			    XAXIVDMA_PARKPTR_OFFSET, RegValue);
		}
		else {
			FrmBits = ChannelCfgPtr->FixedFrameStoreAddr <<
			            XAXIVDMA_WRTREF_SHIFT;

			FrmBits &= XAXIVDMA_PARKPTR_WRTREF_MASK;

			RegValue = XAxiVdma_ReadReg(Channel->InstanceBase,
			              XAXIVDMA_PARKPTR_OFFSET);

			RegValue &= ~XAXIVDMA_PARKPTR_WRTREF_MASK;

			RegValue |= FrmBits;

			XAxiVdma_WriteReg(Channel->InstanceBase,
			    XAXIVDMA_PARKPTR_OFFSET, RegValue);
		}
	}

	if (ChannelCfgPtr->EnableSync) {
		if (Channel->GenLock != XAXIVDMA_GENLOCK_MASTER)
			CrBits |= XAXIVDMA_CR_SYNC_EN_MASK;
	}

	if (ChannelCfgPtr->GenLockRepeat) {
		if ((Channel->GenLock == XAXIVDMA_GENLOCK_MASTER) ||
			(Channel->GenLock == XAXIVDMA_DYN_GENLOCK_MASTER))
			CrBits |= XAXIVDMA_CR_GENLCK_RPT_MASK;
	}

	if (ChannelCfgPtr->EnableFrameCounter) {
		CrBits |= XAXIVDMA_CR_FRMCNT_EN_MASK;
	}

	CrBits |= (ChannelCfgPtr->PointNum << XAXIVDMA_CR_RD_PTR_SHIFT) &
	    XAXIVDMA_CR_RD_PTR_MASK;

	/* Write the control register value out
	 */
	XAxiVdma_WriteReg(Channel->ChanBase, XAXIVDMA_CR_OFFSET,
	    CrBits);

	if (Channel->HasSG) {
		/* Setup the information in BDs
		 *
		 * All information is available except the buffer addrs
		 * Buffer addrs are set through XAxiVdma_ChannelSetBufferAddr()
		 */
		NumBds = Channel->AllCnt;

		for (i = 0; i < NumBds; i++) {
			XAxiVdma_Bd *BdPtr = (XAxiVdma_Bd *)(Channel->HeadBdAddr +
			         i * sizeof(XAxiVdma_Bd));

			Status = XAxiVdma_BdSetVsize(BdPtr,
			             ChannelCfgPtr->VertSizeInput);
			if (Status != 0) {
				printf("Set vertical size failed %d\r\n", Status);

				return Status;
			}

			Status = XAxiVdma_BdSetHsize(BdPtr,
			    ChannelCfgPtr->HoriSizeInput);
			if (Status != 0) {
				printf("Set horizontal size failed %d\r\n", Status);

				return Status;
			}

			Status = XAxiVdma_BdSetStride(BdPtr,
			    ChannelCfgPtr->Stride);
			if (Status != 0) {
				printf("Set stride size failed %d\r\n", Status);

				return Status;
			}

			Status = XAxiVdma_BdSetFrmDly(BdPtr,
			ChannelCfgPtr->FrameDelay);
			if (Status != 0) {
				printf("Set frame delay failed %d\r\n", Status);

				return Status;
			}
		}
	}
	else {   /* direct register mode */
		if ((ChannelCfgPtr->VertSizeInput > XAXIVDMA_MAX_VSIZE) ||
		    (ChannelCfgPtr->VertSizeInput <= 0) ||
		    (ChannelCfgPtr->HoriSizeInput > XAXIVDMA_MAX_HSIZE) ||
		    (ChannelCfgPtr->HoriSizeInput <= 0) ||
		    (ChannelCfgPtr->Stride > XAXIVDMA_MAX_STRIDE) ||
		    (ChannelCfgPtr->Stride <= 0) ||
		    (ChannelCfgPtr->FrameDelay < 0) ||
		    (ChannelCfgPtr->FrameDelay > XAXIVDMA_FRMDLY_MAX)) {

			return XST_INVALID_PARAM;
		}

		XAxiVdma_WriteReg(Channel->StartAddrBase,
		    XAXIVDMA_HSIZE_OFFSET, ChannelCfgPtr->HoriSizeInput);

		XAxiVdma_WriteReg(Channel->StartAddrBase,
		    XAXIVDMA_STRD_FRMDLY_OFFSET,
		    (ChannelCfgPtr->FrameDelay << XAXIVDMA_FRMDLY_SHIFT) |
		    ChannelCfgPtr->Stride);
	}

	return 0;
}

#define XAXIVDMA_VERSION_MAJOR_MASK 0xF0000000 /**< Major version */
#define XAXIVDMA_VERSION_MINOR_MASK 0x0FF00000 /**< Minor version */
#define XAXIVDMA_VERSION_REV_MASK   0x000F0000 /**< Revision letter */

#define XAXIVDMA_VERSION_MAJOR_SHIFT 28
#define XAXIVDMA_VERSION_MINOR_SHIFT 20


int XAxiVdma_Major(XAxiVdma *InstancePtr) {
	uint32_t Reg;

	Reg = XAxiVdma_ReadReg(InstancePtr->BaseAddr, XAXIVDMA_VERSION_OFFSET);

	return (int)((Reg & XAXIVDMA_VERSION_MAJOR_MASK) >>
	          XAXIVDMA_VERSION_MAJOR_SHIFT);
}

#define XAXIVDMA_BD_START_ADDR_OFFSET  0x08  /**< Start address */

void XAxiVdma_BdSetAddr(XAxiVdma_Bd *BdPtr, uint32_t Addr)
{
	XAxiVdma_BdWrite(BdPtr, XAXIVDMA_BD_START_ADDR_OFFSET, Addr);

	return;
}

int XAxiVdma_BdSetFrmDly(XAxiVdma_Bd *BdPtr, int FrmDly)
{
	uint32_t Bits;

	if ((FrmDly < 0) || (FrmDly > XAXIVDMA_FRMDLY_MAX)) {
		printf("FrmDly size %d is not valid\r\n", FrmDly);

		return XST_INVALID_PARAM;
	}

	Bits = XAxiVdma_BdRead(BdPtr, XAXIVDMA_BD_STRIDE_OFFSET) &
	        ~XAXIVDMA_FRMDLY_MASK;

	XAxiVdma_BdWrite(BdPtr, XAXIVDMA_BD_STRIDE_OFFSET,
	    Bits | (FrmDly << XAXIVDMA_FRMDLY_SHIFT));

	return XST_SUCCESS;
}

int XAxiVdma_BdSetVsize(XAxiVdma_Bd *BdPtr, int Vsize)
{
	if ((Vsize <= 0) || (Vsize > XAXIVDMA_VSIZE_MASK)) {
		printf("Veritcal size %d is not valid\r\n", Vsize);

		return XST_INVALID_PARAM;
	}

	XAxiVdma_BdWrite(BdPtr, XAXIVDMA_BD_VSIZE_OFFSET, Vsize);
	return XST_SUCCESS;
}

int XAxiVdma_BdSetHsize(XAxiVdma_Bd *BdPtr, int Hsize)
{
	if ((Hsize <= 0) || (Hsize > XAXIVDMA_HSIZE_MASK)) {
		printf("Horizontal size %d is not valid\r\n", Hsize);

		return XST_INVALID_PARAM;
	}

	XAxiVdma_BdWrite(BdPtr, XAXIVDMA_BD_HSIZE_OFFSET, Hsize);
	return XST_SUCCESS;
}

int XAxiVdma_BdSetStride(XAxiVdma_Bd *BdPtr, int Stride)
{
	uint32_t Bits;

	if ((Stride <= 0) || (Stride > XAXIVDMA_STRIDE_MASK)) {
		printf("Stride size %d is not valid\r\n", Stride);

		return XST_INVALID_PARAM;
	}

	Bits = XAxiVdma_BdRead(BdPtr, XAXIVDMA_BD_STRIDE_OFFSET) &
	        ~XAXIVDMA_STRIDE_MASK;

	XAxiVdma_BdWrite(BdPtr, XAXIVDMA_BD_STRIDE_OFFSET, Bits | Stride);

	return XST_SUCCESS;
}

uint32_t XAxiVdma_BdRead(XAxiVdma_Bd *BdPtr, uint32_t Offset)
{
	//return (*((uint32_t*)BdPtr + Offset/4));
	return XAxiVdma_ReadReg(BdPtr, Offset);
}

void XAxiVdma_BdWrite(XAxiVdma_Bd *BdPtr, uint32_t Offset, uint32_t Value)
{
	
	XAxiVdma_WriteReg(BdPtr, Offset, Value);
	//*((uint32_t*)BdPtr + Offset/4) = Value;
	return;
}

int XAxiVdma_ChannelIsRunning(XAxiVdma_Channel *Channel)
{
	uint32_t Bits;

	/* If halted bit set, channel is not running
	 */
	Bits = XAxiVdma_ReadReg(Channel->ChanBase, XAXIVDMA_SR_OFFSET) &
	          XAXIVDMA_SR_HALTED_MASK;

	if (Bits) {
		return 0;
	}

	/* If Run/Stop bit low, then channel is not running
	 */
	Bits = XAxiVdma_ReadReg(Channel->ChanBase, XAXIVDMA_CR_OFFSET) &
	          XAXIVDMA_CR_RUNSTOP_MASK;

	if (!Bits) {
		return 0;
	}

	return 1;
}

int XAxiVdma_ChannelIsBusy(XAxiVdma_Channel *Channel)
{
	uint32_t Bits;

	/* If the channel is idle, then it is not busy
	 */
	Bits = XAxiVdma_ReadReg(Channel->ChanBase, XAXIVDMA_SR_OFFSET) &
	          XAXIVDMA_SR_IDLE_MASK;

	if (Bits) {
		return 0;
	}

	/* If the channel is halted, then it is not busy
	 */
	Bits = XAxiVdma_ReadReg(Channel->ChanBase, XAXIVDMA_SR_OFFSET) &
	          XAXIVDMA_SR_HALTED_MASK;

	if (Bits) {
		return 0;
	}

	/* Otherwise, it is busy
	 */
	return 1;
}

 void XAxiVdma_BdSetNextPtr(XAxiVdma_Bd *BdPtr, uint32_t NextPtr)
{
	XAxiVdma_BdWrite(BdPtr, XAXIVDMA_BD_NDESC_OFFSET, NextPtr);
	return;
}


int XAxiVdma_ChannelStart(XAxiVdma_Channel *Channel)
{
	uint32_t CrBits;

	if (!Channel->IsValid) {
		printf("Channel not initialized\r\n");

		return XST_FAILURE;
	}

	if (Channel->HasSG && XAxiVdma_ChannelIsBusy(Channel)) {

		printf("Start DMA channel while channel is busy\r\n");

		return XST_DEVICE_BUSY;
	}

	/* If channel is not running, setup the CDESC register and
	 * set the channel to run
	 */
	if (!XAxiVdma_ChannelIsRunning(Channel)) {

		if (Channel->HasSG) {
			/* Set up the current bd register
			 *
			 * Can only setup current bd register when channel is halted
			 */
			XAxiVdma_WriteReg(Channel->ChanBase, XAXIVDMA_CDESC_OFFSET,
			    Channel->HeadBdPhysAddr);
		}

		/* Start DMA hardware
		 */
		CrBits = XAxiVdma_ReadReg(Channel->ChanBase,
		     XAXIVDMA_CR_OFFSET);

		CrBits = XAxiVdma_ReadReg(Channel->ChanBase,
		     XAXIVDMA_CR_OFFSET) | XAXIVDMA_CR_RUNSTOP_MASK;

		XAxiVdma_WriteReg(Channel->ChanBase, XAXIVDMA_CR_OFFSET,
		    CrBits);

	}

	if (XAxiVdma_ChannelIsRunning(Channel)) {

		/* Start DMA transfers
		 *
		 */

		if (Channel->HasSG) {
			/* SG mode:
			 * Update the tail pointer so that hardware will start
			 * fetching BDs
			 */
			XAxiVdma_WriteReg(Channel->ChanBase, XAXIVDMA_TDESC_OFFSET,
			   Channel->TailBdPhysAddr);
		}
		else {
			/* Direct register mode:
			 * Update vsize to start the channel
			 */
			XAxiVdma_WriteReg(Channel->StartAddrBase,
			    XAXIVDMA_VSIZE_OFFSET, Channel->Vsize);

		}

		return XST_SUCCESS;
	}
	else {
		printf("Failed to start channel %x\r\n",
			    (unsigned int)Channel->ChanBase);

		return XST_DMA_ERROR;
	}
}

void XAxiVdma_ChannelStop(XAxiVdma_Channel *Channel)
{
	uint32_t CrBits;

	if (!XAxiVdma_ChannelIsRunning(Channel)) {
		return;
	}

	/* Clear the RS bit in CR register
	 */
	CrBits = XAxiVdma_ReadReg(Channel->ChanBase, XAXIVDMA_CR_OFFSET) &
		(~XAXIVDMA_CR_RUNSTOP_MASK);

	XAxiVdma_WriteReg(Channel->ChanBase, XAXIVDMA_CR_OFFSET, CrBits);

	return;
}