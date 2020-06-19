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

#include "smartbed.h"
#include <jansson.h>
#include <string.h>
#include "cmsis_os.h"
#include "str.h"
#include "gpio.h"
#include "w25qxx.h"
#include "at_wrapper.h"

#define SMART_BED_MODEL "jcyx_smartbed"

static bed_tsl bedtsl;
static char product_key[11] = "xxx";
static char product_secret[16] = "xxx";
static char device_name[20] = "xxx";
static char device_secret[32] = "xxx";
static uint16_t sleep_time = NULL;
static uint8_t sensitivity = 0;
static uint16_t alarm = 0;
static uint16_t alarm_clock = 0;

static bed_action_remain_t action_remain[8];
static bed_positon_save_t device_save_0;
static bed_positon_save_t device_save_1;
static uint16_t max_postion = 20000;
static void* action_mutex;
static uint8_t flash_init = 0;
static uint8_t save_dirty = 0;
static uint32_t dirty_count = 0;

void smartbed_init()
{
	action_mutex = HAL_MutexCreate();
	
	action_remain[0].action = BEDACTION_BEDSIDE_PW|BEDACTION_BEDSIDE_UP;
	action_remain[0].endTick = 0;
	action_remain[1].action = BEDACTION_BEDSIDE_PW|BEDACTION_BEDSIDE_DOWN;
	action_remain[1].endTick = 0;
	action_remain[2].action = BEDACTION_BEDEND_PW|BEDACTION_BEDEND_UP;
	action_remain[2].endTick = 0;
	action_remain[3].action = BEDACTION_BEDEND_PW|BEDACTION_BEDEND_DOWN;
	action_remain[3].endTick = 0;
	action_remain[4].action = (BEDACTION_BEDSIDE_PW|BEDACTION_BEDSIDE_UP) << 4;
	action_remain[4].endTick = 0;
	action_remain[5].action = (BEDACTION_BEDSIDE_PW|BEDACTION_BEDSIDE_DOWN) << 4;
	action_remain[5].endTick = 0;
	action_remain[6].action = (BEDACTION_BEDEND_PW|BEDACTION_BEDEND_UP) << 4;
	action_remain[6].endTick = 0;
	action_remain[7].action = (BEDACTION_BEDEND_PW|BEDACTION_BEDEND_DOWN) << 4;
	action_remain[7].endTick = 0;
	
	SetBedAction(0);
	
	if(W25qxx_IsEmptySector(FLASH_POSITON_SECTOR,0,20) || flash_init)
	{
		device_save_0.current.BedSide = 0;
		device_save_0.current.BedEnd = 0;
		device_save_0.mem0.BedSide = 5000;
		device_save_0.mem0.BedEnd = 5000;
		device_save_0.mem1.BedSide = 10000;
		device_save_0.mem1.BedEnd = 10000;
		device_save_0.mem2.BedSide = 15000;
		device_save_0.mem2.BedEnd = 15000;
		device_save_0.mem3.BedSide = 20000;
		device_save_0.mem3.BedEnd = 20000;
		device_save_0.mem4.BedSide = 0;
		device_save_0.mem4.BedEnd = 0;
		device_save_0.mem5.BedSide = 0;
		device_save_0.mem5.BedEnd = 0;
		device_save_0.mem6.BedSide = 0;
		device_save_0.mem6.BedEnd = 0;
		device_save_0.mem7.BedSide = 0;
		device_save_0.mem7.BedEnd = 0;
		
		device_save_1.current.BedSide = 0;
		device_save_1.current.BedEnd = 0;
		device_save_1.mem0.BedSide = 5000;
		device_save_1.mem0.BedEnd = 5000;
		device_save_1.mem1.BedSide = 10000;
		device_save_1.mem1.BedEnd = 10000;
		device_save_1.mem2.BedSide = 15000;
		device_save_1.mem2.BedEnd = 15000;
		device_save_1.mem3.BedSide = 20000;
		device_save_1.mem3.BedEnd = 20000;
		device_save_1.mem4.BedSide = 0;
		device_save_1.mem4.BedEnd = 0;
		device_save_1.mem5.BedSide = 0;
		device_save_1.mem5.BedEnd = 0;
		device_save_1.mem6.BedSide = 0;
		device_save_1.mem6.BedEnd = 0;
		device_save_1.mem7.BedSide = 0;
		device_save_1.mem7.BedEnd = 0;
		
		SaveDevicePostion();
	}
	else
	{
		W25qxx_ReadSector((uint8_t*)&device_save_0,FLASH_POSITON_SECTOR,0,36);
		W25qxx_ReadSector((uint8_t*)&device_save_1,FLASH_POSITON_SECTOR,36,36);
	}
}

void read_status()
{
	bedtsl.RoomTemp = 20;
	bedtsl.SleepStatus = SS_WAKEUP;
	bedtsl.HeartRate = 1;
	bedtsl.RespiratoryRate = 1;
}

char* bed_status_dumps()
{
	read_status();
	
	json_t* root = json_object();
	json_object_set_new(root,"RoomTemp", json_integer(bedtsl.RoomTemp));
	json_object_set_new(root,"RoomHumidity", json_integer(bedtsl.RoomHumidity));
	json_object_set_new(root,"HeartRate", json_integer(bedtsl.HeartRate));
	json_object_set_new(root,"RespiratoryRate", json_integer(bedtsl.RespiratoryRate));
	json_object_set_new(root,"BedSideAngle", json_integer(bedtsl.BedSideAngle));
	json_object_set_new(root,"BedEndAngle", json_integer(bedtsl.BedEndAngle));
	json_object_set_new(root,"SleepStatus", json_integer(bedtsl.SleepStatus));
	json_object_set_new(root,"BedSideStatus", json_integer(bedtsl.BedSideStatus));
	json_object_set_new(root,"BedEndStatus", json_integer(bedtsl.BedEndStatus));
	char * tmp = json_dumps(root,JSON_INDENT(0)|JSON_COMPACT);
	char * s_repon = strreplace(tmp,"\"","\\\"");
	free(tmp);
	
	if(root)
		json_delete(root);
	
	return s_repon;
}

uint8_t SetBedAction(uint8_t action)
{
	HAL_GPIO_WritePin(GPIOA,MOTOR_ST_Pin,GPIO_PIN_RESET);
	
	//uint8_t act = ~action;
	for(uint8_t i = 0; i < 8; i++){
		HAL_GPIO_WritePin(GPIOA,MOTOR_SCK_Pin,GPIO_PIN_RESET);
		uint8_t bit = action >> (7-i) & 0x01;
		HAL_GPIO_WritePin(GPIOA,MOTOR_MOSI_Pin,bit);
		HAL_GPIO_WritePin(GPIOA,MOTOR_SCK_Pin,GPIO_PIN_SET);
	}
	
	HAL_GPIO_WritePin(GPIOA,MOTOR_ST_Pin,GPIO_PIN_SET);
	
	return 0;
}

uint8_t set_bed_action(char * action,char * param)
{
	BEDACTION actEnum;
	
	if(strncmp(action,"BedSideUp",strlen(action))==0){
		actEnum = BA_BEDSIDEUP;
	}
	if(strncmp(action,"BedSideDown",strlen(action))==0){
		actEnum = BA_BEDSIDEDOWN;
	}
	if(strncmp(action,"BedEndUp",strlen(action))==0){
		actEnum = BA_BEDENDUP;
	}
	if(strncmp(action,"BedEndDown",strlen(action))==0){
		actEnum = BA_BEDENDDOWN;
	}
	if(strncmp(action,"SetMomery",strlen(action))==0){
		actEnum = BA_SETMOMOERY;
	}
	if(strncmp(action,"RestoreMemory",strlen(action))==0){
		actEnum = BA_RESTOREMEMORY;
	}
	if(strncmp(action,"StopAction",strlen(action))==0){
		actEnum = BA_STOPACTION;
	}
	if(strncmp(action,"Reset",strlen(action))==0){
		actEnum = BA_RESET;
	}
	
	int deviceId =0;
	uint8_t offset = deviceId * 4;
	int p = 0;
	sscanf(param, "%d,%d", &deviceId, &p);
	
	//HAL_MutexLock(action_mutex);
	
	bed_positon_save_t *currentDevice;
			
	if(deviceId == 0)
	{
		currentDevice = &device_save_0;
	}
	else
	{
		currentDevice = &device_save_1;
	}
				
	bed_position_t *currentPosition = (bed_position_t*)currentDevice;
	uint32_t tick = HAL_GetTick();
	
	switch(actEnum)
	{
		case BA_BEDSIDEUP:
		{
			action_remain[offset].endTick = tick + 500 * p;
		}
		break;
		case BA_BEDSIDEDOWN:
		{
			action_remain[offset + 1].endTick = tick + 500 * p;
		}
		break;
		case BA_BEDENDUP:
		{
			action_remain[offset + 2].endTick = tick + 500 * p;
		}
		break;
		case BA_BEDENDDOWN:
		{
			action_remain[offset + 3].endTick = tick + 500 * p;
		}
		break;
		case BA_RESTOREMEMORY:
		{
			void * tp = ((uint8_t*)currentDevice) + p * 4 + 4;
			bed_position_t *target = (bed_position_t*)(tp);
			uint16_t targetBedSide = target->BedSide;
			uint16_t targetBedEnd = target->BedEnd;
			
			if(targetBedSide - currentPosition->BedSide > 0)
			{
				action_remain[deviceId * 4].endTick = tick + abs(targetBedSide - currentPosition->BedSide);
			}
			else if(targetBedSide - currentPosition->BedSide < 0)
			{
				action_remain[deviceId * 4 + 1].endTick = tick + abs(targetBedSide - currentPosition->BedSide);
			}
			
			if(targetBedEnd - currentPosition->BedEnd > 0)
			{
				action_remain[deviceId * 4 + 2].endTick = tick + abs(targetBedEnd - currentPosition->BedEnd);
			}
			else if(targetBedEnd - currentPosition->BedEnd < 0)
			{
				action_remain[deviceId * 4 + 3].endTick = tick + abs(targetBedEnd - currentPosition->BedEnd);
			}
		}
		break;
		case BA_SETMOMOERY:
		{			
			bed_position_t *currentPosition = (bed_position_t*)currentDevice;
			bed_position_t *targetMemSlot = (bed_position_t*)(currentPosition + 1 * p + 1);
			targetMemSlot->BedSide = currentPosition->BedSide;
			targetMemSlot->BedEnd = currentPosition->BedEnd;
			
			SaveDevicePostion();
		}
		break;
		case BA_STOPACTION:
		{	
			StopBedAction(p << deviceId*4);
		}
		break;
		case BA_RESET:
		{
			action_remain[deviceId * 4 + 1].endTick = tick + currentPosition->BedSide + 1000;
			action_remain[deviceId * 4 + 3].endTick = tick + currentPosition->BedEnd + 1000;
		}
		break;
	}

	//HAL_MutexUnlock(action_mutex);
	return 0;
}

void SaveCurrentPosition(bed_positon_save_t *currentDevice,uint8_t action)
{
	save_dirty = 1;
	
	if(action & BEDACTION_BEDSIDE_PW)
	{		
		if(action & BEDACTION_BEDSIDE_DOWN)
		{
			if(currentDevice->current.BedSide > 0)
				currentDevice->current.BedSide -= 100;
		}
		else
		{
			if(currentDevice->current.BedSide < max_postion)
				currentDevice->current.BedSide += 100;
		}
	}

	if(action & BEDACTION_BEDEND_PW)
	{	
		if(action & BEDACTION_BEDEND_DOWN)
		{
			if(currentDevice->current.BedEnd > 0)
				currentDevice->current.BedEnd -= 100;
		}
		else
		{
			if(currentDevice->current.BedEnd < max_postion)
				currentDevice->current.BedEnd += 100;	
		}
	}
}

void SaveDevicePostion()
{
	W25qxx_EraseSector(FLASH_POSITON_SECTOR);
	W25qxx_WriteSector((uint8_t*)&device_save_0,FLASH_POSITON_SECTOR,0,36);
	W25qxx_WriteSector((uint8_t*)&device_save_1,FLASH_POSITON_SECTOR,36,36);
		
	save_dirty = 0;
}

void StopBedAction(uint8_t action)
{
	for(uint8_t i = 0; i < 8; i++)
	{
		if(action & (0x01 << i))
		{
			//HAL_MutexLock(action_mutex);
			action_remain[i].endTick = 0;
			//HAL_MutexUnlock(action_mutex);
		}
	}
}

void bed_action_task()
{
	static portTickType xLastWakeTime; 
	xLastWakeTime = xTaskGetTickCount(); 
	const portTickType xFrequency = pdMS_TO_TICKS(100); 
	
	RunBedAction();	
	
  for(;;)
  {
		vTaskDelayUntil(&xLastWakeTime,xFrequency);
		
		RunBedAction();	
  }
}

void RunBedAction()
{
	uint32_t tick = HAL_GetTick();	
	
	uint8_t action = 0x00;
	
	for(uint8_t i = 0;i < 8; i+=2)
	{
		uint8_t mu = 0;
		bed_positon_save_t *currentDevice = i < 4 ? &device_save_0 : &device_save_1 ;
		
		if(action_remain[i].endTick > tick) // up action
		{			
			action |= action_remain[i].action;
			SaveCurrentPosition(currentDevice,action_remain[i].action);
			mu = 1;
		}
		else
		{
			if(action_remain[i].endTick > 0)
			{
				action &= ~action_remain[i].action;
				action_remain[i].endTick = 0;
			}
		}
		
		if(mu == 0) //down action
		{
			if(action_remain[i+1].endTick > tick)
			{
				action |= action_remain[i+1].action;
				SaveCurrentPosition(currentDevice,action_remain[i+1].action);
			}
			else
			{
				if(action_remain[i+1].endTick > 0)
				{
					action &= ~action_remain[i+1].action;
					action_remain[i+1].endTick = 0;
				}
			}
		}
	}
	
	SetBedAction(action);
	
	dirty_count++;
	if(save_dirty && (dirty_count % (3000000)) == 0) // save device position every 5 min
	{
		SaveDevicePostion();
	}
}

