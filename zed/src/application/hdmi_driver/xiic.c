#include "xiic.h"

#include <stdio.h>

int XIic_DynInit(uint32_t* BaseAddress)
{
	uint32_t Status;

	/*
	 * Reset IIC Core.
	 */
	XIic_WriteReg(BaseAddress, XIIC_RESETR_OFFSET, XIIC_RESET_MASK);

	/*
	 * Set receive Fifo depth to maximum (zero based).
	 */
	XIic_WriteReg(BaseAddress,  XIIC_RFD_REG_OFFSET,
			IIC_RX_FIFO_DEPTH - 1);

	/*
	 * Reset Tx Fifo.
	 */
	XIic_WriteReg(BaseAddress,  XIIC_CR_REG_OFFSET,
			XIIC_CR_TX_FIFO_RESET_MASK);

	/*
	 * Enable IIC Device, remove Tx Fifo reset & disable general call.
	 */
	XIic_WriteReg(BaseAddress, XIIC_CR_REG_OFFSET,
			XIIC_CR_ENABLE_DEVICE_MASK);

	/*
	 * Read status register and verify IIC Device is in initial state. Only
	 * the Tx Fifo and Rx Fifo empty bits should be set.
	 */
	Status = XIic_ReadReg(BaseAddress,  XIIC_SR_REG_OFFSET);
    printf("Status: %x \n", Status);
	if(Status == (XIIC_SR_RX_FIFO_EMPTY_MASK |
		XIIC_SR_TX_FIFO_EMPTY_MASK)) {
		return 0;
	}

	return 1L;
}


uint32_t XIic_CheckIsBusBusy(uint32_t* BaseAddress)
{
	uint32_t StatusReg;

	StatusReg = XIic_ReadReg(BaseAddress, XIIC_SR_REG_OFFSET);
	if (StatusReg & XIIC_SR_BUS_BUSY_MASK) {
		return 1;
	} else {
		return 0;
	}
}

uint32_t XIic_WaitBusFree(uint32_t* BaseAddress)
{
	uint32_t BusyCount = 0;

	while (XIic_CheckIsBusBusy(BaseAddress)) {
		if (BusyCount++ > 10000) {
			return XST_FAILURE;
		}
		usleep(100);
	}

	return XST_SUCCESS;
}

unsigned DynSendData(uint32_t* BaseAddress, uint8_t *BufferPtr,
			    uint8_t ByteCount, uint8_t Option)
{
	uint32_t IntrStatus;

	while (ByteCount > 0) {
		/*
		 * Wait for the transmit to be empty before sending any more
		 * data by polling the interrupt status register.
		 */
		while (1) {
			IntrStatus = XIic_ReadIisr(BaseAddress);
			if (IntrStatus & (XIIC_INTR_TX_ERROR_MASK |
					  XIIC_INTR_ARB_LOST_MASK |
					  XIIC_INTR_BNB_MASK)) {
				/*
				 * Error condition (NACK or ARB Lost or BNB
				 * Error Has occurred. Clear the Control
				 * register to send a STOP condition on the Bus
				 * and return the number of bytes still to
				 * transmit.
				 */
				XIic_WriteReg(BaseAddress,  XIIC_CR_REG_OFFSET,
						0x03);
				XIic_WriteReg(BaseAddress,  XIIC_CR_REG_OFFSET,
						0x01);

				return ByteCount;
			}

			/*
			 * Check for the transmit Fifo to become Empty.
			 */
			if (IntrStatus & XIIC_INTR_TX_EMPTY_MASK) {
				break;
			}
		}

		/*
		 * Send data to Tx Fifo. If a stop condition is specified and
		 * the last byte is being sent, then set the dynamic stop bit.
		 */
		if ((ByteCount == 1) && (Option == XIIC_STOP)) {
			/*
			 * The MSMS will be cleared automatically upon setting
			 *  dynamic stop.
			 */
			XIic_WriteReg(BaseAddress,  XIIC_DTR_REG_OFFSET,
					XIIC_TX_DYN_STOP_MASK | *BufferPtr++);
		} else {
			XIic_WriteReg(BaseAddress,  XIIC_DTR_REG_OFFSET,
					*BufferPtr++);
		}

		/*
		 * Update the byte count to reflect the byte sent.
		 */
		ByteCount--;
	}

	if (Option == XIIC_STOP) {
		/*
		 * If the Option is to release the bus after transmission of
		 * data, Wait for the bus to transition to not busy before
		 * returning, the IIC device cannot be disabled until this
		 * occurs.
		 */
		while (1) {
			if (XIic_ReadIisr(BaseAddress) & XIIC_INTR_BNB_MASK) {
				break;
			}
		}
	}

	return ByteCount;
}

unsigned XIic_DynSend(uint32_t* BaseAddress, uint16_t Address, uint8_t *BufferPtr,
			uint8_t ByteCount, uint8_t Option)
{
	unsigned RemainingByteCount;
	uint32_t StatusRegister;

	/* Wait until I2C bus is freed, exit if timed out. */
	if (XIic_WaitBusFree(BaseAddress) != XST_SUCCESS) {
		return 0;
	}

	/*
	 * Clear the latched interrupt status so that it will be updated with
	 * the new state when it changes, this must be done after the address
	 * is put in the FIFO
	 */
	XIic_ClearIisr(BaseAddress, XIIC_INTR_TX_EMPTY_MASK |
			XIIC_INTR_TX_ERROR_MASK | XIIC_INTR_ARB_LOST_MASK);

	/*
	 * Put the address into the Fifo to be sent and indicate that the
	 * operation to be performed on the bus is a write operation. Upon
	 * writing the address, a start condition is initiated. MSMS is
	 * automatically set to master when the address is written to the Fifo.
	 * If MSMS was already set, then a re-start is sent prior to the
	 * address.
	 */
	if(!(Address & XIIC_TX_DYN_STOP_MASK)) {

		XIic_DynSend7BitAddress(BaseAddress, Address,
				XIIC_WRITE_OPERATION);
	} else {
		XIic_DynSendStartStopAddress(BaseAddress, Address,
					XIIC_WRITE_OPERATION);
	}

	/*
	 * Wait for the bus to go busy.
	 */
	StatusRegister = XIic_ReadReg(BaseAddress,  XIIC_SR_REG_OFFSET);

	while (( StatusRegister & XIIC_SR_BUS_BUSY_MASK) !=
			XIIC_SR_BUS_BUSY_MASK) {
		StatusRegister = XIic_ReadReg(BaseAddress,
				XIIC_SR_REG_OFFSET);
	}

	/*
	 * Clear the latched interrupt status for the bus not busy bit which
	 * must be done while the bus is busy.
	 */
	XIic_ClearIisr(BaseAddress, XIIC_INTR_BNB_MASK);

	/*
	 * Send the specified data to the device on the IIC bus specified by the
	 * the address.
	 */
	RemainingByteCount = DynSendData(BaseAddress, BufferPtr, ByteCount,
					 Option);

	/*
	 * The send is complete return the number of bytes that was sent.
	 */
	return ByteCount - RemainingByteCount;
}

static unsigned DynRecvData(uint32_t* BaseAddress, uint8_t *BufferPtr, uint8_t ByteCount)
{
	uint32_t StatusReg;
	uint32_t IntrStatus;
	uint32_t IntrStatusMask;

	while (ByteCount > 0) {

		/*
		 * Setup the mask to use for checking errors because when
		 * receiving one byte OR the last byte of a multibyte message
		 * an error naturally occurs when the no ack is done to tell
		 * the slave the last byte.
		 */
		if (ByteCount == 1) {
			IntrStatusMask =
				XIIC_INTR_ARB_LOST_MASK | XIIC_INTR_BNB_MASK;
		} else {
			IntrStatusMask =
				XIIC_INTR_ARB_LOST_MASK |
				XIIC_INTR_TX_ERROR_MASK | XIIC_INTR_BNB_MASK;
		}

		/*
		 * Wait for a byte to show up in the Rx Fifo.
		 */
		while (1) {
			IntrStatus = XIic_ReadIisr(BaseAddress);
			StatusReg = XIic_ReadReg(BaseAddress,
						  XIIC_SR_REG_OFFSET);

			if ((StatusReg & XIIC_SR_RX_FIFO_EMPTY_MASK) !=
				XIIC_SR_RX_FIFO_EMPTY_MASK) {
				break;
			}
			/*
			 * Check the transmit error after the receive full
			 * because when sending only one byte transmit error
			 * will occur because of the no ack to indicate the end
			 * of the data.
			 */
			if (IntrStatus & IntrStatusMask) {
				return ByteCount;
			}
		}

		/*
		 * Read in byte from the Rx Fifo. If the Fifo reached the
		 * programmed occupancy depth as programmed in the Rx occupancy
		 * reg, this read access will un throttle the bus such that
		 * the next byte is read from the IIC bus.
		 */
		*BufferPtr++ = XIic_ReadReg(BaseAddress,  XIIC_DRR_REG_OFFSET);
		ByteCount--;
	}

	return ByteCount;
}

unsigned XIic_DynRecv(uint32_t* BaseAddress, uint8_t Address, uint8_t *BufferPtr, uint8_t ByteCount)
{
	unsigned RemainingByteCount;
	uint32_t StatusRegister;

	/*
	 * Clear the latched interrupt status so that it will be updated with
	 * the new state when it changes.
	 */
	XIic_ClearIisr(BaseAddress, XIIC_INTR_TX_EMPTY_MASK |
			XIIC_INTR_TX_ERROR_MASK | XIIC_INTR_ARB_LOST_MASK);

	/*
	 * Send the 7 bit slave address for a read operation and set the state
	 * to indicate the address has been sent. Upon writing the address, a
	 * start condition is initiated. MSMS is automatically set to master
	 * when the address is written to the Fifo. If MSMS was already set,
	 * then a re-start is sent prior to the address.
	 */
	XIic_DynSend7BitAddress(BaseAddress, Address, XIIC_READ_OPERATION);

	/*
	 * Wait for the bus to go busy.
	 */
	StatusRegister = XIic_ReadReg(BaseAddress,  XIIC_SR_REG_OFFSET);

	while (( StatusRegister & XIIC_SR_BUS_BUSY_MASK)
			!= XIIC_SR_BUS_BUSY_MASK) {
		StatusRegister = XIic_ReadReg(BaseAddress,
				XIIC_SR_REG_OFFSET);
	}

	/*
	 * Clear the latched interrupt status for the bus not busy bit which
	 * must be done while the bus is busy.
	 */
	XIic_ClearIisr(BaseAddress, XIIC_INTR_BNB_MASK);

	/*
	 * Write to the Tx Fifo the dynamic stop control bit with the number of
	 * bytes that are to be read over the IIC interface from the presently
	 * addressed device.
	 */
	XIic_DynSendStop(BaseAddress, ByteCount);

	/*
	 * Receive the data from the IIC bus.
	 */
	RemainingByteCount = DynRecvData(BaseAddress, BufferPtr, ByteCount);

	/* Wait until I2C bus is freed, exit if timed out. */
	if (XIic_WaitBusFree(BaseAddress) != XST_SUCCESS) {
		return 0;
	}

	/*
	 * The receive is complete. Return the number of bytes that were
	 * received.
	 */
	return ByteCount - RemainingByteCount;
}