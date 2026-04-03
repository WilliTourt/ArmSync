#include "hal_data.h"
#include "key/key.h"
#include "ra/fsp/inc/api/r_external_irq_api.h"


volatile bool key1_sw2_press = false;
void Key_IRQ_Init(void)
{
   fsp_err_t err = FSP_SUCCESS;

   /* Open ICU module */
   err = R_ICU_ExternalIrqOpen(&g_external_irq0_ctrl, &g_external_irq0_cfg);
 
   err = R_ICU_ExternalIrqEnable(&g_external_irq0_ctrl);
}
/* 按键按下标志 */


/* 按键中断回调函数 */
void key_external_irq_callback(external_irq_callback_args_t *p_args)
{
   /* 判断中断通道 */
   if (6 == p_args->channel)
   {
      key1_sw2_press = true;   // 按键KEY1_SW2按下
   }
   
}