#ifndef _I2C_BUS_H_
#define _I2C_BUS_H_
#include "driver/i2c.h"
#include "nvsparam.h"

//#ifdef __cplusplus
//extern "C"
//{
//#endif
#define I2C_SDA_DEFAULT 2
#define I2C_SCL_DEFAULT 0

typedef void* i2c_bus_handle_t;

/**
 * @brief Create and init I2C bus and return a I2C bus handle
 *
 * @param port I2C port number
 * @param conf Pointer to I2C parameters
 *
 * @return
 *     - NULL Fail
 *     - Others Success
 */
i2c_bus_handle_t i2c_bus_create(i2c_port_t port, i2c_config_t* conf);

/**
 * @brief Delete and release the I2C bus object
 *
 * @param bus I2C bus handle
 * @return
 *     - ESP_OK Success
 *     - ESP_FAIL Fail
 */
esp_err_t i2c_bus_delete(i2c_bus_handle_t bus);

/**
 * @brief I2C start sending buffered commands
 *
 * @param bus I2C bus handle
 * @param cmd I2C cmd handle
 * @param ticks_to_wait Maximum blocking time
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_FAIL Fail
 */
esp_err_t i2c_bus_cmd_begin(i2c_bus_handle_t bus, i2c_cmd_handle_t cmd,
portBASE_TYPE ticks_to_wait);
//#ifdef __cplusplus
//}
//#endif

uint8_t i2c_bus_scan(i2c_bus_handle_t bus, uint8_t* devices);
i2c_bus_handle_t i2c_bus_init();

void i2c_load_cfg(i2c_config_t *cfg);
void i2c_save_cfg(const i2c_config_t *cfg);

//#ifdef __cplusplus
/**
 * class of I2c bus
 */
//class CI2CBus
//{
//private:
    //i2c_bus_handle_t m_i2c_bus_handle;

    /**
     * prevent copy constructing
     */
    //CI2CBus(const CI2CBus&);
    //CI2CBus& operator =(const CI2CBus&);
//public:
    /**
     * @brief Constructor for CI2CBus class
     * @param i2c_port I2C hardware port
     * @param scl_io gpio index for slc pin
     * @param sda_io gpio index for sda pin
     * @param clk_hz I2C clock frequency
     * @param i2c_mode mode for I2C bus
     *
     */
    //CI2CBus(i2c_port_t i2c_port, gpio_num_t scl_io, gpio_num_t sda_io,
    //        int clk_hz = 100000, i2c_mode_t i2c_mode = I2C_MODE_MASTER);

    /**
     * @brief Destructor function of CI2CBus class
     */
    //~CI2CBus();

    /**
     * @brief Send command and data to I2C bus
     * @param cmd pointor to command link
     * @ticks_to_wait max block time
     * @return
     *     - ESP_OK Success
     *     - ESP_ERR_INVALID_ARG Parameter error
     *     - ESP_FAIL Sending command error, slave doesn't ACK the transfer.
     *     - ESP_ERR_INVALID_STATE I2C driver not installed or not in master mode.
     *     - ESP_ERR_TIMEOUT Operation timeout because the bus is busy.
     */
    //esp_err_t send(i2c_cmd_handle_t cmd, portBASE_TYPE ticks_to_wait);

    /**
     * @brief Get bus handle
     * @return bus handle
     */
    //i2c_bus_handle_t get_bus_handle();

    //void scan();
//};
//#endif

#endif

