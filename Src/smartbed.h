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

#include <stdint.h>
#include <stdio.h>

typedef enum 
{
  BEDACTION_BEDSIDE_PW       = 0x01U,
	BEDACTION_BEDSIDE_PWOFF = 0x00U,
  BEDACTION_BEDSIDE_DOWN    = 0x02U,
  BEDACTION_BEDSIDE_UP     = 0x00U,
  BEDACTION_BEDEND_PW  = 0x04U,
	BEDACTION_BEDEND_PWOFF = 0x00U,
	BEDACTION_BEDEND_DOWN    = 0x08U,
  BEDACTION_BEDEND_UP     = 0x00U
} bed_action;

typedef enum
{
	BA_BEDSIDEUP,
	BA_BEDSIDEDOWN,
	BA_BEDENDUP,
	BA_BEDENDDOWN,
	BA_SETMOMOERY,
	BA_RESTOREMEMORY,
	BA_STOPACTION,
	BA_RESET
} BEDACTION;

typedef enum 
{
  SS_WAKEUP       = 0x01U,
  SS_LIGHTSLEEP    = 0x02U,
  SS_DEEPSLEEP     = 0x03U,
  SS_OUTOFBED  = 0x04U
} sleep_status;

typedef enum
{
	BMS_WAKEUP       = 0x00U,
  BMS_LIGHTSLEEP    = 0x01U,
  BMS_DEEPSLEEP     = 0x02U,
} bed_motion_status;

typedef enum
{
	BEDMODE_SLEEP       = 0x00U,
  BEDMODE_MOVIE    = 0x01U,
  BEDMODE_READ     = 0x02U,
} bed_mode;

typedef struct {
    int8_t RoomTemp;
    uint8_t RoomHumidity;
    uint8_t HeartRate;
		uint8_t RespiratoryRate;
		uint8_t BedSideAngle;
		uint8_t BedEndAngle;
		sleep_status SleepStatus;
		bed_motion_status BedSideStatus;
		bed_motion_status BedEndStatus;
} bed_tsl;

char* bed_status_dumps(void);

typedef struct {
	uint16_t BedSide;
	uint16_t BedEnd;
} bed_position_t;

typedef struct {
	bed_position_t current;
	bed_position_t mem0;
	bed_position_t mem1;
	bed_position_t mem2;
	bed_position_t mem3;
	bed_position_t mem4;
	bed_position_t mem5;
	bed_position_t mem6;
	bed_position_t mem7;
} bed_positon_save_t;

typedef struct
{
	uint8_t action;
	uint32_t endTick;
} bed_action_remain_t;

#define FLASH_POSITON_SECTOR  	 0
#define FLASH_DEVICEINFO_SECTOR	 1

void smartbed_init(void);
uint8_t BedSideUp(uint8_t angle);
uint8_t BedSideDown(uint8_t angle);
uint8_t BedEndUp(uint8_t angle);
uint8_t BedEndDown(uint8_t angle);
uint8_t SetBedMode(bed_mode mode);
uint8_t SetMomery(uint8_t bedSideAngle,uint8_t bedEndAngel,uint8_t slot);
uint8_t RestoreMemory(uint8_t slot);
uint8_t SetWifi(char * ssid,char * password);
uint8_t QueryStatus(void);
uint8_t SetBedAction(uint8_t action);
void StopBedAction(uint8_t action);
uint8_t BedSideUp(uint8_t angle);
uint8_t BedSideDown(uint8_t angle);
uint8_t BedEndUp(uint8_t angle);
uint8_t BedEndDown(uint8_t angle);
uint8_t set_bed_action(char * action,char * param);
void RunBedAction(void);
void SaveCurrentPosition(bed_positon_save_t *currentDevice,uint8_t action);
void SaveDevicePostion(void);

