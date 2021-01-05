
#ifndef __XIIC_H__
#define __XIIC_H__

#include <stdint.h>
#include "io.h"

#define XIIC_SR_TX_FIFO_FULL_MASK	0x00000010 /**< 1 = Tx FIFO full */
#define XIIC_SR_RX_FIFO_FULL_MASK	0x00000020 /**< 1 = Rx FIFO full */
#define XIIC_SR_RX_FIFO_EMPTY_MASK	0x00000040 /**< 1 = Rx FIFO empty */
#define XIIC_SR_TX_FIFO_EMPTY_MASK	0x00000080 /**< 1 = Tx FIFO empty */

#define XIIC_DGIER_OFFSET	0x1C  /**< Global Interrupt Enable Register */
#define XIIC_IISR_OFFSET	0x20  /**< Interrupt Status Register */
#define XIIC_IIER_OFFSET	0x28  /**< Interrupt Enable Register */
#define XIIC_RESETR_OFFSET	0x40  /**< Reset Register */
#define XIIC_CR_REG_OFFSET	0x100 /**< Control Register */
#define XIIC_SR_REG_OFFSET	0x104 /**< Status Register */
#define XIIC_DTR_REG_OFFSET	0x108 /**< Data Tx Register */
#define XIIC_DRR_REG_OFFSET	0x10C /**< Data Rx Register */
#define XIIC_ADR_REG_OFFSET	0x110 /**< Address Register */
#define XIIC_TFO_REG_OFFSET	0x114 /**< Tx FIFO Occupancy */
#define XIIC_RFO_REG_OFFSET	0x118 /**< Rx FIFO Occupancy */
#define XIIC_TBA_REG_OFFSET	0x11C /**< 10 Bit Address reg */
#define XIIC_RFD_REG_OFFSET	0x120 /**< Rx FIFO Depth reg */
#define XIIC_GPO_REG_OFFSET	0x124 /**< Output Register */

#define XIIC_SR_GEN_CALL_MASK		0x00000001 /**< 1 = A Master issued
						    * a GC */
#define XIIC_SR_ADDR_AS_SLAVE_MASK	0x00000002 /**< 1 = When addressed as
						    * slave */
#define XIIC_SR_BUS_BUSY_MASK		0x00000004 /**< 1 = Bus is busy */
#define XIIC_SR_MSTR_RDING_SLAVE_MASK	0x00000008 /**< 1 = Dir: Master <--
						    * slave */
#define XIIC_SR_TX_FIFO_FULL_MASK	0x00000010 /**< 1 = Tx FIFO full */
#define XIIC_SR_RX_FIFO_FULL_MASK	0x00000020 /**< 1 = Rx FIFO full */
#define XIIC_SR_RX_FIFO_EMPTY_MASK	0x00000040 /**< 1 = Rx FIFO empty */
#define XIIC_SR_TX_FIFO_EMPTY_MASK	0x00000080 /**< 1 = Tx FIFO empty */

#define XIIC_STOP		0x00 /**< Send a stop on the IIC bus after
					* the current data transfer */
#define XIIC_REPEATED_START	0x01 /**< Donot Send a stop on the IIC bus after
					* the current data transfer */

#define XIIC_RESET_MASK		0x0000000A /**< RESET Mask  */

#define IIC_RX_FIFO_DEPTH		16	/**< Rx fifo capacity */

#define XIIC_CR_ENABLE_DEVICE_MASK	0x00000001 /**< Device enable = 1 */
#define XIIC_CR_TX_FIFO_RESET_MASK	0x00000002 /**< Transmit FIFO reset=1 */
#define XIIC_CR_MSMS_MASK		0x00000004 /**< Master starts Txing=1 */
#define XIIC_CR_DIR_IS_TX_MASK		0x00000008 /**< Dir of Tx. Txing=1 */
#define XIIC_CR_NO_ACK_MASK		0x00000010 /**< Tx Ack. NO ack = 1 */
#define XIIC_CR_REPEATED_START_MASK	0x00000020 /**< Repeated start = 1 */
#define XIIC_CR_GENERAL_CALL_MASK	0x00000040 /**< Gen Call enabled = 1 */

#define XIIC_INTR_ARB_LOST_MASK	0x00000001 /**< 1 = Arbitration lost */
#define XIIC_INTR_TX_ERROR_MASK	0x00000002 /**< 1 = Tx error/msg complete */
#define XIIC_INTR_TX_EMPTY_MASK	0x00000004 /**< 1 = Tx FIFO/reg empty */
#define XIIC_INTR_RX_FULL_MASK	0x00000008 /**< 1 = Rx FIFO/reg=OCY level */
#define XIIC_INTR_BNB_MASK	0x00000010 /**< 1 = Bus not busy */
#define XIIC_INTR_AAS_MASK	0x00000020 /**< 1 = When addr as slave */
#define XIIC_INTR_NAAS_MASK	0x00000040 /**< 1 = Not addr as slave */
#define XIIC_INTR_TX_HALF_MASK	0x00000080 /**< 1 = Tx FIFO half empty */
#define XIIC_TX_DYN_START_MASK		0x00000100 /**< 1 = Set dynamic start */
#define XIIC_TX_DYN_STOP_MASK		0x00000200 /**< 1 = Set dynamic stop */
#define IIC_TX_FIFO_DEPTH		16     /**< Tx fifo capacity */

#define XIIC_READ_OPERATION	1 /**< Read operation on the IIC bus */
#define XIIC_WRITE_OPERATION	0 /**< Write operation on the IIC bus */

/**
 * The following constants are used with the transmit FIFO fill function to
 * specify the role which the IIC device is acting as, a master or a slave.
 */
#define XIIC_MASTER_ROLE	1 /**< Master on the IIC bus */
#define XIIC_SLAVE_ROLE		0 /**< Slave on the IIC bus */

#define XIic_WriteIisr(BaseAddress, Status)	\
	XIic_WriteReg((BaseAddress), XIIC_IISR_OFFSET, (Status))


#define XIic_DynSend7BitAddress(BaseAddress, SlaveAddress, Operation)	\
{									\
	uint8_t LocalAddr = (uint8_t)(SlaveAddress << 1);				\
	LocalAddr = (LocalAddr & 0xFE) | (Operation);			\
	XIic_WriteReg(BaseAddress, XIIC_DTR_REG_OFFSET,		\
			XIIC_TX_DYN_START_MASK | LocalAddr);		\
}

#define XIic_DynSendStartStopAddress(BaseAddress, SlaveAddress, Operation) \
{									 \
	uint8_t LocalAddr = (uint8_t)(SlaveAddress << 1);				 \
	LocalAddr = (LocalAddr & 0xFE) | (Operation);			 \
	XIic_WriteReg(BaseAddress, XIIC_DTR_REG_OFFSET,		 \
			XIIC_TX_DYN_START_MASK | XIIC_TX_DYN_STOP_MASK | \
			LocalAddr);					 \
}

#define XIic_DynSendStop(BaseAddress, ByteCount)			\
{									\
	XIic_WriteReg(BaseAddress, XIIC_DTR_REG_OFFSET,		\
			XIIC_TX_DYN_STOP_MASK | ByteCount); 		\
}

#define XIic_ClearIisr(BaseAddress, InterruptMask)		\
	XIic_WriteIisr((BaseAddress),			\
	XIic_ReadIisr(BaseAddress) & (InterruptMask))

 #define XIic_ReadIisr(BaseAddress) 					\
	XIic_ReadReg((BaseAddress), XIIC_IISR_OFFSET)

#define XIic_WriteReg(BaseAddress, RegOffset, RegisterValue) \
	Out32((BaseAddress) + (RegOffset/4), (RegisterValue))

#define XIic_ReadReg(BaseAddress, RegOffset) \
	In32((BaseAddress) + (RegOffset/4))

#define XIic_Write8Bit(BaseAddress, RegOffset, RegisterValue) \
	Out8((BaseAddress) + (RegOffset/4), (RegisterValue))

#define XIic_Read8Bit(BaseAddress, RegOffset) \
	In8((BaseAddress) + (RegOffset/4))



int XIic_DynInit(uint32_t* BaseAddress);


uint32_t XIic_CheckIsBusBusy(uint32_t* BaseAddress);

uint32_t XIic_WaitBusFree(uint32_t* BaseAddress);

unsigned DynSendData(uint32_t* BaseAddress, uint8_t *BufferPtr,
			    uint8_t ByteCount, uint8_t Option);

unsigned XIic_DynSend(uint32_t* BaseAddress, uint16_t Address, uint8_t *BufferPtr,
			uint8_t ByteCount, uint8_t Option);

static unsigned DynRecvData(uint32_t* BaseAddress, uint8_t *BufferPtr, uint8_t ByteCount);

unsigned XIic_DynRecv(uint32_t* BaseAddress, uint8_t Address, uint8_t *BufferPtr, uint8_t ByteCount);

#endif