/**
  @page InputCaptureMode AN2592 InputCaptureMode Readme file
  
  @verbatim
  ******************** (C) COPYRIGHT 2009 STMicroelectronics *******************
  * @file     InputCaptureMode/readme.txt 
  * @author   MCD Application Team
  * @version  V3.0.0
  * @date     04/27/2009
  * @brief    Description of the AN2592 Application note's InputCaptureMode
  *           example.
  ******************************************************************************
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  ******************************************************************************
   @endverbatim

@par Example Description 

This example demonstrates how two perform a 32-bit input capture using timer link
system, frequencies down to 17 mHz can be measured.
The "ExtSignalFreq" variable contains the external signal frequency in mHz, use your
debugger Watch window to monitor its value.

@par Directory contents 

  - InputCaptureMode/inc/stm32f10x_conf.h  Library Configuration file
  - InputCaptureMode/src/stm32f10x_it.c    Interrupt handlers
  - InputCaptureMode/inc/stm32f10x_it.h    Interrupt handlers header file
  - InputCaptureMode/src/main.c            Main program

@par Hardware and Software environment 

  - This example runs on STM32F10x High-Density, STM32F10x Medium-Density and
    STM32F10x Low-Density Devices.
  
  - This example has been tested with STMicroelectronics STM3210E-EVAL (STM32F10x 
    High-Density) and STM3210B-EVAL (STM32F10x Medium-Density) evaluation boards 
    and can be easily tailored to any other supported device and development 
    board.

  - STM3210E-EVAL and STM3210B-EVAL Set-up 
    - Connect the external signal to measure to TIM2 CH1 pin(PA.00) and TIM3 CH1 
      pin(PA.06)       

@par How to use it ? 

 - RVMDK 
    - Open the Timer_InputCapture.Uv2 project
    - In the build toolbar select the project config:
        - STM3210B-EVAL: to configure the project for STM32 Medium-density devices
        - STM3210E-EVAL: to configure the project for STM32 High-density devices
    - Rebuild all files: Project->Rebuild all target files
    - Load project image: Debug->Start/Stop Debug Session
    - Run program: Debug->Run (F5)    

 - EWARMv5 
    - Open the Timer_InputCapture.eww workspace.
    - In the workspace toolbar select the project config:
        - STM3210B-EVAL: to configure the project for STM32 Medium-density devices
        - STM3210E-EVAL: to configure the project for STM32 High-density devices
    - Rebuild all files: Project->Rebuild all
    - Load project image: Project->Debug
    - Run program: Debug->Go(F5)

 - RIDE 
    - Open the Timer_InputCapture.rprj project.
    - In the configuration toolbar(Project->properties) select the project config:
       - STM3210B-EVAL: to configure the project for STM32 Medium-density devices
       - STM3210E-EVAL: to configure the project for STM32 High-density devices
    - Rebuild all files: Project->build project
    - Load project image: Debug->start(ctrl+D)
    - Run program: Debug->Run(ctrl+F9)  

@note
 - Low-density devices are STM32F101xx and STM32F103xx microcontrollers where
   the Flash memory density ranges between 16 and 32 Kbytes.
 - Medium-density devices are STM32F101xx and STM32F103xx microcontrollers where
   the Flash memory density ranges between 32 and 128 Kbytes.
 - High-density devices are STM32F101xx and STM32F103xx microcontrollers where
   the Flash memory density ranges between 256 and 512 Kbytes.  

 * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
 */
