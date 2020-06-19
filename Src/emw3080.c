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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "at_wrapper.h"
#include "emw3080.h"
#include "usart.h"
#include "smartbed.h"
#include "at_parser.h"
#include "FreeRTOS.h"
#include "task.h"
#include "ble.h"
#include "cmsis_os.h"
#include "str.h"
#include <jansson.h>

static char event_buf[256];
static char func_buf[256];
static char wifi_tx_buf[256];
static char wifi_rx_buf[256];

static char fun[20];
static int len = 0;
static int pId = 0;
static char param[40];

void emw3080_init()
{
	at_register_callback("\r\n+WEVENT:","\r\n",event_buf,256,event_cb,NULL);
	at_register_callback("\r\n+IDMSS:","OK\r\n",func_buf,256,func_cb,NULL);
}

void event_cb(void *arg, char *buf, int buflen)
{
	if(strncmp(buf,"\r\n+WEVENT:STATION_DOWN",strlen(buf))==0){
		
	}
}

void func_cb(void *arg, char *buf, int buflen)
{	
	memset(fun,0,20);
	memset(param,0,40);
	char b[60];
	memset(b,0,60);
	
	memcpy(b,buf+11,buflen-17);
	sscanf(b, "%*[\"]%[^\"]%*[\",]%d%*[\",]%d%*[,]%s", fun, &pId, &len, param );
	substr(param,1,strlen(param)-2);
	
	json_error_t error;
	json_t *root = json_loadb( param,strlen(param), 0, &error);
	
	if (root)
	{
    	int deviceId;
		int p;
		json_unpack( root, "{sisi}", "DeviceId", &deviceId, "Param", &p );
		
		memset(param,0,40);
		sprintf(param,"%d,%d",deviceId,p);
		uint8_t res = set_bed_action(fun,param);
		
		json_delete(root);
		
		memset(wifi_tx_buf,0,RECV_BUFFER_SIZE);
		sprintf(wifi_tx_buf,"AT+IDMSSR=0,\"%s\",\"%d\",\"{\\\"code\\\":1}\"\r\n",fun,pId);
		
		int cmdlen = strlen(wifi_tx_buf);
		memset(wifi_rx_buf,0,RECV_BUFFER_SIZE);
		int ret = at_send_no_reply(wifi_tx_buf,cmdlen,NULL);
  }
}

void attr_report_task()
{
	static portTickType xLastWakeTime; 
	xLastWakeTime = xTaskGetTickCount(); 
	const portTickType xFrequency = pdMS_TO_TICKS(300000); 
	
	osDelay(1000);
	attr_report();	
	
  for(;;)
  {
		vTaskDelayUntil(&xLastWakeTime,xFrequency);
		
		attr_report();	
  }
}

void attr_report()
{
		char * json = bed_status_dumps();
		memset(wifi_tx_buf,0,RECV_BUFFER_SIZE);
		sprintf(wifi_tx_buf,"AT+IDMPP=0,\"%s\"\r\n",json);
		free(json);
		memset(wifi_rx_buf,0,RECV_BUFFER_SIZE);
		
		int cmdlen = strlen(wifi_tx_buf);
		int ret = at_send_wait_reply(wifi_tx_buf,cmdlen,NULL,NULL,0,wifi_rx_buf,256,&atcmd_report_config);
		//int ret = at_send_no_reply(at_tx_buf,size,NULL);
		
//		if(ret == 0)
//		{
//				HAL_Printf("");
//		}	
}

