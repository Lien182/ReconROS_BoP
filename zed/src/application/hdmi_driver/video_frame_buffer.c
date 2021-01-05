#include <stdio.h>

//#include "platform.h"
#include "hdmi_display_bsp/ps7_cortexa9_0/include/xparameters.h"

#include "video_resolution.h"

#include "axivdma.h"

#include <unistd.h>

#define NUMBER_OF_READ_FRAMES    XPAR_AXIVDMA_0_NUM_FSTORES
#define NUMBER_OF_WRITE_FRAMES   XPAR_AXIVDMA_0_NUM_FSTORES

int vfb_common_init( uint16_t uDeviceId, uint32_t* base_addr, XAxiVdma *pAxiVdma )
{
   int Status;
   XAxiVdma_Config *Config;

   Config = XAxiVdma_LookupConfig( uDeviceId );
   if (!Config)
   {
      printf( "No video DMA found for ID %d\n\r", uDeviceId);
      return 1;
   }

   Config->BaseAddress = base_addr;

   /* Initialize DMA engine */
   Status = XAxiVdma_CfgInitialize(pAxiVdma, Config, Config->BaseAddress);
   if (Status != 0)
   {
      printf( "Initialization failed %d\n\r", Status);
      return 1;
   }

   return 0;
}

int vfb_rx_init( XAxiVdma *pAxiVdma, XAxiVdma_DmaSetup *pWriteCfg, uint32_t uVideoResolution, uint32_t uStorageResolution, uint32_t uMemAddr, uint32_t uNumFrames )
{
   int Status;
   XAxiVdma_FrameCounter FrameCfg;
   uint32_t uBaseAddr;
   uint32_t uDMACR;

   /* Setup the write channel */
   Status = vfb_rx_setup(pAxiVdma,pWriteCfg,uVideoResolution,uStorageResolution,uMemAddr,uNumFrames);
   if (Status != 0) {
           printf("Write channel setup failed %d\r\n", Status);

           return 1;
   }

   /* Start the DMA engine to transfer
	*/
   Status = vfb_rx_start(pAxiVdma);
   if (Status != 0) {
		   return 1;
   }

	XAxiVdma_FsyncSrcSelect(pAxiVdma, XAXIVDMA_S2MM_TUSER_FSYNC, XAXIVDMA_WRITE);

}

int vfb_tx_init( XAxiVdma *pAxiVdma, XAxiVdma_DmaSetup *pReadCfg, uint32_t uVideoResolution, uint32_t uStorageResolution, uint32_t uMemAddr, uint32_t uNumFrames )
{
   int Status;
   XAxiVdma_FrameCounter FrameCfg;
   uint32_t uBaseAddr;
   uint32_t uDMACR;

   /* Setup the read channel */
   Status = vfb_tx_setup(pAxiVdma,pReadCfg,uVideoResolution,uStorageResolution,uMemAddr,uNumFrames);
   if (Status != 0) {
		   printf("Read channel setup failed %d\n\r", Status);

		   return 1;
   }

   /* Start the DMA engine to transfer
	*/
   Status = vfb_tx_start(pAxiVdma);
   if (Status != 0) {
		   return 1;
   }

#if 0
#if 0
	// This function returns prematurely due to (!Channel->GenLock) evaluating to false
	XAxiVdma_GenLockSourceSelect(pAxiVdma, XAXIVDMA_INTERNAL_GENLOCK, XAXIVDMA_READ);
#else
	uBaseAddr = pAxiVdma->BaseAddr;
	printf( "\t MM2S_DMACR       = 0x%08X\n\r", *((volatile int *)(uBaseAddr+XAXIVDMA_TX_OFFSET+XAXIVDMA_CR_OFFSET )) );
	uDMACR = *((volatile int *)(uBaseAddr+XAXIVDMA_TX_OFFSET+XAXIVDMA_CR_OFFSET ));
	uDMACR |= 0x00000080;
	*((volatile int *)(uBaseAddr+XAXIVDMA_TX_OFFSET+XAXIVDMA_CR_OFFSET )) = uDMACR;
	printf( "\t MM2S_DMACR       = 0x%08X\n\r", *((volatile int *)(uBaseAddr+XAXIVDMA_TX_OFFSET+XAXIVDMA_CR_OFFSET )) );
#endif
#endif
}

#define ___SC_PAGESIZE 0x1000

uintptr_t vtop(uintptr_t vaddr) {
  FILE *pagemap;
    intptr_t paddr = 0;
    int offset = (vaddr / sysconf(_SC_PAGESIZE)) * sizeof(uint64_t);
    uint64_t e;

    // https://www.kernel.org/doc/Documentation/vm/pagemap.txt
    if ((pagemap = fopen("/proc/self/pagemap", "r"))) {
        if (lseek(fileno(pagemap), offset, SEEK_SET) == offset) {
            if (fread(&e, sizeof(uint64_t), 1, pagemap)) {
                if (e & (1ULL << 63)) { // page present ?
                    paddr = e & ((1ULL << 54) - 1); // pfn mask
                    paddr = paddr * sysconf(_SC_PAGESIZE);
                    // add offset within page
                    paddr = paddr | (vaddr & (sysconf(_SC_PAGESIZE) - 1));
                }   
            }   
        }   
        fclose(pagemap);
    }   

    return paddr;
}   


int vfb_rx_setup(XAxiVdma *pAxiVdma, XAxiVdma_DmaSetup *pWriteCfg, uint32_t uVideoResolution, uint32_t uStorageResolution, uint32_t uMemAddr, uint32_t uNumFrames )
{
	int i;
	uint32_t Addr;
	int Status;

	uint32_t video_width, video_height;
	uint32_t storage_width, storage_height, storage_stride, storage_size, storage_offset;

	// Get Video dimensions
	video_height = vres_get_height( uVideoResolution );      // in lines
	video_width  = vres_get_width ( uVideoResolution ) << 2; // in bytes

	// Get Storage dimensions
	storage_height = vres_get_height( uStorageResolution );      // in lines
	storage_width  = vres_get_width ( uStorageResolution ) << 2; // in bytes
	storage_stride = storage_width;
	storage_size   = storage_width * storage_height;
	storage_offset = ((storage_height-video_height)>>1)*storage_width + ((storage_width-video_width)>>1);

	pWriteCfg->VertSizeInput = video_height;
	pWriteCfg->HoriSizeInput = video_width;
	pWriteCfg->Stride        = storage_stride;

	pWriteCfg->FrameDelay = 0;  /* This example does not test frame delay */

	pWriteCfg->EnableCircularBuf = 1;
	pWriteCfg->EnableSync = 1;

	pWriteCfg->PointNum = 1;
	pWriteCfg->EnableFrameCounter = 0; /* Endless transfers */

	pWriteCfg->FixedFrameStoreAddr = 0; /* We are not doing parking */

	Status = XAxiVdma_DmaConfig(pAxiVdma, XAXIVDMA_WRITE, pWriteCfg);
	if (Status != 0) {
			printf("Write channel config failed %d\r\n", Status);

			return 1;
	}

	/* Initialize buffer addresses
	 *
	 * Use physical addresses
	 */
	Addr = vtop(uMemAddr) + storage_offset;
	for(i = 0; i < uNumFrames; i++)
	{
		pWriteCfg->FrameStoreStartAddr[i] = Addr;

		Addr += storage_size;
	}

	/* Set the buffer addresses for transfer in the DMA engine
	 */
	Status = XAxiVdma_DmaSetBufferAddr(pAxiVdma, XAXIVDMA_WRITE,
			pWriteCfg->FrameStoreStartAddr);
	if (Status != 0) {
			printf("Write channel set buffer address failed %d\r\n", Status);

			return 1;
	}


	return 0;
}

int vfb_tx_setup(XAxiVdma *pAxiVdma, XAxiVdma_DmaSetup *pReadCfg, uint32_t uVideoResolution, uint32_t uStorageResolution, uint32_t uMemAddr, uint32_t uNumFrames )
{
	int i;
	uint32_t Addr;
	int Status;

	uint32_t video_width, video_height;
	uint32_t storage_width, storage_height, storage_stride, storage_size, storage_offset;

	// Get Video dimensions
	video_height = vres_get_height( uVideoResolution );      // in lines
	video_width  = vres_get_width ( uVideoResolution ) << 2; // in bytes

	// Get Storage dimensions
	storage_height = vres_get_height( uStorageResolution );      // in lines
	storage_width  = vres_get_width ( uStorageResolution ) << 2; // in bytes
	storage_stride = storage_width;
	storage_size   = storage_width * storage_height;
	storage_offset = ((storage_height-video_height)>>1)*storage_width + ((storage_width-video_width)>>1);

	pReadCfg->VertSizeInput = video_height;
	pReadCfg->HoriSizeInput = video_width;
	pReadCfg->Stride        = storage_stride;

	printf("vert_size = %d, hori_size = %d, stride = %d \n", pReadCfg->VertSizeInput,pReadCfg->HoriSizeInput,pReadCfg->Stride   );


	pReadCfg->FrameDelay = 0;  /* This example does not test frame delay */

	pReadCfg->EnableCircularBuf = 1;
	pReadCfg->EnableSync = 1;

	pReadCfg->PointNum = 1;
	pReadCfg->EnableFrameCounter = 0; /* Endless transfers */

	pReadCfg->FixedFrameStoreAddr = 0; /* We are not doing parking */

	Status = XAxiVdma_DmaConfig(pAxiVdma, XAXIVDMA_READ, pReadCfg);
	if (Status != 0) {
			printf("Read channel config failed %d\n\r", Status);

			return 1;
	}

	/* Initialize buffer addresses
	 *
	 * These addresses are physical addresses
	 */
	Addr = vtop(uMemAddr) + storage_offset;
	printf("physicaladdress: %x \n", Addr);
	for(i = 0; i < uNumFrames; i++)
	{
		pReadCfg->FrameStoreStartAddr[i] = Addr;

		Addr += storage_size;
	}

	/* Set the buffer addresses for transfer in the DMA engine
	 * The buffer addresses are physical addresses
	 */
	Status = XAxiVdma_DmaSetBufferAddr(pAxiVdma, XAXIVDMA_READ,
			pReadCfg->FrameStoreStartAddr);
	if (Status != 0) {
			printf("Read channel set buffer address failed %d\n\r", Status);

			return 1;
	}

	return 0;
}

int vfb_rx_start(XAxiVdma *pAxiVdma)
{
   int Status;

   // S2MM Startup
   Status = XAxiVdma_DmaStart(pAxiVdma, XAXIVDMA_WRITE);
   if (Status != 0)
   {
      printf( "Start Write transfer failed %d\r\n", Status);
      return 1;
   }

   return 0;
}

int vfb_tx_start(XAxiVdma *pAxiVdma)
{
   int Status;

   // MM2S Startup
   Status = XAxiVdma_DmaStart(pAxiVdma, XAXIVDMA_READ);
   if (Status != 0)
   {
      printf("Start read transfer failed %d\n\r", Status);
      return 1;
   }

   return 0;
}

int vfb_rx_stop(XAxiVdma *pAxiVdma)
{
   // S2MM Stop
   XAxiVdma_DmaStop(pAxiVdma, XAXIVDMA_WRITE);

   return 0;
}

int vfb_tx_stop(XAxiVdma *pAxiVdma)
{
   // MM2S Stop
   XAxiVdma_DmaStop(pAxiVdma, XAXIVDMA_READ);

   return 0;
}


int vfb_dump_registers(XAxiVdma *pAxiVdma)
{
   uint32_t* uBaseAddr = pAxiVdma->BaseAddr;

   // Partial Register Dump
   printf( "AXI_VDMA - Partial Register Dump (uBaseAddr = 0x%08X):\n\r", uBaseAddr );
   printf( "\t PARKPTR          = 0x%08X\n\r", XAxiVdma_ReadReg(uBaseAddr,XAXIVDMA_PARKPTR_OFFSET ));
   printf( "\t ----------------\n\r" );
   printf( "\t S2MM_DMACR       = 0x%08X\n\r", XAxiVdma_ReadReg(uBaseAddr,XAXIVDMA_RX_OFFSET+XAXIVDMA_CR_OFFSET) );
   printf( "\t S2MM_DMASR       = 0x%08X\n\r", XAxiVdma_ReadReg(uBaseAddr,XAXIVDMA_RX_OFFSET+XAXIVDMA_SR_OFFSET) );
   printf( "\t S2MM_STRD_FRMDLY = 0x%08X\n\r", XAxiVdma_ReadReg(uBaseAddr,XAXIVDMA_S2MM_ADDR_OFFSET+XAXIVDMA_STRD_FRMDLY_OFFSET) );
   printf( "\t S2MM_START_ADDR0 = 0x%08X\n\r", XAxiVdma_ReadReg(uBaseAddr,XAXIVDMA_S2MM_ADDR_OFFSET+XAXIVDMA_START_ADDR_OFFSET) );
   printf( "\t S2MM_START_ADDR1 = 0x%08X\n\r", XAxiVdma_ReadReg(uBaseAddr,XAXIVDMA_S2MM_ADDR_OFFSET+XAXIVDMA_START_ADDR_OFFSET+4) );
   printf( "\t S2MM_START_ADDR2 = 0x%08X\n\r", XAxiVdma_ReadReg(uBaseAddr,XAXIVDMA_S2MM_ADDR_OFFSET+XAXIVDMA_START_ADDR_OFFSET+8) );
   printf( "\t S2MM_HSIZE       = 0x%08X\n\r", XAxiVdma_ReadReg(uBaseAddr,XAXIVDMA_S2MM_ADDR_OFFSET+XAXIVDMA_HSIZE_OFFSET) );
   printf( "\t S2MM_VSIZE       = 0x%08X\n\r", XAxiVdma_ReadReg(uBaseAddr,XAXIVDMA_S2MM_ADDR_OFFSET+XAXIVDMA_VSIZE_OFFSET) );
   printf( "\t ----------------\n\r" );
   printf( "\t MM2S_DMACR       = 0x%08X\n\r", XAxiVdma_ReadReg(uBaseAddr,XAXIVDMA_TX_OFFSET+XAXIVDMA_CR_OFFSET) );
   printf( "\t MM2S_DMASR       = 0x%08X\n\r", XAxiVdma_ReadReg(uBaseAddr,XAXIVDMA_TX_OFFSET+XAXIVDMA_SR_OFFSET) );
   printf( "\t MM2S_STRD_FRMDLY = 0x%08X\n\r", XAxiVdma_ReadReg(uBaseAddr,XAXIVDMA_MM2S_ADDR_OFFSET+XAXIVDMA_STRD_FRMDLY_OFFSET) );
   printf( "\t MM2S_START_ADDR0 = 0x%08X\n\r", XAxiVdma_ReadReg(uBaseAddr,XAXIVDMA_MM2S_ADDR_OFFSET+XAXIVDMA_START_ADDR_OFFSET) );
   printf( "\t MM2S_START_ADDR1 = 0x%08X\n\r", XAxiVdma_ReadReg(uBaseAddr,XAXIVDMA_MM2S_ADDR_OFFSET+XAXIVDMA_START_ADDR_OFFSET+4) );
   printf( "\t MM2S_START_ADDR2 = 0x%08X\n\r", XAxiVdma_ReadReg(uBaseAddr,XAXIVDMA_MM2S_ADDR_OFFSET+XAXIVDMA_START_ADDR_OFFSET+8) );
   printf( "\t MM2S_HSIZE       = 0x%08X\n\r", XAxiVdma_ReadReg(uBaseAddr,XAXIVDMA_MM2S_ADDR_OFFSET+XAXIVDMA_HSIZE_OFFSET) );
   printf( "\t MM2S_VSIZE       = 0x%08X\n\r", XAxiVdma_ReadReg(uBaseAddr,XAXIVDMA_MM2S_ADDR_OFFSET+XAXIVDMA_VSIZE_OFFSET) );
   printf( "\t ----------------\n\r" );
   printf( "\t S2MM_HSIZE_STATUS= 0x%08X\n\r", XAxiVdma_ReadReg(uBaseAddr,0xF0) );
   printf( "\t S2MM_VSIZE_STATUS= 0x%08X\n\r", XAxiVdma_ReadReg(uBaseAddr,0xF4 ) );
   printf( "\t ----------------\n\r" );

   return 0;
}


int vfb_check_errors(XAxiVdma *pAxiVdma, uint8_t bClearErrors )
{
   uint32_t* uBaseAddr = pAxiVdma->BaseAddr;
   uint32_t inErrors;
   uint32_t outErrors;
   uint32_t Errors;

   // Get Status of Error Flags
   //inErrors = XAxiVdma_GetStatus(pAxiVdma, XAXIVDMA_WRITE) & XAXIVDMA_SR_ERR_ALL_MASK;
   //outErrors = XAxiVdma_GetStatus(pAxiVdma, XAXIVDMA_READ) & XAXIVDMA_SR_ERR_ALL_MASK;


   inErrors  = XAxiVdma_ReadReg(uBaseAddr,XAXIVDMA_RX_OFFSET+XAXIVDMA_SR_OFFSET ) & 0x0000CFF0;
   outErrors = XAxiVdma_ReadReg(uBaseAddr,XAXIVDMA_TX_OFFSET+XAXIVDMA_SR_OFFSET ) & 0x000046F0;
   printf( "AXI_VDMA - Checking Error Flags\n\r" );

   Errors = (inErrors << 16) | (outErrors);

   if ( Errors )
   {
	   if ( inErrors & 0x00004000 )
	   {
          printf( "\tS2MM_DMASR - ErrIrq\n\r" );
	   }
	   if ( inErrors & 0x00008000 )
	   {
          printf( "\tS2MM_DMASR - EOLLateErr\n\r" );
	   }
	   if ( inErrors & 0x00000800 )
	   {
          printf( "\tS2MM_DMASR - SOFLateErr\n\r" );
	   }
	   if ( inErrors & 0x00000400 )
	   {
          printf( "\tS2MM_DMASR - SGDecErr\n\r" );
	   }
	   if ( inErrors & 0x00000200 )
	   {
          printf( "\tS2MM_DMASR - SGSlvErr\n\r" );
	   }
	   if ( inErrors & 0x00000100 )
	   {
          printf( "\tS2MM_DMASR - EOLEarlyErr\n\r" );
	   }
	   if ( inErrors & 0x00000080 )
	   {
          printf( "\tS2MM_DMASR - SOFEarlyErr\n\r" );
	   }
	   if ( inErrors & 0x00000040 )
	   {
          printf( "\tS2MM_DMASR - DMADecErr\n\r" );
	   }
	   if ( inErrors & 0x00000020 )
	   {
          printf( "\tS2MM_DMASR - DMASlvErr\n\r" );
	   }
	   if ( inErrors & 0x00000010 )
	   {
          printf( "\tS2MM_DMASR - DMAIntErr\n\r" );
	   }

	   if ( outErrors & 0x00004000 )
	   {
          printf( "\tMM2S_DMASR - ErrIrq\n\r" );
	   }
	   if ( outErrors & 0x00000400 )
	   {
          printf( "\tMM2S_DMASR - SGDecErr\n\r" );
	   }
	   if ( outErrors & 0x00000200 )
	   {
          printf( "\tMM2S_DMASR - SGSlvErr\n\r" );
	   }
	   if ( outErrors & 0x00000080 )
	   {
          printf( "\tMM2S_DMASR - SOFEarlyErr\n\r" );
	   }
	   if ( outErrors & 0x00000040 )
	   {
          printf( "\tMM2S_DMASR - DMADecErr\n\r" );
	   }
	   if ( outErrors & 0x00000020 )
	   {
          printf( "\tMM2S_DMASR - DMASlvErr\n\r" );
	   }
	   if ( outErrors & 0x00000010 )
	   {
          printf( "\tMM2S_DMASR - DMAIntErr\n\r" );
	   }

	   // Clear error flags
	   printf( "AXI_VDMA - Clearing Error Flags\n\r" );
	   XAxiVdma_WriteReg(uBaseAddr,XAXIVDMA_RX_OFFSET+XAXIVDMA_SR_OFFSET, 0x0000CFF0); //XAXIVDMA_SR_ERR_ALL_MASK;
	   XAxiVdma_WriteReg(uBaseAddr,XAXIVDMA_TX_OFFSET+XAXIVDMA_SR_OFFSET, 0x000046F0); //XAXIVDMA_SR_ERR_ALL_MASK;
   }

   return Errors;
}

