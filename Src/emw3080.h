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
#include "at_parser.h"
#include "usart.h"

#define RECV_PREFIX          "\r\n"
#define RECV_SUCCESS_POSTFIX "\r\n"
#define RECV_FAIL_POSTFIX    "ERROR\r\n"
#define RECV_BUFFER_SIZE 256

static atcmd_config_t atcmd_report_config = {"\r\n+IDMPP","\"{}\"\r\n","\r\n+CME ERROR\r\n"};
static atcmd_config_t atcmd_event_report_config = {"\r\n+IDMEP",",200\r\n",",ERROR\r\n"};
static atcmd_config_t atcmd_event_response = {"\r\n","OK\r\n","\r\n+CME ERROR:50\r\n"};

void emw3080_init(void);
void event_cb(void *arg, char *buf, int buflen);
void func_cb(void *arg, char *buf, int buflen);
void attr_report_task(void);
void attr_report(void);

