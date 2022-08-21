#ifndef NUNCHUK_H
#define NUNCHUK_H

#include "nunchuk_devices.h"
#include "i2c.h"

#define NUNCHUK_I2C_ADDR (0x52 << 1)
#define NUNCHUK_PACKET_LEN 6
#define NUNCHUK_TX_TIMEOUT 1000
#define NUNCHUK_RX_TIMEOUT 1000
#define NUNCHUK_REPORT_ADDR 0x00

/**
 * @brief Sends initialize command to nunchuk and reads device type
 * 
 * @param nunchuk_device_type_t type variable to update
 * @return HAL_StatusTypeDef status of init operation
 */
HAL_StatusTypeDef nunchuk_init(nunchuk_device_type_t *type);
/**
 * @brief Write a byte to a register
 * 
 * @param addr register address to write to
 * @param data register data to write
 * @return HAL_StatusTypeDef
 */
HAL_StatusTypeDef nunchuk_reg_write(uint8_t addr, uint8_t data);
/**
 * @brief Read a packet from a register
 * 
 * @param addr register address to read from
 * @param packet packet to populate with data
 * @return HAL_StatusTypeDef 
 */
HAL_StatusTypeDef nunchuk_packet_read(uint8_t addr, nunchuk_packet_t* packet, int wait_time_ms);

HAL_StatusTypeDef nunchuk_read_standard(nunchuk_standard_t* std);
HAL_StatusTypeDef nunchuk_read_classic(nunchuk_classic_t* cc);
HAL_StatusTypeDef nunchuk_read_guitar_hero(nunchuk_guitar_hero_t* gh);

#endif // NUNCHUK_H