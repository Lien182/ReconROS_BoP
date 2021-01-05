#include "xvtc.h"

#define XPAR_XVTC_NUM_INSTANCES 1

#define XVTC_GPOL_OFFSET	0x06C	/**< Generator Polarity Offset */
#define XVTC_POL_ALLP_MASK	0x0000007F	/**< Bit mask for all */



void XVtc_SetPolarity(XVtc *InstancePtr, XVtc_Polarity *PolarityPtr)
{
	uint32_t PolRegValue;

	/* Verify arguments. */

	/* Read Control register value back and clear all polarity
	 * bits first
	 */
	PolRegValue = XVtc_ReadReg(InstancePtr->Config.BaseAddress,
					(XVTC_GPOL_OFFSET));
	PolRegValue &= (uint32_t)(~(XVTC_POL_ALLP_MASK));

	/* Change the register value according to the setting in the Polarity
	 * configuration structure
	 */
	if (PolarityPtr->ActiveChromaPol)
		PolRegValue |= XVTC_POL_ACP_MASK;

	if (PolarityPtr->ActiveVideoPol)
		PolRegValue |= XVTC_POL_AVP_MASK;

	if (PolarityPtr->FieldIdPol)
		PolRegValue |= XVTC_POL_FIP_MASK;

	if (PolarityPtr->VBlankPol)
		PolRegValue |= XVTC_POL_VBP_MASK;

	if (PolarityPtr->VSyncPol)
		PolRegValue |= XVTC_POL_VSP_MASK;

	if (PolarityPtr->HBlankPol)
		PolRegValue |= XVTC_POL_HBP_MASK;

	if (PolarityPtr->HSyncPol)
		PolRegValue |= XVTC_POL_HSP_MASK;

	XVtc_WriteReg(InstancePtr->Config.BaseAddress, (XVTC_GPOL_OFFSET),
			PolRegValue);
}


void XVtc_SetGeneratorHoriOffset(XVtc *InstancePtr,
				XVtc_HoriOffsets *HoriOffsets)
{
	uint32_t RegValue;

	/* Calculate and update Generator VBlank Hori. Offset 0 register value
	 */
	RegValue = (HoriOffsets->V0BlankHoriStart) & XVTC_XVXHOX_HSTART_MASK;
	RegValue |= (HoriOffsets->V0BlankHoriEnd << XVTC_XVXHOX_HEND_SHIFT) &
					XVTC_XVXHOX_HEND_MASK;
	XVtc_WriteReg(InstancePtr->Config.BaseAddress, XVTC_GVBHOFF_OFFSET,
								RegValue);

	/* Calculate and update Generator VSync Hori. Offset 0 register
	 * value
	 */
	RegValue = (HoriOffsets->V0SyncHoriStart) & XVTC_XVXHOX_HSTART_MASK;
	RegValue |= (HoriOffsets->V0SyncHoriEnd << XVTC_XVXHOX_HEND_SHIFT) &
					XVTC_XVXHOX_HEND_MASK;
	XVtc_WriteReg(InstancePtr->Config.BaseAddress, XVTC_GVSHOFF_OFFSET,
								RegValue);

	/* Calculate and update Generator VBlank Hori. Offset 1 register
	 * value
	 */
	RegValue = (HoriOffsets->V1BlankHoriStart) & XVTC_XVXHOX_HSTART_MASK;
	RegValue |= (HoriOffsets->V1BlankHoriEnd << XVTC_XVXHOX_HEND_SHIFT) &
					XVTC_XVXHOX_HEND_MASK;
	XVtc_WriteReg(InstancePtr->Config.BaseAddress, XVTC_GVBHOFF_F1_OFFSET,
								RegValue);

	/* Calculate and update Generator VSync Hori. Offset 1 register
	 * value
	 */
	RegValue = (HoriOffsets->V1SyncHoriStart) & XVTC_XVXHOX_HSTART_MASK;
	RegValue |= (HoriOffsets->V1SyncHoriEnd << XVTC_XVXHOX_HEND_SHIFT) &
					XVTC_XVXHOX_HEND_MASK;
	XVtc_WriteReg(InstancePtr->Config.BaseAddress,
						XVTC_GVSHOFF_F1_OFFSET,
								RegValue);
}

void XVtc_SetGenerator(XVtc *InstancePtr, XVtc_Signal *SignalCfgPtr)
{
	uint32_t RegValue;
	uint32_t r_htotal, r_vtotal, r_hactive, r_vactive;
	XVtc_Signal *SCPtr;
	XVtc_HoriOffsets horiOffsets;

	SCPtr = SignalCfgPtr;
	if(SCPtr->OriginMode == 0)
	{
		r_htotal = SCPtr->HTotal+1;
		r_vtotal = SCPtr->V0Total+1;

		r_hactive = r_htotal - SCPtr->HActiveStart;
		r_vactive = r_vtotal - SCPtr->V0ActiveStart;

		RegValue = (r_htotal) & XVTC_SB_START_MASK;
		XVtc_WriteReg(InstancePtr->Config.BaseAddress,
					XVTC_GHSIZE_OFFSET, RegValue);

		RegValue = (r_vtotal) & XVTC_VSIZE_F0_MASK;
		RegValue |= ((SCPtr->V1Total+1) << XVTC_VSIZE_F1_SHIFT) &
							XVTC_VSIZE_F1_MASK;
		XVtc_WriteReg(InstancePtr->Config.BaseAddress,
					XVTC_GVSIZE_OFFSET, RegValue);


		RegValue = (r_hactive) & XVTC_ASIZE_HORI_MASK;
		RegValue |= ((r_vactive) << XVTC_ASIZE_VERT_SHIFT ) &
							XVTC_ASIZE_VERT_MASK;
		XVtc_WriteReg(InstancePtr->Config.BaseAddress,
						XVTC_GASIZE_OFFSET, RegValue);

		/* Update the Generator Horizontal 1 Register */
		RegValue = (SCPtr->HSyncStart + r_hactive) &
						XVTC_SB_START_MASK;
		RegValue |= ((SCPtr->HBackPorchStart + r_hactive) <<
				XVTC_SB_END_SHIFT) & XVTC_SB_END_MASK;
		XVtc_WriteReg(InstancePtr->Config.BaseAddress,
					XVTC_GHSYNC_OFFSET, RegValue);

		/* Update the Generator Vertical 1 Register (field 0) */
		RegValue = (SCPtr->V0SyncStart + r_vactive -1) &
						XVTC_SB_START_MASK;
		RegValue |= ((SCPtr->V0BackPorchStart + r_vactive -1) <<
				XVTC_SB_END_SHIFT) & XVTC_SB_END_MASK;
		XVtc_WriteReg(InstancePtr->Config.BaseAddress,
					XVTC_GVSYNC_OFFSET, RegValue);

		/* Update the Generator Vertical Sync Register (field 1) */
		RegValue = (SCPtr->V1SyncStart + r_vactive -1) &
						XVTC_SB_START_MASK;
		RegValue |= ((SCPtr->V1BackPorchStart + r_vactive -1) <<
					XVTC_SB_END_SHIFT) & XVTC_SB_END_MASK;
		XVtc_WriteReg(InstancePtr->Config.BaseAddress,
					XVTC_GVSYNC_F1_OFFSET, RegValue);

		/* Chroma Start */
		RegValue = XVtc_ReadReg(InstancePtr->Config.BaseAddress,
							XVTC_GFENC_OFFSET);
		RegValue &= ~XVTC_ENC_CPARITY_MASK;
		RegValue = (((SCPtr->V0ChromaStart - SCPtr->V0ActiveStart) <<
						XVTC_ENC_CPARITY_SHIFT) &
					XVTC_ENC_CPARITY_MASK) | RegValue;

		RegValue &= ~XVTC_ENC_PROG_MASK;
		RegValue |= (SCPtr->Interlaced << XVTC_ENC_PROG_SHIFT) &
				XVTC_ENC_PROG_MASK;

		XVtc_WriteReg(InstancePtr->Config.BaseAddress,
						XVTC_GFENC_OFFSET, RegValue);

		/* Setup default Horizontal Offsets - can override later with
		 * XVtc_SetGeneratorHoriOffset()
		 */
		horiOffsets.V0BlankHoriStart = r_hactive;
		horiOffsets.V0BlankHoriEnd = r_hactive;
		horiOffsets.V0SyncHoriStart = SCPtr->HSyncStart + r_hactive;
		horiOffsets.V0SyncHoriEnd = SCPtr->HSyncStart + r_hactive;

		horiOffsets.V1BlankHoriStart = r_hactive;
		horiOffsets.V1BlankHoriEnd = r_hactive;
		horiOffsets.V1SyncHoriStart = SCPtr->HSyncStart + r_hactive;
		horiOffsets.V1SyncHoriEnd = SCPtr->HSyncStart + r_hactive;

	}
	else
	{
		/* Total in mode=1 is the line width */
		r_htotal = SCPtr->HTotal;
		/* Total in mode=1 is the frame height */
		r_vtotal = SCPtr->V0Total;
		r_hactive = SCPtr->HFrontPorchStart;
		r_vactive = SCPtr->V0FrontPorchStart;

		RegValue = (r_htotal) & XVTC_SB_START_MASK;
		XVtc_WriteReg(InstancePtr->Config.BaseAddress,
					XVTC_GHSIZE_OFFSET, RegValue);

		RegValue = (r_vtotal) & XVTC_VSIZE_F0_MASK;
		RegValue |= ((SCPtr->V1Total) << XVTC_VSIZE_F1_SHIFT) &
							XVTC_VSIZE_F1_MASK;
		XVtc_WriteReg(InstancePtr->Config.BaseAddress,
						XVTC_GVSIZE_OFFSET, RegValue);


		RegValue = (r_hactive) & XVTC_ASIZE_HORI_MASK;
		RegValue |= ((r_vactive) << XVTC_ASIZE_VERT_SHIFT) &
							XVTC_ASIZE_VERT_MASK;
		XVtc_WriteReg(InstancePtr->Config.BaseAddress,
						XVTC_GASIZE_OFFSET, RegValue);

		/* Update the Generator Horizontal 1 Register */
		RegValue = (SCPtr->HSyncStart) & XVTC_SB_START_MASK;
		RegValue |= ((SCPtr->HBackPorchStart) << XVTC_SB_END_SHIFT) &
						XVTC_SB_END_MASK;
		XVtc_WriteReg(InstancePtr->Config.BaseAddress,
					XVTC_GHSYNC_OFFSET, RegValue);


		/* Update the Generator Vertical Sync Register (field 0) */
		RegValue = (SCPtr->V0SyncStart) & XVTC_SB_START_MASK;
		RegValue |= ((SCPtr->V0BackPorchStart) << XVTC_SB_END_SHIFT) &
						XVTC_SB_END_MASK;
		XVtc_WriteReg(InstancePtr->Config.BaseAddress,
						XVTC_GVSYNC_OFFSET, RegValue);

		/* Update the Generator Vertical Sync Register (field 1) */
		RegValue = (SCPtr->V1SyncStart) & XVTC_SB_START_MASK;
		RegValue |= ((SCPtr->V1BackPorchStart) << XVTC_SB_END_SHIFT) &
						XVTC_SB_END_MASK;
		XVtc_WriteReg(InstancePtr->Config.BaseAddress,
					XVTC_GVSYNC_F1_OFFSET, RegValue);

		/* Chroma Start */
		  RegValue = XVtc_ReadReg(InstancePtr->Config.BaseAddress,
							XVTC_GFENC_OFFSET);
		RegValue &= ~XVTC_ENC_CPARITY_MASK;
		RegValue = (((SCPtr->V0ChromaStart - SCPtr->V0ActiveStart) <<
							XVTC_ENC_CPARITY_SHIFT)
					& XVTC_ENC_CPARITY_MASK) | RegValue;

		RegValue &= ~XVTC_ENC_PROG_MASK;
		RegValue |= (SCPtr->Interlaced << XVTC_ENC_PROG_SHIFT) &
						XVTC_ENC_PROG_MASK;

		XVtc_WriteReg(InstancePtr->Config.BaseAddress,
					XVTC_GFENC_OFFSET, RegValue);

		/* Setup default Horizontal Offsets - can override later with
		 * XVtc_SetGeneratorHoriOffset()
		 */
		horiOffsets.V0BlankHoriStart = r_hactive;
		horiOffsets.V0BlankHoriEnd = r_hactive;
		horiOffsets.V0SyncHoriStart = SCPtr->HSyncStart;
		horiOffsets.V0SyncHoriEnd = SCPtr->HSyncStart;
		horiOffsets.V1BlankHoriStart = r_hactive;
		horiOffsets.V1BlankHoriEnd = r_hactive;
		horiOffsets.V1SyncHoriStart = SCPtr->HSyncStart;
		horiOffsets.V1SyncHoriEnd = SCPtr->HSyncStart;

	}
	XVtc_SetGeneratorHoriOffset(InstancePtr, &horiOffsets);

}


#define XPAR_ZED_HDMI_DISPLAY_V_TC_0_DEVICE_ID 0
#define XPAR_ZED_HDMI_DISPLAY_V_TC_0_BASEADDR 0x43C00000
#define XPAR_ZED_HDMI_DISPLAY_V_TC_0_HIGHADDR 0x43C0FFFF
#define XPAR_ZED_HDMI_DISPLAY_V_TC_0_GENERATE_EN 1
#define XPAR_ZED_HDMI_DISPLAY_V_TC_0_DETECT_EN 0
#define XPAR_ZED_HDMI_DISPLAY_V_TC_0_DET_HSYNC_EN 1
#define XPAR_ZED_HDMI_DISPLAY_V_TC_0_DET_VSYNC_EN 1
#define XPAR_ZED_HDMI_DISPLAY_V_TC_0_DET_HBLANK_EN 1
#define XPAR_ZED_HDMI_DISPLAY_V_TC_0_DET_VBLANK_EN 1
#define XPAR_ZED_HDMI_DISPLAY_V_TC_0_DET_AVIDEO_EN 1
#define XPAR_ZED_HDMI_DISPLAY_V_TC_0_DET_ACHROMA_EN 0

XVtc_Config XVtc_ConfigTable[XPAR_XVTC_NUM_INSTANCES] =
{
	{
		XPAR_ZED_HDMI_DISPLAY_V_TC_0_DEVICE_ID,
		XPAR_ZED_HDMI_DISPLAY_V_TC_0_BASEADDR
	}
};



XVtc_Config *XVtc_LookupConfig(uint16_t  DeviceId)
{
	extern XVtc_Config XVtc_ConfigTable[];
	XVtc_Config *CfgPtr = 0;
	int i;

	/* Checking for device id for which instance it is matching */
	for (i = 0; i < XPAR_XVTC_NUM_INSTANCES; i++) {
		/* Assigning address of config table if both device ids
		 * are matched
		 */
		if (XVtc_ConfigTable[i].DeviceId == DeviceId) {
			CfgPtr = &XVtc_ConfigTable[i];
			break;
		}
	}

	return CfgPtr;
}

void XVtc_DisableGenerator(XVtc *InstancePtr)
{
	uint32_t CtrlRegValue;

	/* Read Control register value back */
	CtrlRegValue = XVtc_ReadReg(InstancePtr->Config.BaseAddress,
					(XVTC_CTL_OFFSET));

	/* Change the value according to the disabling type and write it
	 * back
	 */
	CtrlRegValue &= (uint32_t)(~(XVTC_CTL_GE_MASK));

	XVtc_WriteReg(InstancePtr->Config.BaseAddress, (XVTC_CTL_OFFSET),
			CtrlRegValue);
}


void XVtc_SetSource(XVtc *InstancePtr, XVtc_SourceSelect *SourcePtr)
{
	uint32_t CtrlRegValue;

	/* Read Control register value back and clear all source selection bits
	 * first
	 */
	CtrlRegValue = XVtc_ReadReg(InstancePtr->Config.BaseAddress,
					(XVTC_CTL_OFFSET));
	CtrlRegValue &= ~XVTC_CTL_ALLSS_MASK;

	/* Change the register value according to the setting in the source
	 * selection configuration structure
	 */

	if (SourcePtr->FieldIdPolSrc)
		CtrlRegValue |= XVTC_CTL_FIPSS_MASK;

	if (SourcePtr->ActiveChromaPolSrc)
		CtrlRegValue |= XVTC_CTL_ACPSS_MASK;

	if (SourcePtr->ActiveVideoPolSrc)
		CtrlRegValue |= XVTC_CTL_AVPSS_MASK;

	if (SourcePtr->HSyncPolSrc)
		CtrlRegValue |= XVTC_CTL_HSPSS_MASK;

	if (SourcePtr->VSyncPolSrc)
		CtrlRegValue |= XVTC_CTL_VSPSS_MASK;

	if (SourcePtr->HBlankPolSrc)
		CtrlRegValue |= XVTC_CTL_HBPSS_MASK;

	if (SourcePtr->VBlankPolSrc)
		CtrlRegValue |= XVTC_CTL_VBPSS_MASK;


	if (SourcePtr->VChromaSrc)
		CtrlRegValue |= XVTC_CTL_VCSS_MASK;

	if (SourcePtr->VActiveSrc)
		CtrlRegValue |= XVTC_CTL_VASS_MASK;

	if (SourcePtr->VBackPorchSrc)
		CtrlRegValue |= XVTC_CTL_VBSS_MASK;

	if (SourcePtr->VSyncSrc)
		CtrlRegValue |= XVTC_CTL_VSSS_MASK;

	if (SourcePtr->VFrontPorchSrc)
		CtrlRegValue |= XVTC_CTL_VFSS_MASK;

	if (SourcePtr->VTotalSrc)
		CtrlRegValue |= XVTC_CTL_VTSS_MASK;

	if (SourcePtr->HBackPorchSrc)
		CtrlRegValue |= XVTC_CTL_HBSS_MASK;

	if (SourcePtr->HSyncSrc)
		CtrlRegValue |= XVTC_CTL_HSSS_MASK;

	if (SourcePtr->HFrontPorchSrc)
		CtrlRegValue |= XVTC_CTL_HFSS_MASK;

	if (SourcePtr->HTotalSrc)
		CtrlRegValue |= XVTC_CTL_HTSS_MASK;

	if (SourcePtr->InterlacedMode)
		CtrlRegValue |= XVTC_CTL_INTERLACE_MASK;

	XVtc_WriteReg(InstancePtr->Config.BaseAddress, (XVTC_CTL_OFFSET),
			CtrlRegValue);
}


void XVtc_EnableGenerator(XVtc *InstancePtr)
{
	uint32_t CtrlRegValue;

	/* Read Control register value back */
	CtrlRegValue = XVtc_ReadReg(InstancePtr->Config.BaseAddress,
					(XVTC_CTL_OFFSET));

	/* Change the value according to the enabling type and write it back */
	CtrlRegValue |= XVTC_CTL_GE_MASK;

	XVtc_WriteReg(InstancePtr->Config.BaseAddress, (XVTC_CTL_OFFSET),
			CtrlRegValue);
}

void StubCallBack(void *CallBackRef)
{
	(void)CallBackRef;
}

static void StubErrCallBack(void *CallBackRef, uint32_t ErrorMask)
{
	(void)CallBackRef;
	(void)ErrorMask;
}

#define XIL_COMPONENT_IS_READY     0x11111111U

int XVtc_CfgInitialize(XVtc *InstancePtr, XVtc_Config *CfgPtr,
				uint32_t* EffectiveAddr)
{
	/* Setup the instance */
	memset((void *)InstancePtr, 0, sizeof(XVtc));

	memcpy((void *)&(InstancePtr->Config), (const void *)CfgPtr,
			   sizeof(XVtc_Config));
	InstancePtr->Config.BaseAddress = EffectiveAddr;

	/* Set all handlers to stub values, let user configure this data later */
	InstancePtr->FrameSyncCallBack = (XVtc_CallBack) StubCallBack;
	InstancePtr->LockCallBack = (XVtc_CallBack) StubCallBack;
	InstancePtr->DetectorCallBack = (XVtc_CallBack) StubCallBack;
	InstancePtr->GeneratorCallBack = (XVtc_CallBack) StubCallBack;
	InstancePtr->ErrCallBack = (XVtc_ErrorCallBack) StubErrCallBack;

	/* Set the flag to indicate the driver is ready */
	InstancePtr->IsReady = (uint32_t)(XIL_COMPONENT_IS_READY);

	return XST_SUCCESS;
}

#define XVTC_REG_ADDRGAP	4	/**< Register address gap */

void XVtc_SetFSync(XVtc *InstancePtr, uint16_t FrameSyncIndex, uint16_t VertStart,
			uint16_t HoriStart)
{
	uint32_t RegValue;
	uint32_t RegAddress;



	/* Calculate the sync value */
	RegValue = HoriStart & XVTC_FSXX_HSTART_MASK;
	RegValue |= (VertStart << XVTC_FSXX_VSTART_SHIFT) &
			XVTC_FSXX_VSTART_MASK;

	/* Calculate the frame sync register address to write to */
	RegAddress = XVTC_FS00_OFFSET + FrameSyncIndex * XVTC_REG_ADDRGAP;

	/* Update the Generator Global Delay register w/ the value */
	XVtc_WriteReg(InstancePtr->Config.BaseAddress, RegAddress, RegValue);
}