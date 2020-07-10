#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <driver/spi_master.h>
#include <driver/gpio.h>
#include "esp_log.h"

#include "mirf.h"

#define tag "NRF24"

static const int GPIO_MOSI = 23;
static const int GPIO_MISO = 19;
static const int GPIO_SCLK = 18;

static const int SPI_Frequency = 4000000;
//static const int SPI_Frequency = 8000000; // don't work


spi_device_handle_t handle;
void spi_master_init(NRF24_t * dev, uint8_t ce_pin, uint8_t csn_pin)
{
	esp_err_t ret;

	gpio_pad_select_gpio( ce_pin );
	gpio_set_direction( ce_pin, GPIO_MODE_OUTPUT );
	gpio_set_level( ce_pin, 0 );

	gpio_pad_select_gpio( csn_pin );
	gpio_set_direction( csn_pin, GPIO_MODE_OUTPUT );
	gpio_set_level( csn_pin, 1 );

	spi_bus_config_t spi_bus_config = {
		.sclk_io_num = GPIO_SCLK,
		.mosi_io_num = GPIO_MOSI,
		.miso_io_num = GPIO_MISO,
		.quadwp_io_num = -1,
		.quadhd_io_num = -1
	};

	ret = spi_bus_initialize( HSPI_HOST, &spi_bus_config, 1 );
	ESP_LOGI(tag, "spi_bus_initialize=%d",ret);
	assert(ret==ESP_OK);


	spi_device_interface_config_t devcfg={
		.clock_speed_hz = SPI_Frequency,
		.queue_size = 7,
		.mode = 0,
		.flags = SPI_DEVICE_NO_DUMMY,
	};

	
	ret = spi_bus_add_device( HSPI_HOST, &devcfg, &handle);
	
	dev->_SPIHandle = handle;
	dev->csnPin=csn_pin;
	dev->cePin=ce_pin;
}


bool spi_master_write_byte(NRF24_t * dev, uint8_t* Dataout, size_t DataLength )
{
	spi_transaction_t SPITransaction;

	if ( DataLength > 0 ) {
		memset( &SPITransaction, 0, sizeof( spi_transaction_t ) );
		SPITransaction.length = DataLength * 8;
		SPITransaction.tx_buffer = Dataout;
		SPITransaction.rx_buffer = NULL;
		spi_device_transmit( dev->_SPIHandle, &SPITransaction );
	}

	return true;
}

bool spi_master_read_byte(NRF24_t * dev, uint8_t* Datain, uint8_t* Dataout, size_t DataLength )
{
	spi_transaction_t SPITransaction;

	if ( DataLength > 0 ) {
		memset( &SPITransaction, 0, sizeof( spi_transaction_t ) );
		SPITransaction.length = DataLength * 8;
		SPITransaction.tx_buffer = Dataout;
		SPITransaction.rx_buffer = Datain;
		spi_device_transmit( dev->_SPIHandle, &SPITransaction );
	}

	return true;
}

void Nrf24_transfer(NRF24_t * dev, uint8_t address) {
	uint8_t dataout[2];
	dataout[0] = address;
	spi_master_write_byte(dev, dataout, 1 );
}

void Nrf24_transferSync(NRF24_t * dev, uint8_t *dataout, uint8_t *datain, uint8_t len) {
	spi_master_read_byte(dev, datain, dataout, len );
}

void Nrf24_transmitSync(NRF24_t * dev, uint8_t *dataout, uint8_t len) {
	spi_master_write_byte(dev, dataout, len);
}






void Nrf24_configRegister(NRF24_t * dev, uint8_t reg, uint8_t value)  // Clocks only one byte into the given MiRF register
{
	Nrf24_csnLow(dev);
	Nrf24_transfer(dev, W_REGISTER | (REGISTER_MASK & reg));
	Nrf24_transfer(dev, value);
	Nrf24_csnHi(dev);
}

void Nrf24_readRegister(NRF24_t * dev, uint8_t reg, uint8_t * value, uint8_t len)// Reads an array of bytes from the given start position in the MiRF registers.
{
	Nrf24_csnLow(dev);
	Nrf24_transfer(dev, R_REGISTER | (REGISTER_MASK & reg));
	Nrf24_transferSync(dev, value, value, len);
	Nrf24_csnHi(dev);
}

void Nrf24_writeRegister(NRF24_t * dev, uint8_t reg, uint8_t * value, uint8_t len)// Writes an array of bytes into inte the MiRF registers.
{
	Nrf24_csnLow(dev);
	Nrf24_transfer(dev, W_REGISTER | (REGISTER_MASK & reg));
	Nrf24_transmitSync(dev, value, len);
	Nrf24_csnHi(dev);
}

void Nrf24_send(NRF24_t * dev, uint8_t * value)  // Sends a data package to the default address. Be sure to send the correct
									// amount of bytes as configured as payload on the receiver.
{
	Nrf24_ceLow(dev);
 	Nrf24_csnLow(dev);					  // Pull down chip select
	Nrf24_transfer(dev, FLUSH_TX );		// Write cmd to flush tx fifo
	Nrf24_csnHi(dev);					 // Pull up chip select
	Nrf24_csnLow(dev);					  // Pull down chip select
	Nrf24_transfer(dev, W_TX_PAYLOAD ); // Write cmd to write payload
	Nrf24_transmitSync(dev, value, 16);  // Write payload
	Nrf24_csnHi(dev);					 // Pull up chip select
	Nrf24_ceHi(dev);

	 				 // Pull up chip select
}



void Nrf24_flushTx(NRF24_t * dev)
{
	Nrf24_csnLow(dev);
	Nrf24_transfer(dev, FLUSH_TX );
	Nrf24_csnHi(dev);
}



void Nrf24_ceHi(NRF24_t * dev) {
	gpio_set_level( dev->cePin, 1 );
}

void Nrf24_ceLow(NRF24_t * dev) {
	gpio_set_level( dev->cePin, 0 );
}

void Nrf24_csnHi(NRF24_t * dev) {
	gpio_set_level( dev->csnPin, 1 );
}

void Nrf24_csnLow(NRF24_t * dev) {
	gpio_set_level( dev->csnPin, 0 );
}

