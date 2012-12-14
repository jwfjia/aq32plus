/*
  October 2012

  aq32Plus Rev -

  Copyright (c) 2012 John Ihlein.  All rights reserved.

  Open Source STM32 Based Multicopter Controller Software

  Includes code and/or ideas from:

  1)AeroQuad
  2)BaseFlight
  3)CH Robotics
  4)MultiWii
  5)S.O.H. Madgwick
  6)UAVX

  Designed to run on the AQ32 Flight Control Board

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

///////////////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////////////
// LED Defines
////////////////////////////////////////////////////////////////////////////////

#define BLUE_LED_GPIO    GPIOE
#define BLUE_LED_PIN     GPIO_Pin_5
#define GREEN_LED_GPIO   GPIOE
#define GREEN_LED_PIN    GPIO_Pin_6

#define LED1_GPIO        GPIOD
#define LED1_PIN         GPIO_Pin_7
#define LED2_GPIO        GPIOE
#define LED2_PIN         GPIO_Pin_0
#define LED3_GPIO        GPIOE
#define LED3_PIN         GPIO_Pin_1
#define LED4_GPIO        GPIOD
#define LED4_PIN         GPIO_Pin_4

///////////////////////////////////////////////////////////////////////////////
// LED Initialization
///////////////////////////////////////////////////////////////////////////////

void ledInit();

///////////////////////////////////////////////////////////////////////////////
