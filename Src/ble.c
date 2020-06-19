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


#include "ble.h"
#include <string.h>
#include "cmsis_os.h"
#include "at_wrapper.h"
#include "str.h"
#include "smartbed.h"

#define BLE_RX_SIZE  20

static char ble_rx_buf[BLE_RX_SIZE]= {0};
static char ble_buff[40];

void ble_init()
{
	HAL_UART_Receive_DMA(&huart6, (uint8_t*)ble_rx_buf, BLE_RX_SIZE);
}

void ble_write(char * buf,uint8_t size)
{
	HAL_StatusTypeDef result = HAL_UART_Transmit_DMA(&huart6,(uint8_t*)buf,size);
}

void ble_receive_dma_callback()
{
	char fun[20];
	memset(fun,0,20);
	char param[40];
	memset(param,0,40);
	
	if(strncmp(ble_rx_buf,"+",1)==0){
		memset(ble_buff,0,40);
		
		if(strncmp(ble_rx_buf+19,"+",1)==0){
			memcpy(ble_buff,ble_rx_buf,20);
			goto end;
		}
	}
	
	if(strlen(ble_buff) >= 40){
		return;
	}
	
	memcpy(ble_buff+strlen(ble_buff),ble_rx_buf,20);
	
	if(strncmp(ble_rx_buf+19,"+",1)==0){
		goto end;
	}else{
		return;
	}
	
end:
	sscanf(ble_buff, "+%[^:]:%[^+]+", fun, param );
	if(strlen(fun) == 0)
		return;
	
	set_bed_action(fun,param);
	
//	char b[20];
//	memset(b,0x2B,20);
//	memcpy(b,fun,strlen(fun));
//	ble_write(b,20);
}

