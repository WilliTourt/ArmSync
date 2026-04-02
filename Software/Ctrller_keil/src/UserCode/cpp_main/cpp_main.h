#ifndef __CPP_MAIN_H__
#define __CPP_MAIN_H__

#ifdef __cplusplus
extern "C" {
#endif

    // #include "main.h"
    #include "hal_data.h"
    // #include "usart.h"
    
    #include <stdio.h>
    #include <string.h>
    
    #include "math.h"

    void cpp_main(void);

#ifdef __cplusplus
}
#endif

extern volatile uint32_t sysTick;

#endif
