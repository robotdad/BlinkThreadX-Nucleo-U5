# ThreadX Nucleo-U575ZI-Q

This repository was created by following the steps below. This walks you through creating a Azure RTOS (threadx) project in CubeMX, importing it into VS Code, building it, adding application code to blink LEDs on the device, and debugging the running firmware on the board.

To use this just clone this repo and you can open in VS Code. Make sure you have completed the setup steps, then you can jump ahead to building the firmware.

```
git clone https://github.com/robotdad/BlinkThreadX-Nucleo-U5.git
cd BlinkThreadX-Nucleo-U5
code .
```

# Setup
Have CubeMX, CubeIDE, and VS Code with Embedded Tools VS Code Extension installed.
Make sure you have updated your STLink firmware on your board.

I have included the steps for creating git repos and connecting them to GitHub below. If you are following along with those steps as well you need git and the GitHub cli installed.

# Azure RTOS from scratch with CubeMX

In CubeMX go to board selector, search for the Nucleo-U575ZI-Q, select it, and start project. Initialize all peripherals to default state, do not enable TrustZone.

Starting on the Pinout tab, under Categories, Middleware select THREADX, then select Core.

Under Categories, System Code, select Sys and change the Time base source to TIM6. Now select ICACHE and enable 1-way mode.

Select the Project Manager tab give it a name and select your project location. Select STM32CubeIDE for the toolchain and generate under root.

Navigate to where you saved your generated project and setup a git repo.
```
git init -b main
git add . && git commit -m "initial commit"
gh repo create
```

Follow the prompts for creating the repo on GitHub. You will want to add a remote, defaults are fine.

## Importing an ST Project
Open VS Code, in the command palette (Ctrl + Shift + P) use Create project from ST Project. This will launch the file browser, navigate to where the project you created in CubeMX is, and select the .cproject. If prompted to trust the authors, agree. You will be prompted to choose a configuration, select debug.

The result of the above actions is that a CMake project will be generated, default tasks will be created for the project for use in VS Code and VS, and a vcpkg-configuration file will be created and activated. The vcpkg step configures the environment so that CMake, Ninja, and the ST compilers from the STM32IDE are all on path and ready to use.

In VS Code you can select the git activity and commit the generated artifacts from the import.

You should now create a .gitignore file. Minimally make sure to exclude the build folder by adding the line "build/" to the file. You can also copy a [complete .gitignore from here](https://raw.githubusercontent.com/robotdad/BlinkThreadX-Nucleo-U5/main/.gitignore). When done commit the file. 

## Creating tasks
Now we want to create some threads to blink the leds on the board. In the file explorer in VS Code expand the folder Core > Src and select app_threadx.c. 

Notice that there are comments of USER CODE BEGIN and USER CODE END throughout the source. Always place your changes between these blocks as it will be preserved if you need to regenerate code using CubeMX. You may need to do that if you make changes to peripheral options under pinout for example.

In the user code section for includes add a reference to main.h. We want to do this to use friendly names for some of the peripherals on the board that were generated in CubeMX.
```
#include "main.h"
```

In the user section for private defines we need to define the thread size.
```
#define THREAD_STACK_SIZE 1024
```

In the private variables section define a thread stack and thread pointer for each of our leds.
```
uint8_t red_thread_stack[THREAD_STACK_SIZE];
TX_THREAD red_thread_ptr;
uint8_t green_thread_stack[THREAD_STACK_SIZE];
TX_THREAD green_thread_ptr;
uint8_t blue_thread_stack[THREAD_STACK_SIZE];
TX_THREAD blue_thread_ptr;
```

In the function prototypes user code section define ones for each led.
```
VOID red_thread(ULONG initial_input);
VOID green_thread(ULONG initial_input);
VOID blue_thread(ULONG initial_input);
```

Select Ctrl + T to search for symbols, and search for App_ThreadX_Init and select it (not the declaration). In the user code block for App_ThreadX_Init add the following lines to create a thread for each led.

```
tx_thread_create(&red_thread_ptr, "Red LED", red_thread, 0x1234, red_thread_stack, THREAD_STACK_SIZE, 15, 15, 1, TX_AUTO_START);
tx_thread_create(&green_thread_ptr, "Green LED", green_thread, 0x1234, green_thread_stack, THREAD_STACK_SIZE, 15, 15, 1, TX_AUTO_START);
tx_thread_create(&blue_thread_ptr, "Blue LED", blue_thread, 0x1234, blue_thread_stack, THREAD_STACK_SIZE, 15, 15, 1, TX_AUTO_START);
```

Now, at the end of the file in the user code section we will add our implementations for each led function.
```
VOID red_thread(ULONG initial_input)
{
  while (1)
  {
    HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
    tx_thread_sleep(50);
  }
}
VOID green_thread(ULONG initial_input)
{
  while (1)
  {
    HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
    tx_thread_sleep(50);
  }
}
VOID blue_thread(ULONG initial_input)
{
  while (1)
  {
    HAL_GPIO_TogglePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin);
    tx_thread_sleep(50);
  }
}
```

Now, to understand what is being used on the hardware right select LED_RED_GPIO_Port and select Peek, Peek Definition. You will see that this is defined as GPIOA. Do the same thing for LED_RED_Pin, or select Alt F12, and notice it is GPIO Pin 9. You can do the same for the same for the green and blue variables.

## Building the firmware

Select the CMakeLists.txt file in the file explorer and from the context menu select Clean Reconfigure All Projects. This ensures we have generated the build files using the proper compilers that are now on the path. From the context menu for CMakeLists.txt you can now select Build All Projects. This should build your firmware under the build directory.


## Debugging
Select the Debug icon in the activity bar. Launch should already be selected. You can select the gear icon to see the details of the provided launch configuration. Note that the miDebuggerPath and debugServerPath are configured using environment variables provided by the Embedded Tools extension that use ST's gdb and gdbserver executables provided by STM32IDE.

The postRemoteConnectCommands has been generated with the firmware name from the CMake project target to be loaded onto the device. There is also an svdPath configured to load the correct SVD file for our part.

By default stopAtConnect is set to True which will cause us to break in the startup handler.

Place a breakpoint in any of the led functions created above.

Connect your board and select the play button to start debugging, or press F5.

### Examine peripheral registers
When you hit a break point open the peripheral view from the command palette, select Focus on peripheral view. Expand the view that opens in the debug pane. Find the GPIO section corresponding to your breakpoint. The pins are under the ODR section. You can step over the HAL_GPIO_TogglePin call and either watch the value for ODR change, or expand it to see the specific pin value update.

GPIOs for LEDs:
* Red: GPIOG ODR2
* Green: GPIOC ODR7
* Blue: GPIOB ODR7

### Inspect the RTOS threads
When you hit a break point, or while the debugger is paused, open the RTOS view from the command palette, select Focus on RTOS view. 

This view will show you the RTOS threads in your application, what state they are in, the size, and other details. This varies somewhat depending on what the RTOS makes available. The view is automatically set to the detected RTOS for threadx, FreeRTOS, and Zephyr.

