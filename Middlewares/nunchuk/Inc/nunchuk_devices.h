#ifndef NUNCHUK_DEVICES_H
#define NUNCHUK_DEVICES_H

#include <stdint.h>

// Raw data struct for the nunchuk report packet
typedef union {
    // raw bytes
    uint8_t raw[6];
    // standard nunchuk
    struct {
        // byte 0
        uint8_t sx; // stick x
        // byte 1
        uint8_t sy; // stick y
        // byte 2
        uint8_t ax_9_2; // bits 9-2 of accelerometer x
        // byte 3
        uint8_t ay_9_2; // bits 9-2 of accelerometer y
        // byte 4
        uint8_t az_9_2; // bits 9-2 of accelerometer z
        // byte 5
        struct {
            uint8_t z : 1;
            uint8_t c : 1;
        } buttons;
        uint8_t ax_1_0 : 2; // bits 1-0 of accelerometer x
        uint8_t ay_1_0 : 2; // bits 1-0 of accelerometer y
        uint8_t az_1_0 : 2; // bits 1-0 of accelerometer z
    } standard_controller;
    // guitar hero controller
    struct {
        // byte 0
        uint8_t sx : 6; // stick x
        uint8_t gh3_0 : 2; // if these bits are set, this guitar hero controller is a guitar hero 3 (GH3) controller
        // byte 1
        uint8_t sy : 6; // stick y
        uint8_t gh3_1 : 2; // if unset, it's a guitar hero world tour controller (GHWT)
        // byte 2
        uint8_t tb : 5; // touch bar
        uint8_t : 3;
        // byte 3
        uint8_t wb : 5; // wammy bar
        uint8_t : 3;
        struct {
            // byte 4
            uint8_t : 2;
            uint8_t plus : 1; // plus button
            uint8_t : 1;
            uint8_t minus : 1; // minus button
            uint8_t : 1;
            uint8_t d : 1; // down strum button
            uint8_t : 1;
            // byte 5
            uint8_t u : 1; // up strum button
            uint8_t : 2;
            uint8_t y : 1; // yellow button (3rd)
            uint8_t g : 1; // green  button (1st)
            uint8_t b : 1; // blue   button (4th)
            uint8_t r : 1; // red    button (2nd)
            uint8_t o : 1; // orange button (5th)
        } buttons;
    } gh_controller;
    // classic controller struct
    struct {
        // byte 0
        uint8_t lx : 6; // left stick x
        uint8_t rx_4_3 : 2; // bits 4-3 of the right stick x axis
        // byte 1
        uint8_t ly : 6; // left stick y
        uint8_t rx_2_1 : 2; // bits 2-1 of the right stick x axis
        // byte 2
        uint8_t ry : 5; // right stick y
        uint8_t lt_4_3 : 2; // bits 4-3 of the left trigger
        uint8_t rx_0 : 1; // bit 0 of the right stick x axis
        // byte 3
        uint8_t rt : 5; // right trigger
        uint8_t lt_2_0 : 3; // bits 2-0 of the left trigger
        struct {
            // byte 4
            uint8_t : 1;
            uint8_t rt : 1; // right trigger
            uint8_t plus : 1;
            uint8_t h : 1;
            uint8_t minus : 1;
            uint8_t lt : 1; // left trigger
            uint8_t dd : 1; // dpad down
            uint8_t dr : 1; // dpad right
            // byte 5
            uint8_t du : 1; // dpad up
            uint8_t dl : 1; // dpad left
            uint8_t zr : 1; // right z
            uint8_t x : 1;
            uint8_t a : 1;
            uint8_t y : 1;
            uint8_t b : 1;
            uint8_t zl : 1; // left z
        } buttons;
    } classic_controller;
} nunchuk_packet_t;

// more user friendly structs, converted from raw data struct
typedef struct {
    int8_t sx;
    int8_t sy;
    int16_t ax;
    int16_t ay;
    int16_t az;
    struct {
        uint8_t z;
        uint8_t c;
    } buttons;
} nunchuk_standard_t;

typedef struct {
    int8_t sx; // stick x
    int8_t sy; // stick y
    // if this field is set, this guitar hero controller is a guitar hero 3 (GH3) controller
    // if unset, it's a guitar hero world tour controller (GHWT)
    uint8_t gh3;
    int8_t tb; // touch bar
    int8_t wb; // wammy bar
    struct {
        uint8_t plus;
        uint8_t minus;
        uint8_t strum_down;
        uint8_t strum_up;
        uint8_t green;
        uint8_t red;
        uint8_t yellow;
        uint8_t blue;
        uint8_t orange
    } buttons;
} nunchuk_guitar_hero_t;

typedef struct {
    int8_t lx; // left stick x
    int8_t ly; // left stick y
    int8_t rx; // right stick x
    int8_t ry; // right stick y
    uint8_t left_trigger;
    uint8_t right_trigger;
    struct {
        uint8_t a;
        uint8_t b;
        uint8_t x;
        uint8_t y;
        uint8_t plus;
        uint8_t home;
        uint8_t minus;
        uint8_t left_trigger;
        uint8_t right_trigger;
        uint8_t zr;
        uint8_t zl;
        struct {
            uint8_t down;
            uint8_t right;
            uint8_t up;
            uint8_t left;
        } dpad;
    } buttons;
} nunchuk_classic_t;

// device enum
typedef enum _nunchuk_device_type_t {
    UNKNOWN = 0,
    STANDARD,
    GUITAR_HERO,
    CLASSIC,
} nunchuk_device_type_t;

// field populating methods
void nunchuk_fill_standard(nunchuk_packet_t* packet, nunchuk_standard_t* std);
void nunchuk_fill_guitar_hero(nunchuk_packet_t* packet, nunchuk_guitar_hero_t* gh);
void nunchuk_fill_classic(nunchuk_packet_t* packet, nunchuk_classic_t* cc);

#endif // NUNCHUK_DEVICES_H