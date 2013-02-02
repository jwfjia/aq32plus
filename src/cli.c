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

uint8_t cliBusy = false;

static volatile uint8_t queryType;
static volatile uint8_t validCommand = false;

uint8_t highSpeedTelem1Enabled = false;
uint8_t highSpeedTelem2Enabled = false;
uint8_t highSpeedTelem3Enabled = false;
uint8_t highSpeedTelem4Enabled = false;
uint8_t highSpeedTelem5Enabled = false;
uint8_t highSpeedTelem6Enabled = false;
uint8_t highSpeedTelem7Enabled = false;
uint8_t highSpeedTelem8Enabled = false;
uint8_t highSpeedTelem9Enabled = false;

///////////////////////////////////////////////////////////////////////////////
// High Speed Telem Disable
///////////////////////////////////////////////////////////////////////////////

void highSpeedTelemDisable(void)
{
	highSpeedTelem1Enabled = false;
	highSpeedTelem2Enabled = false;
	highSpeedTelem3Enabled = false;
	highSpeedTelem4Enabled = false;
	highSpeedTelem5Enabled = false;
	highSpeedTelem6Enabled = false;
	highSpeedTelem7Enabled = false;
	highSpeedTelem8Enabled = false;
	highSpeedTelem9Enabled = false;
}

///////////////////////////////////////////////////////////////////////////////
// Read Character String from USB Comm
///////////////////////////////////////////////////////////////////////////////

char *readStringUsb(char *data, uint8_t length)
{
    uint8_t index    = 0;
    uint8_t timeout  = 0;

    do
    {
        if ((data[index] = usbRead()) == 0)
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
// Read Float from USB Comm
///////////////////////////////////////////////////////////////////////////////

float readFloatUsb(void)
{
    uint8_t index    = 0;
    uint8_t timeout  = 0;
    char    data[13] = "";

    do
    {
        if ((data[index] = usbRead()) == 0)
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
// Read PID Values from USB Comm
///////////////////////////////////////////////////////////////////////////////

void readUsbPID(unsigned char PIDid)
{
  struct PIDdata* pid = &eepromConfig.PID[PIDid];

  pid->B              = readFloatUsb();
  pid->P              = readFloatUsb();
  pid->I              = readFloatUsb();
  pid->D              = readFloatUsb();
  pid->windupGuard    = readFloatUsb();
  pid->iTerm          = 0.0f;
  pid->lastDcalcValue = 0.0f;
  pid->lastDterm      = 0.0f;
  pid->lastLastDterm  = 0.0f;
  pid->dErrorCalc     =(uint8_t)readFloatUsb();
}

///////////////////////////////////////////////////////////////////////////////
// CLI Communication
///////////////////////////////////////////////////////////////////////////////

void cliCom(void)
{
    char    rcOrderString[9];
    float   tempFloat;
    uint8_t tempInt;
    uint8_t index;
    uint8_t rows, columns;

    if ((usbAvailable() && !validCommand))
    	queryType = usbRead();

    switch (queryType)
    {
        ///////////////////////////////

        case 'a': // Loop Delta Times
        	itoa( deltaTime1000Hz, numberString, 10 ); usbPrint(numberString); usbPrint(", ");
        	itoa( deltaTime500Hz,  numberString, 10 ); usbPrint(numberString); usbPrint(", ");
        	itoa( deltaTime100Hz,  numberString, 10 ); usbPrint(numberString); usbPrint(", ");
        	itoa( deltaTime50Hz,   numberString, 10 ); usbPrint(numberString); usbPrint(", ");
        	itoa( deltaTime10Hz,   numberString, 10 ); usbPrint(numberString); usbPrint(", ");
        	itoa( deltaTime5Hz,    numberString, 10 ); usbPrint(numberString); usbPrint(", ");
        	itoa( deltaTime1Hz,    numberString, 10 ); usbPrint(numberString); usbPrint("\n");

        	validCommand = false;
        	break;

        ///////////////////////////////

        case 'b': // Loop Execution Times
        	itoa( executionTime1000Hz,  numberString, 10 ); usbPrint(numberString); usbPrint(", ");
        	itoa( executionTime500Hz,   numberString, 10 ); usbPrint(numberString); usbPrint(", ");
        	itoa( executionTime100Hz,   numberString, 10 ); usbPrint(numberString); usbPrint(", ");
        	itoa( executionTime50Hz,    numberString, 10 ); usbPrint(numberString); usbPrint(", ");
        	itoa( executionTime10Hz,    numberString, 10 ); usbPrint(numberString); usbPrint(", ");
        	itoa( executionTime5Hz,     numberString, 10 ); usbPrint(numberString); usbPrint(", ");
        	itoa( executionTime1Hz,     numberString, 10 ); usbPrint(numberString); usbPrint("\n");

        	validCommand = false;
        	break;

        ///////////////////////////////

        case 'c': // MPU6000 Calibration
            mpu6000Calibration();

            queryType = 'x';
            validCommand = false;
            break;

        ///////////////////////////////

        case 'd': // Mag Calibration
            magCalibration(HMC5883L_I2C);

            queryType = 'x';
            validCommand = false;
            break;

        ///////////////////////////////

        case 'e': // ESC Calibration
        	escCalibration();

        	queryType = 'x';
        	validCommand = false;
        	break;

        ///////////////////////////////

        case 'f': // 500 Hz Gyro Data
        	ftoa( sensors.gyro500Hz[ROLL ] * 57.3f, numberString ); usbPrint(numberString); usbPrint(", ");
        	ftoa( sensors.gyro500Hz[PITCH] * 57.3f, numberString ); usbPrint(numberString); usbPrint(", ");
        	ftoa( sensors.gyro500Hz[YAW  ] * 57.3f, numberString ); usbPrint(numberString); usbPrint(", ");
        	ftoa( mpu6000Temperature,               numberString ); usbPrint(numberString); usbPrint("\n");

        	validCommand = false;
        	break;

        ///////////////////////////////

        case 'g': // 500 Hz Accel Data
        	ftoa( sensors.accel500Hz[XAXIS], numberString ); usbPrint(numberString); usbPrint(", ");
        	ftoa( sensors.accel500Hz[YAXIS], numberString ); usbPrint(numberString); usbPrint(", ");
        	ftoa( sensors.accel500Hz[ZAXIS], numberString ); usbPrint(numberString); usbPrint("\n");

        	validCommand = false;
        	break;

        ///////////////////////////////

        case 'h': // Earth Axis Accel Data
        	ftoa( earthAxisAccels[XAXIS], numberString ); usbPrint(numberString); usbPrint(", ");
        	ftoa( earthAxisAccels[YAXIS], numberString ); usbPrint(numberString); usbPrint(", ");
        	ftoa( earthAxisAccels[ZAXIS], numberString ); usbPrint(numberString); usbPrint("\n");

        	validCommand = false;
        	break;

        ///////////////////////////////

        case 'i': // 10 Hz Mag Data
        	ftoa( sensors.mag10Hz[XAXIS], numberString ); usbPrint(numberString); usbPrint(", ");
        	ftoa( sensors.mag10Hz[YAXIS], numberString ); usbPrint(numberString); usbPrint(", ");
        	ftoa( sensors.mag10Hz[ZAXIS], numberString ); usbPrint(numberString); usbPrint("\n");

        	validCommand = false;
        	break;

        ///////////////////////////////

        case 'j': // Vertical Axis Variables
        	ftoa( earthAxisAccels[ZAXIS],  numberString ); usbPrint(numberString); usbPrint(", ");
        	ftoa( sensors.pressureAlt10Hz, numberString ); usbPrint(numberString); usbPrint(", ");
        	ftoa( hDotEstimate,            numberString ); usbPrint(numberString); usbPrint(", ");
        	ftoa( hEstimate,               numberString ); usbPrint(numberString); usbPrint("\n");

        	validCommand = false;
        	break;

        ///////////////////////////////

        case 'k': // Euler Angles
        	ftoa( sensors.attitude500Hz[ROLL ]   * 57.3f, numberString ); usbPrint(numberString); usbPrint(", ");
        	ftoa( sensors.attitude500Hz[PITCH]   * 57.3f, numberString ); usbPrint(numberString); usbPrint(", ");
        	ftoa( sensors.attitude500Hz[YAW  ]   * 57.3f, numberString ); usbPrint(numberString); usbPrint("\n");

        	validCommand = false;
        	break;

        ///////////////////////////////

        case 'l': // Not Used
        	queryType = 'x';
        	validCommand = false;
        	break;

        ///////////////////////////////

        case 'm': // Processed Receiver Commands
            for (index = 0; index < 7; index++)
            {
    			ftoa( rxCommand[index], numberString );
    			usbPrint( numberString ); usbPrint( ", " );
    		}

            ftoa( rxCommand[7], numberString );
            usbPrint( numberString ); usbPrint( "\n" );

            validCommand = false;
            break;

        ///////////////////////////////

        case 'n': // Command in Detent Discretes
            if ( commandInDetent[ROLL] == true )
                usbPrint( "true" );
            else
                usbPrint( "false" );
            usbPrint(", ");

            if ( commandInDetent[PITCH] == true )
                usbPrint( "true" );
            else
                usbPrint( "false" );
            usbPrint(", ");

            if ( commandInDetent[YAW] == true )
                usbPrint( "true" );
            else
                usbPrint( "false" );
            usbPrint("\n");

            validCommand = false;
            break;

        ///////////////////////////////

        case 'o': // ESC PWM Outputs
            itoa(TIM8->CCR4, numberString, 10); usbPrint(numberString); usbPrint(", ");
            itoa(TIM8->CCR3, numberString, 10); usbPrint(numberString); usbPrint(", ");
            itoa(TIM8->CCR2, numberString, 10); usbPrint(numberString); usbPrint(", ");
            itoa(TIM8->CCR1, numberString, 10); usbPrint(numberString); usbPrint(", ");
            itoa(TIM2->CCR2, numberString, 10); usbPrint(numberString); usbPrint(", ");
            itoa(TIM3->CCR1, numberString, 10); usbPrint(numberString); usbPrint(", ");
            itoa(TIM3->CCR2, numberString, 10); usbPrint(numberString); usbPrint(", ");
            itoa(TIM2->CCR1, numberString, 10); usbPrint(numberString); usbPrint("\n");

            validCommand = false;
            break;

        ///////////////////////////////

        case 'p': // Servo PWM Outputs
            itoa(TIM5->CCR3, numberString, 10); usbPrint(numberString); usbPrint(", ");
            itoa(TIM5->CCR2, numberString, 10); usbPrint(numberString); usbPrint(", ");
            itoa(TIM5->CCR1, numberString, 10); usbPrint(numberString); usbPrint("\n");

            validCommand = false;
            break;

        ///////////////////////////////

        case 'q': // Rate PIDs
            usbPrint("\n");

            usbPrint("Roll Rate PID:  ");
            ftoa(eepromConfig.PID[ROLL_RATE_PID].B,            numberString); usbPrint(numberString); usbPrint(", ");
            ftoa(eepromConfig.PID[ROLL_RATE_PID].P,            numberString); usbPrint(numberString); usbPrint(", ");
            ftoa(eepromConfig.PID[ROLL_RATE_PID].I,            numberString); usbPrint(numberString); usbPrint(", ");
            ftoa(eepromConfig.PID[ROLL_RATE_PID].D,            numberString); usbPrint(numberString); usbPrint(", ");
            ftoa(eepromConfig.PID[ROLL_RATE_PID].windupGuard,  numberString); usbPrint(numberString); usbPrint(", ");
            if  (eepromConfig.PID[ROLL_RATE_PID].dErrorCalc)
                usbPrint("Error\n");
            else
                usbPrint("State\n");

            usbPrint("Pitch Rate PID: ");
            ftoa(eepromConfig.PID[PITCH_RATE_PID].B,           numberString); usbPrint(numberString); usbPrint(", ");
            ftoa(eepromConfig.PID[PITCH_RATE_PID].P,           numberString); usbPrint(numberString); usbPrint(", ");
            ftoa(eepromConfig.PID[PITCH_RATE_PID].I,           numberString); usbPrint(numberString); usbPrint(", ");
            ftoa(eepromConfig.PID[PITCH_RATE_PID].D,           numberString); usbPrint(numberString); usbPrint(", ");
            ftoa(eepromConfig.PID[PITCH_RATE_PID].windupGuard, numberString); usbPrint(numberString); usbPrint(", ");
            if  (eepromConfig.PID[PITCH_RATE_PID].dErrorCalc)
                usbPrint("Error\n");
            else
                usbPrint("State\n");

            usbPrint("Yaw Rate PID:   ");
            ftoa(eepromConfig.PID[YAW_RATE_PID].B,             numberString); usbPrint(numberString); usbPrint(", ");
            ftoa(eepromConfig.PID[YAW_RATE_PID].P,             numberString); usbPrint(numberString); usbPrint(", ");
            ftoa(eepromConfig.PID[YAW_RATE_PID].I,             numberString); usbPrint(numberString); usbPrint(", ");
            ftoa(eepromConfig.PID[YAW_RATE_PID].D,             numberString); usbPrint(numberString); usbPrint(", ");
            ftoa(eepromConfig.PID[YAW_RATE_PID].windupGuard,   numberString); usbPrint(numberString); usbPrint(", ");
            if  (eepromConfig.PID[YAW_RATE_PID].dErrorCalc)
                usbPrint("Error\n");
            else
                usbPrint("State\n");

            queryType = 'x';
            validCommand = false;
            break;

        ///////////////////////////////

        case 'r': // Attitude PIDs
            usbPrint("\n");

            usbPrint("Roll Attitude PID:  ");
            ftoa(eepromConfig.PID[ROLL_ATT_PID].B,            numberString); usbPrint(numberString); usbPrint(", ");
            ftoa(eepromConfig.PID[ROLL_ATT_PID].P,            numberString); usbPrint(numberString); usbPrint(", ");
            ftoa(eepromConfig.PID[ROLL_ATT_PID].I,            numberString); usbPrint(numberString); usbPrint(", ");
            ftoa(eepromConfig.PID[ROLL_ATT_PID].D,            numberString); usbPrint(numberString); usbPrint(", ");
            ftoa(eepromConfig.PID[ROLL_ATT_PID].windupGuard,  numberString); usbPrint(numberString); usbPrint(", ");
            if  (eepromConfig.PID[ROLL_ATT_PID].dErrorCalc)
                usbPrint("Error\n");
            else
                usbPrint("State\n");

            usbPrint("Pitch Attitude PID: ");
            ftoa(eepromConfig.PID[PITCH_ATT_PID].B,           numberString); usbPrint(numberString); usbPrint(", ");
            ftoa(eepromConfig.PID[PITCH_ATT_PID].P,           numberString); usbPrint(numberString); usbPrint(", ");
            ftoa(eepromConfig.PID[PITCH_ATT_PID].I,           numberString); usbPrint(numberString); usbPrint(", ");
            ftoa(eepromConfig.PID[PITCH_ATT_PID].D,           numberString); usbPrint(numberString); usbPrint(", ");
            ftoa(eepromConfig.PID[PITCH_ATT_PID].windupGuard, numberString); usbPrint(numberString); usbPrint(", ");
            if  (eepromConfig.PID[PITCH_ATT_PID].dErrorCalc)
                usbPrint("Error\n");
            else
                usbPrint("State\n");

            usbPrint("Heading PID:        ");
            ftoa(eepromConfig.PID[HEADING_PID].B,             numberString); usbPrint(numberString); usbPrint(", ");
            ftoa(eepromConfig.PID[HEADING_PID].P,             numberString); usbPrint(numberString); usbPrint(", ");
            ftoa(eepromConfig.PID[HEADING_PID].I,             numberString); usbPrint(numberString); usbPrint(", ");
            ftoa(eepromConfig.PID[HEADING_PID].D,             numberString); usbPrint(numberString); usbPrint(", ");
            ftoa(eepromConfig.PID[HEADING_PID].windupGuard,   numberString); usbPrint(numberString); usbPrint(", ");
            if  (eepromConfig.PID[HEADING_PID].dErrorCalc)
                usbPrint("Error\n");
            else
                usbPrint("State\n");

            queryType = 'x';
            validCommand = false;
            break;

        ///////////////////////////////

        case 's': // Altitude PIDs
        	queryType = 'x';
        	validCommand = false;
        	break;

        ///////////////////////////////

        case 't': // Sensor Calibration Data
            usbPrint("\n");

            usbPrint("Accel One G:               ");
            ftoa(accelOneG,                                numberString); usbPrint(numberString); usbPrint("\n");

            usbPrint("Accel Temp Comp Slope:     ");
            ftoa(eepromConfig.accelTCBiasSlope[XAXIS],     numberString); usbPrint(numberString); usbPrint(", ");
            ftoa(eepromConfig.accelTCBiasSlope[YAXIS],     numberString); usbPrint(numberString); usbPrint(", ");
            ftoa(eepromConfig.accelTCBiasSlope[ZAXIS],     numberString); usbPrint(numberString); usbPrint("\n");

            usbPrint("Accel Temp Comp Bias:      ");
            ftoa(eepromConfig.accelTCBiasIntercept[XAXIS], numberString); usbPrint(numberString); usbPrint(", ");
            ftoa(eepromConfig.accelTCBiasIntercept[YAXIS], numberString); usbPrint(numberString); usbPrint(", ");
            ftoa(eepromConfig.accelTCBiasIntercept[ZAXIS], numberString); usbPrint(numberString); usbPrint("\n");

            usbPrint("Gyro Temp Comp Slope:      ");
            ftoa(eepromConfig.gyroTCBiasSlope[ROLL ],      numberString); usbPrint(numberString); usbPrint(", ");
            ftoa(eepromConfig.gyroTCBiasSlope[PITCH],      numberString); usbPrint(numberString); usbPrint(", ");
            ftoa(eepromConfig.gyroTCBiasSlope[YAW  ],      numberString); usbPrint(numberString); usbPrint("\n");

            usbPrint("Gyro Temp Comp Intercept:  ");
            ftoa(eepromConfig.gyroTCBiasIntercept[ROLL ],  numberString); usbPrint(numberString); usbPrint(", ");
            ftoa(eepromConfig.gyroTCBiasIntercept[PITCH],  numberString); usbPrint(numberString); usbPrint(", ");
            ftoa(eepromConfig.gyroTCBiasIntercept[YAW  ],  numberString); usbPrint(numberString); usbPrint("\n");

            usbPrint("Mag Bias:                  ");
            ftoa(eepromConfig.magBias[XAXIS],              numberString); usbPrint(numberString); usbPrint(", ");
            ftoa(eepromConfig.magBias[YAXIS],              numberString); usbPrint(numberString); usbPrint(", ");
            ftoa(eepromConfig.magBias[ZAXIS],              numberString); usbPrint(numberString); usbPrint("\n");

            usbPrint("Accel Cutoff:              ");
            ftoa(eepromConfig.accelCutoff,                 numberString); usbPrint(numberString); usbPrint("\n");

            usbPrint("KpAcc (MARG):              ");
            ftoa(eepromConfig.KpAcc,                       numberString); usbPrint(numberString); usbPrint("\n");

            usbPrint("KiAcc (MARG):              ");
            ftoa(eepromConfig.KiAcc,                       numberString); usbPrint(numberString); usbPrint("\n");

            usbPrint("KpMag (MARG):              ");
            ftoa(eepromConfig.KpMag,                       numberString); usbPrint(numberString); usbPrint("\n");

            usbPrint("KiMag (MARG):              ");
            ftoa(eepromConfig.KiMag,                       numberString); usbPrint(numberString); usbPrint("\n");

            usbPrint("Complementary Filter A:    ");
            ftoa(eepromConfig.compFilterA,                 numberString); usbPrint(numberString); usbPrint("\n");

            usbPrint("Complementary Filter B:    ");
            ftoa(eepromConfig.compFilterB,                 numberString); usbPrint(numberString); usbPrint("\n");

            usbPrint("MPU6000 DLPF:               ");
            switch(eepromConfig.dlpfSetting)
            {
                case DLPF_256HZ:
                    usbPrint("256 Hz\n");
                    break;
                case DLPF_188HZ:
                    usbPrint("188 Hz\n");
                    break;
                case DLPF_98HZ:
                    usbPrint("98 Hz\n");
                    break;
                case DLPF_42HZ:
                    usbPrint("42 Hz\n");
                    break;
            }

            queryType = 'x';
            validCommand = false;
            break;

        ///////////////////////////////

        case 'u': // System Configuration Data
            usbPrint("\nReceiver Type:                  ");
            switch(eepromConfig.receiverType)
            {
                case PARALLEL_PWM:
                    usbPrint("Parallel\n");
                    break;
                case SERIAL_PWM:
                    usbPrint("Serial\n");
                    break;
                case SPEKTRUM:
                    usbPrint("Spektrum ");
                    if (eepromConfig.spektrumHires)
                        usbPrint("11 Bit Mode, ");
                    else
                        usbPrint("10 Bit Mode,");

                    itoa(eepromConfig.spektrumChannels, numberString,10); usbPrint(numberString); usbPrint(" Channels\n");
                    break;
		    }

            usbPrint("Current RC Channel Assignment:  ");
            for (index = 0; index < 8; index++)
                rcOrderString[eepromConfig.rcMap[index]] = rcChannelLetters[index];

            rcOrderString[index] = '\0';

            usbPrint(rcOrderString);
            usbPrint("\n");

            usbPrint("Mixer Configuration:            ");
            switch (eepromConfig.mixerConfiguration)
            {
                case MIXERTYPE_GIMBAL:
                	usbPrint("MIXERTYPE GIMBAL\n");
                	break;

                ///////////////////////

                case MIXERTYPE_FLYING_WING:
                	usbPrint("MIXERTYPE FLYING WING\n");
                	break;

                ///////////////////////

                case MIXERTYPE_BI:
                    usbPrint("MIXERTYPE BICOPTER\n");
                    break;

                ///////////////////////

                case MIXERTYPE_TRI:
                    usbPrint("MIXERTYPE TRICOPTER\n");
                    break;

                ///////////////////////

                case MIXERTYPE_QUADP:
                    usbPrint("MIXERTYPE QUAD PLUS\n");
                    break;

                case MIXERTYPE_QUADX:
                    usbPrint("MIXERTYPE QUAD X\n");
                    break;

                case MIXERTYPE_VTAIL4_NO_COMP:
                	usbPrint("MULTITYPE VTAIL NO COMP\n");
                	break;

                case MIXERTYPE_VTAIL4_Y_COMP:
                	usbPrint("MULTITYPE VTAIL Y COMP\n");
                	break;

                case MIXERTYPE_VTAIL4_RY_COMP:
                	usbPrint("MULTITYPE VTAIL RY COMP\n");
                	break;

                case MIXERTYPE_VTAIL4_PY_COMP:
                	usbPrint("MULTITYPE VTAIL PY COMP\n");
                	break;

                case MIXERTYPE_VTAIL4_RP_COMP:
                	usbPrint("MULTITYPE VTAIL RP COMP\n");
                	break;

                case MIXERTYPE_VTAIL4_RPY_COMP:
                	usbPrint("MULTITYPE VTAIL RPY COMP\n");
                	break;

                case MIXERTYPE_Y4:
                	usbPrint("MIXERTYPE Y4\n");
                	break;

                ///////////////////////

                case MIXERTYPE_HEX6P:
                    usbPrint("MIXERTYPE HEX PLUS\n");
                    break;

                case MIXERTYPE_HEX6X:
                    usbPrint("MIXERTYPE HEX X\n");
                    break;

                case MIXERTYPE_Y6:
                    usbPrint("MIXERTYPE Y6\n");
                    break;

                ///////////////////////

                case MIXERTYPE_OCTOF8P:
                    usbPrint("MIXERTYPE FLAT OCTO PLUS\n");
                    break;

                case MIXERTYPE_OCTOF8X:
                    usbPrint("MIXERTYPE FLAT OCTO X\n");
                    break;

                case MIXERTYPE_OCTOX8P:
                    usbPrint("MIXERTYPE COAXIAL OCTO PLUS\n");
                    break;

                case MIXERTYPE_OCTOX8X:
                    usbPrint("MIXERTYPE COAXIAL OCTO X\n");
                    break;

                ///////////////////////

                case MIXERTYPE_FREEMIX:
                	usbPrint("MIXERTYPE FREE MIX\n");
                	break;
            }

            if (eepromConfig.mixerConfiguration == MIXERTYPE_VTAIL4_Y_COMP  ||
                eepromConfig.mixerConfiguration == MIXERTYPE_VTAIL4_RY_COMP ||
                eepromConfig.mixerConfiguration == MIXERTYPE_VTAIL4_PY_COMP ||
                eepromConfig.mixerConfiguration == MIXERTYPE_VTAIL4_RP_COMP ||
                eepromConfig.mixerConfiguration == MIXERTYPE_VTAIL4_RPY_COMP)
            {
                usbPrint("V Tail Angle                   ");
                ftoa(eepromConfig.vTailAngle, numberString); usbPrint(numberString); usbPrint("\n");
			}

            usbPrint("Yaw Direction:                  ");
            itoa((int8_t)eepromConfig.yawDirection,   numberString, 10); usbPrint(numberString); usbPrint("\n");

            usbPrint("Number of Motors:               ");
            itoa(numberMotor,                         numberString, 10); usbPrint(numberString); usbPrint("\n");

            usbPrint("ESC PWM Rate:                   ");
            itoa((uint16_t)eepromConfig.escPwmRate,   numberString, 10); usbPrint(numberString); usbPrint("\n");

            usbPrint("Servo PWM Rate:                 ");
            itoa((uint16_t)eepromConfig.servoPwmRate, numberString, 10); usbPrint(numberString); usbPrint("\n");

            usbPrint("Mid Command:                    ");
            itoa((uint16_t)eepromConfig.midCommand,   numberString, 10); usbPrint(numberString); usbPrint("\n");

            usbPrint("Min Check:                      ");
            itoa((uint16_t)eepromConfig.minCheck,     numberString, 10); usbPrint(numberString); usbPrint("\n");

            usbPrint("Max Check:                      ");
            itoa((uint16_t)eepromConfig.maxCheck,     numberString, 10); usbPrint(numberString); usbPrint("\n");

            usbPrint("Min Throttle:                   ");
            itoa((uint16_t)eepromConfig.minThrottle,  numberString, 10); usbPrint(numberString); usbPrint("\n");

            usbPrint("Max Thottle:                    ");
            itoa((uint16_t)eepromConfig.maxThrottle,  numberString, 10); usbPrint(numberString); usbPrint("\n");

            if ( eepromConfig.mixerConfiguration == MIXERTYPE_GIMBAL )
            {
                usbPrint("Gimbal Roll Servo Min:          ");
                itoa((uint16_t)eepromConfig.gimbalRollServoMin,  numberString, 10); usbPrint(numberString); usbPrint("\n");
                usbPrint("Gimbal Roll Servo Mid:          ");
                itoa((uint16_t)eepromConfig.gimbalRollServoMid,  numberString, 10); usbPrint(numberString); usbPrint("\n");
                usbPrint("Gimbal Roll Servo Max:          ");
                itoa((uint16_t)eepromConfig.gimbalRollServoMax,  numberString, 10); usbPrint(numberString); usbPrint("\n");
                usbPrint("Gimbal Roll Servo Gain:         ");
                ftoa(eepromConfig.gimbalRollServoGain, numberString);               usbPrint(numberString); usbPrint("\n");
                usbPrint("Gimbal Pitch Servo Min:         ");
                itoa((uint16_t)eepromConfig.gimbalPitchServoMin, numberString, 10); usbPrint(numberString); usbPrint("\n");
                usbPrint("Gimbal Pitch Servo Mid:         ");
                itoa((uint16_t)eepromConfig.gimbalPitchServoMid, numberString, 10); usbPrint(numberString); usbPrint("\n");
                usbPrint("Gimbal Pitch Servo Max:         ");
                itoa((uint16_t)eepromConfig.gimbalPitchServoMax, numberString, 10); usbPrint(numberString); usbPrint("\n");
                usbPrint("Gimbal Pitch Servo Gain:        ");
                ftoa(eepromConfig.gimbalPitchServoGain, numberString);              usbPrint(numberString); usbPrint("\n");
            }

            if ( eepromConfig.mixerConfiguration == MIXERTYPE_BI )
            {
                usbPrint("BiCopter Left Servo Min:        ");
                itoa((uint16_t)eepromConfig.biLeftServoMin,  numberString, 10); usbPrint(numberString); usbPrint("\n");
                usbPrint("BiCopter Left Servo Mid:        ");
                itoa((uint16_t)eepromConfig.biLeftServoMid,  numberString, 10); usbPrint(numberString); usbPrint("\n");
                usbPrint("BiCopter Left Servo Max:        ");
                itoa((uint16_t)eepromConfig.biLeftServoMax,  numberString, 10); usbPrint(numberString); usbPrint("\n");
                usbPrint("BiCopter Right Servo Min:       ");
                itoa((uint16_t)eepromConfig.biRightServoMin, numberString, 10); usbPrint(numberString); usbPrint("\n");
                usbPrint("BiCopter Right Servo Mid:       ");
                itoa((uint16_t)eepromConfig.biRightServoMid, numberString, 10); usbPrint(numberString); usbPrint("\n");
                usbPrint("BiCopter Right Servo Max:       ");
                itoa((uint16_t)eepromConfig.biRightServoMax, numberString, 10); usbPrint(numberString); usbPrint("\n");
            }

            if ( eepromConfig.mixerConfiguration == MIXERTYPE_TRI )
            {
                usbPrint("TriCopter Yaw Servo Min:        ");
                itoa((uint16_t)eepromConfig.triYawServoMin, numberString, 10); usbPrint(numberString); usbPrint("\n");
                usbPrint("TriCopter Yaw Servo Mid:        ");
                itoa((uint16_t)eepromConfig.triYawServoMid, numberString, 10); usbPrint(numberString); usbPrint("\n");
                usbPrint("TriCopter Yaw Servo Max:        ");
                itoa((uint16_t)eepromConfig.triYawServoMax, numberString, 10); usbPrint(numberString); usbPrint("\n");
            }

            if ( eepromConfig.mixerConfiguration == MIXERTYPE_FLYING_WING )
            {
                usbPrint("Roll Direction Left:            ");
                itoa((uint16_t)eepromConfig.rollDirectionLeft,   numberString, 10); usbPrint(numberString); usbPrint("\n");
                usbPrint("Roll Direction Right:           ");
                itoa((uint16_t)eepromConfig.rollDirectionRight,  numberString, 10); usbPrint(numberString); usbPrint("\n");
                usbPrint("Pitch Direction Left:           ");
                itoa((uint16_t)eepromConfig.pitchDirectionLeft,  numberString, 10); usbPrint(numberString); usbPrint("\n");
                usbPrint("Pitch Direction Right:          ");
                itoa((uint16_t)eepromConfig.pitchDirectionRight, numberString, 10); usbPrint(numberString); usbPrint("\n");
                usbPrint("Wing Left Minimum:              ");
                itoa((uint16_t)eepromConfig.wingLeftMinimum,     numberString, 10); usbPrint(numberString); usbPrint("\n");
                usbPrint("Wing Left Maximum:              ");
                itoa((uint16_t)eepromConfig.wingLeftMaximum,     numberString, 10); usbPrint(numberString); usbPrint("\n");
                usbPrint("Wing Right Minimum:             ");
                itoa((uint16_t)eepromConfig.wingRightMinimum,    numberString, 10); usbPrint(numberString); usbPrint("\n");
                usbPrint("Wing Right Maximum:             ");
                itoa((uint16_t)eepromConfig.wingRightMaximum,    numberString, 10); usbPrint(numberString); usbPrint("\n");
            }

            usbPrint("OSD Installed:                  ");
            if (eepromConfig.osdInstalled)
            	usbPrint("True\n");
            else
            	usbPrint("False\n");

            if (eepromConfig.osdInstalled)
            {
				usbPrint("   OSD Default Video Standard:  ");
                if (eepromConfig.defaultVideoStandard)
                	usbPrint("PAL\n");
                else
                	usbPrint("NTSC\n");

                usbPrint("   OSD Display Units:           ");
                if (eepromConfig.metricUnits)
                	usbPrint("Metric\n");
                else
                	usbPrint("English\n");

                usbPrint("   OSD Altitude:                ");
                if (eepromConfig.osdDisplayAlt)
                	usbPrint("On\n");
                else
                	usbPrint("Off\n");

                usbPrint("   OSD Artifical Horizon:       ");
                if (eepromConfig.osdDisplayAH)
                	usbPrint("On\n");
                else
                	usbPrint("Off\n");

                usbPrint("   OSD Attitude:                ");
                if (eepromConfig.osdDisplayAtt)
                	usbPrint("On\n");
                else
                	usbPrint("Off\n");

                usbPrint("   OSD Heading:                 ");
                if (eepromConfig.osdDisplayHdg)
                	usbPrint("On\n");
                else
                	usbPrint("Off\n");
			}

            queryType = 'x';
            validCommand = false;
            break;

        ///////////////////////////////

        case 'v': // Free Mix Matrix
        	usbPrint("\nNumber of Free Mixer Motors:  ");
        	itoa( eepromConfig.freeMixMotors, numberString, 10 ); usbPrint( numberString ); usbPrint("\n\n");
            usbPrint("         Roll    Pitch   Yaw\n");

        	for ( index = 0; index < eepromConfig.freeMixMotors; index++ )
        	{
        		usbPrint("Motor"); itoa(index, numberString, 10);       usbPrint(numberString); usbPrint("  ");
        		ftoa(eepromConfig.freeMix[index][ROLL ], numberString); usbPrint(numberString); usbPrint("  ");
        		ftoa(eepromConfig.freeMix[index][PITCH], numberString); usbPrint(numberString); usbPrint("  ");
        		ftoa(eepromConfig.freeMix[index][YAW  ], numberString); usbPrint(numberString); usbPrint("\n");
        	}

        	queryType = 'x';
        	validCommand = false;
        	break;

        ///////////////////////////////

        case 'w': // Raw Receiver Commands
            if (eepromConfig.receiverType == SPEKTRUM)
            {
				for (index = 0; index < eepromConfig.spektrumChannels - 1; index++)
                {
    		    	itoa(spektrumChannelData[index], numberString, 10);
    		    	usbPrint(numberString); usbPrint(", ");
    		    }

                itoa(spektrumChannelData[eepromConfig.spektrumChannels - 1], numberString, 10);
                usbPrint(numberString); usbPrint("\n");
		    }
		    else
		    {
				for (index = 0; index < 7; index++)
                {
    		    	itoa(rxRead(index), numberString, 10);
    		    	usbPrint(numberString); usbPrint(", ");
    		    }

                itoa(rxRead(7), numberString, 10);
                usbPrint(numberString); usbPrint("\n");
			}

        	validCommand = false;
        	break;

        ///////////////////////////////

        case 'x':
        	validCommand = false;
        	break;

        ///////////////////////////////

        case 'y':
        	queryType ='x';
        	break;

        ///////////////////////////////

        case 'z':
            queryType = 'x';
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

        case '0': // Disable high speed telemetry
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
            readUsbPID(ROLL_RATE_PID);
            usbPrint( "\nRoll Rate PID Received....\n" );

        	queryType = 'q';
        	validCommand = false;
        	break;

        ///////////////////////////////

        case 'B': // Read Pitch Rate PID Values
            readUsbPID(PITCH_RATE_PID);
            usbPrint( "\nPitch Rate PID Received....\n" );

        	queryType = 'q';
        	validCommand = false;
        	break;

        ///////////////////////////////

        case 'C': // Read Yaw Rate PID Values
            readUsbPID(YAW_RATE_PID);
            usbPrint( "\nYaw Rate PID Received....\n" );

        	queryType = 'q';
        	validCommand = false;
        	break;

        ///////////////////////////////

        case 'D': // Read Roll Attitude PID Values
            readUsbPID(ROLL_ATT_PID);
            usbPrint( "\nRoll Attitude PID Received....\n" );

        	queryType = 'r';
        	validCommand = false;
        	break;

        ///////////////////////////////

        case 'E': // Read Pitch Attitude PID Values
            readUsbPID(PITCH_ATT_PID);
            usbPrint( "\nPitch Attitude PID Received....\n" );

        	queryType = 'r';
        	validCommand = false;
        	break;

        ///////////////////////////////

        case 'F': // Read Heading Hold PID Values
            readUsbPID(HEADING_PID);
            usbPrint( "\nHeading PID Received....\n" );

        	queryType = 'r';
        	validCommand = false;
        	break;

        ///////////////////////////////

        case 'G': // Read Altitude Hold PID Values
        	queryType = 'x';
        	validCommand = false;
        	break;

        ///////////////////////////////

        case 'H': // Read RX Input Type
            eepromConfig.receiverType = (uint8_t)readFloatUsb();
			usbPrint( "\nReceiver Type Received....\n");

			usbPrint("\nSystem Reseting....\n");
			delay(100);
			writeEEPROM();
			systemReset(false);

		    validCommand = false;
            break;

        ///////////////////////////////

        case 'I': // Read RC Control Order
            readStringUsb( rcOrderString, 8 );
            usbPrint( "\nRC Channel Order Received...." );
            usbPrint( rcOrderString );
            usbPrint("\n");
            parseRcChannels( rcOrderString );

          	queryType = 'u';
          	validCommand = false;
          	break;

       	///////////////////////////////

        case 'J': // Read Spektrum Resolution
            eepromConfig.spektrumHires = (uint8_t)readFloatUsb();
            usbPrint("\nSpektrum Resolution Received....\n");

            queryType = 'u';
            validCommand = false;
        	break;

        ///////////////////////////////

        case 'K': // Read Controlling Pressure Sensor
            eepromConfig.spektrumChannels = (uint8_t)readFloatUsb();
            usbPrint("\nSpektrum Channels Received....\n");

            queryType = 'u';
            validCommand = false;
        	break;

        ///////////////////////////////

        case 'L': // Read Mixer Configuration
            eepromConfig.mixerConfiguration = (uint8_t)readFloatUsb();
            usbPrint( "\nMixer Configuration Received....\n" );
        	initMixer();

        	queryType = 'u';
        	validCommand = false;
        	break;

        ///////////////////////////////

        case 'M': // Read V Tail Angle
        	eepromConfig.vTailAngle = readFloatUsb();
        	usbPrint("\nV Tail Angle Received....\n");

        	queryType = 'u';
        	validCommand = false;
        	break;

        ///////////////////////////////

        case 'N': // Read ESC and Servo PWM Update Rates
            eepromConfig.escPwmRate   = (uint16_t)readFloatUsb();
            eepromConfig.servoPwmRate = (uint16_t)readFloatUsb();
            usbPrint( "\nPWM Update Rates Received....\n");

            pwmEscInit(eepromConfig.escPwmRate);
            pwmServoInit(eepromConfig.servoPwmRate);

            queryType = 'u';
            validCommand = false;
            break;

        ///////////////////////////////

        case 'O': // Read RC Control Points
            eepromConfig.midCommand   = readFloatUsb();
    	    eepromConfig.minCheck     = readFloatUsb();
    		eepromConfig.maxCheck     = readFloatUsb();
    		eepromConfig.minThrottle  = readFloatUsb();
    		eepromConfig.maxThrottle  = readFloatUsb();
            usbPrint( "\nRC Control Points Received....\n" );

            queryType = 'u';
            validCommand = false;
            break;

        ///////////////////////////////

        case 'P': // Read Gimbal Roll Servo Parameters
         	eepromConfig.gimbalRollServoMin  = readFloatUsb();
           	eepromConfig.gimbalRollServoMid  = readFloatUsb();
           	eepromConfig.gimbalRollServoMax  = readFloatUsb();
           	eepromConfig.gimbalRollServoGain = readFloatUsb();
           	usbPrint( "\nGimbal Roll Servo Parameters Received....\n");

           	queryType = 'u';
           	validCommand = false;
           	break;

        ///////////////////////////////

        case 'Q': // Read Gimbal Pitch Servo Parameters
           	eepromConfig.gimbalPitchServoMin  = readFloatUsb();
           	eepromConfig.gimbalPitchServoMid  = readFloatUsb();
           	eepromConfig.gimbalPitchServoMax  = readFloatUsb();
           	eepromConfig.gimbalPitchServoGain = readFloatUsb();
           	usbPrint( "\nGimbal Pitch Servo Parameters Received....\n");

           	queryType = 'u';
           	validCommand = false;
           	break;

        ///////////////////////////////

        case 'R': // Read BiCopter Left Servo Parameters
           	eepromConfig.biLeftServoMin = readFloatUsb();
           	eepromConfig.biLeftServoMid = readFloatUsb();
           	eepromConfig.biLeftServoMax = readFloatUsb();
           	usbPrint( "\nBiCopter Left Servo Parameters Received....\n");

           	queryType = 'u';
           	validCommand = false;
           	break;

        ///////////////////////////////

        case 'S': // Read BiCopter Right Servo Parameters
           	eepromConfig.biRightServoMin = readFloatUsb();
           	eepromConfig.biRightServoMid = readFloatUsb();
           	eepromConfig.biRightServoMax = readFloatUsb();
           	usbPrint( "\nBiCopter Right Servo Parameters Received....\n");

           	queryType = 'u';
           	validCommand = false;
           	break;

        ///////////////////////////////

        case 'T': // Read TriCopter YawServo Parameters
        	eepromConfig.triYawServoMin = readFloatUsb();
           	eepromConfig.triYawServoMid = readFloatUsb();
           	eepromConfig.triYawServoMax = readFloatUsb();
           	usbPrint( "\nTriCopter Yaw Servo Parameters Received....\n");

           	queryType = 'u';
           	validCommand = false;
           	break;

        ///////////////////////////////

        case 'U': // Read Flying Wing Servo Directions
            eepromConfig.rollDirectionLeft   = readFloatUsb();
            eepromConfig.rollDirectionRight  = readFloatUsb();
            eepromConfig.pitchDirectionLeft  = readFloatUsb();
            eepromConfig.pitchDirectionRight = readFloatUsb();
          	usbPrint( "\nFlying Wing Servo Directions Received....\n");

         	queryType = 'u';
         	validCommand = false;
         	break;

        ///////////////////////////////

        case 'V': // Read Flying Wing Servo Limits
           	eepromConfig.wingLeftMinimum  = readFloatUsb();
           	eepromConfig.wingLeftMaximum  = readFloatUsb();
           	eepromConfig.wingRightMinimum = readFloatUsb();
           	eepromConfig.wingRightMaximum = readFloatUsb();
           	usbPrint( "\nFlying Wing Servo Limits Received....\n");

           	queryType = 'u';
           	validCommand = false;
           	break;

        ///////////////////////////////

        case 'W': // Read Free Mix Motor Number
           	eepromConfig.freeMixMotors = (uint8_t)readFloatUsb();
           	usbPrint( "\nFree Mix Motor Number Received....\n");
           	initMixer();

           	queryType = 'v';
           	validCommand = false;
           	break;

        ///////////////////////////////

        case 'X': // Read Free Mix Matrix Element
            rows    = (uint8_t)readFloatUsb();
            columns = (uint8_t)readFloatUsb();
            eepromConfig.freeMix[rows][columns] = readFloatUsb();
            usbPrint( "\nFree Mix Value Received....\n");

            queryType = 'v';
            validCommand = false;
            break;

        ///////////////////////////////

        case 'Y': // Read Accel Cutoff
            eepromConfig.accelCutoff = readFloatUsb();
            usbPrint( "\nAccel Cutoff Received....\n" );

            queryType = 't';
        	break;

        ///////////////////////////////

        case 'Z':
            tempInt = (uint8_t)readFloatUsb();

            switch(tempInt)
            {
                case DLPF_256HZ:
                    eepromConfig.dlpfSetting = BITS_DLPF_CFG_256HZ;
                    break;

                case DLPF_188HZ:
                	eepromConfig.dlpfSetting = BITS_DLPF_CFG_188HZ;
                	break;

                case DLPF_98HZ:
                	eepromConfig.dlpfSetting = BITS_DLPF_CFG_98HZ;
                	break;

                case DLPF_42HZ:
                	eepromConfig.dlpfSetting = BITS_DLPF_CFG_42HZ;
                	break;
            }

            usbPrint("\nMPU6000 DLPF Configuration Received....\n");

            setSPIdivisor(MPU6000_SPI, 64);  // 0.65625 MHz SPI clock (within 20 +/- 10%)

            GPIO_ResetBits(MPU6000_CS_GPIO, MPU6000_CS_PIN);
			spiTransfer(MPU6000_SPI, MPU6000_CONFIG);
			spiTransfer(MPU6000_SPI, eepromConfig.dlpfSetting);
			GPIO_SetBits(MPU6000_CS_GPIO, MPU6000_CS_PIN);

            setSPIdivisor(MPU6000_SPI, 2);  // 21 MHz SPI clock (within 20 +/- 10%)

            queryType = 't';
            break;

        ///////////////////////////////

        case '!': // Read kpAcc, kiAcc
            eepromConfig.KpAcc = readFloatUsb();
            eepromConfig.KiAcc = readFloatUsb();
            usbPrint("\nkpAcc and kiAcc Received....\n");

            queryType = 't';
            break;

        ///////////////////////////////

        case '@': // Read kpMag, kiMag
            eepromConfig.KpMag = readFloatUsb();
            eepromConfig.KiMag = readFloatUsb();
            usbPrint("\nkpMag and kiMag Received....\n");

            queryType = 't';
            break;

        ///////////////////////////////

        case '#':
            eepromConfig.compFilterA = readFloatUsb();
            eepromConfig.compFilterB = readFloatUsb();
            usbPrint("\nComp Filter A and Comp Filter B Received....\n");

            queryType = 't';
            break;

        ///////////////////////////////

        case '$':  // Read yaw direction
            tempFloat = readFloatUsb();
            if (tempFloat >= 0.0)
                tempFloat = 1.0;
            else
            	tempFloat = -1.0;

            eepromConfig.yawDirection = tempFloat;
            usbPrint("\nYaw Direction Received....\n");

            queryType = 'u';
            break;

        ///////////////////////////////

        case '%':
            queryType = 'x';
        	break;

        ///////////////////////////////

        case '^': // Reset EEPROM Parameters
            usbPrint( "\nEEPROM Parameters Reset....\n" );
            checkFirstTime(true);
            usbPrint("\nSystem Resetting....\n\n");
            delay(100);
            systemReset(false);
            break;

        ///////////////////////////////

        case '&': // Write EEPROM Parameters
            usbPrint("\nWriting EEPROM Parameters....\n");
            writeEEPROM();
            usbPrint("\nSystem Resetting....\n\n");
            delay(100);
            systemReset(false);
            break;

        ///////////////////////////////

        case '*': // Reset System
        	usbPrint("\nSystem Reseting....\n\n");
        	delay(100);
        	systemReset(false);
        	break;

        ///////////////////////////////

        case '(': // Reset to Bootloader
        	usbPrint("\nEntering Bootloader....\n\n");
        	delay(100);
        	systemReset(true);
        	break;

        ///////////////////////////////

        case ')': // MAX7456 CLI
           	max7456cli();

           	queryType = 'x';
           	break;

        ///////////////////////////////

        case '?': // Command Summary
        	cliBusy = true;

        	usbPrint("\n");
   		    usbPrint("'a' Loop Delta Times                       'A' Set Roll Rate PID Data               AB;P;I;D;WindupGuard;dErrorCalc\n");
   		    usbPrint("'b' Loop Execution Times                   'B' Set Pitch Rate PID Data              BB;P;I;D;WindupGuard;dErrorCalc\n");
   		    usbPrint("'c' MPU6000 Calibration                    'C' Set Yaw Rate PID Data                CB;P;I;D;WindupGuard;dErrorCalc\n");
   		    usbPrint("'d' Mag Calibration                        'D' Set Roll Att PID Data                DB;P;I;D;WindupGuard;dErrorCalc\n");
   		    usbPrint("'e' ESC Calibration                        'E' Set Pitch Att PID Data               EB;P;I;D;WindupGuard;dErrorCalc\n");
   		    usbPrint("'f' 500 Hz Gyro Data                       'F' Set Hdg Hold PID Data                FB;P;I;D;WindupGuard;dErrorCalc\n");
   		    usbPrint("'g' 500 Hz Accel Data                      'G' Set Alt Hold PID Data                TBD\n");
   		    usbPrint("'h' Earth Axis Accels                      'H' Set RX Input Type                    HX, 1=Parallel, 2=Serial, 3=Spektrum\n");
   		    usbPrint("'i' 10 Hz Mag Data                         'I' Set RC Control Order                 ITAER1234\n");
   		    usbPrint("'j' Vertical Axis Variables                'J' Set Spektrum Resolution              J0 or J1\n");
   		    usbPrint("'k' Euler Angles                           'K' Set Number of Spektrum Channels      K6 thru 12\n");
   		    usbPrint("'l' Not Used                               'L' Set Mixer Configuration              L1 thru 21, see aq32Plus.h\n");
   		    usbPrint("\n");

   		    usbPrint("Press space bar for more, or enter a command....\n");
   		    while (usbAvailable() == false);
   		    queryType = usbRead();
   		    if (queryType != ' ')
   		    {
   		        validCommand = true;
   		        cliBusy = false;
   		    	return;
   		    }

   		    usbPrint("\n");
   		    usbPrint("'m' Processed Receiver Commands            'M' Set V Tail Angle                     MAngle\n");
   		    usbPrint("'n' Command in Detent Discretes            'N' Set PWM Rates                        NESC;Servo\n");
   		    usbPrint("'o' ESC PWM Outputs                        'O' Set RC Control Points                OMidCmd;minChk;MaxChk;MinThrot;MaxThrot\n");
   		    usbPrint("'p' Servo PWM Outputs                      'P' Set Gimbal Roll Servo Parameters     PMin;Mid;Max;Gain\n");
   		    usbPrint("'q' Rate PIDs                              'Q' Set Gimbal Pitch Servo Parameters    QMin;Mid;Max;Gain\n");
   		    usbPrint("'r' Attitude PIDs                          'R' Set BiCopter Left Servo Parameters   RMin;Mid;Max\n");
   		    usbPrint("'s' Altitude PIDs (TBD)                    'S' Set BiCopter Right Servo Parameters  SMin;Mid;Max\n");
   		    usbPrint("'t' Sensor Calibration Data                'T' Set TriCopter Servo Paramters        TMin;Mid;Max\n");
   		    usbPrint("'u' System Configuration Data              'U' Set Flying Wing Servo Directions     URollLeft;RollRight;PitchLeft;PitchRight\n");
   		    usbPrint("'v' Free Mix Element Display               'V' Set Flying Wing Servo Limits         VLeftMin;LeftMax;RightMin;RightMax\n");
   		    usbPrint("'w' Raw Receiver Commands                  'W' Set Number of FreeMix Motors         WNumber\n");
   		    usbPrint("'x' Terminate Serial Communication         'X' Set FreeMix Matrix Element           XRow;Column;Element\n");
   		    usbPrint("\n");

   		    usbPrint("Press space bar for more, or enter a command....\n");
   		    while (usbAvailable() == false);
   		    queryType = usbRead();
   		    if (queryType != ' ')
   		    {
   		    	validCommand = true;
   		    	cliBusy = false;
   		    	return;
   		    }

   		    usbPrint("\n");
   		    usbPrint("'y' Not Used                               'Y' Set Accel Cutoff                     YAccelCutoff\n");
   		    usbPrint("'z' Not Used                               'Z' Set MPU6000 DLPF                     Z0 thru 3, see aq32Plus.h\n");
   		    usbPrint("'1' High Speed Telemetry 1 Enable          '!' Set kpAcc/kiAcc                      !KpAcc;KiAcc\n");
   		    usbPrint("'2' High Speed Telemetry 2 Enable          '@' Set kpMag/kiMag                      @KpMag;KiMag\n");
   		    usbPrint("'3' High Speed Telemetry 3 Enable          '#' Set Complementary Filter A/B         #A;B\n");
   		    usbPrint("'4' High Speed Telemetry 4 Enable          '$' Set Yaw Direction                    $1 or $-1\n");
   		    usbPrint("'5' High Speed Telemetry 5 Enable          '%' Not Used\n");
   		    usbPrint("'6' High Speed Telemetry 6 Enable          '^' Reset EEPROM Parameters\n");
   		    usbPrint("'7' High Speed Telemetry 7 Enable          '&' Write EEPROM Parameters\n");
   		    usbPrint("'8' High Speed Telemetry 8 Enable          '*' Reset\n");
   		    usbPrint("'9' High Speed Telemetry 9 Enable          '(' Reset and Enter Bootloader\n");
   		    usbPrint("'0' High Speed Telemetry Disable           ')' MAX7456 CLI\n");
   		    usbPrint("                                           '?' Command Summary\n");
            usbPrint("\n");

  		    queryType = 'x';
  		    cliBusy = false;
   		    break;

            ///////////////////////////////
    }
}

///////////////////////////////////////////////////////////////////////////////
