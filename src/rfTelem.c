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

uint8_t rfBusy = false;

static volatile uint8_t queryType;

///////////////////////////////////////////////////////////////////////////////
// Read Character String from RF Comm
///////////////////////////////////////////////////////////////////////////////

char *readStringRF(char *data, uint8_t length)
{
    uint8_t index    = 0;
    uint8_t timeout  = 0;

    do
    {
        if ((data[index] = uart3Read()) == 0)
        {
            delay(10);
            timeout++;
        }
        else
        {
            timeout = 0;
            index++;
        }
    }
    while ((index == 0 || data[index-1] != ';') && (timeout < 5) && (index < length));

    data[index] = '\0';

    return data;
}

///////////////////////////////////////////////////////////////////////////////
// Read Float from RF Comm
///////////////////////////////////////////////////////////////////////////////

float readFloatRF(void)
{
    uint8_t index    = 0;
    uint8_t timeout  = 0;
    char    data[13] = "";

    do
    {
        if ((data[index] = uart3Read()) == 0)
        {
            delay(10);
            timeout++;
        }
        else
        {
            timeout = 0;
            index++;
        }
    }
    while ((index == 0 || data[index-1] != ';') && (timeout < 5) && (index < sizeof(data)-1));

    data[index] = '\0';

    return stringToFloat(data);
}

///////////////////////////////////////////////////////////////////////////////
// Read PID Values from RF Comm
///////////////////////////////////////////////////////////////////////////////

void readRFPID(unsigned char PIDid)
{
  struct PIDdata* pid = &eepromConfig.PID[PIDid];

  pid->B              = readFloatRF();
  pid->P              = readFloatRF();
  pid->I              = readFloatRF();
  pid->D              = readFloatRF();
  pid->windupGuard    = readFloatRF();
  pid->iTerm          = 0.0f;
  pid->lastDcalcValue = 0.0f;
  pid->lastDterm      = 0.0f;
  pid->lastLastDterm  = 0.0f;
  pid->dErrorCalc     = (uint8_t)readFloatRF();
}

///////////////////////////////////////////////////////////////////////////////
// RF Telem Communication
///////////////////////////////////////////////////////////////////////////////

void rfCom(void)
{
    uint8_t index;
    uint8_t rows, columns;

    if (uart3Available())
    	queryType = uart3Read();

    switch (queryType)
    {
        ///////////////////////////////

         case 'h': // 500 Hz Gyro Data
        	ftoa( sensors.gyro500Hz[ROLL ] * 57.3f, numberString ); uart3Print(numberString); uart3Print(", ");
        	ftoa( sensors.gyro500Hz[PITCH] * 57.3f, numberString ); uart3Print(numberString); uart3Print(", ");
        	ftoa( sensors.gyro500Hz[YAW  ] * 57.3f, numberString ); uart3Print(numberString); uart3Print(", ");
        	ftoa( mpu6000Temperature,               numberString ); uart3Print(numberString); uart3Print("\n");

        	break;

        ///////////////////////////////

        case 'i': // 500 Hz Accel Data
        	ftoa( sensors.accel500Hz[XAXIS], numberString ); uart3Print(numberString); uart3Print(", ");
        	ftoa( sensors.accel500Hz[YAXIS], numberString ); uart3Print(numberString); uart3Print(", ");
        	ftoa( sensors.accel500Hz[ZAXIS], numberString ); uart3Print(numberString); uart3Print("\n");

        	break;

        ///////////////////////////////

        case 'j': // 50 Hz Mag Data
        	ftoa( sensors.mag10Hz[XAXIS], numberString ); uart3Print(numberString); uart3Print(", ");
        	ftoa( sensors.mag10Hz[YAXIS], numberString ); uart3Print(numberString); uart3Print(", ");
        	ftoa( sensors.mag10Hz[ZAXIS], numberString ); uart3Print(numberString); uart3Print("\n");

        	break;

        ///////////////////////////////

        case 'k': // 10Hz Pressure Altitude
        	ftoa(sensors.pressureAlt10Hz, numberString); uart3Print(numberString); uart3Print("\n");

        	break;

        ///////////////////////////////

        case 'l': // Euler Angles
        	ftoa( sensors.attitude500Hz[ROLL ]   * 57.3f, numberString ); uart3Print(numberString); uart3Print(", ");
        	ftoa( sensors.attitude500Hz[PITCH]   * 57.3f, numberString ); uart3Print(numberString); uart3Print(", ");
        	ftoa( sensors.attitude500Hz[YAW  ]   * 57.3f, numberString ); uart3Print(numberString); uart3Print("\n");

        	break;

        ///////////////////////////////

        case 'q': // Rate PIDs
            uart3Print("\n");

            uart3Print("Roll Rate PID:  ");
            ftoa(eepromConfig.PID[ROLL_RATE_PID].B,            numberString); uart3Print(numberString); uart3Print(", ");
            ftoa(eepromConfig.PID[ROLL_RATE_PID].P,            numberString); uart3Print(numberString); uart3Print(", ");
            ftoa(eepromConfig.PID[ROLL_RATE_PID].I,            numberString); uart3Print(numberString); uart3Print(", ");
            ftoa(eepromConfig.PID[ROLL_RATE_PID].D,            numberString); uart3Print(numberString); uart3Print(", ");
            ftoa(eepromConfig.PID[ROLL_RATE_PID].windupGuard,  numberString); uart3Print(numberString); uart3Print(", ");
            if  (eepromConfig.PID[ROLL_RATE_PID].dErrorCalc)
                uart3Print("Error\n");
            else
                uart3Print("State\n");

            uart3Print("Pitch Rate PID: ");
            ftoa(eepromConfig.PID[PITCH_RATE_PID].B,           numberString); uart3Print(numberString); uart3Print(", ");
            ftoa(eepromConfig.PID[PITCH_RATE_PID].P,           numberString); uart3Print(numberString); uart3Print(", ");
            ftoa(eepromConfig.PID[PITCH_RATE_PID].I,           numberString); uart3Print(numberString); uart3Print(", ");
            ftoa(eepromConfig.PID[PITCH_RATE_PID].D,           numberString); uart3Print(numberString); uart3Print(", ");
            ftoa(eepromConfig.PID[PITCH_RATE_PID].windupGuard, numberString); uart3Print(numberString); uart3Print(", ");
            if  (eepromConfig.PID[PITCH_RATE_PID].dErrorCalc)
                uart3Print("Error\n");
            else
                uart3Print("State\n");

            uart3Print("Yaw Rate PID:   ");
            ftoa(eepromConfig.PID[YAW_RATE_PID].B,             numberString); uart3Print(numberString); uart3Print(", ");
            ftoa(eepromConfig.PID[YAW_RATE_PID].P,             numberString); uart3Print(numberString); uart3Print(", ");
            ftoa(eepromConfig.PID[YAW_RATE_PID].I,             numberString); uart3Print(numberString); uart3Print(", ");
            ftoa(eepromConfig.PID[YAW_RATE_PID].D,             numberString); uart3Print(numberString); uart3Print(", ");
            ftoa(eepromConfig.PID[YAW_RATE_PID].windupGuard,   numberString); uart3Print(numberString); uart3Print(", ");
            if  (eepromConfig.PID[YAW_RATE_PID].dErrorCalc)
                uart3Print("Error\n");
            else
                uart3Print("State\n");

            queryType = 'x';
            break;

        ///////////////////////////////

        case 'r': // Attitude PIDs
            uart3Print("\n");

            uart3Print("Roll Attitude PID:  ");
            ftoa(eepromConfig.PID[ROLL_ATT_PID].B,            numberString); uart3Print(numberString); uart3Print(", ");
            ftoa(eepromConfig.PID[ROLL_ATT_PID].P,            numberString); uart3Print(numberString); uart3Print(", ");
            ftoa(eepromConfig.PID[ROLL_ATT_PID].I,            numberString); uart3Print(numberString); uart3Print(", ");
            ftoa(eepromConfig.PID[ROLL_ATT_PID].D,            numberString); uart3Print(numberString); uart3Print(", ");
            ftoa(eepromConfig.PID[ROLL_ATT_PID].windupGuard,  numberString); uart3Print(numberString); uart3Print(", ");
            if  (eepromConfig.PID[ROLL_ATT_PID].dErrorCalc)
                uart3Print("Error\n");
            else
                uart3Print("State\n");

            uart3Print("Pitch Attitude PID: ");
            ftoa(eepromConfig.PID[PITCH_ATT_PID].B,           numberString); uart3Print(numberString); uart3Print(", ");
            ftoa(eepromConfig.PID[PITCH_ATT_PID].P,           numberString); uart3Print(numberString); uart3Print(", ");
            ftoa(eepromConfig.PID[PITCH_ATT_PID].I,           numberString); uart3Print(numberString); uart3Print(", ");
            ftoa(eepromConfig.PID[PITCH_ATT_PID].D,           numberString); uart3Print(numberString); uart3Print(", ");
            ftoa(eepromConfig.PID[PITCH_ATT_PID].windupGuard, numberString); uart3Print(numberString); uart3Print(", ");
            if  (eepromConfig.PID[PITCH_ATT_PID].dErrorCalc)
                uart3Print("Error\n");
            else
                uart3Print("State\n");

            uart3Print("Heading PID:        ");
            ftoa(eepromConfig.PID[HEADING_PID].B,             numberString); uart3Print(numberString); uart3Print(", ");
            ftoa(eepromConfig.PID[HEADING_PID].P,             numberString); uart3Print(numberString); uart3Print(", ");
            ftoa(eepromConfig.PID[HEADING_PID].I,             numberString); uart3Print(numberString); uart3Print(", ");
            ftoa(eepromConfig.PID[HEADING_PID].D,             numberString); uart3Print(numberString); uart3Print(", ");
            ftoa(eepromConfig.PID[HEADING_PID].windupGuard,   numberString); uart3Print(numberString); uart3Print(", ");
            if  (eepromConfig.PID[HEADING_PID].dErrorCalc)
                uart3Print("Error\n");
            else
                uart3Print("State\n");

            queryType = 'x';
            break;

        ///////////////////////////////

        case 's': // Altitude PIDs
        	queryType = 'x';
        	break;

        ///////////////////////////////

        case 'v': // Free Mix Matrix
        	uart3Print("\nNumber of Free Mixer Motors:  ");
        	itoa( eepromConfig.freeMixMotors, numberString, 10 ); uart3Print( numberString ); uart3Print("\n\n");
            uart3Print("         Roll    Pitch   Yaw\n");

        	for ( index = 0; index < eepromConfig.freeMixMotors; index++ )
        	{
        		uart3Print("Motor"); itoa(index, numberString, 10);     uart3Print(numberString); uart3Print("  ");
        		ftoa(eepromConfig.freeMix[index][ROLL ], numberString); uart3Print(numberString); uart3Print("  ");
        		ftoa(eepromConfig.freeMix[index][PITCH], numberString); uart3Print(numberString); uart3Print("  ");
        		ftoa(eepromConfig.freeMix[index][YAW  ], numberString); uart3Print(numberString); uart3Print("\n");
        	}

        	queryType = 'x';
        	break;

        ///////////////////////////////

        case 'x':
        	break;

        ///////////////////////////////

        case '1': // Turn high speed telemetry 1 on
        	highSpeedTelemDisable();
          	highSpeedTelem1Enabled = true;

        	queryType = 'x';
            break;

        ///////////////////////////////

        case '2': // Turn high speed telemetry 2 on
           	highSpeedTelemDisable();
           	highSpeedTelem2Enabled = true;

            queryType = 'x';
           	break;

        ///////////////////////////////

        case '3': // Turn high speed telemetry 3 on
           	highSpeedTelemDisable();
           	highSpeedTelem3Enabled = true;

            queryType = 'x';
           	break;

        ///////////////////////////////

        case '4': // Turn high speed telemetry 4 on
           	highSpeedTelemDisable();
           	highSpeedTelem4Enabled = true;

            queryType = 'x';
           	break;

        ///////////////////////////////

        case '5': // Turn high speed telemetry 5 on
           	highSpeedTelemDisable();
           	highSpeedTelem5Enabled = true;

            queryType = 'x';
           	break;

        ///////////////////////////////

        case '6': // Turn high speed telemetry 6 on
           	highSpeedTelemDisable();
           	highSpeedTelem6Enabled = true;

            queryType = 'x';
           	break;

        ///////////////////////////////

        case '7': // Turn high speed telemetry 7 on
           	highSpeedTelemDisable();
           	highSpeedTelem7Enabled = true;

            queryType = 'x';
           	break;

        ///////////////////////////////

        case '8': // Turn high speed telemetry 8 on
           	highSpeedTelemDisable();
           	highSpeedTelem8Enabled = true;

            queryType = 'x';
           	break;

        ///////////////////////////////

        case '9': // Turn high speed telemetry 9 on
           	highSpeedTelemDisable();
           	highSpeedTelem9Enabled = true;

            queryType = 'x';
           	break;

        ///////////////////////////////

        case '0': // Disable high speed telemetry 0
           	highSpeedTelemDisable();

            queryType = 'x';
           	break;


        ///////////////////////////////

        ///////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////

        ///////////////////////////////

        case 'A': // Read Roll Rate PID Values
            readRFPID(ROLL_RATE_PID);
            uart3Print( "\nRoll Rate PID Received....\n" );

        	queryType = 'q';
        	break;

        ///////////////////////////////

        case 'B': // Read Pitch Rate PID Values
            readRFPID(PITCH_RATE_PID);
            uart3Print( "\nPitch Rate PID Received....\n" );

        	queryType = 'q';
        	break;

        ///////////////////////////////

        case 'C': // Read Yaw Rate PID Values
            readRFPID(YAW_RATE_PID);
            uart3Print( "\nYaw Rate PID Received....\n" );

        	queryType = 'q';
        	break;

        ///////////////////////////////

        case 'D': // Read Roll Attitude PID Values
            readRFPID(ROLL_ATT_PID);
            uart3Print( "\nRoll Attitude PID Received....\n" );

        	queryType = 'r';
        	break;

        ///////////////////////////////

        case 'E': // Read Pitch Attitude PID Values
            readRFPID(PITCH_ATT_PID);
            uart3Print( "\nPitch Attitude PID Received....\n" );

        	queryType = 'r';
        	break;

        ///////////////////////////////

        case 'F': // Read Heading Hold PID Values
            readRFPID(HEADING_PID);
            uart3Print( "\nHeading PID Received....\n" );

        	queryType = 'r';
        	break;

        ///////////////////////////////

        case 'G': // Read Altitude Hold PID Values
        	queryType = 'x';
        	break;

        ///////////////////////////////

        case 'X': // Read Free Mix Matrix Element
            rows    = (uint8_t)readFloatRF();
            columns = (uint8_t)readFloatRF();
            eepromConfig.freeMix[rows][columns] = readFloatRF();
            uart3Print( "\nFree Mix Value Received....\n");

            queryType = 'v';
            break;

        ///////////////////////////////

        case 'Y': // Read Accel Cutoff
            eepromConfig.accelCutoff = readFloatRF();
            uart3Print( "\nAccel Cutoff Received....\n" );

            queryType = 'x';
        	break;

        ///////////////////////////////

        case '!': // Read kpAcc, kiAcc
            eepromConfig.KpAcc = readFloatRF();
            eepromConfig.KiAcc = readFloatRF();
            uart3Print("\nkpAcc and kiAcc Received....\n");

            queryType = 'x';
            break;

        ///////////////////////////////

        case '@': // Read kpMag, kiMag
            eepromConfig.KpMag = readFloatRF();
            eepromConfig.KiMag = readFloatRF();
            uart3Print("\nkpMag and kiMaf Received....\n");

            queryType = 'x';
            break;

        ///////////////////////////////
    }
}

///////////////////////////////////////////////////////////////////////////////
