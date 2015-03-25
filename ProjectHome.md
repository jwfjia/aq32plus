Features of aq32Plus:

1)Focused at the capabilities of the aq32/STM32F407 hardware

2)1 KHz sampling of gyro and accel, averaged to 500 Hz for use

3)Pressure data sampling at 100 Hz, averged to 10 Hz for use

4)Mag data sampling at 50 Hz

5)Interrupt driven system timing, 1 KHz max rate for sensor reads

6)500Hz, 200Hz, 100Hz, 50Hz, 10 Hz, 5 Hz, and 1 Hz task frames.  Not quite a RTOS.

7)1 to 8 ESC PWM outputs, adjustable output rate, default 450 Hz, 0.5 uSec resolution

8)1 to 3 Servo PWM outputs, adjustable output rate, default 50 Hz, 0.5 uSec resolution

9)Parallel PWM (default), serial PWM, and Spektrum RX support(binding with RX too) 

10)MARG attitude estimator with functioning accel cutoff

11)Attitude estimation and flight control inner loops running at 500 Hz

12)Delta times for integration functions computed via hardware timers at 0.5 uSec resolution

13)Wide range of mixers:
> Gimbal
> Flying Wing
> BiCopter
> TriCopter
> QuadP, QuadX, VTail(6 different schemes), Y4
> Hex6P, Hex6X, Y6
> OctoF8P, OctoF8X, OctoX8P, OctoX8X
> FreeMix (Up to 8 motors/Surfaces)

14)Drivers for I2C 1 and 2, SPI 1,2, and 3, uart2(GPS), uart3(RF telem for now)

15)Setup via command line interface (CLI), no configurator interface at this time

16)Limited high speed (100 Hz) RF telemetry for sensor evaluation and PID tuning

17)Built with the Eclipse/Code Sourcery/STM Standard Peripherial Library tools

18)Programmed thru USB port via STM DFUSE demo program

19)Not Arduino compatible (sorry)

20)Whatever I forgot to add here......

 PWM at 0.5 uSec resolution, both low res and hi res Spektrum data

Bench Tested Features of aq32Plus:

1)Parallel PWM Inputs
2)Spektrum Satellite Input and Binding, 8 channel low res mode
3)Quad X Calculations
4)CLI Setup
5)RF Telem

Flight Tested Features of aq32Plus:

1)Spektrum Satellite Input and Binding, 8 channel low res mode
2)Quad X
3)CLI Setup

Planned Additions (in no particular order):

  1. Vertical VelocityHold/Altitude Hold
> 2)Velocity Hold
> 3)Position Hold
> 4)Battery Monitor
> 5)SD Card Read/Write
> 6)AGL Sensor
> 7)Optical Flow velocity/position aiding
> 8)OSD
> 9)Second Spektrum Satellite
10)PID autotuning