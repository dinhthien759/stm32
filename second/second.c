#include "stm32f10x.h"
#include <stdio.h>

/* ================= MPU6050 ================= */
#define MPU_ADDR        0x68
#define MPU_ADDR_W      (MPU_ADDR << 1)
#define MPU_ADDR_R      ((MPU_ADDR << 1) | 1)

void Delay_ms(unsigned int time){
	unsigned int i, j; 
	for(i = 0; i<time; i++){
		for(j = 0; j<0x2aff; j++);
	}
}
/* ================= USART ================= */
void USART1_Init(void)
{
    GPIO_InitTypeDef gpio;
    USART_InitTypeDef usart;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |
                           RCC_APB2Periph_USART1, ENABLE);

    /* PA9 = USART1_TX */
    gpio.GPIO_Pin = GPIO_Pin_9;
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio);

    USART_StructInit(&usart);
    usart.USART_BaudRate = 115200;
    usart.USART_WordLength = USART_WordLength_8b;
    usart.USART_StopBits = USART_StopBits_1;
    usart.USART_Parity = USART_Parity_No;
    usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    usart.USART_Mode = USART_Mode_Tx;

    USART_Init(USART1, &usart);
    USART_Cmd(USART1, ENABLE);
}


void USART1_SendChar(char c)
{
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    USART_SendData(USART1, c);
}

void USART1_SendString(char *str)
{
    while (*str)
        USART1_SendChar(*str++);
}

/* ================= I2C1 ================= */
void I2C1_Init(void)
{
    GPIO_InitTypeDef gpio;
    I2C_InitTypeDef i2c;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

    /* PB6 = SCL, PB7 = SDA */
    gpio.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    gpio.GPIO_Mode = GPIO_Mode_AF_OD;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &gpio);

    I2C_DeInit(I2C1);

    I2C_StructInit(&i2c);
    i2c.I2C_ClockSpeed = 100000;
    i2c.I2C_Mode = I2C_Mode_I2C;
    i2c.I2C_DutyCycle = I2C_DutyCycle_2;
    i2c.I2C_Ack = I2C_Ack_Enable;
    i2c.I2C_OwnAddress1 = 0x00;

    I2C_Init(I2C1, &i2c);
    I2C_Cmd(I2C1, ENABLE);
}

/* ================= I2C WRITE ================= */
void MPU_Write(uint8_t reg, uint8_t data)
{
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));

    I2C_GenerateSTART(I2C1, ENABLE);
    while (!I2C_CheckEvent(I2C1,
           I2C_EVENT_MASTER_MODE_SELECT));

    I2C_Send7bitAddress(I2C1, MPU_ADDR_W,
                        I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(I2C1,
           I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    I2C_SendData(I2C1, reg);
    while (!I2C_CheckEvent(I2C1,
           I2C_EVENT_MASTER_BYTE_TRANSMITTING));

    I2C_SendData(I2C1, data);
    while (!I2C_CheckEvent(I2C1,
           I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    I2C_GenerateSTOP(I2C1, ENABLE);
}

/* ================= I2C READ ================= */
uint8_t MPU_Read(uint8_t reg)
{
    uint8_t data;

    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));

    /* Send register address */
    I2C_GenerateSTART(I2C1, ENABLE);
    while (!I2C_CheckEvent(I2C1,
           I2C_EVENT_MASTER_MODE_SELECT));

    I2C_Send7bitAddress(I2C1, MPU_ADDR_W,
                        I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(I2C1,
           I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    I2C_SendData(I2C1, reg);
    while (!I2C_CheckEvent(I2C1,
           I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    /* Restart + read */
    I2C_GenerateSTART(I2C1, ENABLE);
    while (!I2C_CheckEvent(I2C1,
           I2C_EVENT_MASTER_MODE_SELECT));

    I2C_Send7bitAddress(I2C1, MPU_ADDR_R,
                        I2C_Direction_Receiver);
    while (!I2C_CheckEvent(I2C1,
           I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

    I2C_AcknowledgeConfig(I2C1, DISABLE);
    I2C_GenerateSTOP(I2C1, ENABLE);

    while (!I2C_CheckEvent(I2C1,
           I2C_EVENT_MASTER_BYTE_RECEIVED));

    data = I2C_ReceiveData(I2C1);

    I2C_AcknowledgeConfig(I2C1, ENABLE);

    return data;
}

/* ================= READ 16-BIT ================= */
int16_t MPU_Read16(uint8_t reg)
{
    uint8_t high = MPU_Read(reg);
    uint8_t low  = MPU_Read(reg + 1);

    return (int16_t)((high << 8) | low);
}

/* ================= MPU6050 INIT ================= */
void MPU6050_Init(void)
{
    /* Wake up MPU6050 */
    MPU_Write(0x6B, 0x00);

    /* Accelerometer ±2g */
    MPU_Write(0x1C, 0x00);

    /* Gyroscope ±250 °/s */
    MPU_Write(0x1B, 0x00);
}

/* ================= DELAY ================= */
/*void Delay_ms(uint32_t ms)
{
    uint32_t i, j;

    for (i = 0; i < ms; i++)
        for (j = 0; j < 7200; j++);
}
*/
/* ================= MAIN ================= */
int main(void)
{
    int16_t ax_raw, ay_raw, az_raw;
    int16_t gx_raw, gy_raw, gz_raw;

    float ax, ay, az;
    float gx, gy, gz;

    char buffer[120];

    I2C1_Init();
    USART1_Init();

    Delay_ms(100);

    MPU6050_Init();

    USART1_SendString("MPU6050 START\r\n");

    while (1)
    {
        /* Read raw data */
        ax_raw = MPU_Read16(0x3B);
        ay_raw = MPU_Read16(0x3D);
        az_raw = MPU_Read16(0x3F);

        gx_raw = MPU_Read16(0x43);
        gy_raw = MPU_Read16(0x45);
        gz_raw = MPU_Read16(0x47);

        /* Convert to g */
        ax = ax_raw / 16384.0f;
        ay = ay_raw / 16384.0f;
        az = az_raw / 16384.0f;

        /* Convert to degree/s */
        gx = gx_raw / 131.0f;
        gy = gy_raw / 131.0f;
        gz = gz_raw / 131.0f;

        /* Send CSV */
        sprintf(buffer,
                "AX:%.3f g, AY:%.3f g, AZ:%.3f g, "
                "GX:%.3f deg/s, GY:%.3f deg/s, GZ:%.3f deg/s\r\n",
                ax, ay, az, gx, gy, gz);

        USART1_SendString(buffer);

        Delay_ms(1000);
    }
}
