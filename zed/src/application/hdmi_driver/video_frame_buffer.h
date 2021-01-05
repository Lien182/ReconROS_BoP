
#ifndef __VIDEO_FRAME_BUFFER_H__
#define __VIDEO_FRAME_BUFFER_H__

#include "axivdma.h"

int vfb_common_init( uint16_t uDeviceId, uint32_t* base_addr, XAxiVdma * InstancePtr );

int vfb_rx_init( XAxiVdma *pAxiVdma, XAxiVdma_DmaSetup *pWriteCfg, uint32_t uVideoResolution, uint32_t uStorageResolution, uint32_t uMemAddr, uint32_t uNumFrames );
int vfb_rx_setup( XAxiVdma *pAxiVdma, XAxiVdma_DmaSetup *pWriteCfg, uint32_t uVideoResolution, uint32_t uStorageResolution, uint32_t uMemAddr, uint32_t uNumFrames );
int vfb_rx_start( XAxiVdma *pAxiVdma );
int vfb_rx_stop ( XAxiVdma *pAxiVdma );

int vfb_tx_init( XAxiVdma *pAxiVdma, XAxiVdma_DmaSetup *pReadCfg , uint32_t uVideoResolution, uint32_t uStorageResolution, uint32_t uMemAddr, uint32_t uNumFrames );
int vfb_tx_setup( XAxiVdma *pAxiVdma, XAxiVdma_DmaSetup *pReadCfg , uint32_t uVideoResolution, uint32_t uStorageResolution, uint32_t uMemAddr, uint32_t uNumFrames );
int vfb_tx_start( XAxiVdma *pAxiVdma );
int vfb_tx_stop ( XAxiVdma *pAxiVdma );

int vfb_dump_registers( XAxiVdma *pAxiVdma);
int vfb_check_errors( XAxiVdma *pAxiVdma, uint8_t bClearErrors );

#endif // __VIDEO_FRAME_BUFFER_H__

