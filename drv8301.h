#ifndef __DRV_8301__

#define __DRV_8301__

#include "DSP2802x_Device.h"

enum {
	__DRV8301_OCP_MODE_LIMIT = 0,
	__DRV8301_OCP_MODE_SHUT_DOWN,
	__DRV8301_OCP_MODE_REPORT,
	__DRV8301_OCP_MODE_DISABLED
};

enum {
	__DRV8301_GAIN_10 = 0,
	__DRV8301_GAIN_20 = 1,
	__DRV8301_GAIN_40 = 2,
	__DRV8301_GAIN_80 = 3
};

enum {
	__DRV8301_WRITE = 0x00,
	__DRV8301_READ = 0x01
};

enum {
	__DRV8301_STATUS_REG1 = 0x00,
	__DRV8301_STATUS_REG2 = 0x01,
	__DRV8301_CONTROL_REG1 = 0x02,
	__DRV8301_CONTROL_REG = 0x03
};

enum {
	__DRV8301_FETLC_OC = 0,
	__DRV8301_FETHC_OC,
	__DRV8301_FETLB_OC,
	__DRV8301_FETHB_OC,
	__DRV8301_FETLA_OC,
	__DRV8301_FETHA_OC,
	__DRV8301_OTW,
	__DRV8301_OTSD,
	__DRV8301_PVDD_UV,
	__DRV8301_GVDD_UV,
	__DRV8301_FAULT,

	__DRV8301_FAULT_LENGTH
};

struct SPI_DATA_BITS {
	Uint16 data:11; //0..10
	Uint16 addr:4;  //11..14
	Uint16 w0:1; 	//15
};

struct DRV_8301_REG1_BITS {
	Uint16 GATE_CURRENT:2; //0..1
	Uint16 GATE_RESET:1;   //2
	Uint16 PWM_MODE:1;	   //3
	Uint16 OC_MODE:2;	   //4..5
	Uint16 OC_ADJ_SET:5;   //6..10
};

typedef union {
	Uint16 all;
	struct DRV_8301_REG1_BITS bit;
} DRV_8301_REG1_DATA;

struct DRV_8301_REG2_BITS {
	Uint16 OCTW_SET:2; 	//0..1
	Uint16 GAIN:2; 	    //2..3
	Uint16 DC_CAL_CH1:1;//4
	Uint16 DC_CAL_CH2:1;//5
	Uint16 OC_TOFF:1;	//6
};

typedef union {
	Uint16 all;
	struct DRV_8301_REG2_BITS bit;
} DRV_8301_REG2_DATA;

typedef union  {
	Uint16 all;
	struct SPI_DATA_BITS bit;
} DRV_8301_DATA_SEND;

struct SPI_DATA_RESPONSE_BITS {
	Uint16 data:11; //0..10
	Uint16 addr:4;	//11..14
	Uint16 f0:1;	//15
};

typedef union  {
	Uint16 all;
	struct SPI_DATA_RESPONSE_BITS bit;
} DRV_8301_DATA_RESPONSE;


/**
 * Отправляет данные по SPI в DRV8301
 * @param addr
 * @param data
 * @param rw  1-read; 0-write
 * @return response of cmd
 */
Uint16 drv8301_send_cmd(Uint16 addr, Uint16 data, Uint16 rw);

/**
 * Инициализация DRV8301
 */
void drv8301_init();
/**
 * Инициализация SPI для DRV8301
 */
void drv8301_spi_init();

/**
 * Макросы установки и сброса сигнала EN_GATE
 */
#define __DRV8301_EN_GATE_SET GpioDataRegs.GPASET.bit.GPIO6 = 1
#define __DRV8301_EN_GATE_RESET GpioDataRegs.GPACLEAR.bit.GPIO6 = 1

inline void drv8301_gate_reset();

void InitSpiaGpio();

void drv8301_pwm_mode_set(Uint16 mode);

#endif
