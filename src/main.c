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

#include "board.h"

///////////////////////////////////////////////////////////////////////////////

__attribute__((__section__(".eeprom"), used)) const int8_t eepromArray[16384];

eepromConfig_t eepromConfig;

uint8_t        execUpCount = 0;

sensors_t      sensors;

uint16_t       timerValue;

///////////////////////////////////////////////////////////////////////////////

int main(void)
{
	///////////////////////////////////////////////////////////////////////////

	uint32_t currentTime;

    char numberString[12];

    systemInit();

    systemReady = true;

    while (1)
    {
    	///////////////////////////////

        if (frame_50Hz)
        {
        	frame_50Hz = false;

        	currentTime      = micros();
			deltaTime50Hz    = currentTime - previous50HzTime;
			previous50HzTime = currentTime;

			if (newMagData == true)
			{
				sensors.mag50Hz[XAXIS] =   (float)rawMag[XAXIS].value * magScaleFactor[XAXIS] - eepromConfig.magBias[XAXIS];
			    sensors.mag50Hz[YAXIS] =   (float)rawMag[YAXIS].value * magScaleFactor[YAXIS] - eepromConfig.magBias[YAXIS];
			    sensors.mag50Hz[ZAXIS] = -((float)rawMag[ZAXIS].value * magScaleFactor[ZAXIS] - eepromConfig.magBias[ZAXIS]);
			}

			processFlightCommands();

			executionTime50Hz = micros() - currentTime;
        }

        ///////////////////////////////

        if (frame_10Hz)
        {
        	frame_10Hz = false;

        	currentTime      = micros();
			deltaTime10Hz    = currentTime - previous10HzTime;
			previous10HzTime = currentTime;

        	d1Average = d1Sum / 10;
        	d1Sum = 0;
        	calculateTemperature();
        	calculatePressureAltitude();

        	pressureAltValid = true;

        	cliCom();

        	rfCom();

            executionTime10Hz = micros() - currentTime;
        }

        ///////////////////////////////

        if (frame_500Hz)
        {
			frame_500Hz = false;

       	    currentTime       = micros();
       	    deltaTime500Hz    = currentTime - previous500HzTime;
       	    previous500HzTime = currentTime;

       	    TIM_Cmd(TIM7, DISABLE);
       	 	timerValue = TIM_GetCounter(TIM7);
       	 	TIM_SetCounter(TIM7, 0);
       	 	TIM_Cmd(TIM7, ENABLE);

       	 	dt500Hz = (float)timerValue * 0.0000005f;  // For integrations in 500 Hz loop

            computeMPU6000TCBias();

            sensors.accel500Hz[XAXIS] =  ((float)accelSummedSamples500Hz[XAXIS] / 2.0f - accelTCBias[XAXIS]) * ACCEL_SCALE_FACTOR;
			sensors.accel500Hz[YAXIS] = -((float)accelSummedSamples500Hz[YAXIS] / 2.0f - accelTCBias[YAXIS]) * ACCEL_SCALE_FACTOR;
			sensors.accel500Hz[ZAXIS] = -((float)accelSummedSamples500Hz[ZAXIS] / 2.0f - accelTCBias[ZAXIS]) * ACCEL_SCALE_FACTOR;

            sensors.gyro500Hz[ROLL ] =  ((float)gyroSummedSamples500Hz[ROLL]  / 2.0f - gyroRTBias[ROLL ] - gyroTCBias[ROLL ]) * GYRO_SCALE_FACTOR;
			sensors.gyro500Hz[PITCH] = -((float)gyroSummedSamples500Hz[PITCH] / 2.0f - gyroRTBias[PITCH] - gyroTCBias[PITCH]) * GYRO_SCALE_FACTOR;
            sensors.gyro500Hz[YAW  ] = -((float)gyroSummedSamples500Hz[YAW]   / 2.0f - gyroRTBias[YAW  ] - gyroTCBias[YAW  ]) * GYRO_SCALE_FACTOR;

            MargAHRSupdate( sensors.gyro500Hz[ROLL],   sensors.gyro500Hz[PITCH],  sensors.gyro500Hz[YAW],
                            sensors.accel500Hz[XAXIS], sensors.accel500Hz[YAXIS], sensors.accel500Hz[ZAXIS],
                            sensors.mag50Hz[XAXIS],    sensors.mag50Hz[YAXIS],    sensors.mag50Hz[ZAXIS],
                            eepromConfig.accelCutoff,
                            newMagData,
                            dt500Hz );

            newMagData = false;

            computeAxisCommands(dt500Hz);
            mixTable();
            writeServos();
            writeMotors();

       	    executionTime500Hz = micros() - currentTime;
		}

        ///////////////////////////////

        if (frame_200Hz)
        {
        	frame_200Hz = false;

       	    currentTime       = micros();
       	    deltaTime200Hz    = currentTime - previous200HzTime;
       	    previous200HzTime = currentTime;

       	    TIM_Cmd(TIM10, DISABLE);
       	    timerValue = TIM_GetCounter(TIM10);
       	    TIM_SetCounter(TIM10, 0);
       	    TIM_Cmd(TIM10, ENABLE);

       	    dt200Hz = (float)timerValue * 0.0000005f;  // For integrations in 200 Hz loop

            executionTime200Hz = micros() - currentTime;
        }

        ///////////////////////////////

        if (frame_100Hz)
        {
        	frame_100Hz = false;

        	currentTime       = micros();
			deltaTime100Hz    = currentTime - previous100HzTime;
			previous100HzTime = currentTime;

			TIM_Cmd(TIM11, DISABLE);
			timerValue = TIM_GetCounter(TIM11);
			TIM_SetCounter(TIM11, 0);
			TIM_Cmd(TIM11, ENABLE);

			dt100Hz = (float)timerValue * 0.0000005f;  // For integrations in 100 Hz loop

			sensors.accel100Hz[XAXIS] =  ((float)accelSummedSamples100Hz[XAXIS] / 10.0f - accelTCBias[XAXIS]) * ACCEL_SCALE_FACTOR;
			sensors.accel100Hz[YAXIS] = -((float)accelSummedSamples100Hz[YAXIS] / 10.0f - accelTCBias[YAXIS]) * ACCEL_SCALE_FACTOR;
			sensors.accel100Hz[ZAXIS] = -((float)accelSummedSamples100Hz[ZAXIS] / 10.0f - accelTCBias[ZAXIS]) * ACCEL_SCALE_FACTOR;

        	createRotationMatrix();
        	bodyAccelToEarthAccel();
        	vertCompFilter(dt100Hz);

        	// High Speed Telemetry Test Code Begin

            if ( highSpeedTelem1Enabled == true )
            {
            	// 500 Hz Accels
            	ftoa(sensors.accel500Hz[XAXIS], numberString); uart3Print(numberString); uart3Print(",");
            	ftoa(sensors.accel500Hz[YAXIS], numberString); uart3Print(numberString); uart3Print(",");
            	ftoa(sensors.accel500Hz[ZAXIS], numberString); uart3Print(numberString); uart3Print("\n");
            }

            if ( highSpeedTelem2Enabled == true )
            {
            	// 500 Hz Gyros
            	ftoa(sensors.gyro500Hz[ROLL ], numberString); uart3Print(numberString); uart3Print(",");
            	ftoa(sensors.gyro500Hz[PITCH], numberString); uart3Print(numberString); uart3Print(",");
            	ftoa(sensors.gyro500Hz[YAW  ], numberString); uart3Print(numberString); uart3Print("\n");
            }

            if ( highSpeedTelem3Enabled == true )
            {
            	// Roll Rate, Roll Rate Command
            	ftoa(sensors.gyro500Hz[ROLL],        numberString); uart3Print(numberString); uart3Print(",");
            	ftoa(rxCommand[ROLL] * RATE_SCALING, numberString); uart3Print(numberString); uart3Print("\n");
            }

            if ( highSpeedTelem4Enabled == true )
            {
            	// Pitch Rate, Pitch Rate Command
            	ftoa(sensors.gyro500Hz[PITCH],        numberString); uart3Print(numberString); uart3Print(",");
            	ftoa(rxCommand[PITCH] * RATE_SCALING, numberString); uart3Print(numberString); uart3Print("\n");
            }

            if ( highSpeedTelem5Enabled == true )
            {
            	// Yaw Rate, Yaw Rate Command
            	ftoa(sensors.gyro500Hz[YAW],        numberString); uart3Print(numberString); uart3Print(",");
            	ftoa(rxCommand[YAW] * RATE_SCALING, numberString); uart3Print(numberString); uart3Print("\n");
            }

            if ( highSpeedTelem6Enabled == true )
            {
            	// 500 Hz Attitudes
            	ftoa(sensors.attitude500Hz[ROLL ], numberString); uart3Print(numberString); uart3Print(",");
            	ftoa(sensors.attitude500Hz[PITCH], numberString); uart3Print(numberString); uart3Print(",");
            	ftoa(sensors.attitude500Hz[YAW  ], numberString); uart3Print(numberString); uart3Print("\n");
            }

            if ( highSpeedTelem7Enabled == true )
            {
               	// Vertical Variables
               	ftoa(earthAxisAccels[ZAXIS], numberString); uart3Print(numberString); uart3Print(",");
               	ftoa(sensors.pressureAlt,    numberString); uart3Print(numberString); uart3Print(",");
                ftoa(hDotEstimate,           numberString); uart3Print(numberString); uart3Print(", ");
                ftoa(hEstimate,              numberString); uart3Print(numberString); uart3Print("\n");
            }

            // High Speed Telemetry Test Code End

            executionTime100Hz = micros() - currentTime;
        }

        ///////////////////////////////

        if (frame_5Hz)
        {
        	frame_5Hz = false;

        	currentTime     = micros();
			deltaTime5Hz    = currentTime - previous5HzTime;
			previous5HzTime = currentTime;

			if (execUp == true)
			    GPIO_ToggleBits(BLUE_LED_GPIO,  BLUE_LED_PIN);

        	executionTime5Hz = micros() - currentTime;
        }

        ///////////////////////////////

        if (frame_1Hz)
        {
        	frame_1Hz = false;

        	currentTime     = micros();
			deltaTime1Hz    = currentTime - previous1HzTime;
			previous1HzTime = currentTime;

			if (execUp == true)
			    GPIO_ToggleBits(GREEN_LED_GPIO,  GREEN_LED_PIN);

			if (execUp == false)
			    execUpCount++;

			if (execUpCount == 5)
			    execUp = true;

			executionTime1Hz = micros() - currentTime;
        }

        ////////////////////////////////
    }

    ///////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////
