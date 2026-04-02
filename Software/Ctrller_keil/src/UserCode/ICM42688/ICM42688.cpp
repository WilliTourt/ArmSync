#include "ICM42688.h"
#include "r_ioport.h"
#include "r_spi.h"
#include "r_spi_api.h"
#include "ra/fsp/src/bsp/mcu/all/bsp_io.h"
#include <cstdint>

volatile bool g_SPI_transfer_complete = false;

void spi_callback(spi_callback_args_t * p_args)
{
    if(p_args->event == SPI_EVENT_TRANSFER_COMPLETE)
    {
      g_SPI_transfer_complete = true;
    }

}

//###########################################################################################################

#ifdef HAL_I2C_MODULE_ENABLED
ICM42688::ICM42688(I2C_HandleTypeDef* hi2c,
                  uint8_t address,
                  ICM42688_MountingOrientation mounting)
    : general(this), Int(this), fifo(this), _hi2c(hi2c), _address(address), _mounting(mounting)
{
    if(_address != ICM42688_I2C_ADDR_LOW && _address != ICM42688_I2C_ADDR_HIGH)
        _is_right_address = false; // 错误地址

    _is_i2c = true;
}
#endif

#ifdef HAL_SPI_MODULE_ENABLED
ICM42688::ICM42688(SPI_HandleTypeDef* hspi,
                  GPIO_TypeDef* cs_port,
                  uint16_t cs_pin,
                  ICM42688_MountingOrientation mounting)
    : general(this), Int(this), fifo(this), _hspi(hspi), _cs_port(cs_port), _cs_pin(cs_pin), _mounting(mounting)
{
    _is_spi = true;
}
#endif

#ifdef R_SPI_API_H
ICM42688::ICM42688(ICM42688_MountingOrientation mounting)
    : general(this), Int(this), fifo(this), _mounting(mounting)
{
    _is_spi = true;
}
#endif

#ifdef HAL_SPI_MODULE_ENABLED
ICM42688::ICM42688_StatusTypeDef ICM42688::csSet()
{
    HAL_GPIO_WritePin(_cs_port, _cs_pin, GPIO_PIN_SET);
    return ICM42688_StatusTypeDef::OK;
}

ICM42688::ICM42688_StatusTypeDef ICM42688::csReset()
{
    HAL_GPIO_WritePin(_cs_port, _cs_pin, GPIO_PIN_RESET);
    return ICM42688_StatusTypeDef::OK;
}
#endif

#ifdef R_SPI_API_H
ICM42688::ICM42688_StatusTypeDef ICM42688::csSet()
{
    R_IOPORT_PinWrite(g_ioport.p_ctrl, ICM42688P_CS, BSP_IO_LEVEL_HIGH);
    return ICM42688_StatusTypeDef::OK;
}

ICM42688::ICM42688_StatusTypeDef ICM42688::csReset()
{
    R_IOPORT_PinWrite(g_ioport.p_ctrl, ICM42688P_CS, BSP_IO_LEVEL_LOW);
    return ICM42688_StatusTypeDef::OK;
}
#endif

ICM42688::ICM42688_StatusTypeDef ICM42688::readReg(uint8_t reg, uint8_t* pdata, uint16_t len)
{
    ICM42688::ICM42688_StatusTypeDef res = ICM42688_StatusTypeDef::OK;

    #ifdef HAL_I2C_MODULE_ENABLED
    if(_hi2c != nullptr)
        if(HAL_I2C_Mem_Read(_hi2c, _address << 1, reg, I2C_MEMADD_SIZE_8BIT, pdata, len, 1000) != HAL_OK)
        {
            #ifdef ENABLE_DEBUG
                printf("%sI2C read register wrong.\n", ERROR_TYPE);
            #endif
            res = ICM42688_StatusTypeDef::ERROR;
        }
    #endif

    #ifdef HAL_SPI_MODULE_ENABLED
    if(_hspi != nullptr)
    {
        csReset();
        uint8_t reg_addr = reg | 0x80;
        if(HAL_SPI_Transmit(_hspi, &reg_addr, 1, 1000) != HAL_OK)
        {
            #ifdef ENABLE_DEBUG
                printf("%sSPI write register wrong when trying to read a register.\n", ERROR_TYPE);
            #endif
            res = ICM42688_StatusTypeDef::ERROR;
        }
        if(HAL_SPI_Receive(_hspi, pdata, len, 1000) != HAL_OK)
        {
            #ifdef ENABLE_DEBUG
                printf("%sSPI read register wrong.\n", ERROR_TYPE);
            #endif
            res = ICM42688_StatusTypeDef::ERROR;
        }   
        csSet();
    }
    #endif

    #ifdef R_SPI_API_H

    fsp_err_t err;
    #if 1
    csReset();
    
    
    uint8_t reg_addr = reg | 0x80;
    
    err = R_SPI_Write(g_spi1.p_ctrl, &reg_addr, 1, SPI_BIT_WIDTH_8_BITS);
    while(g_SPI_transfer_complete == false);
    g_SPI_transfer_complete = false;
    if(FSP_SUCCESS != err)
        res = ICM42688_StatusTypeDef::ERROR;
    
    err = R_SPI_Read(g_spi1.p_ctrl, pdata, len, SPI_BIT_WIDTH_8_BITS);
    while(g_SPI_transfer_complete == false);
    g_SPI_transfer_complete = false;
    if(FSP_SUCCESS != err)
        res = ICM42688_StatusTypeDef::ERROR;  
    csSet();
    
    #endif
    
    #if 0
    
    err = R_SPI_WriteRead(g_spi1.p_ctrl, &reg_addr, pdata, len, SPI_BIT_WIDTH_8_BITS);
    while(g_SPI_transfer_complete == false);
    g_SPI_transfer_complete = false;
    if(FSP_SUCCESS != err)
        res = ICM42688_StatusTypeDef::ERROR;
    
    csSet();

    #endif
    
    #endif

    return res;
}

ICM42688::ICM42688_StatusTypeDef ICM42688::writeReg(uint8_t reg, uint8_t* pdata, uint16_t len)
{
    ICM42688::ICM42688_StatusTypeDef res = ICM42688_StatusTypeDef::OK;

    #ifdef HAL_I2C_MODULE_ENABLED
    if(_hi2c != nullptr)
        if(HAL_I2C_Mem_Write(_hi2c, _address << 1, reg, I2C_MEMADD_SIZE_8BIT, pdata, len, 1000) != HAL_OK)
        {
            #ifdef ENABLE_DEBUG
                printf("%sI2C write register wrong.\n", ERROR_TYPE);
            #endif
            res = ICM42688_StatusTypeDef::ERROR;
        }  
    #endif

    #ifdef HAL_SPI_MODULE_ENABLED
    if(_hspi != nullptr)
    {
        csReset();
        uint8_t reg_addr = reg & 0x7F;
        if(HAL_SPI_Transmit(_hspi, &reg_addr, 1, 1000) != HAL_OK)
        {
            #ifdef ENABLE_DEBUG
                printf("%sSPI write register wrong.\n", ERROR_TYPE);
            #endif
            res = ICM42688_StatusTypeDef::ERROR;
        }  
        if(HAL_SPI_Transmit(_hspi, pdata, len, 1000) != HAL_OK)
        {
            #ifdef ENABLE_DEBUG
                printf("%sSPI write data wrong.\n", ERROR_TYPE);
            #endif
            res = ICM42688_StatusTypeDef::ERROR;
        }  
        csSet();
    }
    #endif

    #ifdef R_SPI_API_H

    #if 1 
    fsp_err_t err;
    
    uint8_t send_buffer[7] = {0};

    send_buffer[0] = reg & 0x7F;
    for(int i = 0; i < len; i++)
        send_buffer[i + 1] = *(pdata + i);

    csReset();
    err = R_SPI_Write(g_spi1.p_ctrl, send_buffer, len + 1, SPI_BIT_WIDTH_8_BITS);
    while(g_SPI_transfer_complete == false);
    g_SPI_transfer_complete = false;
    if(FSP_SUCCESS != err)
        res = ICM42688_StatusTypeDef::ERROR;     
    csSet();

    #endif
    
    
    
    #endif

    return res;
}

void ICM42688::icm_delay(uint32_t ms)
{
    R_BSP_SoftwareDelay(ms, BSP_DELAY_UNITS_MILLISECONDS);
}

//-----------------------------------------------------------------------------------------------------------

ICM42688::ICM42688_StatusTypeDef ICM42688::begin(void)
{
    ICM42688::ICM42688_StatusTypeDef res = ICM42688_StatusTypeDef::OK;

    if(_is_i2c == false && _is_spi == false)
    {
        #ifdef ENABLE_DEBUG
            printf("%splease open i2c or spi or both.\n", ERROR_TYPE);
        #endif
        return ICM42688_StatusTypeDef::ERROR;
    }

    res = general.begin();
    if(res != ICM42688_StatusTypeDef::OK) return res;
    res = Int.begin();
    if(res != ICM42688_StatusTypeDef::OK) return res;

    return res;
}

//===========================================================================================================

//###########################################################################################################

ICM42688::General::General(ICM42688* parent)
    : _parent(parent)
{
}

ICM42688::General::StatusTypeDef ICM42688::General::readReg(uint8_t reg, uint8_t* pdata, uint16_t len)
{
    ICM42688::ICM42688_StatusTypeDef res = ICM42688::ICM42688_StatusTypeDef::OK;

    res = _parent->readReg(reg, pdata, len);
    if(res != ICM42688::ICM42688_StatusTypeDef::OK)
        return ICM42688::General::StatusTypeDef::ERROR;
    else
        return ICM42688::General::StatusTypeDef::OK;
}

ICM42688::General::StatusTypeDef ICM42688::General::writeReg(uint8_t reg, uint8_t* pdata, uint16_t len)
{
    ICM42688::ICM42688_StatusTypeDef res = ICM42688::ICM42688_StatusTypeDef::OK;

    res = _parent->writeReg(reg, pdata, len);
    if(res != ICM42688::ICM42688_StatusTypeDef::OK)
        return ICM42688::General::StatusTypeDef::ERROR;
    else
        return ICM42688::General::StatusTypeDef::OK;
}

ICM42688::General::StatusTypeDef ICM42688::General::find_AFF(ACCEL_ODR odr, uint8_t* AAF_DELT, uint16_t* AAF_DELTSQR, uint8_t* AAF_BITSHIFT)
{
    uint16_t half_odr = 0;

    switch(odr)
    {
        case ACCEL_ODR::ACCEL_ODR_32000HZ:
            half_odr = 16000;
            break;
        case ACCEL_ODR::ACCEL_ODR_16000HZ:
            half_odr = 8000;
            break;
        case ACCEL_ODR::ACCEL_ODR_8000HZ:
            half_odr = 4000;
            break;
        case ACCEL_ODR::ACCEL_ODR_4000HZ:
            half_odr = 2000;
            break;
        case ACCEL_ODR::ACCEL_ODR_2000HZ:
            half_odr = 1000;
            break;
        case ACCEL_ODR::ACCEL_ODR_1000HZ:
            half_odr = 500;
            break;
        case ACCEL_ODR::ACCEL_ODR_200HZ:    
            half_odr = 100;
            break;  
        case ACCEL_ODR::ACCEL_ODR_100HZ:    
            half_odr = 50;
            break;
        default:
            half_odr = 0;
            break;
    }

    if(half_odr == 0)
    {
        #ifdef ENABLE_DEBUG
            printf("%sCould not find the AAF parameter.\n", WARNING_TYPE);
        #endif
        return ICM42688::General::StatusTypeDef::ERROR; 
    }
    else if(half_odr >= 4000)
    {
        *AAF_DELT = 63;
        *AAF_DELTSQR = 3968;
        *AAF_BITSHIFT = 3;
    }
    else if(half_odr == 2000)
    {
        *AAF_DELT = 37;
        *AAF_DELTSQR = 1376;
        *AAF_BITSHIFT = 4;
    }
    else if(half_odr == 1000)
    {
        *AAF_DELT = 21;
        *AAF_DELTSQR = 440;
        *AAF_BITSHIFT = 6;
    }
    else if(half_odr == 500)
    {
        *AAF_DELT = 11;
        *AAF_DELTSQR = 122;
        *AAF_BITSHIFT = 8;
    }
    else if(half_odr == 100)
    {
        *AAF_DELT = 2;
        *AAF_DELTSQR = 4;
        *AAF_BITSHIFT = 13;
    }
    else if(half_odr == 50)
    {
        *AAF_DELT = 1;
        *AAF_DELTSQR = 1;
        *AAF_BITSHIFT = 15;
    }
    
    return ICM42688::General::StatusTypeDef::OK;
} 

ICM42688::General::StatusTypeDef ICM42688::General::find_AFF(GYRO_ODR odr, uint8_t* AAF_DELT, uint16_t* AAF_DELTSQR, uint8_t* AAF_BITSHIFT)
{
    *AAF_DELT = 10;          // 示例值
    *AAF_DELTSQR = 100;     // 示例值
    *AAF_BITSHIFT = 4;      // 示例值

    uint16_t half_odr = 0;

    switch(odr)
    {
        case GYRO_ODR::GYRO_ODR_32000HZ:
            half_odr = 16000;
            break;
        case GYRO_ODR::GYRO_ODR_16000HZ:
            half_odr = 8000;
            break;
        case GYRO_ODR::GYRO_ODR_8000HZ:
            half_odr = 4000;
            break;
        case GYRO_ODR::GYRO_ODR_4000HZ:
            half_odr = 2000;
            break;
        case GYRO_ODR::GYRO_ODR_2000HZ:
            half_odr = 1000;
            break;
        case GYRO_ODR::GYRO_ODR_1000HZ:
            half_odr = 500;
            break;
        case GYRO_ODR::GYRO_ODR_200HZ:    
            half_odr = 100;
            break;  
        case GYRO_ODR::GYRO_ODR_100HZ:    
            half_odr = 50;
            break;
        default:
            half_odr = 0;
            break;
    }

    if(half_odr == 0)
    {
        #ifdef ENABLE_DEBUG
            printf("%sCould not find the AAF parameter.\n", WARNING_TYPE);
        #endif
        return ICM42688::General::StatusTypeDef::ERROR; 
    }
    else if(half_odr >= 4000)
    {
        *AAF_DELT = 63;
        *AAF_DELTSQR = 3968;
        *AAF_BITSHIFT = 3;
    }
    else if(half_odr == 2000)
    {
        *AAF_DELT = 37;
        *AAF_DELTSQR = 1376;
        *AAF_BITSHIFT = 4;
    }
    else if(half_odr == 1000)
    {
        *AAF_DELT = 21;
        *AAF_DELTSQR = 440;
        *AAF_BITSHIFT = 6;
    }
    else if(half_odr == 500)
    {
        *AAF_DELT = 11;
        *AAF_DELTSQR = 122;
        *AAF_BITSHIFT = 8;
    }
    else if(half_odr == 100)
    {
        *AAF_DELT = 2;
        *AAF_DELTSQR = 4;
        *AAF_BITSHIFT = 13;
    }
    else if(half_odr == 50)
    {
        *AAF_DELT = 1;
        *AAF_DELTSQR = 1;
        *AAF_BITSHIFT = 15;
    }
    
    return ICM42688::General::StatusTypeDef::OK;
} 

ICM42688::General::StatusTypeDef ICM42688::General::find_NF(uint16_t fdesired, uint8_t* NF_COSWZ_SEL, uint16_t* NF_COSWZ)
{
    StatusTypeDef res = StatusTypeDef::OK;

    if(fdesired == 0)   // 使用工厂值
        return res;

    if(fdesired < 1000 || fdesired > 3000)
    {
        #ifdef ENABLE_DEBUG
            printf("%sNotch filter's range is begin at 1kHz and end at 3kHz.\n", ERROR_TYPE);
        #endif
        return StatusTypeDef::ERROR;
    }
        
    float COSWZ = cos(2 * PI * fdesired / 32);

    if(abs(COSWZ) <= 0.875f)
    {
        *NF_COSWZ = round(COSWZ * 256);
        *NF_COSWZ_SEL = 0;
    }
    else
    {
        *NF_COSWZ_SEL = 1;
        if(COSWZ > 0.875f)
            *NF_COSWZ = round(8 * (1 - COSWZ) * 256);
        else if(COSWZ < -0.875f)
            *NF_COSWZ = round(-8 * (1 + COSWZ) * 256);
        else 
            return StatusTypeDef::ERROR;
    }

    return res;
}

ICM42688::General::StatusTypeDef ICM42688::General::set_acc_filter(ACCEL_MODE mode, ACCEL_UIF_LN UIF_bw_LN, ACCEL_UIF_LP UIF_bw_LP, ACCEL_ORDER_UIF UIF_order, ICM42688_SWITCH M2F_enable, ICM42688_SWITCH AAF_enable, uint8_t AAF_DELT, uint16_t AAF_DELTSQR, uint8_t AAF_BITSHIFT)
{
    StatusTypeDef res = StatusTypeDef::OK;
    uint8_t reg_data = 0;

    #ifdef ENABLE_DEBUG
        printf("%sstarting set accel filter.\n", PROCESS_TYPE);
    #endif

    if(M2F_enable == ICM42688_SWITCH::OPEN)
    {
        reg_data = static_cast<uint8_t>(UIF_order) << 3 | 0x04;
        res = writeReg(REG_ACCEL_CONFIG1, &reg_data, 1);
        if(res != StatusTypeDef::OK) return res;
        #ifdef ENABLE_DEBUG
            printf("M2 filter set success.\n");
        #endif
    }
    else if(M2F_enable == ICM42688_SWITCH::CLOSE)
    {
        reg_data = static_cast<uint8_t>(UIF_order) << 3 | 0x00;
        res = writeReg(REG_ACCEL_CONFIG1, &reg_data, 1);
        if(res != StatusTypeDef::OK) return res;
        #ifdef ENABLE_DEBUG
            printf("M2 filter not set.\n");
        #endif
    }
    else
    {
        #ifdef ENABLE_DEBUG
            printf("%sM2 filter AND UI filter Order set wrong.\n", ERROR_TYPE);
        #endif
        return StatusTypeDef::ERROR;
    }

    #ifdef ENABLE_DEBUG
        printf("UI filter ORDER: %s%s%s%s.\nUI filter set success.\n", COLOR_BRIGHT_MAGENTA, UNDERLINE, ACCEL_ORDER_UIF_str[static_cast<uint8_t>(UIF_order)], COLOR_RESET);
    #endif

    if(mode == ACCEL_MODE::ACCEL_MODE_LN)
    {
        res = readReg(REG_PWR_MGMT0, &reg_data, 1);
        if(res != StatusTypeDef::OK) return res;
        reg_data = reg_data & 0xFC | 0x03;
        res = writeReg(REG_PWR_MGMT0, &reg_data, 1);
        if(res != StatusTypeDef::OK) return res;

        #ifdef ENABLE_DEBUG
            printf("accel Mode: %s%s%s%s.\naccel Mode set success.\n", COLOR_BRIGHT_MAGENTA, UNDERLINE, ACCEL_MODE_str[static_cast<uint8_t>(mode)], COLOR_RESET);
        #endif

        res = readReg(REG_GYRO_ACCEL_CONFIG0, &reg_data, 1);
        if(res != StatusTypeDef::OK) return res;
        reg_data = reg_data & 0x0F | static_cast<uint8_t>(UIF_bw_LN) << 4;
        res = writeReg(REG_GYRO_ACCEL_CONFIG0, &reg_data, 1);
        if(res != StatusTypeDef::OK) return res;

        #ifdef ENABLE_DEBUG
            printf("UI filter BW: %s%s%s%s.\nUI filter BW success.\n", COLOR_BRIGHT_MAGENTA, UNDERLINE, ACCEL_UIF_LN_str[static_cast<uint8_t>(UIF_bw_LN)], COLOR_RESET);
        #endif

        #ifdef ENABLE_DEBUG
            printf("AAF_DELT: %s%s%d%s.  AAF_DELTSQR: %s%s%d%s.  AAF_BITSHIFT: %s%s%d%s.\n", COLOR_BRIGHT_MAGENTA, UNDERLINE, AAF_DELT, COLOR_RESET, COLOR_BRIGHT_MAGENTA, UNDERLINE, AAF_DELTSQR, COLOR_RESET, COLOR_BRIGHT_MAGENTA, UNDERLINE, AAF_BITSHIFT, COLOR_RESET);
        #endif

        if(AAF_enable == ICM42688_SWITCH::OPEN)
        {
            reg_data = 0x02;
            res = writeReg(REG_REG_BANK_SEL, &reg_data, 1);
            if(res != StatusTypeDef::OK) return res;

            reg_data = (AAF_DELT & 0x3F) << 1;
            res = writeReg(REG_ACCEL_CONFIG_STATIC2, &reg_data, 1);
            if(res != StatusTypeDef::OK) return res;

            reg_data = AAF_DELTSQR & 0xFF;
            res = writeReg(REG_ACCEL_CONFIG_STATIC3, &reg_data, 1);
            if(res != StatusTypeDef::OK) return res;

            reg_data = (AAF_BITSHIFT & 0x0F) << 4 | (AAF_DELTSQR >> 8) & 0x0F;
            res = writeReg(REG_ACCEL_CONFIG_STATIC4, &reg_data, 1);
            if(res != StatusTypeDef::OK) return res;

            reg_data = 0x00;
            res = writeReg(REG_REG_BANK_SEL, &reg_data, 1);
            if(res != StatusTypeDef::OK) return res;

            #ifdef ENABLE_DEBUG
                printf("AAF set success.\n");
            #endif
        }
        else if(AAF_enable == ICM42688_SWITCH::CLOSE)
        {
            res = readReg(REG_ACCEL_CONFIG_STATIC2, &reg_data, 1);
            if(res != StatusTypeDef::OK) return res;
            reg_data = reg_data & 0xFE | 0x01;
            res = writeReg(REG_ACCEL_CONFIG_STATIC2, &reg_data, 1);
            if(res != StatusTypeDef::OK) return res;

            #ifdef ENABLE_DEBUG
                printf("AAF not set.Maybe you didn't open it or AAF's paramaters were finded worng.\n");
            #endif 
        }
        else
        {
            #ifdef ENABLE_DEBUG
                printf("%sAAF set wrong.\n", ERROR_TYPE);
            #endif
            return StatusTypeDef::ERROR;
        }
    }
    else if(mode == ACCEL_MODE::ACCEL_MODE_LP)
    {
        res = readReg(REG_PWR_MGMT0, &reg_data, 1);
        if(res != StatusTypeDef::OK) return res;
        reg_data = reg_data & 0xFC | 0x02;
        res = writeReg(REG_PWR_MGMT0, &reg_data, 1);
        if(res != StatusTypeDef::OK) return res;

        #ifdef ENABLE_DEBUG
            printf("accel Mode: %s%s%s%s.\naccel Mode set success.\n", COLOR_BRIGHT_MAGENTA, UNDERLINE, ACCEL_MODE_str[static_cast<uint8_t>(mode)], COLOR_RESET);
        #endif

        res = readReg(REG_GYRO_ACCEL_CONFIG0, &reg_data, 1);
        if(res != StatusTypeDef::OK) return res;
        reg_data = reg_data & 0x0F | static_cast<uint8_t>(UIF_bw_LP) << 4;
        res = writeReg(REG_GYRO_ACCEL_CONFIG0, &reg_data, 1);
        if(res != StatusTypeDef::OK) return res;

        #ifdef ENABLE_DEBUG
            printf("UI filter BW: %s%s%s%s.\nUI filter BW success.\n", COLOR_BRIGHT_MAGENTA, UNDERLINE, ACCEL_UIF_LP_str[static_cast<uint8_t>(UIF_bw_LP)], COLOR_RESET);
        #endif

        #ifdef ENABLE_DEBUG
            printf("AAF not set.AAF is not allowed under LP mode.\n");
        #endif 
    }
    else
    {
        #ifdef ENABLE_DEBUG
            printf("%smode set wrong.\n", ERROR_TYPE);
        #endif
        return StatusTypeDef::ERROR;
    }

    #ifdef ENABLE_DEBUG
        printf("%sfinish set accel filter.\n", SUCCESS_TYPE);
    #endif

    return StatusTypeDef::OK;
}

ICM42688::General::StatusTypeDef ICM42688::General::set_gyro_filter(GYRO_UIF_LN UIF_bw, GYRO_ORDER_UIF UIF_order, ICM42688_SWITCH M2F_enable,ICM42688_SWITCH NF_enable, uint16_t NF_certenfreq_x, uint16_t NF_certenfreq_y, uint16_t NF_certenfreq_z, NF_BW NF_bw, ICM42688_SWITCH AAF_enable, uint8_t AAF_DELT, uint16_t AAF_DELTSQR, uint8_t AAF_BITSHIFT)
{
    StatusTypeDef res = StatusTypeDef::OK;
    uint8_t reg_data = 0;

    #ifdef ENABLE_DEBUG
        printf("%sstarting set gyro filter.\n", PROCESS_TYPE);
    #endif

    res = readReg(REG_PWR_MGMT0, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;
    reg_data = reg_data & 0xF3 | 0x0C;
    res = writeReg(REG_PWR_MGMT0, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    #ifdef ENABLE_DEBUG
        printf("gyro Mode: %s%sMODE_LN%s.\ngyro Mode set success.\n", COLOR_BRIGHT_MAGENTA, UNDERLINE, COLOR_RESET);
    #endif

    if(M2F_enable == ICM42688_SWITCH::OPEN)
    {
        res = readReg(REG_GYRO_CONFIG1, &reg_data, 1);
        if(res != StatusTypeDef::OK) return res;
        reg_data = reg_data & 0xF0 | static_cast<uint8_t>(UIF_order) << 2 | 0x02;
        res = writeReg(REG_GYRO_CONFIG1, &reg_data, 1);
        if(res != StatusTypeDef::OK) return res;
        #ifdef ENABLE_DEBUG
            printf("M2 filter set success.\n");
        #endif
    }
    else if(M2F_enable == ICM42688_SWITCH::CLOSE)
    {
        res = readReg(REG_GYRO_CONFIG1, &reg_data, 1);
        if(res != StatusTypeDef::OK) return res;
        reg_data = reg_data & 0xF0 | static_cast<uint8_t>(UIF_order) << 2 | 0x00;
        res = writeReg(REG_GYRO_CONFIG1, &reg_data, 1);
        if(res != StatusTypeDef::OK) return res;
        #ifdef ENABLE_DEBUG
            printf("M2 filter not set.\n");
        #endif
    }
    else
    {
        #ifdef ENABLE_DEBUG
            printf("%sM2 filter AND UI filter Order set wrong.\n", ERROR_TYPE);
        #endif
        return StatusTypeDef::ERROR;
    }

    #ifdef ENABLE_DEBUG
        printf("UI filter ORDER: %s%s%s%s.\nUI filter set success.\n", COLOR_BRIGHT_MAGENTA, UNDERLINE, GYRO_ORDER_UIF_str[static_cast<uint8_t>(UIF_order)], COLOR_RESET);
    #endif

    res = readReg(REG_GYRO_ACCEL_CONFIG0, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;
    reg_data = reg_data & 0xF0 | static_cast<uint8_t>(UIF_bw);
    res = writeReg(REG_GYRO_ACCEL_CONFIG0, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    #ifdef ENABLE_DEBUG
        printf("UI filter BW: %s%s%s%s.\nUI filter BW success.\n", COLOR_BRIGHT_MAGENTA, UNDERLINE, GYRO_UIF_LN_str[static_cast<uint8_t>(UIF_bw)], COLOR_RESET);
    #endif

    reg_data = 0x01;
    res = writeReg(REG_REG_BANK_SEL, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    if(NF_enable == ICM42688_SWITCH::OPEN)
    {
        res = readReg(REG_GYRO_CONFIG_STATIC2, &reg_data, 1);
        if(res != StatusTypeDef::OK) return res;
        reg_data = reg_data & 0xFE | 0x00;
        res = writeReg(REG_GYRO_CONFIG_STATIC2, &reg_data, 1);
        if(res != StatusTypeDef::OK) return res;

        uint8_t NF_COSWZ_SEL_x = 0;
        uint16_t NF_COSWZ_x = 0;
        uint8_t NF_COSWZ_SEL_y = 0;
        uint16_t NF_COSWZ_y = 0;
        uint8_t NF_COSWZ_SEL_z = 0;
        uint16_t NF_COSWZ_z = 0;

        if(NF_certenfreq_x == 0 && NF_certenfreq_y == 0 && NF_certenfreq_z == 0)
        {
            reg_data = static_cast<uint8_t>(NF_bw) << 4;
            res = writeReg(REG_GYRO_CONFIG_STATIC10, &reg_data, 1);
            if(res != StatusTypeDef::OK) return res; 

            res = readReg(REG_GYRO_CONFIG_STATIC9, &reg_data, 1);
            if(res != StatusTypeDef::OK) return res;
            NF_COSWZ_SEL_z = (reg_data & 0x20) >> 5;
            NF_COSWZ_SEL_y = (reg_data & 0x10) >> 4;
            NF_COSWZ_SEL_x = (reg_data & 0x08) >> 3;
            NF_COSWZ_z = (reg_data & 0x04) << 6;
            NF_COSWZ_y = (reg_data & 0x02) << 7;
            NF_COSWZ_x = (reg_data & 0x01) << 8;
            res = readReg(REG_GYRO_CONFIG_STATIC6, &reg_data, 1);
            if(res != StatusTypeDef::OK) return res;
            NF_COSWZ_x |= reg_data;
            res = readReg(REG_GYRO_CONFIG_STATIC7, &reg_data, 1);
            if(res != StatusTypeDef::OK) return res;
            NF_COSWZ_y |= reg_data;
            res = readReg(REG_GYRO_CONFIG_STATIC8, &reg_data, 1);
            if(res != StatusTypeDef::OK) return res;
            NF_COSWZ_z |= reg_data;
            res = readReg(REG_GYRO_CONFIG_STATIC10, &reg_data, 1);
            if(res != StatusTypeDef::OK) return res;

            #ifdef ENABLE_DEBUG
                printf("%sWe are using the factory parameter, it is OK.\nNotch filter set success by factory parameter.\n", WARNING_TYPE);
            #endif
            #ifdef ENABLE_DEBUG
                printf("NF_COSWZ_SEL_x: %s%s%d%s.  NF_COSWZ_x: %s%s%d%s.\n", COLOR_BRIGHT_MAGENTA, UNDERLINE, NF_COSWZ_SEL_x, COLOR_RESET, COLOR_BRIGHT_MAGENTA, UNDERLINE, NF_COSWZ_x, COLOR_RESET);
                printf("NF_COSWZ_SEL_y: %s%s%d%s.  NF_COSWZ_y: %s%s%d%s.\n", COLOR_BRIGHT_MAGENTA, UNDERLINE, NF_COSWZ_SEL_y, COLOR_RESET, COLOR_BRIGHT_MAGENTA, UNDERLINE, NF_COSWZ_y, COLOR_RESET);
                printf("NF_COSWZ_SEL_z: %s%s%d%s.  NF_COSWZ_z: %s%s%d%s.\n", COLOR_BRIGHT_MAGENTA, UNDERLINE, NF_COSWZ_SEL_z, COLOR_RESET, COLOR_BRIGHT_MAGENTA, UNDERLINE, NF_COSWZ_z, COLOR_RESET);
                printf("NF_bw: %s%s%s%s.\n", COLOR_BRIGHT_MAGENTA, UNDERLINE, NF_BW_str[reg_data >> 4], COLOR_RESET);
            #endif
        }
        else if(find_NF(NF_certenfreq_x, &NF_COSWZ_SEL_x, &NF_COSWZ_x) == StatusTypeDef::ERROR || find_NF(NF_certenfreq_y, &NF_COSWZ_SEL_y, &NF_COSWZ_y) == StatusTypeDef::ERROR || find_NF(NF_certenfreq_z, &NF_COSWZ_SEL_z, &NF_COSWZ_z) == StatusTypeDef::ERROR)
        {
            res = readReg(REG_GYRO_CONFIG_STATIC2, &reg_data, 1);
            if(res != StatusTypeDef::OK) return res;
            reg_data = reg_data & 0xFE | 0x01;
            res = writeReg(REG_GYRO_CONFIG_STATIC2, &reg_data, 1);
            if(res != StatusTypeDef::OK) return res;

            #ifdef ENABLE_DEBUG
                printf("%sNotice: NF's range is begin at 1kHz and end at 3kHz.\n", WARNING_TYPE);
                printf("%sWe disabled the Notch filter bacause we could not find right parameters of NF, you could try filling all 0 to use the factory parameter.\n", WARNING_TYPE);
            #endif
        }
        else
        {
            find_NF(NF_certenfreq_x, &NF_COSWZ_SEL_x, &NF_COSWZ_x);
            find_NF(NF_certenfreq_y, &NF_COSWZ_SEL_y, &NF_COSWZ_y);
            find_NF(NF_certenfreq_z, &NF_COSWZ_SEL_z, &NF_COSWZ_z);

            #ifdef ENABLE_DEBUG
                printf("NF_certenfreq_x: %s%s%d%s.  NF_COSWZ_SEL_x: %s%s%d%s.  NF_COSWZ_x: %s%s%d%s.\n", COLOR_BRIGHT_MAGENTA, UNDERLINE, NF_certenfreq_x, COLOR_RESET, COLOR_BRIGHT_MAGENTA, UNDERLINE, NF_COSWZ_SEL_x, COLOR_RESET, COLOR_BRIGHT_MAGENTA, UNDERLINE, NF_COSWZ_x, COLOR_RESET);
                printf("NF_certenfreq_y: %s%s%d%s.  NF_COSWZ_SEL_y: %s%s%d%s.  NF_COSWZ_y: %s%s%d%s.\n", COLOR_BRIGHT_MAGENTA, UNDERLINE, NF_certenfreq_y, COLOR_RESET, COLOR_BRIGHT_MAGENTA, UNDERLINE, NF_COSWZ_SEL_y, COLOR_RESET, COLOR_BRIGHT_MAGENTA, UNDERLINE, NF_COSWZ_y, COLOR_RESET);
                printf("NF_certenfreq_z: %s%s%d%s.  NF_COSWZ_SEL_z: %s%s%d%s.  NF_COSWZ_z: %s%s%d%s.\n", COLOR_BRIGHT_MAGENTA, UNDERLINE, NF_certenfreq_z, COLOR_RESET, COLOR_BRIGHT_MAGENTA, UNDERLINE, NF_COSWZ_SEL_z, COLOR_RESET, COLOR_BRIGHT_MAGENTA, UNDERLINE, NF_COSWZ_z, COLOR_RESET);
                printf("NF_bw: %s%s%s%s.\n", COLOR_BRIGHT_MAGENTA, UNDERLINE, NF_BW_str[static_cast<uint8_t>(NF_bw)], COLOR_RESET);
            #endif

            reg_data = NF_COSWZ_SEL_z << 5 | NF_COSWZ_SEL_y << 4 | NF_COSWZ_SEL_x << 3 | (NF_COSWZ_z >> 8) << 2 | (NF_COSWZ_y >> 8) << 1 | (NF_COSWZ_x >> 8);
            res = writeReg(REG_GYRO_CONFIG_STATIC9, &reg_data, 1);
            if(res != StatusTypeDef::OK) return res; 

            reg_data = NF_COSWZ_x & 0xFF;
            res = writeReg(REG_GYRO_CONFIG_STATIC6, &reg_data, 1);
            if(res != StatusTypeDef::OK) return res; 
        
            reg_data = NF_COSWZ_y & 0xFF;
            res = writeReg(REG_GYRO_CONFIG_STATIC7, &reg_data, 1);
            if(res != StatusTypeDef::OK) return res; 
    
            reg_data = NF_COSWZ_z & 0xFF;
            res = writeReg(REG_GYRO_CONFIG_STATIC8, &reg_data, 1);
            if(res != StatusTypeDef::OK) return res; 

            reg_data = static_cast<uint8_t>(NF_bw) << 4;
            res = writeReg(REG_GYRO_CONFIG_STATIC10, &reg_data, 1);
            if(res != StatusTypeDef::OK) return res; 

            #ifdef ENABLE_DEBUG
                printf("Notch filter set success.\n");
            #endif
        }
    }
    else if(NF_enable == ICM42688_SWITCH::CLOSE)
    {
        res = readReg(REG_GYRO_CONFIG_STATIC2, &reg_data, 1);
        if(res != StatusTypeDef::OK) return res;
        reg_data = reg_data & 0xFE | 0x01;
        res = writeReg(REG_GYRO_CONFIG_STATIC2, &reg_data, 1);
        if(res != StatusTypeDef::OK) return res; 

        #ifdef ENABLE_DEBUG
            printf("Notch filter not set.\n");
        #endif
    }
    else
    {
        #ifdef ENABLE_DEBUG
            printf("%sNotch filter set wrong.\n", ERROR_TYPE);
        #endif
        return StatusTypeDef::ERROR;
    }

    #ifdef ENABLE_DEBUG
        printf("AAF_DELT: %s%s%d%s.  AAF_DELTSQR: %s%s%d%s.  AAF_BITSHIFT: %s%s%d%s.\n", COLOR_BRIGHT_MAGENTA, UNDERLINE, AAF_DELT, COLOR_RESET, COLOR_BRIGHT_MAGENTA, UNDERLINE, AAF_DELTSQR, COLOR_RESET, COLOR_BRIGHT_MAGENTA, UNDERLINE, AAF_BITSHIFT, COLOR_RESET);
    #endif

    if(AAF_enable == ICM42688_SWITCH::OPEN)
    {
        res = readReg(REG_GYRO_CONFIG_STATIC2, &reg_data, 1);
        if(res != StatusTypeDef::OK) return res;
        reg_data = reg_data & 0xFD | 0x00;
        res = writeReg(REG_GYRO_CONFIG_STATIC2, &reg_data, 1);
        if(res != StatusTypeDef::OK) return res; 

        reg_data = AAF_DELT & 0x3F;
        res = writeReg(REG_GYRO_CONFIG_STATIC3, &reg_data, 1);
        if(res != StatusTypeDef::OK) return res; 

        reg_data = AAF_DELTSQR & 0xFF;
        res = writeReg(REG_GYRO_CONFIG_STATIC4, &reg_data, 1);
        if(res != StatusTypeDef::OK) return res; 

        reg_data = (AAF_BITSHIFT & 0x0F) << 4 | (AAF_DELTSQR >> 4) & 0x0F;
        res = writeReg(REG_GYRO_CONFIG_STATIC4, &reg_data, 1);
        if(res != StatusTypeDef::OK) return res; 

        #ifdef ENABLE_DEBUG
            printf("AAF set success.\n");
        #endif
    }
    else if(AAF_enable == ICM42688_SWITCH::CLOSE)
    {
        res = readReg(REG_GYRO_CONFIG_STATIC2, &reg_data, 1);
        if(res != StatusTypeDef::OK) return res;
        reg_data = reg_data & 0xFD | 0x01;
        res = writeReg(REG_GYRO_CONFIG_STATIC2, &reg_data, 1);
        if(res != StatusTypeDef::OK) return res; 

        #ifdef ENABLE_DEBUG
            printf("AAF not set.\n");
        #endif
    }
    else
    {
        #ifdef ENABLE_DEBUG
            printf("%sAAF set wrong.\n", ERROR_TYPE);
        #endif
        return StatusTypeDef::ERROR;
    }

    reg_data = 0x00;
    res = writeReg(REG_REG_BANK_SEL, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    #ifdef ENABLE_DEBUG
        printf("%sfinish set gyro filter.\n", SUCCESS_TYPE);
    #endif

    return res;
}

ICM42688::General::StatusTypeDef ICM42688::General::set_acc_gyro_offset(uint16_t samples)
{
    StatusTypeDef res = StatusTypeDef::OK;
    uint8_t reg_data = 0;

    float ax_off = 0;
    float ay_off = 0;
    float az_off = 0;
    float gx_off = 0;
    float gy_off = 0;
    float gz_off = 0; 

    _acc_offset_x = 0;
    _acc_offset_y = 0;
    _acc_offset_z = 0;
    _gyro_offset_x = 0;
    _gyro_offset_y = 0;
    _gyro_offset_z = 0;
    
    reg_data = 0x04;
    res = writeReg(REG_REG_BANK_SEL, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    reg_data = 0x00;
    res = writeReg(REG_OFFSET_USER0, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    reg_data = 0x00;
    res = writeReg(REG_OFFSET_USER1, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    reg_data = 0x00;
    res = writeReg(REG_OFFSET_USER2, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    reg_data = 0x00;
    res = writeReg(REG_OFFSET_USER3, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    reg_data = 0x00;
    res = writeReg(REG_OFFSET_USER4, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    reg_data = 0x00;
    res = writeReg(REG_OFFSET_USER5, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    reg_data = 0x00;
    res = writeReg(REG_OFFSET_USER6, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    reg_data = 0x00;
    res = writeReg(REG_OFFSET_USER7, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    reg_data = 0x00;
    res = writeReg(REG_OFFSET_USER8, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    reg_data = 0x00;
    res = writeReg(REG_REG_BANK_SEL, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    _parent->icm_delay(100);

    #ifdef ENABLE_DEBUG
        printf("%sstarting set accel and gyro offset.\n", PROCESS_TYPE);
    #endif

    #ifdef ENABLE_DEBUG
        printf("now data are:\n");
        for(int i = 0; i < 5; i++)
        {
            read_data();
            printf("ax = %f ay = %f az = %f gx = %f gy = %f gz = %f\n", get_ax(), get_ay(), get_az(), get_gx(), get_gy(), get_gz());
        }    
    #endif

    #ifdef ENABLE_DEBUG
        printf("Sampling...Please keep IMU stable.\n");
    #endif

    for(int i = 0; i < samples; i++)
    {
        read_data();
        
        switch(static_cast<uint8_t>(_parent->_mounting))
        {
            case 0:
            {
                ax_off += get_ax();
                ay_off += get_ay();
                az_off += (get_az() - g);
                break;
            }
            case 1:
            {
                ax_off += get_ax();
                ay_off += get_ay();
                az_off += (get_az() + g);
                break;
            }
            case 2:
            {
                ax_off += (get_ax() - g);
                ay_off += get_ay();
                az_off += get_az();
                break;
            }
            case 3:
            {
                ax_off += (get_ax() + g);
                ay_off += get_ay();
                az_off += get_az();
                break;
            }
            case 4:
            {
                ax_off += get_ax();
                ay_off += (get_ay() - g);
                az_off += get_az();
                break;
            }
            case 5:
            {
                ax_off += get_ax();
                ay_off += (get_ay() + g);
                az_off += get_az();
                break;
            }
            default:
                return StatusTypeDef::ERROR; 
        }
        gx_off += get_gx();
        gy_off += get_gy();
        gz_off += get_gz();


        #ifdef ENABLE_DEBUG
            if(i % 100 == 1)
                printf("Sampling...Please keep IMU stable.\n"); 
        #endif
    }

    #ifdef ENABLE_DEBUG
        printf("Sampling done.\n");
        printf("ave_ax: %s%s%f%s  ave_ay: %s%s%f%s  ave_az: %s%s%f%s  ave_gx: %s%s%f%s  ave_gy: %s%s%f%s  ave_gz: %s%s%f%s.\n", 
            COLOR_BRIGHT_MAGENTA, UNDERLINE, ax_off / samples, COLOR_RESET, COLOR_BRIGHT_MAGENTA, UNDERLINE, ay_off / samples, COLOR_RESET,
            COLOR_BRIGHT_MAGENTA, UNDERLINE, az_off / samples, COLOR_RESET, COLOR_BRIGHT_MAGENTA, UNDERLINE, gx_off / samples, COLOR_RESET,
            COLOR_BRIGHT_MAGENTA, UNDERLINE, gy_off / samples, COLOR_RESET, COLOR_BRIGHT_MAGENTA, UNDERLINE, gz_off / samples, COLOR_RESET);
    #endif

    int16_t ax_off_real = -ax_off / g / samples / 0.0005f;
    int16_t ay_off_real = -ay_off / g / samples / 0.0005f;
    int16_t az_off_real = -az_off / g / samples / 0.0005f;
    int16_t gx_off_real = -gx_off / samples / 0.03125f;
    int16_t gy_off_real = -gy_off / samples / 0.03125f;
    int16_t gz_off_real = -gz_off / samples / 0.03125f;

    reg_data = 0x04;
    res = writeReg(REG_REG_BANK_SEL, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    reg_data = gx_off_real & 0xFF;
    res = writeReg(REG_OFFSET_USER0, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    reg_data = (((gy_off_real >> 8) & 0x0F) << 4) | ((gx_off_real >> 8) & 0x0F);
    res = writeReg(REG_OFFSET_USER1, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    reg_data = gy_off_real & 0xFF;
    res = writeReg(REG_OFFSET_USER2, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    reg_data = gz_off_real & 0xFF;
    res = writeReg(REG_OFFSET_USER3, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    reg_data = (gz_off_real >> 8) & 0x0F;
    res = writeReg(REG_OFFSET_USER4, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;
    
//    reg_data = (((ax_off_real >> 8) & 0x0F) << 4) | ((gz_off_real >> 8) & 0x0F);
//    res = writeReg(REG_OFFSET_USER4, &reg_data, 1);
//    if(res != StatusTypeDef::OK) return res;

//    reg_data = ax_off_real & 0xFF;
//    res = writeReg(REG_OFFSET_USER5, &reg_data, 1);
//    if(res != StatusTypeDef::OK) return res;

//    reg_data = ay_off_real & 0xFF;
//    res = writeReg(REG_OFFSET_USER6, &reg_data, 1);
//    if(res != StatusTypeDef::OK) return res;

//    reg_data = (((az_off_real >> 8) & 0x0F) << 4) | ((ay_off_real >> 8) & 0x0F);
//    res = writeReg(REG_OFFSET_USER7, &reg_data, 1);
//    if(res != StatusTypeDef::OK) return res;

//    reg_data = az_off_real & 0xFF;
//    res = writeReg(REG_OFFSET_USER8, &reg_data, 1);
//    if(res != StatusTypeDef::OK) return res;

//    reg_data = 0x00;
//    res = writeReg(REG_REG_BANK_SEL, &reg_data, 1);
//    if(res != StatusTypeDef::OK) return res;

    _parent->icm_delay(100);

    #ifdef ENABLE_DEBUG
        printf("after offset data are:\n");
        for(int i = 0; i < 5; i++)
        {
            read_data();
            printf("ax = %f ay = %f az = %f gx = %f gy = %f gz = %f\n", get_ax(), get_ay(), get_az(), get_gx(), get_gy(), get_gz());
        }    
    #endif

    ax_off = 0;
    ay_off = 0;
    az_off = 0;
    gx_off = 0;
    gy_off = 0;
    gz_off = 0; 

    #ifdef ENABLE_DEBUG
        printf("Software sampling...Please keep IMU stable.\n");
    #endif

    for(int i = 0; i < samples; i++)
    {
        read_data();
        
        switch(static_cast<uint8_t>(_parent->_mounting))
        {
            case 0:
            {
                ax_off += get_ax();
                ay_off += get_ay();
                az_off += (get_az() - g);
                break;
            }
            case 1:
            {
                ax_off += get_ax();
                ay_off += get_ay();
                az_off += (get_az() + g);
                break;
            }
            case 2:
            {
                ax_off += (get_ax() - g);
                ay_off += get_ay();
                az_off += get_az();
                break;
            }
            case 3:
            {
                ax_off += (get_ax() + g);
                ay_off += get_ay();
                az_off += get_az();
                break;
            }
            case 4:
            {
                ax_off += get_ax();
                ay_off += (get_ay() - g);
                az_off += get_az();
                break;
            }
            case 5:
            {
                ax_off += get_ax();
                ay_off += (get_ay() + g);
                az_off += get_az();
                break;
            }
            default:
                return StatusTypeDef::ERROR; 
        }
        gx_off += get_gx();
        gy_off += get_gy();
        gz_off += get_gz();

        #ifdef ENABLE_DEBUG
            if(i % 100 == 1)
                printf("Software sampling...Please keep IMU stable.\n"); 
        #endif
    }

    #ifdef ENABLE_DEBUG
        printf("Software sampling done.\n");
        printf("ave_ax: %s%s%f%s  ave_ay: %s%s%f%s  ave_az: %s%s%f%s  ave_gx: %s%s%f%s  ave_gy: %s%s%f%s  ave_gz: %s%s%f%s.\n", 
            COLOR_BRIGHT_MAGENTA, UNDERLINE, ax_off / samples, COLOR_RESET, COLOR_BRIGHT_MAGENTA, UNDERLINE, ay_off / samples, COLOR_RESET,
            COLOR_BRIGHT_MAGENTA, UNDERLINE, az_off / samples, COLOR_RESET, COLOR_BRIGHT_MAGENTA, UNDERLINE, gx_off / samples, COLOR_RESET,
            COLOR_BRIGHT_MAGENTA, UNDERLINE, gy_off / samples, COLOR_RESET, COLOR_BRIGHT_MAGENTA, UNDERLINE, gz_off / samples, COLOR_RESET);
    #endif

//    _acc_offset_x = -ax_off / samples;
//    _acc_offset_y = -ay_off / samples;
//    _acc_offset_z = -az_off / samples;
    _gyro_offset_x = -gx_off / samples;
    _gyro_offset_y = -gy_off / samples;
    _gyro_offset_z = -gz_off / samples;

    #ifdef ENABLE_DEBUG
        printf("%sfinish set accel and gyro Software offset.\n", SUCCESS_TYPE);
    #endif

    _parent->icm_delay(10);

    #ifdef ENABLE_DEBUG
        printf("after Software offset data are:\n");
        for(int i = 0; i < 5; i++)
        {
            read_data();
            printf("ax = %f ay = %f az = %f gx = %f gy = %f gz = %f\n", get_ax(), get_ay(), get_az(), get_gx(), get_gy(), get_gz());
        }    
    #endif

    _parent->icm_delay(1);

    return res;
}

//-----------------------------------------------------------------------------------------------------------

ICM42688::ICM42688_StatusTypeDef ICM42688::General::begin(void)
{
    StatusTypeDef res = StatusTypeDef::OK;
    uint8_t reg_data = 0;

    #ifdef ENABLE_DEBUG
        printf("%sstarting initial the General.\n", PROCESS_TYPE);
    #endif

    #ifdef HAL_I2C_MODULE_ENABLED
        if(_parent->_is_right_address == false)
        {
            #ifdef ENABLE_DEBUG
                printf("%sWrong I2C Address.\n", ERROR_TYPE);
            #endif
            return ICM42688::ICM42688_StatusTypeDef::ERROR;
        }
    #endif

    reg_data = 0x01;
    res = writeReg(REG_DEVICE_CONFIG, &reg_data, 1);
    if(res != StatusTypeDef::OK) return ICM42688::ICM42688_StatusTypeDef::ERROR;

    _parent->icm_delay(20);

    uint8_t who_am_i = 0;
    res = readReg(REG_WHO_AM_I, &who_am_i, 1);
    if(res != StatusTypeDef::OK) return ICM42688::ICM42688_StatusTypeDef::ERROR;
    if(who_am_i != ICM42688_WHO_AM_I)
    {
        #ifdef ENABLE_DEBUG
            printf("%sWrong WHO_AM_I test.\n", ERROR_TYPE);
        #endif
        return ICM42688::ICM42688_StatusTypeDef::ERROR;
    }

    if(_parent->_is_i2c == true)
    {
        #ifdef ENABLE_DEBUG
            printf("communicataion method: %s%sI2C%s.\n", COLOR_BRIGHT_MAGENTA, UNDERLINE, COLOR_RESET);
        #endif
        uint8_t reg_data = 0x32;
        res = readReg(REG_INTF_CONFIG0, &reg_data, 1);
        if(res != StatusTypeDef::OK) return ICM42688::ICM42688_StatusTypeDef::ERROR;
    }
    else if(_parent->_is_spi == true)
    {
        #ifdef ENABLE_DEBUG
            printf("communicataion method: %s%sSPI%s.\n", COLOR_BRIGHT_MAGENTA, UNDERLINE, COLOR_RESET);
        #endif
        uint8_t reg_data = 0x33;
        res = readReg(REG_INTF_CONFIG0, &reg_data, 1);
        if(res != StatusTypeDef::OK) return ICM42688::ICM42688_StatusTypeDef::ERROR;
    }
    else 
        return ICM42688::ICM42688_StatusTypeDef::ERROR;

    reg_data = 0x91;
    res = writeReg(REG_INTF_CONFIG1, &reg_data, 1);
    if(res != StatusTypeDef::OK) return ICM42688::ICM42688_StatusTypeDef::ERROR;

    reg_data = 0x01;
    res = writeReg(REG_REG_BANK_SEL, &reg_data, 1);
    if(res != StatusTypeDef::OK) return ICM42688::ICM42688_StatusTypeDef::ERROR;

    reg_data = 0x02;
    res = writeReg(REG_INTF_CONFIG4, &reg_data, 1);
    if(res != StatusTypeDef::OK) return ICM42688::ICM42688_StatusTypeDef::ERROR;

    reg_data = 0x00;
    res = writeReg(REG_INTF_CONFIG5, &reg_data, 1);
    if(res != StatusTypeDef::OK) return ICM42688::ICM42688_StatusTypeDef::ERROR;

    reg_data = 0x5F;
    res = writeReg(REG_INTF_CONFIG6, &reg_data, 1);
    if(res != StatusTypeDef::OK) return ICM42688::ICM42688_StatusTypeDef::ERROR;

    reg_data = 0x00;
    res = writeReg(REG_REG_BANK_SEL, &reg_data, 1);
    if(res != StatusTypeDef::OK) return ICM42688::ICM42688_StatusTypeDef::ERROR;

    #ifdef ENABLE_DEBUG
        printf("%sfinish initial the General.\n", SUCCESS_TYPE);
    #endif

    return ICM42688::ICM42688_StatusTypeDef::OK;
}

ICM42688::General::StatusTypeDef ICM42688::General::enable_acc(ACCEL_ODR odr, ACCEL_FS fs, ACCEL_MODE mode, ACCEL_UIF_LN UIF_bw_LN, ACCEL_UIF_LP UIF_bw_LP, ACCEL_ORDER_UIF UIF_order, ICM42688_SWITCH M2F_enable, ICM42688_SWITCH AAF_enable, uint8_t AAF_DELT, uint16_t AAF_DELTSQR, uint8_t AAF_BITSHIFT)
{
    StatusTypeDef res = StatusTypeDef::OK;
    uint8_t reg_data = 0;

    #ifdef ENABLE_DEBUG
        printf("%sstarting enable the accel.\n", PROCESS_TYPE);
    #endif

    if(mode == ACCEL_MODE::ACCEL_MODE_LP && static_cast<uint8_t>(odr) <= 6)
    {
        #ifdef ENABLE_DEBUG
            printf("%sWrong accel mode, if odr >= 12.5Hz, mode could be LN; if odr <= 200Hz, mode could be LP.\n", ERROR_TYPE);
        #endif
        return StatusTypeDef::ERROR;
    }
        
    reg_data = 0x01;
    res = writeReg(REG_REG_BANK_SEL, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    res = readReg(REG_SENSOR_CONFIG0, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;
    reg_data = reg_data & 0xF8 | 0x00;
    res = writeReg(REG_SENSOR_CONFIG0, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    #ifdef ENABLE_DEBUG
        printf("open accel axis: %s%sX Y Z%s.\n", COLOR_BRIGHT_MAGENTA, UNDERLINE, COLOR_RESET);
    #endif

    reg_data = 0x00;
    res = writeReg(REG_REG_BANK_SEL, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    reg_data = static_cast<uint8_t>(fs) << 5 | static_cast<uint8_t>(odr);
    res = writeReg(REG_ACCEL_CONFIG0, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    #ifdef ENABLE_DEBUG
        printf("accel ODR: %s%s%s%s.\naccel FS: %s%s%s%s\n", COLOR_BRIGHT_MAGENTA, UNDERLINE, ACCEL_ODR_str[static_cast<uint8_t>(odr)], COLOR_RESET, COLOR_BRIGHT_MAGENTA, UNDERLINE, ACCEL_FS_str[static_cast<uint8_t>(fs)], COLOR_RESET);
    #endif

    _acc_fs = static_cast<uint8_t>(fs);

    res = set_acc_filter(mode, UIF_bw_LN, UIF_bw_LP, UIF_order, M2F_enable, AAF_enable, AAF_DELT, AAF_DELTSQR, AAF_BITSHIFT);
    if(res != StatusTypeDef::OK) return res;

    #ifdef ENABLE_DEBUG
        printf("%sfinish enable the accel.\n", SUCCESS_TYPE);
    #endif

    _parent->icm_delay(1);

    return StatusTypeDef::OK;
}

ICM42688::General::StatusTypeDef ICM42688::General::enable_acc(ACCEL_ODR odr, ACCEL_FS fs, ACCEL_MODE mode, ACCEL_UIF_LN UIF_bw_LN, ACCEL_UIF_LP UIF_bw_LP, ACCEL_ORDER_UIF UIF_order, ICM42688_SWITCH M2F_enable, ICM42688_SWITCH AAF_enable)
{
    StatusTypeDef res = StatusTypeDef::OK;
    uint8_t reg_data = 0;

    #ifdef ENABLE_DEBUG
        printf("%sstarting enable the accel.\n", PROCESS_TYPE);
    #endif

    if(mode == ACCEL_MODE::ACCEL_MODE_LP && static_cast<uint8_t>(odr) <= 6)
    {
        #ifdef ENABLE_DEBUG
            printf("%sWrong accel mode, if odr >= 12.5Hz, mode could be LN; if odr <= 200Hz, mode could be LP.\n", ERROR_TYPE);
        #endif
        return StatusTypeDef::ERROR;
    }

    uint8_t AAF_DELT = 0;
    uint16_t AAF_DELTSQR = 0;
    uint8_t AAF_BITSHIFT = 0;
    ICM42688_SWITCH _AAF_enable = AAF_enable;

    if(_AAF_enable == ICM42688_SWITCH::OPEN)
    {
        res = find_AFF(odr, &AAF_DELT, &AAF_DELTSQR, &AAF_BITSHIFT);
        if(res != StatusTypeDef::OK)
        {
            #ifdef ENABLE_DEBUG
                printf("%sWe closed your AAF because AAF's range just begin at 42Hz and end at 3979Hz.We coulded figure out right parameters of AAF.\n", WARNING_TYPE);
            #endif
            _AAF_enable = ICM42688_SWITCH::CLOSE;
        }
    }

    reg_data = 0x01;
    res = writeReg(REG_REG_BANK_SEL, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    res = readReg(REG_SENSOR_CONFIG0, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;
    reg_data = reg_data & 0xF8 | 0x00;
    res = writeReg(REG_SENSOR_CONFIG0, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    #ifdef ENABLE_DEBUG
        printf("open accel axis: %s%sX Y Z%s.\n", COLOR_BRIGHT_MAGENTA, UNDERLINE, COLOR_RESET);
    #endif

    reg_data = 0x00;
    res = writeReg(REG_REG_BANK_SEL, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    reg_data = static_cast<uint8_t>(fs) << 5 | static_cast<uint8_t>(odr);
    res = writeReg(REG_ACCEL_CONFIG0, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    #ifdef ENABLE_DEBUG
        printf("accel ODR: %s%s%s%s.\naccel FS: %s%s%s%s\n", COLOR_BRIGHT_MAGENTA, UNDERLINE, ACCEL_ODR_str[static_cast<uint8_t>(odr)], COLOR_RESET, COLOR_BRIGHT_MAGENTA, UNDERLINE, ACCEL_FS_str[static_cast<uint8_t>(fs)], COLOR_RESET);
    #endif

    _acc_fs = static_cast<uint8_t>(fs);

    res = set_acc_filter(mode, UIF_bw_LN, UIF_bw_LP, UIF_order, M2F_enable, _AAF_enable, AAF_DELT, AAF_DELTSQR, AAF_BITSHIFT);
    if(res != StatusTypeDef::OK) return res;

    #ifdef ENABLE_DEBUG
        printf("%sfinish enable the accel.\n", SUCCESS_TYPE);
    #endif

    _parent->icm_delay(1);

    return StatusTypeDef::OK;
}

ICM42688::General::StatusTypeDef ICM42688::General::enable_acc(ACCEL_ODR odr, ACCEL_FS fs, FILTER_LEVEL filter_level)
{
    StatusTypeDef res = StatusTypeDef::OK;
    uint8_t reg_data = 0;

    #ifdef ENABLE_DEBUG
        printf("%sstarting enable the accel.\n", PROCESS_TYPE);
    #endif

    #ifdef ENABLE_DEBUG
        printf("accel Mode: %s%s%s%s.\n", COLOR_BRIGHT_MAGENTA, UNDERLINE, FILTER_LEVEL_str[static_cast<uint8_t>(filter_level)], COLOR_RESET);
    #endif

    ACCEL_MODE mode;
    ACCEL_UIF_LN UIF_bw_LN;
    ACCEL_UIF_LP UIF_bw_LP;
    ACCEL_ORDER_UIF UIF_order;
    ICM42688_SWITCH M2F_enable;
    ICM42688_SWITCH AAF_enable;
    uint8_t AAF_DELT;
    uint16_t AAF_DELTSQR;
    uint8_t AAF_BITSHIFT;

    switch(filter_level)
    {
        case FILTER_LEVEL::VERY_HIGH:
            mode = ACCEL_MODE::ACCEL_MODE_LN;
            UIF_bw_LN = ACCEL_UIF_LN::ACCEL_UIF_LN_400InODR_40;
            UIF_bw_LP = ACCEL_UIF_LP::ICM42688_ACCEL_UIF_LP_1AVG;
            UIF_order = ACCEL_ORDER_UIF::ACCEL_ORDER_UIF_3;
            M2F_enable = ICM42688_SWITCH::OPEN;
            AAF_enable = ICM42688_SWITCH::OPEN;
            res = find_AFF(odr, &AAF_DELT, &AAF_DELTSQR, &AAF_BITSHIFT);
            break;
        case FILTER_LEVEL::HIGH:
            mode = ACCEL_MODE::ACCEL_MODE_LN;
            UIF_bw_LN = ACCEL_UIF_LN::ACCEL_UIF_LN_400InODR_16;
            UIF_bw_LP = ACCEL_UIF_LP::ICM42688_ACCEL_UIF_LP_1AVG;
            UIF_order = ACCEL_ORDER_UIF::ACCEL_ORDER_UIF_3;
            M2F_enable = ICM42688_SWITCH::OPEN;
            AAF_enable = ICM42688_SWITCH::OPEN;
            res = find_AFF(odr, &AAF_DELT, &AAF_DELTSQR, &AAF_BITSHIFT);
            break;
        case FILTER_LEVEL::MEDIUM:
            mode = ACCEL_MODE::ACCEL_MODE_LN;
            UIF_bw_LN = ACCEL_UIF_LN::ACCEL_UIF_LN_400InODR_8;
            UIF_bw_LP = ACCEL_UIF_LP::ICM42688_ACCEL_UIF_LP_1AVG;
            UIF_order = ACCEL_ORDER_UIF::ACCEL_ORDER_UIF_2;
            M2F_enable = ICM42688_SWITCH::OPEN;
            AAF_enable = ICM42688_SWITCH::OPEN;
            res = find_AFF(odr, &AAF_DELT, &AAF_DELTSQR, &AAF_BITSHIFT);
            break;
        case FILTER_LEVEL::LOW:
            mode = ACCEL_MODE::ACCEL_MODE_LN;
            UIF_bw_LN = ACCEL_UIF_LN::ACCEL_UIF_LN_ODR_2;
            UIF_bw_LP = ACCEL_UIF_LP::ICM42688_ACCEL_UIF_LP_1AVG;
            UIF_order = ACCEL_ORDER_UIF::ACCEL_ORDER_UIF_1;
            M2F_enable = ICM42688_SWITCH::CLOSE;
            AAF_enable = ICM42688_SWITCH::OPEN;
            res = find_AFF(odr, &AAF_DELT, &AAF_DELTSQR, &AAF_BITSHIFT);
            break;
        case FILTER_LEVEL::VERY_LOW:
            mode = ACCEL_MODE::ACCEL_MODE_LP;
            UIF_bw_LN = ACCEL_UIF_LN::ACCEL_UIF_LN_ODR_2;
            UIF_bw_LP = ACCEL_UIF_LP::ICM42688_ACCEL_UIF_LP_16AVG;
            UIF_order = ACCEL_ORDER_UIF::ACCEL_ORDER_UIF_1;
            M2F_enable = ICM42688_SWITCH::CLOSE;
            AAF_enable = ICM42688_SWITCH::CLOSE;
            break;
        case FILTER_LEVEL::VERT_VERY_LOW:
            mode = ACCEL_MODE::ACCEL_MODE_LP;
            UIF_bw_LN = ACCEL_UIF_LN::ACCEL_UIF_LN_ODR_2;
            UIF_bw_LP = ACCEL_UIF_LP::ICM42688_ACCEL_UIF_LP_1AVG;
            UIF_order = ACCEL_ORDER_UIF::ACCEL_ORDER_UIF_1;
            M2F_enable = ICM42688_SWITCH::CLOSE;
            AAF_enable = ICM42688_SWITCH::CLOSE;
            break;
        default:
            return StatusTypeDef::ERROR;
    }
    if(res != StatusTypeDef::OK)
    {
        #ifdef ENABLE_DEBUG
            printf("%sWe closed your AAF because AAF's range just begin at 42Hz and end at 3979Hz.We coulded figure out right parameters of AAF.\n", WARNING_TYPE);
        #endif
        AAF_enable = ICM42688_SWITCH::CLOSE;
    }

    reg_data = 0x01;
    res = writeReg(REG_REG_BANK_SEL, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    res = readReg(REG_SENSOR_CONFIG0, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;
    reg_data = reg_data & 0xF8 | 0x00;
    res = writeReg(REG_SENSOR_CONFIG0, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    #ifdef ENABLE_DEBUG
        printf("open accel axis: %s%sX Y Z%s.\n", COLOR_BRIGHT_MAGENTA, UNDERLINE, COLOR_RESET);
    #endif

    reg_data = 0x00;
    res = writeReg(REG_REG_BANK_SEL, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    reg_data = static_cast<uint8_t>(fs) << 5 | static_cast<uint8_t>(odr);
    res = writeReg(REG_ACCEL_CONFIG0, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    #ifdef ENABLE_DEBUG
        printf("accel ODR: %s%s%s%s.\naccel FS: %s%s%s%s\n", COLOR_BRIGHT_MAGENTA, UNDERLINE, ACCEL_ODR_str[static_cast<uint8_t>(odr)], COLOR_RESET, COLOR_BRIGHT_MAGENTA, UNDERLINE, ACCEL_FS_str[static_cast<uint8_t>(fs)], COLOR_RESET);
    #endif

    _acc_fs = static_cast<uint8_t>(fs);

    res = set_acc_filter(mode, UIF_bw_LN, UIF_bw_LP, UIF_order, M2F_enable, AAF_enable, AAF_DELT, AAF_DELTSQR, AAF_BITSHIFT);
    if(res != StatusTypeDef::OK) return res;

    #ifdef ENABLE_DEBUG
        printf("%sfinish enable the accel.\n", SUCCESS_TYPE);
    #endif

    _parent->icm_delay(1);

    return StatusTypeDef::OK;
}

ICM42688::General::StatusTypeDef ICM42688::General::disable_acc(void)
{
    StatusTypeDef res = StatusTypeDef::OK;
    uint8_t reg_data = 0;

    #ifdef ENABLE_DEBUG
        printf("%sstarting disable the accel.\n", PROCESS_TYPE);
    #endif

    reg_data = 0x01;
    res = writeReg(REG_REG_BANK_SEL, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    res = readReg(REG_SENSOR_CONFIG0, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;
    reg_data = reg_data & 0xF8 | 0x07;
    res = writeReg(REG_SENSOR_CONFIG0, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    #ifdef ENABLE_DEBUG
        printf("closed accel axis: %s%sX Y Z%s.\n", COLOR_BRIGHT_MAGENTA, UNDERLINE, COLOR_RESET);
    #endif

    reg_data = 0x00;
    res = writeReg(REG_REG_BANK_SEL, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    res = readReg(REG_PWR_MGMT0, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;
    reg_data = reg_data & 0xFC | 0x00;
    res = writeReg(REG_PWR_MGMT0, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    #ifdef ENABLE_DEBUG
        printf("closed accel.\n");
    #endif

    reg_data = 0x02;
    res = writeReg(REG_REG_BANK_SEL, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    res = readReg(REG_ACCEL_CONFIG_STATIC2, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;
    reg_data = reg_data & 0xFE | 0x01;
    res = writeReg(REG_ACCEL_CONFIG_STATIC2, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    #ifdef ENABLE_DEBUG
        printf("closed accel's AAF.\n");
    #endif

    reg_data = 0x00;
    res = writeReg(REG_REG_BANK_SEL, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    #ifdef ENABLE_DEBUG
        printf("%sfinish disable the accel.\n", SUCCESS_TYPE);
    #endif

    _parent->icm_delay(1);

    return res;
}

ICM42688::General::StatusTypeDef ICM42688::General::enable_gyro(GYRO_ODR odr, GYRO_FS fs, GYRO_UIF_LN UIF_bw, GYRO_ORDER_UIF UIF_order, ICM42688_SWITCH M2F_enable, ICM42688_SWITCH NF_enable, uint16_t NF_certenfreq_x, uint16_t NF_certenfreq_y, uint16_t NF_certenfreq_z, NF_BW NF_bw, ICM42688_SWITCH AAF_enable, uint8_t AAF_DELT, uint16_t AAF_DELTSQR, uint8_t AAF_BITSHIFT)
{
    StatusTypeDef res = StatusTypeDef::OK;
    uint8_t reg_data = 0;

    #ifdef ENABLE_DEBUG
        printf("%sstarting enable the gyro.\n", PROCESS_TYPE);
    #endif

    reg_data = 0x01;
    res = writeReg(REG_REG_BANK_SEL, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    res = readReg(REG_SENSOR_CONFIG0, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;
    reg_data = reg_data & 0xC7 | 0x00;
    res = writeReg(REG_SENSOR_CONFIG0, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    #ifdef ENABLE_DEBUG
        printf("open gyro axis: %s%sX Y Z%s.\n", COLOR_BRIGHT_MAGENTA, UNDERLINE, COLOR_RESET);
    #endif

    reg_data = 0x00;
    res = writeReg(REG_REG_BANK_SEL, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    reg_data = static_cast<uint8_t>(fs) << 5 | static_cast<uint8_t>(odr);
    res = writeReg(REG_GYRO_CONFIG0, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    #ifdef ENABLE_DEBUG
        printf("gyro ODR: %s%s%s%s.\ngyro FS: %s%s%s%s\n", COLOR_BRIGHT_MAGENTA, UNDERLINE, GYRO_ODR_str[static_cast<uint8_t>(odr)], COLOR_RESET, COLOR_BRIGHT_MAGENTA, UNDERLINE, GYRO_FS_str[static_cast<uint8_t>(fs)], COLOR_RESET);
    #endif

    _gyro_fs = static_cast<uint8_t>(fs);

    res = set_gyro_filter(UIF_bw, UIF_order, M2F_enable, NF_enable, NF_certenfreq_x, NF_certenfreq_y, NF_certenfreq_z, NF_bw, AAF_enable, AAF_DELT, AAF_DELTSQR, AAF_BITSHIFT);
    if(res != StatusTypeDef::OK) return res;

    #ifdef ENABLE_DEBUG
        printf("%sfinish enable the gyro.\n", SUCCESS_TYPE);
    #endif

    _parent->icm_delay(1);

    return res;
}

ICM42688::General::StatusTypeDef ICM42688::General::enable_gyro(GYRO_ODR odr, GYRO_FS fs, GYRO_UIF_LN UIF_bw, GYRO_ORDER_UIF UIF_order, ICM42688_SWITCH M2F_enable, ICM42688_SWITCH NF_enable, ICM42688_SWITCH AAF_enable)
{
    StatusTypeDef res = StatusTypeDef::OK;
    uint8_t reg_data = 0;

    #ifdef ENABLE_DEBUG
        printf("%sstarting enable the gyro.\n", PROCESS_TYPE);
    #endif

    uint8_t AAF_DELT = 0;
    uint16_t AAF_DELTSQR = 0;
    uint8_t AAF_BITSHIFT = 0;
    ICM42688_SWITCH _AAF_enable = AAF_enable;

    if(_AAF_enable == ICM42688_SWITCH::OPEN)
    {
        res = find_AFF(odr, &AAF_DELT, &AAF_DELTSQR, &AAF_BITSHIFT);
        if(res != StatusTypeDef::OK)
        {
            #ifdef ENABLE_DEBUG
                printf("%sWe closed your AAF because AAF's range just begin at 42Hz and end at 3979Hz.We coulded figure out right parameters of AAF.\n", WARNING_TYPE);
            #endif
            _AAF_enable = ICM42688_SWITCH::CLOSE;
        }
    }

    uint16_t NF_certenfreq = 0; // 使用工厂值
    NF_BW NF_bw = NF_BW::NF_BW_680HZ;

    reg_data = 0x01;
    res = writeReg(REG_REG_BANK_SEL, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    res = readReg(REG_SENSOR_CONFIG0, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;
    reg_data = reg_data & 0xC7 | 0x00;
    res = writeReg(REG_SENSOR_CONFIG0, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    #ifdef ENABLE_DEBUG
        printf("open gyro axis: %s%sX Y Z%s.\n", COLOR_BRIGHT_MAGENTA, UNDERLINE, COLOR_RESET);
    #endif

    reg_data = 0x00;
    res = writeReg(REG_REG_BANK_SEL, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    reg_data = static_cast<uint8_t>(fs) << 5 | static_cast<uint8_t>(odr);
    res = writeReg(REG_GYRO_CONFIG0, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    #ifdef ENABLE_DEBUG
        printf("gyro ODR: %s%s%s%s.\ngyro FS: %s%s%s%s\n", COLOR_BRIGHT_MAGENTA, UNDERLINE, GYRO_ODR_str[static_cast<uint8_t>(odr)], COLOR_RESET, COLOR_BRIGHT_MAGENTA, UNDERLINE, GYRO_FS_str[static_cast<uint8_t>(fs)], COLOR_RESET);
    #endif

    _gyro_fs = static_cast<uint8_t>(fs);

    res = set_gyro_filter(UIF_bw, UIF_order, M2F_enable, NF_enable, NF_certenfreq, NF_certenfreq, NF_certenfreq, NF_bw, _AAF_enable, AAF_DELT, AAF_DELTSQR, AAF_BITSHIFT);
    if(res != StatusTypeDef::OK) return res;

    #ifdef ENABLE_DEBUG
        printf("%sfinish enable the gyro.\n", SUCCESS_TYPE);
    #endif

    _parent->icm_delay(1);

    return res;
}

ICM42688::General::StatusTypeDef ICM42688::General::enable_gyro(GYRO_ODR odr, GYRO_FS fs, FILTER_LEVEL filter_level)
{
    StatusTypeDef res = StatusTypeDef::OK;
    uint8_t reg_data = 0;

    #ifdef ENABLE_DEBUG
        printf("%sstarting enable the gyro.\n", PROCESS_TYPE);
    #endif

    #ifdef ENABLE_DEBUG
        printf("accel Mode: %s%s%s%s.\n", COLOR_BRIGHT_MAGENTA, UNDERLINE, FILTER_LEVEL_str[static_cast<uint8_t>(filter_level)], COLOR_RESET);
    #endif

    GYRO_UIF_LN UIF_bw;
    GYRO_ORDER_UIF UIF_order;
    ICM42688_SWITCH M2F_enable;
    ICM42688_SWITCH NF_enable;
    uint16_t NF_certenfreq = 0; // 使用工厂值
    NF_BW NF_bw = NF_BW::NF_BW_680HZ;
    ICM42688_SWITCH AAF_enable;
    uint8_t AAF_DELT = 0;
    uint16_t AAF_DELTSQR = 0;
    uint8_t AAF_BITSHIFT = 0;

    switch(filter_level)
    {
        case FILTER_LEVEL::VERY_HIGH:
            UIF_bw = GYRO_UIF_LN::GYRO_UIF_LN_400InODR_40;
            UIF_order = GYRO_ORDER_UIF::GYRO_ORDER_UIF_3;
            M2F_enable = ICM42688_SWITCH::OPEN;
            NF_enable = ICM42688_SWITCH::OPEN;
            AAF_enable = ICM42688_SWITCH::OPEN;
            res = find_AFF(odr, &AAF_DELT, &AAF_DELTSQR, &AAF_BITSHIFT);
            break;
        case FILTER_LEVEL::HIGH:
            UIF_bw = GYRO_UIF_LN::GYRO_UIF_LN_400InODR_16;
            UIF_order = GYRO_ORDER_UIF::GYRO_ORDER_UIF_3;
            M2F_enable = ICM42688_SWITCH::OPEN;
            NF_enable = ICM42688_SWITCH::OPEN;
            AAF_enable = ICM42688_SWITCH::OPEN;
            res = find_AFF(odr, &AAF_DELT, &AAF_DELTSQR, &AAF_BITSHIFT);
            break;
        case FILTER_LEVEL::MEDIUM:
            UIF_bw = GYRO_UIF_LN::GYRO_UIF_LN_400InODR_8;
            UIF_order = GYRO_ORDER_UIF::GYRO_ORDER_UIF_2;
            M2F_enable = ICM42688_SWITCH::OPEN;
            NF_enable = ICM42688_SWITCH::OPEN;
            AAF_enable = ICM42688_SWITCH::OPEN;
            res = find_AFF(odr, &AAF_DELT, &AAF_DELTSQR, &AAF_BITSHIFT);
            break;
        case FILTER_LEVEL::LOW:
            UIF_bw = GYRO_UIF_LN::GYRO_UIF_LN_ODR_2;
            UIF_order = GYRO_ORDER_UIF::GYRO_ORDER_UIF_1;
            M2F_enable = ICM42688_SWITCH::CLOSE;
            NF_enable = ICM42688_SWITCH::OPEN;
            AAF_enable = ICM42688_SWITCH::OPEN;
            res = find_AFF(odr, &AAF_DELT, &AAF_DELTSQR, &AAF_BITSHIFT);
            break;
        case FILTER_LEVEL::VERY_LOW:
            UIF_bw = GYRO_UIF_LN::GYRO_UIF_LN_ODR_2;
            UIF_order = GYRO_ORDER_UIF::GYRO_ORDER_UIF_1;
            M2F_enable = ICM42688_SWITCH::CLOSE;
            NF_enable = ICM42688_SWITCH::OPEN;
            AAF_enable = ICM42688_SWITCH::OPEN;
            res = find_AFF(odr, &AAF_DELT, &AAF_DELTSQR, &AAF_BITSHIFT);
            break;
        case FILTER_LEVEL::VERT_VERY_LOW:
            UIF_bw = GYRO_UIF_LN::GYRO_UIF_LN_ODR_2;
            UIF_order = GYRO_ORDER_UIF::GYRO_ORDER_UIF_1;
            M2F_enable = ICM42688_SWITCH::CLOSE;
            NF_enable = ICM42688_SWITCH::OPEN;
            AAF_enable = ICM42688_SWITCH::OPEN;
            res = find_AFF(odr, &AAF_DELT, &AAF_DELTSQR, &AAF_BITSHIFT);
            break;
        default:
            return StatusTypeDef::ERROR;
    }
    if(res != StatusTypeDef::OK)
    {
        #ifdef ENABLE_DEBUG
            printf("%sWe closed your AAF because AAF's range just begin at 42Hz and end at 3979Hz.We coulded figure out right parameters of AAF.\n", WARNING_TYPE);
        #endif
        AAF_enable = ICM42688_SWITCH::CLOSE;
    }

    reg_data = 0x01;
    res = writeReg(REG_REG_BANK_SEL, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    res = readReg(REG_SENSOR_CONFIG0, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;
    reg_data = reg_data & 0xC7 | 0x00;
    res = writeReg(REG_SENSOR_CONFIG0, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    #ifdef ENABLE_DEBUG
        printf("open gyro axis: %s%sX Y Z%s.\n", COLOR_BRIGHT_MAGENTA, UNDERLINE, COLOR_RESET);
    #endif

    reg_data = 0x00;
    res = writeReg(REG_REG_BANK_SEL, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    reg_data = static_cast<uint8_t>(fs) << 5 | static_cast<uint8_t>(odr);
    res = writeReg(REG_GYRO_CONFIG0, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    #ifdef ENABLE_DEBUG
        printf("gyro ODR: %s%s%s%s.\ngyro FS: %s%s%s%s\n", COLOR_BRIGHT_MAGENTA, UNDERLINE, GYRO_ODR_str[static_cast<uint8_t>(odr)], COLOR_RESET, COLOR_BRIGHT_MAGENTA, UNDERLINE, GYRO_FS_str[static_cast<uint8_t>(fs)], COLOR_RESET);
    #endif

    _gyro_fs = static_cast<uint8_t>(fs);

    res = set_gyro_filter(UIF_bw, UIF_order, M2F_enable, NF_enable, NF_certenfreq, NF_certenfreq, NF_certenfreq, NF_bw, AAF_enable, AAF_DELT, AAF_DELTSQR, AAF_BITSHIFT);
    if(res != StatusTypeDef::OK) return res;

    #ifdef ENABLE_DEBUG
        printf("%sfinish enable the gyro.\n", SUCCESS_TYPE);
    #endif

    _parent->icm_delay(1);

    return res;
}

ICM42688::General::StatusTypeDef ICM42688::General::disable_gyro(void)
{
    StatusTypeDef res = StatusTypeDef::OK;
    uint8_t reg_data = 0;

    #ifdef ENABLE_DEBUG
        printf("%sstarting disable the gyro.\n", PROCESS_TYPE);
    #endif

    reg_data = 0x01;
    res = writeReg(REG_REG_BANK_SEL, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    res = readReg(REG_SENSOR_CONFIG0, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;
    reg_data = reg_data & 0xC7 | 0x38;
    res = writeReg(REG_SENSOR_CONFIG0, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    #ifdef ENABLE_DEBUG
        printf("closed gyro axis: %s%sX Y Z%s.\n", COLOR_BRIGHT_MAGENTA, UNDERLINE, COLOR_RESET);
    #endif

    reg_data = 0x00;
    res = writeReg(REG_REG_BANK_SEL, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    res = readReg(REG_PWR_MGMT0, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;
    reg_data = reg_data & 0xF3 | 0x00;
    res = writeReg(REG_PWR_MGMT0, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    #ifdef ENABLE_DEBUG
        printf("closed gyro.\n");
    #endif

    reg_data = 0x01;
    res = writeReg(REG_REG_BANK_SEL, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    res = readReg(REG_GYRO_CONFIG_STATIC2, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;
    reg_data = reg_data & 0xFC | 0x03;
    res = writeReg(REG_GYRO_CONFIG_STATIC2, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res; 

    #ifdef ENABLE_DEBUG
        printf("closed gyro's NF and AAF.\n");
    #endif

    reg_data = 0x00;
    res = writeReg(REG_REG_BANK_SEL, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    #ifdef ENABLE_DEBUG
        printf("%sfinish disable the gyro.\n", SUCCESS_TYPE);
    #endif

    _parent->icm_delay(1);

    return res;
}

ICM42688::General::StatusTypeDef ICM42688::General::enable_temp(TEMP_DLPF_BW DLPF_bw)
{
    StatusTypeDef res = StatusTypeDef::OK;
    uint8_t reg_data = 0;

    #ifdef ENABLE_DEBUG
        printf("%sstarting enable the temp.\n", PROCESS_TYPE);
    #endif

    res = readReg(REG_PWR_MGMT0, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;
    reg_data = reg_data & 0xDF | 0x00;
    res = writeReg(REG_PWR_MGMT0, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    #ifdef ENABLE_DEBUG
        printf("open temp out.\n");
    #endif

    res = readReg(REG_GYRO_CONFIG1, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;
    reg_data = reg_data & 0x1F | static_cast<uint8_t>(DLPF_bw) << 5;
    res = writeReg(REG_GYRO_CONFIG1, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    #ifdef ENABLE_DEBUG
        printf("temp DLPF BW: %s%s%s%s.\ntemp DLPF BW set success.\n", COLOR_BRIGHT_MAGENTA, UNDERLINE, TEMP_DLPF_BW_str[static_cast<uint8_t>(DLPF_bw)], COLOR_RESET);
    #endif

    #ifdef ENABLE_DEBUG
        printf("%sfinish enable the temp.\n", SUCCESS_TYPE);
    #endif

    _parent->icm_delay(1);

    return res;
}

ICM42688::General::StatusTypeDef ICM42688::General::enable_temp(void)
{
    StatusTypeDef res = StatusTypeDef::OK;
    uint8_t reg_data = 0;

    #ifdef ENABLE_DEBUG
        printf("%sstarting enable the temp.\n", PROCESS_TYPE);
    #endif

    res = readReg(REG_PWR_MGMT0, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;
    reg_data = reg_data & 0xDF | 0x00;
    res = writeReg(REG_PWR_MGMT0, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    #ifdef ENABLE_DEBUG
        printf("open temp out.\n");
    #endif

    TEMP_DLPF_BW DLPF_bw = TEMP_DLPF_BW::TEMP_DLPF_BW_170Hz;

    res = readReg(REG_GYRO_CONFIG1, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;
    reg_data = reg_data & 0x1F | static_cast<uint8_t>(DLPF_bw) << 5;
    res = writeReg(REG_GYRO_CONFIG1, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    #ifdef ENABLE_DEBUG
        printf("temp DLPF BW: %s%s%s%s.\ntemp DLPF BW set success.\n", COLOR_BRIGHT_MAGENTA, UNDERLINE, TEMP_DLPF_BW_str[static_cast<uint8_t>(DLPF_bw)], COLOR_RESET);
    #endif

    #ifdef ENABLE_DEBUG
        printf("%sfinish enable the temp.\n", SUCCESS_TYPE);
    #endif

    _parent->icm_delay(1);

    return res;  
}

ICM42688::General::StatusTypeDef ICM42688::General::disable_temp(void)
{
    StatusTypeDef res = StatusTypeDef::OK;
    uint8_t reg_data = 0;

    res = readReg(REG_PWR_MGMT0, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;
    reg_data = reg_data & 0xDF | 0x01;
    res = writeReg(REG_PWR_MGMT0, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    _parent->icm_delay(1);

    return res; 
}

ICM42688::General::StatusTypeDef ICM42688::General::enable_tmst(TMST_RESOL resol)
{
    StatusTypeDef res = StatusTypeDef::OK;
    uint8_t reg_data = 0;

    #ifdef ENABLE_DEBUG
        printf("%sstarting enable the tmst.\n", PROCESS_TYPE);
    #endif

    reg_data = static_cast<uint8_t>(resol) << 3 | 0x13;
    res = writeReg(REG_TMST_CONFIG, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    #ifdef ENABLE_DEBUG
        printf("time stamp resolution: %s%s%d%s us.\n", COLOR_BRIGHT_MAGENTA, UNDERLINE,((resol == TMST_RESOL::TMST_RESOL_16us) ? 16 : 1), COLOR_RESET);
    #endif

    _tmst_res = ((resol == TMST_RESOL::TMST_RESOL_16us) ? 16 : 1);

    #ifdef ENABLE_DEBUG
        printf("%sfinish enable the tmst.\n", SUCCESS_TYPE);
    #endif

    return res; 
}

ICM42688::General::StatusTypeDef ICM42688::General::enable_tmst(void)
{
    StatusTypeDef res = StatusTypeDef::OK;
    uint8_t reg_data = 0;

    #ifdef ENABLE_DEBUG
        printf("%sstarting enable the tmst.\n", PROCESS_TYPE);
    #endif

    reg_data = 0x13;
    res = writeReg(REG_TMST_CONFIG, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    #ifdef ENABLE_DEBUG
        printf("time stamp resolution: %s%s1%s us.\n", COLOR_BRIGHT_MAGENTA, UNDERLINE, COLOR_RESET);
    #endif

    _tmst_res = 16;

    #ifdef ENABLE_DEBUG
        printf("%sfinish enable the tmst.\n", SUCCESS_TYPE);
    #endif

    return res; 
}

ICM42688::General::StatusTypeDef ICM42688::General::disable_tmst(void)
{
    StatusTypeDef res = StatusTypeDef::OK;
    uint8_t reg_data = 0;

    reg_data = 0x00;
    res = writeReg(REG_TMST_CONFIG, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    return res; 
}

ICM42688::General::StatusTypeDef ICM42688::General::read_data(void)
{
    StatusTypeDef res = StatusTypeDef::OK;
    uint8_t reg_data[3] = {0};
    uint8_t flag;

    res = readReg(REG_PWR_MGMT0, reg_data, 1);
    if(res != StatusTypeDef::OK) return res;
    flag = reg_data[0];
    if((flag & 0x20) == 0)
    {
        res = readReg(REG_TEMP_DATA1, reg_data, 2);
        if(res != StatusTypeDef::OK) return res;

        _temp_raw = reg_data[0] << 8 | reg_data[1];
    }

    reg_data[0] = 0x01;
    res = writeReg(REG_REG_BANK_SEL, reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    res = readReg(REG_SENSOR_CONFIG0, reg_data, 1);
    if(res != StatusTypeDef::OK) return res;
    flag = reg_data[0];

    reg_data[0] = 0x00;
    res = writeReg(REG_REG_BANK_SEL, reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    if((flag & 0x20) == 0)
    {
        res = readReg(REG_GYRO_DATA_Z1, reg_data, 2);
        if(res != StatusTypeDef::OK) return res;

        _gz_raw = reg_data[0] << 8 | reg_data[1];
    }

    if((flag & 0x10) == 0)
    {
        res = readReg(REG_GYRO_DATA_Y1, reg_data, 2);
        if(res != StatusTypeDef::OK) return res;

        _gy_raw = reg_data[0] << 8 | reg_data[1];
    }

    if((flag & 0x08) == 0)
    {
        res = readReg(REG_GYRO_DATA_X1, reg_data, 2);
        if(res != StatusTypeDef::OK) return res;

        _gx_raw = reg_data[0] << 8 | reg_data[1];
    }

    if((flag & 0x04) == 0)
    {
        res = readReg(REG_ACCEL_DATA_Z1, reg_data, 2);
        if(res != StatusTypeDef::OK) return res;

        _az_raw = reg_data[0] << 8 | reg_data[1];
    }

    if((flag & 0x02) == 0)
    {
        res = readReg(REG_ACCEL_DATA_Y1, reg_data, 2);
        if(res != StatusTypeDef::OK) return res;

        _ay_raw = reg_data[0] << 8 | reg_data[1];
    }

    if((flag & 0x01) == 0)
    {
        res = readReg(REG_ACCEL_DATA_X1, reg_data, 2);
        if(res != StatusTypeDef::OK) return res;

        _ax_raw = reg_data[0] << 8 | reg_data[1];
    }

    res = readReg(REG_TMST_CONFIG, reg_data, 1);
    if(res != StatusTypeDef::OK) return res;
    flag = reg_data[0];

    if((flag & 0x01) == 1)
    {
        res = readReg(REG_TMST_FSYNCH, reg_data, 2);
        if(res != StatusTypeDef::OK) return res;

        _tmst_fsync_raw = reg_data[0] << 8 | reg_data[1];

        if((flag & 0x10) == 0x10)
        {
            res = readReg(REG_SIGNAL_PATH_RESET, reg_data, 1);
            if(res != StatusTypeDef::OK) return res;
            reg_data[0] = (reg_data[0] & 0xFB) | 0x04;
            res = writeReg(REG_SIGNAL_PATH_RESET, reg_data, 1);
            if(res != StatusTypeDef::OK) return res;

            reg_data[0] = 0x01;
            res = writeReg(REG_REG_BANK_SEL, reg_data, 1);
            if(res != StatusTypeDef::OK) return res;

            res = readReg(REG_TMSTVAL0, reg_data, 3);
            if(res != StatusTypeDef::OK) return res;

            reg_data[0] = 0x00;
            res = writeReg(REG_REG_BANK_SEL, reg_data, 1);
            if(res != StatusTypeDef::OK) return res;

            _tmst_raw = ((reg_data[2] & 0x0F) << 16) | (reg_data[1] << 8) | reg_data[0];
        }
    }

    return res;
}

float ICM42688::General::get_ax(void)
{
    _ax = (float)_ax_raw / ACCLE_FS_SENSITIVITY[_acc_fs] * g + _acc_offset_x;

    return _ax;
}

float ICM42688::General::get_ay(void)
{
    _ay = (float)_ay_raw / ACCLE_FS_SENSITIVITY[_acc_fs] * g + _acc_offset_y;
    return _ay;
}

float ICM42688::General::get_az(void)
{
    _az = (float)_az_raw / ACCLE_FS_SENSITIVITY[_acc_fs] * g + _acc_offset_z;
    return _az;
}

float ICM42688::General::get_gx(void)
{
    _gx = (float)_gx_raw / GYRO_FS_SENSITIVITY[_gyro_fs] + _gyro_offset_x;
    return _gx;
}

float ICM42688::General::get_gy(void)
{
    _gy = (float)_gy_raw / GYRO_FS_SENSITIVITY[_gyro_fs] + _gyro_offset_y;
    return _gy;
}

float ICM42688::General::get_gz(void)
{
    _gz = (float)_gz_raw / GYRO_FS_SENSITIVITY[_gyro_fs] + _gyro_offset_z;
    return _gz;
}

float ICM42688::General::get_temp(void)
{
    _temp = (float)_temp_raw / 132.48f + 25;
    return _temp;
}

float ICM42688::General::get_tmst_fsync(void)
{
    _tmst_fsync = (float)_tmst_fsync_raw / 1000;
    return _tmst_fsync;
}

float ICM42688::General::get_tmst(void)
{
    _tmst = _tmst_raw / 1000;
    return _tmst;
}

//===========================================================================================================

//###########################################################################################################

ICM42688::FIFO::FIFO(ICM42688* parent)
    : _parent(parent)
{
}

ICM42688::FIFO::StatusTypeDef ICM42688::FIFO::readReg(uint8_t reg, uint8_t* pdata, uint16_t len)
{
    ICM42688::ICM42688_StatusTypeDef res = ICM42688::ICM42688_StatusTypeDef::OK;

    res = _parent->readReg(reg, pdata, len);
    if(res != ICM42688::ICM42688_StatusTypeDef::OK)
        return ICM42688::FIFO::StatusTypeDef::ERROR;
    else
        return ICM42688::FIFO::StatusTypeDef::OK;
}

ICM42688::FIFO::StatusTypeDef ICM42688::FIFO::writeReg(uint8_t reg, uint8_t* pdata, uint16_t len)
{
    ICM42688::ICM42688_StatusTypeDef res = ICM42688::ICM42688_StatusTypeDef::OK;

    res = _parent->writeReg(reg, pdata, len);
    if(res != ICM42688::ICM42688_StatusTypeDef::OK)
        return ICM42688::FIFO::StatusTypeDef::ERROR;
    else
        return ICM42688::FIFO::StatusTypeDef::OK;
}

//-----------------------------------------------------------------------------------------------------------

ICM42688::FIFO::StatusTypeDef ICM42688::FIFO::enable(FIFO_MODE_1 fifo_mode_1, FIFO_MODE_2 fifo_mode_2, FIFO_MODE_3 fifo_mode_3, ICM42688_SWITCH acc_enable, ICM42688_SWITCH gyro_enable, ICM42688_SWITCH temp_enable, ICM42688_SWITCH timestamp_enable)
{
    StatusTypeDef res = StatusTypeDef::OK;
    uint8_t reg_data = 0;


    return res;
}

ICM42688::FIFO::StatusTypeDef ICM42688::FIFO::disable(void)
{
    StatusTypeDef res = StatusTypeDef::OK;
    uint8_t reg_data = 0;


    return res;
}



//===========================================================================================================

//###########################################################################################################

ICM42688::INT::INT(ICM42688* parent)
    : _parent(parent)
{
    // 初始化所有事件
    for (size_t i = 0; i < static_cast<size_t>(IntEvent::NUM_EVENTS); ++i) {
        _events[i] = EventInfo();  // 调用默认构造函数
    }
    
    // 初始化中断状态
    _UI_FSYNC_INT_state = false;
    _PLL_RDY_INT_state = false;
    _RESET_DONE_INT_state = false;
    _DATA_RDY_INT_state = false;
    _FIFO_THS_INT_state = false;
    _FIFO_FULL_INT_state = false;
    _UI_AGC_RDY_INT_state = false;
    _I3C_ERROR_INT_state = false;
    _SMD_INT_state = false;
    _WOM_Z_INT_state = false;
    _WOM_Y_INT_state = false;
    _WOM_X_INT_state = false;
    _STEP_DET_INT_state = false;
    _STEP_CNT_OF_INT_state = false;
    _TILT_DET_INT_state = false;
    _WAKE_DET_INT_state = false;
    _SLEEP_DET_INT_state = false;
    _TAP_DET_INT_state = false;
}

ICM42688::INT::StatusTypeDef ICM42688::INT::readReg(uint8_t reg, uint8_t* pdata, uint16_t len)
{
    ICM42688::ICM42688_StatusTypeDef res = ICM42688::ICM42688_StatusTypeDef::OK;

    res = _parent->readReg(reg, pdata, len);
    if(res != ICM42688::ICM42688_StatusTypeDef::OK)
        return ICM42688::INT::StatusTypeDef::ERROR;
    else
        return ICM42688::INT::StatusTypeDef::OK;
}

ICM42688::INT::StatusTypeDef ICM42688::INT::writeReg(uint8_t reg, uint8_t* pdata, uint16_t len)
{
    ICM42688::ICM42688_StatusTypeDef res = ICM42688::ICM42688_StatusTypeDef::OK;

    res = _parent->writeReg(reg, pdata, len);
    if(res != ICM42688::ICM42688_StatusTypeDef::OK)
        return ICM42688::INT::StatusTypeDef::ERROR;
    else
        return ICM42688::INT::StatusTypeDef::OK;
}

ICM42688::INT::StatusTypeDef ICM42688::INT::set_int_source(INT_NUM int_num, IntEvent event, bool enable)
{
    StatusTypeDef res = StatusTypeDef::OK;
    uint8_t reg_data = 0;
    uint8_t reg_addr = 0;
    uint8_t bit_pos = 0;
    uint8_t bank = 0;

    switch(event)
    {
        case IntEvent::UI_FSYNC_INT:
            if(int_num == INT_NUM::INT1) reg_addr = REG_INT_SOURCE0;
            else if(int_num == INT_NUM::INT2) reg_addr = REG_INT_SOURCE3;
            bit_pos = 6;
            bank = 0;
            break;
        case IntEvent::PLL_RDY_INT:
            if(int_num == INT_NUM::INT1) reg_addr = REG_INT_SOURCE0;
            else if(int_num == INT_NUM::INT2) reg_addr = REG_INT_SOURCE3;
            bit_pos = 5;
            bank = 0;
            break;
        case IntEvent::RESET_DONE_INT:
            if(int_num == INT_NUM::INT1) reg_addr = REG_INT_SOURCE0;
            else if(int_num == INT_NUM::INT2) reg_addr = REG_INT_SOURCE3;
            bit_pos = 4;
            bank = 0;
            break;
        case IntEvent::DATA_RDY_INT:
            if(int_num == INT_NUM::INT1) reg_addr = REG_INT_SOURCE0;
            else if(int_num == INT_NUM::INT2) reg_addr = REG_INT_SOURCE3;
            bit_pos = 3;
            bank = 0;
            break;
        case IntEvent::FIFO_THS_INT:
            if(int_num == INT_NUM::INT1) reg_addr = REG_INT_SOURCE0;
            else if(int_num == INT_NUM::INT2) reg_addr = REG_INT_SOURCE3;
            bit_pos = 2;
            bank = 0;
            break;
        case IntEvent::FIFO_FULL_INT:
            if(int_num == INT_NUM::INT1) reg_addr = REG_INT_SOURCE0;
            else if(int_num == INT_NUM::INT2) reg_addr = REG_INT_SOURCE3;
            bit_pos = 1;
            bank = 0;
            break;
        case IntEvent::UI_AGC_RDY_INT:
            if(int_num == INT_NUM::INT1) reg_addr = REG_INT_SOURCE0;
            else if(int_num == INT_NUM::INT2) reg_addr = REG_INT_SOURCE3;
            bit_pos = 0;
            bank = 0;
            break;
        case IntEvent::I3C_ERROR_INT:
            if(int_num == INT_NUM::INT1) reg_addr = REG_INT_SOURCE1;
            else if(int_num == INT_NUM::INT2) reg_addr = REG_INT_SOURCE4;
            bit_pos = 6;
            bank = 0;
            break;
        case IntEvent::SMD_INT:
            if(int_num == INT_NUM::INT1) reg_addr = REG_INT_SOURCE1;
            else if(int_num == INT_NUM::INT2) reg_addr = REG_INT_SOURCE4;
            bit_pos = 3;
            bank = 0;
            break;
        case IntEvent::WOM_Z_INT:
            if(int_num == INT_NUM::INT1) reg_addr = REG_INT_SOURCE1;
            else if(int_num == INT_NUM::INT2) reg_addr = REG_INT_SOURCE4;
            bit_pos = 2;
            bank = 0;
            break;
        case IntEvent::WOM_Y_INT:
            if(int_num == INT_NUM::INT1) reg_addr = REG_INT_SOURCE1;
            else if(int_num == INT_NUM::INT2) reg_addr = REG_INT_SOURCE4;
            bit_pos = 1;
            bank = 0;
            break;
        case IntEvent::WOM_X_INT:
            if(int_num == INT_NUM::INT1) reg_addr = REG_INT_SOURCE1;
            else if(int_num == INT_NUM::INT2) reg_addr = REG_INT_SOURCE4;
            bit_pos = 0;
            bank = 4;
            break;
        case IntEvent::STEP_DET_INT:
            if(int_num == INT_NUM::INT1) reg_addr = REG_INT_SOURCE6;
            else if(int_num == INT_NUM::INT2) reg_addr = REG_INT_SOURCE7;
            bit_pos = 5;
            bank = 4;
            break;
        case IntEvent::STEP_CNT_OF_INT:
            if(int_num == INT_NUM::INT1) reg_addr = REG_INT_SOURCE6;
            else if(int_num == INT_NUM::INT2) reg_addr = REG_INT_SOURCE7;
            bit_pos = 4;
            bank = 4;
            break;
        case IntEvent::TILT_DET_INT:
            if(int_num == INT_NUM::INT1) reg_addr = REG_INT_SOURCE6;
            else if(int_num == INT_NUM::INT2) reg_addr = REG_INT_SOURCE7;
            bit_pos = 3;
            bank = 4;
            break;
        case IntEvent::WAKE_DET_INT:
            if(int_num == INT_NUM::INT1) reg_addr = REG_INT_SOURCE6;
            else if(int_num == INT_NUM::INT2) reg_addr = REG_INT_SOURCE7;
            bit_pos = 2;
            bank = 4;
            break;
        case IntEvent::SLEEP_DET_INT:
            if(int_num == INT_NUM::INT1) reg_addr = REG_INT_SOURCE6;
            else if(int_num == INT_NUM::INT2) reg_addr = REG_INT_SOURCE7;
            bit_pos = 1;
            bank = 4;
            break; 
        case IntEvent::TAP_DET_INT:
            if(int_num == INT_NUM::INT1) reg_addr = REG_INT_SOURCE6;
            else if(int_num == INT_NUM::INT2) reg_addr = REG_INT_SOURCE7;  
            bit_pos = 0;
            bank = 4;
            break;
        default:
            return StatusTypeDef::ERROR;
    }

    reg_data = bank;
    res = writeReg(REG_REG_BANK_SEL, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    res = readReg(reg_addr, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;
    reg_data = reg_data & (0xFF & ~(1 << bit_pos)) | (enable ? 1 : 0) << bit_pos;
    res = writeReg(reg_addr, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    reg_data = 0x00;
    res = writeReg(REG_REG_BANK_SEL, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    return res;
}

ICM42688::INT::StatusTypeDef ICM42688::INT::get_int_state(void)
{
    // 读取中断状态寄存器
    StatusTypeDef res = StatusTypeDef::OK;
    uint8_t int_status1, int_status2, int_status3;
    
    res = readReg(REG_INT_STATUS, &int_status1, 1);
    if (res != StatusTypeDef::OK) return res;
    
    res = readReg(REG_INT_STATUS2, &int_status2, 1);
    if (res != StatusTypeDef::OK) return res;
    
    res = readReg(REG_INT_STATUS3, &int_status3, 1);
    if (res != StatusTypeDef::OK) return res;
    
    // 更新中断事件状态
    _UI_FSYNC_INT_state = (int_status1 & 0x40) != 0;
    _PLL_RDY_INT_state = (int_status1 & 0x20) != 0;
    _RESET_DONE_INT_state = (int_status1 & 0x10) != 0;
    _DATA_RDY_INT_state = (int_status1 & 0x08) != 0;
    _FIFO_THS_INT_state = (int_status1 & 0x04) != 0;
    _FIFO_FULL_INT_state = (int_status1 & 0x02) != 0;
    _UI_AGC_RDY_INT_state = (int_status1 & 0x01) != 0;
    _I3C_ERROR_INT_state = (int_status2 & 0x40) != 0;
    _SMD_INT_state = (int_status2 & 0x08) != 0;
    _WOM_Z_INT_state = (int_status2 & 0x04) != 0;
    _WOM_Y_INT_state = (int_status2 & 0x02) != 0;
    _WOM_X_INT_state = (int_status2 & 0x01) != 0;
    _STEP_DET_INT_state = (int_status3 & 0x20) != 0;
    _STEP_CNT_OF_INT_state = (int_status3 & 0x10) != 0;
    _TILT_DET_INT_state = (int_status3 & 0x08) != 0;
    _WAKE_DET_INT_state = (int_status3 & 0x04) != 0;
    _SLEEP_DET_INT_state = (int_status3 & 0x02) != 0;
    _TAP_DET_INT_state = (int_status3 & 0x01) != 0;

    return res;
}

//-----------------------------------------------------------------------------------------------------------

ICM42688::ICM42688_StatusTypeDef ICM42688::INT::begin(void)
{
    StatusTypeDef res = StatusTypeDef::OK;
    uint8_t reg_data = 0;

    #ifdef ENABLE_DEBUG
        printf("%sstarting initial the INT.\n", PROCESS_TYPE);
    #endif

    res = readReg(REG_INT_CONFIG1, &reg_data, 1);
    if(res != StatusTypeDef::OK) return ICM42688::ICM42688_StatusTypeDef::ERROR;
    reg_data = reg_data & 0xF7 | 0x01;
    res = writeReg(REG_INT_CONFIG1, &reg_data, 1);
    if(res != StatusTypeDef::OK) return ICM42688::ICM42688_StatusTypeDef::ERROR;

    _parent->icm_delay(10);

    res = readReg(REG_INT_CONFIG1, &reg_data, 1);
    if(res != StatusTypeDef::OK) return ICM42688::ICM42688_StatusTypeDef::ERROR;
    reg_data = reg_data & 0xF7 | 0x00;
    res = writeReg(REG_INT_CONFIG1, &reg_data, 1);
    if(res != StatusTypeDef::OK) return ICM42688::ICM42688_StatusTypeDef::ERROR;

    #ifdef ENABLE_DEBUG
        printf("INT reset OK.\n");
    #endif

    reg_data = 0x00;
    res = writeReg(REG_INT_CONFIG1, &reg_data, 1);
    if(res != StatusTypeDef::OK) return ICM42688::ICM42688_StatusTypeDef::ERROR;

    #ifdef ENABLE_DEBUG
        printf("Interrupt pulse duration is 100µs.\nThe interrupt de-assertion duration is set to a minimum of 100µs.\n");
    #endif

    res = set_int_latchClear_mode();
    if(res != StatusTypeDef::OK) return ICM42688::ICM42688_StatusTypeDef::ERROR;

    #ifdef ENABLE_DEBUG
        printf("%sfinish initial the INT.\n", SUCCESS_TYPE);
    #endif

    return ICM42688::ICM42688_StatusTypeDef::OK;
}

ICM42688::INT::StatusTypeDef ICM42688::INT::set_int_latchClear_mode(CLEAR_MODE DRDY_clear_mode, CLEAR_MODE FIFO_THS_clear_mode, CLEAR_MODE FIFO_FULL_clear_mode)
{
    StatusTypeDef res = StatusTypeDef::OK;
    uint8_t reg_data = 0;

    reg_data = static_cast<uint8_t>(DRDY_clear_mode) << 4 | static_cast<uint8_t>(FIFO_THS_clear_mode) << 2 | static_cast<uint8_t>(FIFO_FULL_clear_mode);
    res = writeReg(REG_INT_CONFIG1, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    #ifdef ENABLE_DEBUG
        printf("DRDY_clear_mode: %s%s%s%s.\nFIFO_THS_clear_mode: %s%s%s%s.\nFIFO_FULL_clear_mode: %s%s%s%s.\nClear mode set success.\n", 
            COLOR_BRIGHT_MAGENTA, UNDERLINE, CLEAR_MODE_str[static_cast<uint8_t>(DRDY_clear_mode)], COLOR_RESET, 
            COLOR_BRIGHT_MAGENTA, UNDERLINE, CLEAR_MODE_str[static_cast<uint8_t>(FIFO_THS_clear_mode)], COLOR_RESET, 
            COLOR_BRIGHT_MAGENTA, UNDERLINE, CLEAR_MODE_str[static_cast<uint8_t>(FIFO_FULL_clear_mode)], COLOR_RESET);
    #endif

    return res;
}

ICM42688::INT::StatusTypeDef ICM42688::INT::set_int1_pin_cfg(LEVEL level, DRIVE drive, MODE mode)
{
    StatusTypeDef res = StatusTypeDef::OK;
    uint8_t reg_data = 0;

    #ifdef ENABLE_DEBUG
        printf("%sstarting set INT1.\n", PROCESS_TYPE);
    #endif

    #ifdef ENABLE_DEBUG
        printf("INT1_Pin: %s%s%s%s%s.\n", COLOR_BRIGHT_MAGENTA, UNDERLINE, _parent->get_GPIO_PortName(int1_port), _parent->get_GPIO_PinName(int1_pin), COLOR_RESET);
    #endif

    res = readReg(REG_INT_CONFIG, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;
    reg_data = reg_data & 0xF8 | static_cast<uint8_t>(level) << 2 | static_cast<uint8_t>(drive) << 1 | static_cast<uint8_t>(mode);
    res = writeReg(REG_INT_CONFIG, &reg_data, 1);
    if(res != StatusTypeDef::OK) return res;

    #ifdef ENABLE_DEBUG
        printf("level: %s%s%s%s.  drive: %s%s%s%s.  mode: %s%s%s%s.\nINT1 PIN MODE set success.\n", 
            COLOR_BRIGHT_MAGENTA, UNDERLINE, LEVEL_str[static_cast<uint8_t>(level)], COLOR_RESET, 
            COLOR_BRIGHT_MAGENTA, UNDERLINE, DRIVE_str[static_cast<uint8_t>(drive)], COLOR_RESET, 
            COLOR_BRIGHT_MAGENTA, UNDERLINE, MODE_str[static_cast<uint8_t>(mode)], COLOR_RESET);
    #endif

    #ifdef ENABLE_DEBUG
        printf("%sfinish set INT1.\n", SUCCESS_TYPE);
    #endif

    return res;
}

// ICM42688::INT::StatusTypeDef ICM42688::INT::set_int2_pin_cfg(GPIO_TypeDef* int2_port, uint16_t int2_pin, LEVEL level, DRIVE drive, MODE mode)
// {
//     StatusTypeDef res = StatusTypeDef::OK;
//     uint8_t reg_data = 0;

//     #ifdef ENABLE_DEBUG
//         printf("%sstarting set INT2.\n", PROCESS_TYPE);
//     #endif

//     _int2_port = int2_port;
//     _int2_pin = int2_pin;

//     #ifdef ENABLE_DEBUG
//         printf("INT2_Pin: %s%s%s%s%s.\n", COLOR_BRIGHT_MAGENTA, UNDERLINE, _parent->get_GPIO_PortName(int2_port), _parent->get_GPIO_PinName(int2_pin), COLOR_RESET);
//     #endif

//     res = readReg(REG_INT_CONFIG, &reg_data, 1);
//     if(res != StatusTypeDef::OK) return res;
//     reg_data = reg_data & 0xC7 | static_cast<uint8_t>(level) << 5 | static_cast<uint8_t>(drive) << 4 | static_cast<uint8_t>(mode) << 3;
//     res = writeReg(REG_INT_CONFIG, &reg_data, 1);
//     if(res != StatusTypeDef::OK) return res;

//     #ifdef ENABLE_DEBUG
//         printf("level: %s%s%s%s.  drive: %s%s%s%s.  mode: %s%s%s%s.\nINT2 PIN MODE set success.\n", 
//             COLOR_BRIGHT_MAGENTA, UNDERLINE, LEVEL_str[static_cast<uint8_t>(level)], COLOR_RESET, 
//             COLOR_BRIGHT_MAGENTA, UNDERLINE, DRIVE_str[static_cast<uint8_t>(drive)], COLOR_RESET, 
//             COLOR_BRIGHT_MAGENTA, UNDERLINE, MODE_str[static_cast<uint8_t>(mode)], COLOR_RESET);
//     #endif

//     #ifdef ENABLE_DEBUG
//         printf("%sfinish set INT2.\n", SUCCESS_TYPE);
//     #endif

//     return res;
// }

ICM42688::INT::StatusTypeDef ICM42688::INT::register_event(IntEvent event, INT_NUM int_num, IntCallback callback, void* context, EVENT_PRIORITY priority)
{
    StatusTypeDef res = StatusTypeDef::OK;

    uint8_t index = static_cast<uint8_t>(event);
    if (index >= static_cast<uint8_t>(IntEvent::NUM_EVENTS))
        return StatusTypeDef::ERROR;

    if(callback == nullptr)
        switch(event)
        {
            case IntEvent::UI_FSYNC_INT: callback = &UI_FSYNC_callback;
                break;
            case IntEvent::PLL_RDY_INT: callback = &PLL_RDY_callback;
                break;
            case IntEvent::RESET_DONE_INT: callback = &RESET_DONE_callback;
                break;
            case IntEvent::DATA_RDY_INT: callback = &DATA_RDY_callback;
                break;
            case IntEvent::FIFO_THS_INT: callback = &FIFO_THS_callback;
                break;
            case IntEvent::FIFO_FULL_INT: callback = &FIFO_FULL_callback;
                break;
            case IntEvent::UI_AGC_RDY_INT: callback = &UI_AGC_RDY_callback;
                break;
            case IntEvent::I3C_ERROR_INT: callback = &I3C_ERROR_callback;
                break;
            case IntEvent::SMD_INT: callback = &SMD_callback;
                break;
            case IntEvent::WOM_Z_INT: callback = &WOM_Z_callback;
                break;
            case IntEvent::WOM_Y_INT: callback = &WOM_Y_callback;
                break;
            case IntEvent::WOM_X_INT: callback = &WOM_X_callback;
                break;
            case IntEvent::STEP_DET_INT: callback = &STEP_DET_callback;
                break;
            case IntEvent::STEP_CNT_OF_INT: callback = &STEP_CNT_OF_callback;
                break;
            case IntEvent::TILT_DET_INT: callback = &TILT_DET_callback;
                break;
            case IntEvent::WAKE_DET_INT: callback = &WAKE_DET_callback;
                break;
            case IntEvent::SLEEP_DET_INT: callback = &SLEEP_DET_callback;
                break; 
            case IntEvent::TAP_DET_INT: callback = &TAP_DET_callback;
                break;
            default:
                return StatusTypeDef::ERROR;
        }
    
    EventInfo& info = _events[index];
    
    // 如果已经注册，先注销
    if(int_num == INT_NUM::BOTH)
    {
        if(info.registered_INT1 == true && info.registered_INT2 == true)
            unregister_event(event, INT_NUM::BOTH);
        else if(info.registered_INT1 == true)
            unregister_event(event, INT_NUM::INT1);
        else if(info.registered_INT2 == true)
            unregister_event(event, INT_NUM::INT2);
    }
    else if(int_num == INT_NUM::INT1 && info.registered_INT1 == true)
        unregister_event(event, int_num);
    else if(int_num == INT_NUM::INT2 && info.registered_INT2 == true)
        unregister_event(event, int_num);
    
    // 注册新事件
    if(int_num == INT_NUM::BOTH)
    {
        info.callback_INT1 = callback;
        info.context_INT1 = context;
        info.priority_INT1 = priority;
        info.registered_INT1 = true;
        info.enabled_INT1 = true;

        info.callback_INT2 = callback;
        info.context_INT2 = context;
        info.priority_INT2 = priority;
        info.registered_INT2 = true;
        info.enabled_INT2 = true;
    }
    else if(int_num == INT_NUM::INT1)
    {
        info.callback_INT1 = callback;
        info.context_INT1 = context;
        info.priority_INT1 = priority;
        info.registered_INT1 = true;
        info.enabled_INT1 = true;
    }
    else if(int_num == INT_NUM::INT2)
    {
        info.callback_INT2 = callback;
        info.context_INT2 = context;
        info.priority_INT2 = priority;
        info.registered_INT2 = true;
        info.enabled_INT2 = true;
    }
    
    // 启用芯片中断
    res = set_int_source(int_num, event, true);
    
    return res;
}

ICM42688::INT::StatusTypeDef ICM42688::INT::unregister_event(IntEvent event, INT_NUM int_num)
{
    StatusTypeDef res = StatusTypeDef::OK;

    uint8_t index = static_cast<uint8_t>(event);
    if (index >= static_cast<uint8_t>(IntEvent::NUM_EVENTS))
        return StatusTypeDef::ERROR;
    
    EventInfo& info = _events[index];
    
    if(int_num == INT_NUM::BOTH && info.registered_INT1 == false && info.registered_INT2 == false)
        return StatusTypeDef::ERROR;
    else if(int_num == INT_NUM::INT1 && info.registered_INT1 == false)
        return StatusTypeDef::ERROR;
    else if(int_num == INT_NUM::INT2 && info.registered_INT2 == false)
        return StatusTypeDef::ERROR;
    
    // 禁用芯片中断
    res = set_int_source(int_num, event, false);
    
    if(int_num == INT_NUM::BOTH)
    {
        info.callback_INT1 = nullptr;
        info.context_INT1 = nullptr;
        info.priority_INT1 = EVENT_PRIORITY::PRIORITY_NORMAL;
        info.registered_INT1 = false;
        info.enabled_INT1 = false;

        info.callback_INT2 = nullptr;
        info.context_INT2 = nullptr;
        info.priority_INT2 = EVENT_PRIORITY::PRIORITY_NORMAL;
        info.registered_INT2 = false;
        info.enabled_INT2 = false;
    }
    else if(int_num == INT_NUM::INT1)
    {
        info.callback_INT1 = nullptr;
        info.context_INT1 = nullptr;
        info.priority_INT1 = EVENT_PRIORITY::PRIORITY_NORMAL;
        info.registered_INT1 = false;
        info.enabled_INT1 = false;
    }
    else if(int_num == INT_NUM::INT2)
    {
        info.callback_INT2 = nullptr;
        info.context_INT2 = nullptr;
        info.priority_INT2 = EVENT_PRIORITY::PRIORITY_NORMAL;
        info.registered_INT2 = false;
        info.enabled_INT2 = false;
    }
    
    return res;
}

ICM42688::INT::StatusTypeDef ICM42688::INT::set_event_priority(IntEvent event, INT_NUM int_num, EVENT_PRIORITY priority)
{
    StatusTypeDef res = StatusTypeDef::OK;

    uint8_t index = static_cast<uint8_t>(event);
    if (index >= static_cast<uint8_t>(IntEvent::NUM_EVENTS))
        return StatusTypeDef::ERROR;
    
    EventInfo& info = _events[index];
    
    if(int_num == INT_NUM::BOTH && info.registered_INT1 == false && info.registered_INT2 == false)
        return StatusTypeDef::ERROR;
    else if(int_num == INT_NUM::INT1 && info.registered_INT1 == false)
        return StatusTypeDef::ERROR;
    else if(int_num == INT_NUM::INT2 && info.registered_INT2 == false)
        return StatusTypeDef::ERROR;
    
    if(int_num == INT_NUM::BOTH)
    {
        info.priority_INT1 = priority;
        info.priority_INT2 = priority;
    }
    else if(int_num == INT_NUM::INT1)
    {
        info.priority_INT1 = priority;
    }
    else if(int_num == INT_NUM::INT2)
    {
        info.priority_INT2 = priority;
    }
    
    return res;
}

ICM42688::INT::StatusTypeDef ICM42688::INT::set_event_enable(IntEvent event, INT_NUM int_num, bool enable)
{
    StatusTypeDef res = StatusTypeDef::OK;

    uint8_t index = static_cast<uint8_t>(event);
    if (index >= static_cast<uint8_t>(IntEvent::NUM_EVENTS))
        return StatusTypeDef::ERROR;
    
    EventInfo& info = _events[index];
    
    if(int_num == INT_NUM::BOTH && info.registered_INT1 == false && info.registered_INT2 == false)
        return StatusTypeDef::ERROR;
    else if(int_num == INT_NUM::INT1 && info.registered_INT1 == false)
        return StatusTypeDef::ERROR;
    else if(int_num == INT_NUM::INT2 && info.registered_INT2 == false)
        return StatusTypeDef::ERROR;
    
    if(int_num == INT_NUM::BOTH)
    {
        info.enabled_INT1 = enable;
        info.enabled_INT2 = enable;
    }
    else if(int_num == INT_NUM::INT1)
    {
        info.enabled_INT1 = enable;
    }
    else if(int_num == INT_NUM::INT2)
    {
        info.enabled_INT2 = enable;
    }
    
    return res;
}

ICM42688::INT::StatusTypeDef ICM42688::INT::process_interrupts(INT_NUM int_num)
{
    StatusTypeDef res = StatusTypeDef::OK;

    res = get_int_state();
    if (res != StatusTypeDef::OK) return res;

    struct ActiveEvent 
    {
        IntEvent event;
        EVENT_PRIORITY priority;
    };
    ActiveEvent active_events[static_cast<size_t>(IntEvent::NUM_EVENTS)];
    uint8_t active_event_count = 0;

    for(int i = 0; i < static_cast<size_t>(IntEvent::NUM_EVENTS); i++)
    {
        EventInfo& info = _events[i];

        if(info.registered_INT1 == false && info.registered_INT2 == false)
            continue;

        switch(static_cast<IntEvent>(i))
        {
            case IntEvent::UI_FSYNC_INT:
                if(!_UI_FSYNC_INT_state) continue;
                break;
            case IntEvent::PLL_RDY_INT:
                if(!_PLL_RDY_INT_state) continue;
                break;
            case IntEvent::RESET_DONE_INT:
                if(!_RESET_DONE_INT_state) continue;
                break;
            case IntEvent::DATA_RDY_INT:
                if(!_DATA_RDY_INT_state) continue;
                break;
            case IntEvent::FIFO_THS_INT:
                if(!_FIFO_THS_INT_state) continue;
                break; 
            case IntEvent::FIFO_FULL_INT:
                if(!_FIFO_FULL_INT_state) continue;
                break;
            case IntEvent::UI_AGC_RDY_INT:
                if(!_UI_AGC_RDY_INT_state) continue;
                break;  
            case IntEvent::I3C_ERROR_INT:
                if(!_I3C_ERROR_INT_state) continue;
                break;
            case IntEvent::SMD_INT:
                if(!_SMD_INT_state) continue;
                break;
            case IntEvent::WOM_Z_INT:
                if(!_WOM_Z_INT_state) continue;
                break;
            case IntEvent::WOM_Y_INT:
                if(!_WOM_Y_INT_state) continue;
                break;
            case IntEvent::WOM_X_INT:
                if(!_WOM_X_INT_state) continue;
                break;
            case IntEvent::STEP_DET_INT:    
                if(!_STEP_DET_INT_state) continue;
                break;
            case IntEvent::STEP_CNT_OF_INT:
                if(!_STEP_CNT_OF_INT_state) continue;
                break;
            case IntEvent::TILT_DET_INT:    
                if(!_TILT_DET_INT_state) continue;
                break;
            case IntEvent::WAKE_DET_INT:
                if(!_WAKE_DET_INT_state) continue;
                break;
            case IntEvent::SLEEP_DET_INT:
                if(!_SLEEP_DET_INT_state) continue;
                break;
            case IntEvent::TAP_DET_INT:
                if(!_TAP_DET_INT_state) continue;
                break;
            default:
                continue;
        }

        if(info.enabled_INT1 == true && int_num == INT_NUM::INT1)
        {
            active_events[active_event_count].event = static_cast<IntEvent>(i);
            active_events[active_event_count].priority = info.priority_INT1;
            active_event_count++;
        }
        else if(info.enabled_INT2 == true && int_num == INT_NUM::INT2)
        {
            active_events[active_event_count].event = static_cast<IntEvent>(i);
            active_events[active_event_count].priority = info.priority_INT2;
            active_event_count++;
        }
    }

    for(int i = 0; i < active_event_count - 1; i++)
    {
        for(int j = 0; j < active_event_count - i - 1; j++)
        {
            if(active_events[j].priority < active_events[j + 1].priority)
            {
                ActiveEvent temp = active_events[j];
                active_events[j] = active_events[j + 1];
                active_events[j + 1] = temp;
            }
        }
    }
    
    for(int i = 0; i < active_event_count; i++)
    {
        EventInfo& info = _events[static_cast<size_t>(active_events[i].event)];

        if(int_num == INT_NUM::INT1 && info.callback_INT1 != nullptr)
            info.callback_INT1(info.context_INT1);
        else if(int_num == INT_NUM::INT2 && info.callback_INT2 != nullptr)
            info.callback_INT2(info.context_INT2);
    }
    
    return res;
}

bool ICM42688::INT::is_event_registered(IntEvent event, INT_NUM int_num) const 
{
    uint8_t index = static_cast<uint8_t>(event);
    if (index >= static_cast<uint8_t>(IntEvent::NUM_EVENTS))
        return false;
    
    if(int_num == INT_NUM::INT1)
        return _events[index].registered_INT1;
    else if(int_num == INT_NUM::INT2)
        return _events[index].registered_INT2;

    return false;
}

#ifdef ENABLE_DEBUG
void ICM42688::INT::print_int_source(void)
{
    #ifdef ENABLE_DEBUG
        printf("<=====================================================>\n");
        printf("%sINT1 INT SOURCE BOARD:%s\n", COLOR_BRIGHT_BLUE, COLOR_RESET);
        if(is_event_registered(IntEvent::UI_FSYNC_INT, INT_NUM::INT1)) printf("UI_FSYNC_INT\t%s%sENABLE%s\n", COLOR_GREEN, UNDERLINE, COLOR_RESET);
        else printf("UI_FSYNC_INT\t%s%sDISABLE%s\n", COLOR_RED, UNDERLINE, COLOR_RESET);
        if(is_event_registered(IntEvent::PLL_RDY_INT, INT_NUM::INT1)) printf("PLL_RDY_INT\t%s%sENABLE%s\n", COLOR_GREEN, UNDERLINE, COLOR_RESET);
        else printf("PLL_RDY_INT\t%s%sDISABLE%s\n", COLOR_RED, UNDERLINE, COLOR_RESET);
        if(is_event_registered(IntEvent::RESET_DONE_INT, INT_NUM::INT1)) printf("RESET_DONE_INT\t%s%sENABLE%s\n", COLOR_GREEN, UNDERLINE, COLOR_RESET);
        else printf("RESET_DONE_INT\t%s%sDISABLE%s\n", COLOR_RED, UNDERLINE, COLOR_RESET);
        if(is_event_registered(IntEvent::DATA_RDY_INT, INT_NUM::INT1)) printf("DRDY_INT\t%s%sENABLE%s\n", COLOR_GREEN, UNDERLINE, COLOR_RESET);
        else printf("DRDY_INT\t%s%sDISABLE%s\n", COLOR_RED, UNDERLINE, COLOR_RESET);
        if(is_event_registered(IntEvent::FIFO_THS_INT, INT_NUM::INT1)) printf("FIFO_THS_INT\t%s%sENABLE%s\n", COLOR_GREEN, UNDERLINE, COLOR_RESET);
        else printf("FIFO_THS_INT\t%s%sDISABLE%s\n", COLOR_RED, UNDERLINE, COLOR_RESET);
        if(is_event_registered(IntEvent::FIFO_FULL_INT, INT_NUM::INT1)) printf("FIFO_FULL_INT\t%s%sENABLE%s\n", COLOR_GREEN, UNDERLINE, COLOR_RESET);
        else printf("FIFO_FULL_INT\t%s%sDISABLE%s\n", COLOR_RED, UNDERLINE, COLOR_RESET);
        if(is_event_registered(IntEvent::UI_AGC_RDY_INT, INT_NUM::INT1)) printf("UI_AGC_RDY_INT\t%s%sENABLE%s\n", COLOR_GREEN, UNDERLINE, COLOR_RESET);
        else printf("UI_AGC_RDY_INT\t%s%sDISABLE%s\n", COLOR_RED, UNDERLINE, COLOR_RESET);
        if(is_event_registered(IntEvent::I3C_ERROR_INT, INT_NUM::INT1)) printf("I3C_ERROR_INT\t%s%sENABLE%s\n", COLOR_GREEN, UNDERLINE, COLOR_RESET);
        else printf("I3C_ERROR_INT\t%s%sDISABLE%s\n", COLOR_RED, UNDERLINE, COLOR_RESET);
        if(is_event_registered(IntEvent::SMD_INT, INT_NUM::INT1)) printf("SMD_INT\t\t%s%sENABLE%s\n", COLOR_GREEN, UNDERLINE, COLOR_RESET);
        else printf("SMD_INT\t\t%s%sDISABLE%s\n", COLOR_RED, UNDERLINE, COLOR_RESET);
        if(is_event_registered(IntEvent::WOM_Z_INT, INT_NUM::INT1)) printf("WOM_Z_INT\t%s%sENABLE%s\n", COLOR_GREEN, UNDERLINE, COLOR_RESET);
        else printf("WOM_Z_INT\t%s%sDISABLE%s\n", COLOR_RED, UNDERLINE, COLOR_RESET);
        if(is_event_registered(IntEvent::WOM_Y_INT, INT_NUM::INT1)) printf("WOM_Y_INT\t%s%sENABLE%s\n", COLOR_GREEN, UNDERLINE, COLOR_RESET);
        else printf("WOM_Y_INT\t%s%sDISABLE%s\n", COLOR_RED, UNDERLINE, COLOR_RESET);
        if(is_event_registered(IntEvent::WOM_X_INT, INT_NUM::INT1)) printf("WOM_X_INT\t%s%sENABLE%s\n", COLOR_GREEN, UNDERLINE, COLOR_RESET);
        else printf("WOM_X_INT\t%s%sDISABLE%s\n", COLOR_RED, UNDERLINE, COLOR_RESET);
        if(is_event_registered(IntEvent::STEP_DET_INT, INT_NUM::INT1)) printf("STEP_DET_INT\t%s%sENABLE%s\n", COLOR_GREEN, UNDERLINE, COLOR_RESET);
        else printf("STEP_DET_INT\t%s%sDISABLE%s\n", COLOR_RED, UNDERLINE, COLOR_RESET);
        if(is_event_registered(IntEvent::STEP_CNT_OF_INT, INT_NUM::INT1)) printf("STEP_CNT_OF_INT\t%s%sENABLE%s\n", COLOR_GREEN, UNDERLINE, COLOR_RESET);
        else printf("STEP_CNT_OF_INT\t%s%sDISABLE%s\n", COLOR_RED, UNDERLINE, COLOR_RESET);
        if(is_event_registered(IntEvent::TILT_DET_INT, INT_NUM::INT1)) printf("TILT_DET_INT\t%s%sENABLE%s\n", COLOR_GREEN, UNDERLINE, COLOR_RESET);
        else printf("TILT_DET_INT\t%s%sDISABLE%s\n", COLOR_RED, UNDERLINE, COLOR_RESET);
        if(is_event_registered(IntEvent::WAKE_DET_INT, INT_NUM::INT1)) printf("WAKE_DET_INT\t%s%sENABLE%s\n", COLOR_GREEN, UNDERLINE, COLOR_RESET);
        else printf("WAKE_DET_INT\t%s%sDISABLE%s\n", COLOR_RED, UNDERLINE, COLOR_RESET);
        if(is_event_registered(IntEvent::SLEEP_DET_INT, INT_NUM::INT1)) printf("SLEEP_DET_INT\t%s%sENABLE%s\n", COLOR_GREEN, UNDERLINE, COLOR_RESET);
        else printf("SLEEP_DET_INT\t%s%sDISABLE%s\n", COLOR_RED, UNDERLINE, COLOR_RESET);
        if(is_event_registered(IntEvent::TAP_DET_INT, INT_NUM::INT1)) printf("TAP_DET_INT\t%s%sENABLE%s\n", COLOR_GREEN, UNDERLINE, COLOR_RESET);
        else printf("TAP_DET_INT\t%s%sDISABLE%s\n", COLOR_RED, UNDERLINE, COLOR_RESET);
        printf("<=====================================================>\n");
        printf("%sINT2 INT SOURCE BOARD:%s\n", COLOR_BRIGHT_BLUE, COLOR_RESET);
        if(is_event_registered(IntEvent::UI_FSYNC_INT, INT_NUM::INT2)) printf("UI_FSYNC_INT\t%s%sENABLE%s\n", COLOR_GREEN, UNDERLINE, COLOR_RESET);
        else printf("UI_FSYNC_INT\t%s%sDISABLE%s\n", COLOR_RED, UNDERLINE, COLOR_RESET);
        if(is_event_registered(IntEvent::PLL_RDY_INT, INT_NUM::INT2)) printf("PLL_RDY_INT\t%s%sENABLE%s\n", COLOR_GREEN, UNDERLINE, COLOR_RESET);
        else printf("PLL_RDY_INT\t%s%sDISABLE%s\n", COLOR_RED, UNDERLINE, COLOR_RESET);
        if(is_event_registered(IntEvent::RESET_DONE_INT, INT_NUM::INT2)) printf("RESET_DONE_INT\t%s%sENABLE%s\n", COLOR_GREEN, UNDERLINE, COLOR_RESET);
        else printf("RESET_DONE_INT\t%s%sDISABLE%s\n", COLOR_RED, UNDERLINE, COLOR_RESET);
        if(is_event_registered(IntEvent::DATA_RDY_INT, INT_NUM::INT2)) printf("DRDY_INT\t%s%sENABLE%s\n", COLOR_GREEN, UNDERLINE, COLOR_RESET);
        else printf("DRDY_INT\t%s%sDISABLE%s\n", COLOR_RED, UNDERLINE, COLOR_RESET);
        if(is_event_registered(IntEvent::FIFO_THS_INT, INT_NUM::INT2)) printf("FIFO_THS_INT\t%s%sENABLE%s\n", COLOR_GREEN, UNDERLINE, COLOR_RESET);
        else printf("FIFO_THS_INT\t%s%sDISABLE%s\n", COLOR_RED, UNDERLINE, COLOR_RESET);
        if(is_event_registered(IntEvent::FIFO_FULL_INT, INT_NUM::INT2)) printf("FIFO_FULL_INT\t%s%sENABLE%s\n", COLOR_GREEN, UNDERLINE, COLOR_RESET);
        else printf("FIFO_FULL_INT\t%s%sDISABLE%s\n", COLOR_RED, UNDERLINE, COLOR_RESET);
        if(is_event_registered(IntEvent::UI_AGC_RDY_INT, INT_NUM::INT2)) printf("UI_AGC_RDY_INT\t%s%sENABLE%s\n", COLOR_GREEN, UNDERLINE, COLOR_RESET);
        else printf("UI_AGC_RDY_INT\t%s%sDISABLE%s\n", COLOR_RED, UNDERLINE, COLOR_RESET);
        if(is_event_registered(IntEvent::I3C_ERROR_INT, INT_NUM::INT2)) printf("I3C_ERROR_INT\t%s%sENABLE%s\n", COLOR_GREEN, UNDERLINE, COLOR_RESET);
        else printf("I3C_ERROR_INT\t%s%sDISABLE%s\n", COLOR_RED, UNDERLINE, COLOR_RESET);
        if(is_event_registered(IntEvent::SMD_INT, INT_NUM::INT2)) printf("SMD_INT\t\t%s%sENABLE%s\n", COLOR_GREEN, UNDERLINE, COLOR_RESET);
        else printf("SMD_INT\t\t%s%sDISABLE%s\n", COLOR_RED, UNDERLINE, COLOR_RESET);
        if(is_event_registered(IntEvent::WOM_Z_INT, INT_NUM::INT2)) printf("WOM_Z_INT\t%s%sENABLE%s\n", COLOR_GREEN, UNDERLINE, COLOR_RESET);
        else printf("WOM_Z_INT\t%s%sDISABLE%s\n", COLOR_RED, UNDERLINE, COLOR_RESET);
        if(is_event_registered(IntEvent::WOM_Y_INT, INT_NUM::INT2)) printf("WOM_Y_INT\t%s%sENABLE%s\n", COLOR_GREEN, UNDERLINE, COLOR_RESET);
        else printf("WOM_Y_INT\t%s%sDISABLE%s\n", COLOR_RED, UNDERLINE, COLOR_RESET);
        if(is_event_registered(IntEvent::WOM_X_INT, INT_NUM::INT2)) printf("WOM_X_INT\t%s%sENABLE%s\n", COLOR_GREEN, UNDERLINE, COLOR_RESET);
        else printf("WOM_X_INT\t%s%sDISABLE%s\n", COLOR_RED, UNDERLINE, COLOR_RESET);
        if(is_event_registered(IntEvent::STEP_DET_INT, INT_NUM::INT2)) printf("STEP_DET_INT\t%s%sENABLE%s\n", COLOR_GREEN, UNDERLINE, COLOR_RESET);
        else printf("STEP_DET_INT\t%s%sDISABLE%s\n", COLOR_RED, UNDERLINE, COLOR_RESET);
        if(is_event_registered(IntEvent::STEP_CNT_OF_INT, INT_NUM::INT2)) printf("STEP_CNT_OF_INT\t%s%sENABLE%s\n", COLOR_GREEN, UNDERLINE, COLOR_RESET);
        else printf("STEP_CNT_OF_INT\t%s%sDISABLE%s\n", COLOR_RED, UNDERLINE, COLOR_RESET);
        if(is_event_registered(IntEvent::TILT_DET_INT, INT_NUM::INT2)) printf("TILT_DET_INT\t%s%sENABLE%s\n", COLOR_GREEN, UNDERLINE, COLOR_RESET);
        else printf("TILT_DET_INT\t%s%sDISABLE%s\n", COLOR_RED, UNDERLINE, COLOR_RESET);
        if(is_event_registered(IntEvent::WAKE_DET_INT, INT_NUM::INT2)) printf("WAKE_DET_INT\t%s%sENABLE%s\n", COLOR_GREEN, UNDERLINE, COLOR_RESET);
        else printf("WAKE_DET_INT\t%s%sDISABLE%s\n", COLOR_RED, UNDERLINE, COLOR_RESET);
        if(is_event_registered(IntEvent::SLEEP_DET_INT, INT_NUM::INT2)) printf("SLEEP_DET_INT\t%s%sENABLE%s\n", COLOR_GREEN, UNDERLINE, COLOR_RESET);
        else printf("SLEEP_DET_INT\t%s%sDISABLE%s\n", COLOR_RED, UNDERLINE, COLOR_RESET);
        if(is_event_registered(IntEvent::TAP_DET_INT, INT_NUM::INT2)) printf("TAP_DET_INT\t%s%sENABLE%s\n", COLOR_GREEN, UNDERLINE, COLOR_RESET);
        else printf("TAP_DET_INT\t%s%sDISABLE%s\n", COLOR_RED, UNDERLINE, COLOR_RESET);
        printf("<=====================================================>\n");
    #endif
}
#endif

//===========================================================================================================

#if 1
void UI_FSYNC_callback(void* context)
{
    #ifdef ENABLE_DEBUG
    printf("UI FSYNC interrupt triggered!\n");
    #endif
}

void PLL_RDY_callback(void* context)
{
    #ifdef ENABLE_DEBUG
    printf("PLL ready interrupt triggered!\n");
    #endif
}

void RESET_DONE_callback(void* context)
{
    #ifdef ENABLE_DEBUG
    printf("Reset done interrupt triggered!\n");
    #endif
}

void DATA_RDY_callback(void* context)
{
    #ifdef ENABLE_DEBUG
    // printf("Data ready! Reading sensor data ...\n");

    (static_cast<ICM42688*>(context))->general.read_data();
    #endif
}

void FIFO_THS_callback(void* context)
{
    #ifdef ENABLE_DEBUG
    printf("FIFO threshold interrupt triggered!\n");
    #endif
}

void FIFO_FULL_callback(void* context)
{
    #ifdef ENABLE_DEBUG
    printf("FIFO full interrupt triggered!\n");
    #endif
}

void UI_AGC_RDY_callback(void* context)
{
    #ifdef ENABLE_DEBUG
    printf("UI AGC ready interrupt triggered!\n");
    #endif
}

void I3C_ERROR_callback(void* context)
{
    #ifdef ENABLE_DEBUG
    printf("I3C error interrupt triggered!\n");
    #endif
}

void SMD_callback(void* context)
{
    #ifdef ENABLE_DEBUG
    printf("Significant Motion Detection interrupt triggered!\n");
    #endif
}

void WOM_Z_callback(void* context)
{
    #ifdef ENABLE_DEBUG
    printf("Wake on motion Z-axis interrupt triggered!\n");
    #endif
}

void WOM_Y_callback(void* context)
{
    #ifdef ENABLE_DEBUG
    printf("Wake on motion Y-axis interrupt triggered!\n");
    #endif
}

void WOM_X_callback(void* context)
{
    #ifdef ENABLE_DEBUG
    printf("Wake on motion X-axis interrupt triggered!\n");
    #endif
}

void STEP_DET_callback(void* context)
{
    #ifdef ENABLE_DEBUG
    printf("Step detection interrupt triggered! Step count: %d\n", 1);
    #endif
}

void STEP_CNT_OF_callback(void* context)
{
    #ifdef ENABLE_DEBUG
    printf("Step counter overflow interrupt triggered!\n");
    #endif
}

void TILT_DET_callback(void* context)
{
    #ifdef ENABLE_DEBUG
    printf("Tilt detection interrupt triggered!\n");     
    #endif
}

void WAKE_DET_callback(void* context)
{
    #ifdef ENABLE_DEBUG
    printf("Wake detection interrupt triggered!\n");
    #endif
}

void SLEEP_DET_callback(void* context)
{
    #ifdef ENABLE_DEBUG
    printf("Sleep detection interrupt triggered!\n");
    #endif
}

void TAP_DET_callback(void* context)
{
    #ifdef ENABLE_DEBUG
    printf("Tap detection interrupt triggered!\n");
    #endif
}
#endif



