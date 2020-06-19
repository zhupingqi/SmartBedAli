/**
 * Copyright (c) 2030 河北极创异想科技有限公司 - 朱平齐 iotsys.net
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "ld3320.h"
#include <string.h>
#include "cmsis_os.h"
#include "at_wrapper.h"
#include "str.h"
#include "smartbed.h"

#define LD3320_RX_SIZE  20

static char ld3320_rx_buf[LD3320_RX_SIZE]= {0};
static char ld3320_buff[40];

void ld3320_init()
{
	HAL_UART_Receive_DMA(&huart2, (uint8_t*)ld3320_rx_buf, LD3320_RX_SIZE);
}

void ld3320_receive_dma_callback()
{
	char fun[20];
	memset(fun,0,20);
	char param[40];
	memset(param,0,40);
	
	if(strncmp(ld3320_rx_buf,"+",1)==0){
		memset(ld3320_buff,0,40);
		
		if(strncmp(ld3320_rx_buf+19,"+",1)==0){
			memcpy(ld3320_buff,ld3320_rx_buf,20);
			goto end;
		}
	}
	
	if(strlen(ld3320_buff) >= 40){
		return;
	}
	
	memcpy(ld3320_buff+strlen(ld3320_buff),ld3320_rx_buf,20);
	
	if(strncmp(ld3320_rx_buf+19,"+",1)==0){
		goto end;
	}else{
		return;
	}
	
end:
	sscanf(ld3320_buff, "+%[^:]:%[^+]+", fun, param );
	if(strlen(fun) == 0)
		return;
	
	set_bed_action(fun,param);
}

