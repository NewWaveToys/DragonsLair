/* Copyright  (C) 2010-2015 The RetroArch team
 *
 * ---------------------------------------------------------------------------------------
 * The following license statement only applies to this file (rthreads.h).
 * ---------------------------------------------------------------------------------------
 *
 * Permission is hereby granted, free of charge,
 * to any person obtaining a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef __KEY_DEF_H__
#define __KEY_DEF_H__

#if defined(__cplusplus) 
extern "C" {
#endif

#define UP_KEY		1
#define DOWN_KEY	1<<1
#define LEFT_KEY	1<<2
#define RIGHT_KEY	1<<3

#define START1_KEY	1<<4
#define START2_KEY	1<<5

#define BUTTON1_KEY	1<<6
#define BUTTON2_KEY	1<<7
#define BUTTON3_KEY	1<<8

#define COIN1_KEY	1<<9
#define COIN2_KEY	1<<10

#define SKILL1_KEY	1<<11
#define SKILL2_KEY	1<<12
#define SKILL3_KEY	1<<13

#define SERVICE_KEY	1<<14

#define PAUSE_KEY	1<<15
#define RESUME_KEY	1<<16


#if defined(__cplusplus) 
}
#endif

#endif
