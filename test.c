#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "./api/vl53l5cx_api.h"

#if 0
#define I2C_BUS_TOF (__CONCAT(i2c, 0))
#endif

i2c_inst_t vl53l5cx_i2c = {i2c0_hw, false};

int main (void) {
    stdio_init_all();
    sleep_ms(3000);

    gpio_init(14);
    gpio_set_dir(14, true);

    i2c_init(&vl53l5cx_i2c, 400 * 1000); // 400kHz
    gpio_set_function(8, GPIO_FUNC_I2C);
    gpio_set_function(9, GPIO_FUNC_I2C);

    static VL53L5CX_Configuration dev;
    static VL53L5CX_ResultsData results;

    dev.platform.address = 0x29;
    //dev.platform.i2c_bus = I2C_BUS_TOF;

    printf("VL53L5CX ULD version: %s\n", VL53L5CX_API_REVISION);

    uint8_t is_alive, status;
    status = vl53l5cx_is_alive(&dev, &is_alive);
    if (!is_alive){
		printf("VL53L5CXV0 not detected at requested address (0x%x)\n", dev.platform.address);
		return -1;
	}

    status = vl53l5cx_init(&dev);
    if (status) {
        printf("failed to load ULD\n");
        return -1;
    }

    status = vl53l5cx_start_ranging(&dev);

    while (1) {
        uint8_t is_ready;
        // check if data is ready
        status = vl53l5cx_check_data_ready(&dev, &is_ready);

        if (is_ready) {
            vl53l5cx_get_ranging_data(&dev, &results);
            printf("streamcount: %3u\n", dev.streamcount);
            for (int j = 0; j < 16; j++) {
                printf("zone: %3d, status: %3u, distance: %3d\n", 
                    j,
                    results.target_status[VL53L5CX_NB_TARGET_PER_ZONE*j],
                    results.distance_mm[VL53L5CX_NB_TARGET_PER_ZONE*j]);
            }
        }
        sleep_ms(5);
    }

    status = vl53l5cx_stop_ranging(&dev);
    printf("end of test\n");
    return status;
}

