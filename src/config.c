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

#define FLASH_WRITE_EEPROM_ADDR  0x08004000  // FLASH_Sector_1

const char rcChannelLetters[] = "AERT1234";

float vTailThrust;

static uint8_t checkNewEEPROMConf = 3;

///////////////////////////////////////////////////////////////////////////////

void parseRcChannels(const char *input)
{
    const char *c, *s;

    for (c = input; *c; c++)
    {
        s = strchr(rcChannelLetters, *c);
        if (s)
            eepromConfig.rcMap[s - rcChannelLetters] = c - input;
    }
}

///////////////////////////////////////////////////////////////////////////////

void readEEPROM(void)
{
    memcpy(&eepromConfig, (char *)FLASH_WRITE_EEPROM_ADDR, sizeof(eepromConfig_t));

    accConfidenceDecay = 1.0f / sqrt(eepromConfig.accelCutoff);

    eepromConfig.yawDirection = constrain(eepromConfig.yawDirection, -1.0f, 1.0f);

    vTailThrust = sinf(eepromConfig.vTailAngle);
}

///////////////////////////////////////////////////////////////////////////////

void writeEEPROM(void)
{
    FLASH_Status status;
    uint32_t i;

    FLASH_Unlock();

    FLASH_ClearFlag(FLASH_FLAG_EOP    | FLASH_FLAG_OPERR  | FLASH_FLAG_WRPERR |
                    FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

    if (FLASH_EraseSector(FLASH_Sector_1, VoltageRange_3) == FLASH_COMPLETE)
    {
        for (i = 0; i < sizeof(eepromConfig_t); i += 4)
        {
            status = FLASH_ProgramWord(FLASH_WRITE_EEPROM_ADDR + i, *(uint32_t *)((char *)&eepromConfig + i ));
            if (status != FLASH_COMPLETE)
                break; // TODO: fail
        }
    }

    FLASH_Lock();

    readEEPROM();
}

///////////////////////////////////////////////////////////////////////////////

void checkFirstTime(bool eepromReset)
{
    uint8_t test_val;

    test_val = *(uint8_t *)FLASH_WRITE_EEPROM_ADDR;

    if (eepromReset || test_val != checkNewEEPROMConf)
    {
		// Default settings
        eepromConfig.version = checkNewEEPROMConf;

	    ///////////////////////////////

        eepromConfig.accelTCBiasSlope[XAXIS] = 0.0f;
        eepromConfig.accelTCBiasSlope[YAXIS] = 0.0f;
        eepromConfig.accelTCBiasSlope[ZAXIS] = 0.0f;

        ///////////////////////////////

        eepromConfig.accelTCBiasIntercept[XAXIS] = 0.0f;
        eepromConfig.accelTCBiasIntercept[YAXIS] = 0.0f;
        eepromConfig.accelTCBiasIntercept[ZAXIS] = 0.0f;

        ///////////////////////////////

        eepromConfig.gyroTCBiasSlope[ROLL ] = 0.0f;
        eepromConfig.gyroTCBiasSlope[PITCH] = 0.0f;
        eepromConfig.gyroTCBiasSlope[YAW  ] = 0.0f;

	    ///////////////////////////////

	    eepromConfig.gyroTCBiasIntercept[ROLL ] = 0.0f;
	    eepromConfig.gyroTCBiasIntercept[PITCH] = 0.0f;
	    eepromConfig.gyroTCBiasIntercept[YAW  ] = 0.0f;

	    ///////////////////////////////

	    eepromConfig.magBias[XAXIS] = 0.0f;
	    eepromConfig.magBias[YAXIS] = 0.0f;
	    eepromConfig.magBias[ZAXIS] = 0.0f;

		///////////////////////////////

		eepromConfig.accelCutoff = 1.0f;

		///////////////////////////////

	    eepromConfig.KpAcc = 5.0f;    // proportional gain governs rate of convergence to accelerometer
	    eepromConfig.KiAcc = 0.0f;    // integral gain governs rate of convergence of gyroscope biases
	    eepromConfig.KpMag = 5.0f;    // proportional gain governs rate of convergence to magnetometer
	    eepromConfig.KiMag = 0.0f;    // integral gain governs rate of convergence of gyroscope biases

	    ///////////////////////////////

	    eepromConfig.dlpfSetting = BITS_DLPF_CFG_98HZ;

	    ///////////////////////////////

	    eepromConfig.compFilterA =  0.005f;
		eepromConfig.compFilterB =  0.005f;

	    ///////////////////////////////

	    eepromConfig.receiverType  = PARALLEL_PWM;
	    eepromConfig.spektrumChannels = 7;
	    eepromConfig.spektrumHires = 0;

	    parseRcChannels("TAER1234");

	    eepromConfig.escPwmRate   = 450;
        eepromConfig.servoPwmRate = 50;

        eepromConfig.mixerConfiguration = MIXERTYPE_QUADX;
        eepromConfig.yawDirection = 1.0f;

        eepromConfig.midCommand   = 3000.0f;
        eepromConfig.minCheck     = (float)(MINCOMMAND + 200);
        eepromConfig.maxCheck     = (float)(MAXCOMMAND - 200);
        eepromConfig.minThrottle  = (float)(MINCOMMAND + 200);
        eepromConfig.maxThrottle  = (float)(MAXCOMMAND);

        eepromConfig.PID[ROLL_RATE_PID].B              =   1.0f;
        eepromConfig.PID[ROLL_RATE_PID].P              = 120.0f;
        eepromConfig.PID[ROLL_RATE_PID].I              =   0.0f;
        eepromConfig.PID[ROLL_RATE_PID].D              =   0.0f;
        eepromConfig.PID[ROLL_RATE_PID].iTerm          =   0.0f;
        eepromConfig.PID[ROLL_RATE_PID].windupGuard    = 200.0f;  // PWMs
        eepromConfig.PID[ROLL_RATE_PID].lastError      =   0.0f;
        eepromConfig.PID[ROLL_RATE_PID].lastDterm      =   0.0f;
        eepromConfig.PID[ROLL_RATE_PID].lastLastDterm  =   0.0f;
        eepromConfig.PID[ROLL_RATE_PID].type           =   0;

        eepromConfig.PID[PITCH_RATE_PID].B             =   1.0f;
        eepromConfig.PID[PITCH_RATE_PID].P             = 120.0f;
        eepromConfig.PID[PITCH_RATE_PID].I             =   0.0f;
        eepromConfig.PID[PITCH_RATE_PID].D             =   0.0f;
        eepromConfig.PID[PITCH_RATE_PID].iTerm         =   0.0f;
        eepromConfig.PID[PITCH_RATE_PID].windupGuard   = 200.0f;  // PWMs
        eepromConfig.PID[PITCH_RATE_PID].lastError     =   0.0f;
        eepromConfig.PID[PITCH_RATE_PID].lastDterm     =   0.0f;
        eepromConfig.PID[PITCH_RATE_PID].lastLastDterm =   0.0f;
        eepromConfig.PID[PITCH_RATE_PID].type          =   0;

        eepromConfig.PID[YAW_RATE_PID].B               =   1.0f;
        eepromConfig.PID[YAW_RATE_PID].P               = 120.0f;
        eepromConfig.PID[YAW_RATE_PID].I               =   0.0f;
        eepromConfig.PID[YAW_RATE_PID].D               =   0.0f;
        eepromConfig.PID[YAW_RATE_PID].iTerm           =   0.0f;
        eepromConfig.PID[YAW_RATE_PID].windupGuard     = 200.0f;  // PWMs
        eepromConfig.PID[YAW_RATE_PID].lastError       =   0.0f;
        eepromConfig.PID[YAW_RATE_PID].lastDterm       =   0.0f;
        eepromConfig.PID[YAW_RATE_PID].lastLastDterm   =   0.0f;
        eepromConfig.PID[YAW_RATE_PID].type            =   0;

        eepromConfig.PID[ROLL_ATT_PID].B               =   1.0f;
        eepromConfig.PID[ROLL_ATT_PID].P               =   2.0f;
        eepromConfig.PID[ROLL_ATT_PID].I               =   0.0f;
        eepromConfig.PID[ROLL_ATT_PID].D               =   0.0f;
        eepromConfig.PID[ROLL_ATT_PID].iTerm           =   0.0f;
        eepromConfig.PID[ROLL_ATT_PID].windupGuard     =   0.5f;  // radians/sec
        eepromConfig.PID[ROLL_ATT_PID].lastError       =   0.0f;
        eepromConfig.PID[ROLL_ATT_PID].lastDterm       =   0.0f;
        eepromConfig.PID[ROLL_ATT_PID].lastLastDterm   =   0.0f;
        eepromConfig.PID[ROLL_ATT_PID].type            =   1;

        eepromConfig.PID[PITCH_ATT_PID].B              =   1.0f;
        eepromConfig.PID[PITCH_ATT_PID].P              =   2.0f;
        eepromConfig.PID[PITCH_ATT_PID].I              =   0.0f;
        eepromConfig.PID[PITCH_ATT_PID].D              =   0.0f;
        eepromConfig.PID[PITCH_ATT_PID].iTerm          =   0.0f;
        eepromConfig.PID[PITCH_ATT_PID].windupGuard    =   0.5f;  // radians/sec
        eepromConfig.PID[PITCH_ATT_PID].lastError      =   0.0f;
        eepromConfig.PID[PITCH_ATT_PID].lastDterm      =   0.0f;
        eepromConfig.PID[PITCH_ATT_PID].lastLastDterm  =   0.0f;
        eepromConfig.PID[PITCH_ATT_PID].type           =   1;

        eepromConfig.PID[HEADING_PID].B                =   1.0f;
        eepromConfig.PID[HEADING_PID].P                =   3.0f;
        eepromConfig.PID[HEADING_PID].I                =   0.0f;
        eepromConfig.PID[HEADING_PID].D                =   0.0f;
        eepromConfig.PID[HEADING_PID].iTerm            =   0.0f;
        eepromConfig.PID[HEADING_PID].windupGuard      =   0.5f;  // radians/sec
        eepromConfig.PID[HEADING_PID].lastError        =   0.0f;
        eepromConfig.PID[HEADING_PID].lastDterm        =   0.0f;
        eepromConfig.PID[HEADING_PID].lastLastDterm    =   0.0f;
        eepromConfig.PID[HEADING_PID].type             =   1;

        eepromConfig.gimbalRollServoMin   = 2000.0f;
		eepromConfig.gimbalRollServoMid   = 3000.0f;
		eepromConfig.gimbalRollServoMax   = 4000.0f;
		eepromConfig.gimbalRollServoGain  = 1.0f;

		eepromConfig.gimbalPitchServoMin  = 2000.0f;
		eepromConfig.gimbalPitchServoMid  = 3000.0f;
		eepromConfig.gimbalPitchServoMax  = 4000.0f;
		eepromConfig.gimbalPitchServoGain = 1.0f;

        eepromConfig.rollDirectionLeft    = -1.0f;
        eepromConfig.rollDirectionRight   =  1.0f;
        eepromConfig.pitchDirectionLeft   = -1.0f;
        eepromConfig.pitchDirectionRight  =  1.0f;

        eepromConfig.wingLeftMinimum      = 2000.0f;
        eepromConfig.wingLeftMaximum      = 4000.0f;
        eepromConfig.wingRightMinimum     = 2000.0f;
        eepromConfig.wingRightMaximum     = 4000.0f;

        eepromConfig.biLeftServoMin       = 2000.0f;
        eepromConfig.biLeftServoMid       = 3000.0f;
        eepromConfig.biLeftServoMax       = 4000.0f;

        eepromConfig.biRightServoMin      = 2000.0f;
        eepromConfig.biRightServoMid      = 3000.0f;
        eepromConfig.biRightServoMax      = 4000.0f;

        eepromConfig.triYawServoMin       = 2000.0f;
        eepromConfig.triYawServoMid       = 3000.0f;
        eepromConfig.triYawServoMax       = 4000.0f;

        eepromConfig.vTailAngle           = 40.0f;

        // Free Mix Defaults to Quad X
		eepromConfig.freeMixMotors        = 4;

		eepromConfig.freeMix[0][ROLL ]    =  1.0f;
        eepromConfig.freeMix[0][PITCH]    = -1.0f;
        eepromConfig.freeMix[0][YAW  ]    = -1.0f;

        eepromConfig.freeMix[1][ROLL ]    = -1.0f;
        eepromConfig.freeMix[1][PITCH]    = -1.0f;
        eepromConfig.freeMix[1][YAW  ]    =  1.0f;

        eepromConfig.freeMix[2][ROLL ]    = -1.0f;
        eepromConfig.freeMix[2][PITCH]    =  1.0f;
        eepromConfig.freeMix[2][YAW  ]    = -1.0f;

        eepromConfig.freeMix[3][ROLL ]    =  1.0f;
        eepromConfig.freeMix[3][PITCH]    =  1.0f;
        eepromConfig.freeMix[3][YAW  ]    =  1.0f;

        eepromConfig.freeMix[4][ROLL ]    =  0.0f;
        eepromConfig.freeMix[4][PITCH]    =  0.0f;
        eepromConfig.freeMix[4][YAW  ]    =  0.0f;

        eepromConfig.freeMix[5][ROLL ]    =  0.0f;
        eepromConfig.freeMix[5][PITCH]    =  0.0f;
        eepromConfig.freeMix[5][YAW  ]    =  0.0f;

        writeEEPROM();
	}
}

///////////////////////////////////////////////////////////////////////////////
