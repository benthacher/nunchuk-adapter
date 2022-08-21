#include "nunchuk.h"
#include "nunchuk_devices.h"
#include "main.h"
#include "i2c.h"
#include "string.h"
#include "gpio.h"

// device ids
static const uint8_t NUNCHUK_STANDARD_ID[6] = { 0x00, 0x00, 0xA4, 0x20, 0x00, 0x00 };
static const uint8_t NUNCHUK_CLASSIC_ID[6] = { 0x01, 0x00, 0xA4, 0x20, 0x01, 0x01 };
static const uint8_t NUNCHUK_GUITAR_HERO_ID[6] = { 0x00, 0x00, 0xA4, 0x20, 0x01, 0x03 };

int clamp_input(int input, int min, int max) {
    if (input > max)
        return max;
    if (input < min)
        return min;
    return input;
}

HAL_StatusTypeDef nunchuk_reg_write(uint8_t addr, uint8_t data) {
    uint8_t to_send[2] = { addr, data };

    // write register address and data to nunchuk
    return HAL_I2C_Master_Transmit(&hi2c2, NUNCHUK_I2C_ADDR, to_send, sizeof(to_send), NUNCHUK_TX_TIMEOUT);
}

/**
 * @brief Reads a 6 byte packet from an address in the nunchuk
 * 
 * @param addr Address of packet
 * @param packet Pointer to packet
 * @param wait_time_ms Time to wait between sending the address and receiving the packet
 * @return HAL_StatusTypeDef 
 */
HAL_StatusTypeDef nunchuk_packet_read(uint8_t addr, nunchuk_packet_t* packet, int wait_time_ms) {
    // write register address to nunchuk
    HAL_StatusTypeDef ret = HAL_I2C_Master_Transmit(&hi2c2, NUNCHUK_I2C_ADDR, (uint8_t*) &addr, 1, NUNCHUK_TX_TIMEOUT);
    
    if (ret != HAL_OK)
        return ret;

    HAL_Delay(wait_time_ms);

    return HAL_I2C_Master_Receive(&hi2c2, NUNCHUK_I2C_ADDR, (uint8_t*) packet, NUNCHUK_PACKET_LEN, NUNCHUK_RX_TIMEOUT);
}

HAL_StatusTypeDef nunchuk_init(nunchuk_device_type_t *type) {
    *type = UNKNOWN;

    // de-encrypt device
    if (nunchuk_reg_write(0xF0, 0x55) != HAL_OK)
        return HAL_ERROR;

    HAL_Delay(10);

    if (nunchuk_reg_write(0xFB, 0x00) != HAL_OK)
        return HAL_ERROR;

    HAL_Delay(10);

    nunchuk_packet_t packet;
    // read id packet
    if (nunchuk_packet_read(0xFA, &packet, 10) != HAL_OK)
        return HAL_ERROR;

    // compare raw packet with ID constants to check the ID, return the ID if it matches
    if (memcmp(packet.raw, NUNCHUK_STANDARD_ID, NUNCHUK_PACKET_LEN) == 0) {
        *type = STANDARD;
    }
    if (memcmp(packet.raw, NUNCHUK_CLASSIC_ID, NUNCHUK_PACKET_LEN) == 0) {
        *type = CLASSIC;
    }
    if (memcmp(packet.raw, NUNCHUK_GUITAR_HERO_ID, NUNCHUK_PACKET_LEN) == 0) {
        *type = GUITAR_HERO;
    }
    return HAL_OK; // even if the type is unknown, it still initialized successfully
}

HAL_StatusTypeDef nunchuk_read_standard(nunchuk_standard_t* std) {
    nunchuk_packet_t packet;
    HAL_StatusTypeDef ret = nunchuk_packet_read(NUNCHUK_REPORT_ADDR, &packet, 5);

    if (ret != HAL_OK)
        return ret;
    
    nunchuk_fill_standard(&packet, std);
    return HAL_OK;
}

HAL_StatusTypeDef nunchuk_read_classic(nunchuk_classic_t* cc) {
    nunchuk_packet_t packet;
    HAL_StatusTypeDef ret = nunchuk_packet_read(NUNCHUK_REPORT_ADDR, &packet, 5);

    if (ret != HAL_OK)
        return ret;
    
    nunchuk_fill_classic(&packet, cc);
    return HAL_OK;
}

HAL_StatusTypeDef nunchuk_read_guitar_hero(nunchuk_guitar_hero_t* gh) {
    nunchuk_packet_t packet;
    HAL_StatusTypeDef ret = nunchuk_packet_read(NUNCHUK_REPORT_ADDR, &packet, 5);

    if (ret != HAL_OK)
        return ret;
    
    nunchuk_fill_guitar_hero(&packet, gh);
    return HAL_OK;
}

void nunchuk_fill_standard(nunchuk_packet_t* packet, nunchuk_standard_t* std) {
    std->ax = (packet->standard_controller.ax_9_2 << 2) |
              (packet->standard_controller.ax_1_0);
    std->ay = (packet->standard_controller.ay_9_2 << 2) |
              (packet->standard_controller.ay_1_0);
    std->az = (packet->standard_controller.az_9_2 << 2) |
              (packet->standard_controller.az_1_0);
    
    std->sx = packet->standard_controller.sx;
    std->sy = packet->standard_controller.sy;

    std->buttons.c = !packet->standard_controller.buttons.c;
    std->buttons.z = !packet->standard_controller.buttons.z;
}

void nunchuk_fill_guitar_hero(nunchuk_packet_t* packet, nunchuk_guitar_hero_t* gh) {
    // if both gh3 fields are all zeros, set gh3 to 1
    gh->gh3 = packet->gh_controller.gh3_0 == 0b11 && packet->gh_controller.gh3_1 == 0b11;

    gh->sx = clamp_input(packet->gh_controller.sx, GH_SX_MIN, GH_SX_MAX);
    gh->sy = clamp_input(packet->gh_controller.sy, GH_SY_MIN, GH_SY_MAX);

    gh->tb = clamp_input(packet->gh_controller.tb, GH_TB_MIN, GH_TB_MAX);
    gh->wb = clamp_input(packet->gh_controller.wb, GH_WB_MIN, GH_WB_MAX);

    gh->buttons.plus = !packet->gh_controller.buttons.plus;
    gh->buttons.minus = !packet->gh_controller.buttons.minus;
    gh->buttons.strum_up = !packet->gh_controller.buttons.u;
    gh->buttons.strum_down = !packet->gh_controller.buttons.d;
    gh->buttons.green = !packet->gh_controller.buttons.g;
    gh->buttons.red = !packet->gh_controller.buttons.r;
    gh->buttons.yellow = !packet->gh_controller.buttons.y;
    gh->buttons.blue = !packet->gh_controller.buttons.b;
    gh->buttons.orange = !packet->gh_controller.buttons.o;
}

void nunchuk_fill_classic(nunchuk_packet_t* packet, nunchuk_classic_t* cc) {
    cc->lx =  packet->classic_controller.lx;
    cc->ly =  packet->classic_controller.ly;
    cc->rx = (packet->classic_controller.rx_4_3 << 3) |
              (packet->classic_controller.rx_2_1 << 1) |
              (packet->classic_controller.rx_0);
    cc->ry =  packet->classic_controller.ry;

    cc->left_trigger = (packet->classic_controller.lt_4_3 << 3) |
                        (packet->classic_controller.lt_2_0);
    cc->right_trigger = packet->classic_controller.rt;
    
    cc->buttons.a =             !packet->classic_controller.buttons.a;
    cc->buttons.b =             !packet->classic_controller.buttons.b;
    cc->buttons.x =             !packet->classic_controller.buttons.x;
    cc->buttons.y =             !packet->classic_controller.buttons.y;
    cc->buttons.plus =          !packet->classic_controller.buttons.plus;
    cc->buttons.home =          !packet->classic_controller.buttons.h;
    cc->buttons.minus =         !packet->classic_controller.buttons.minus;
    cc->buttons.left_trigger =  !packet->classic_controller.buttons.lt;
    cc->buttons.right_trigger = !packet->classic_controller.buttons.rt;
    cc->buttons.zl =            !packet->classic_controller.buttons.zl;
    cc->buttons.zr =            !packet->classic_controller.buttons.zr;
    cc->buttons.dpad.down =     !packet->classic_controller.buttons.dd;
    cc->buttons.dpad.right =    !packet->classic_controller.buttons.dr;
    cc->buttons.dpad.up =       !packet->classic_controller.buttons.du;
    cc->buttons.dpad.left =     !packet->classic_controller.buttons.dl;
}