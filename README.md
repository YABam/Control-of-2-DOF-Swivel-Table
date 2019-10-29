# Control-of-2-DOF-Swivel-Table
This is a STM32F103ZET6 program used to control a 2-DOF swivel table.
DOF X uses a lead screw with a colsed-loop feedback motor to control. DOF Z uses a servo motor to control using a closed-loop driver.
The X angle is detected by a MPU9250 module, which sends data to STM32 with 100Hz.
Other hardwares like digital display is used to display some information like version or mode.
