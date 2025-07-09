
# Autonomous Obstacle-Solving Hovercraft
_Placed 1st in the ENGR 290 Competition at Concordia University!_
<br />
<br />



## Project Structure
This project contains two main components:
- A **Roblox Lua script** to compute and visualize the center of mass of a hovercraft model.
- An **AVC/C code** to control the hovercraft's hardware components such as motors and sensors.

```
├── centerOfMass.lua            # Lua script for physical visualisation of center of mass in Roblox Studio
├── centerOfMassGraph.m         # MATLAB script for graph visualisation of center of mass
├── hovercraft.c                # Code for the hovercraft
├── hovercraft.rblx             # RBLX file with 3d representation of the hovercraft
├── hovercraft.obj              # OBJ file with 3d render of the hovercraft
├── tests.ino                   # Ardunio code for testing/debugging the hovercraft's components
└── README.md                   # Project documentation
```
<br />

## Overview

### `centerOfMass.lua`
A Roblox Lua script that:
- Calculates the center of mass of a hovercraft based on individual part masses and positions.
- Creates a visual marker at the calculated center of mass (Neon white cube in the workspace).

### `centerOfMass.m`
A MATLAB script that:
- Calculates the center of mass of a hovercraft based on individual part masses and positions.
- Creates a graph with center of mass' coordinates relative to the hovercraft's base.

### `hovercraft.c`
An AVC/C code that:
- Interfaces with the **MPU6050** gyro sensor to monitor the hovercraft's yaw (rotation).
- Reads data from **two ultrasonic sensors** & **an infrared sensor** for measuring distances to obstacles on the front, left and right.
- Controls a **servo motor** to rotate the hovercraft based on the sensor data, performing turns when necessary.
- Sends real-time distance and turning status to a serial monitor over UART.
- Includes a basic **yaw control** system to ensure smooth and controlled turns, updating orientation during movement.

#### Key Functions:
- `MPU6050_init()`: Initializes the MPU6050 sensor.
- `MPU6050_read_gyro_z()`: Reads the gyroscope data to detect rotation.
- `trigger_US()`: Triggers the ultrasonic sensors to measure distance.
- `servo_left()`, `servo_right()`, `servo_center()`: Controls the servo motors based on the movement direction.

#### Turning Logic:
- When a difference in distance between the left and right ultrasonic sensors is detected, the system determines the turn direction (left or right).
- It uses gyroscope data to track yaw and ensure a 90-degree turn.
- If the turn takes too long (timeout), it resets the servo to a centered position.
<br />

### `hovercraft.rblx`
A RBLX model that:
- Visualises the physical components of the hovercraft.
- Follows precise measurements taken during construction of the hovercraft.

### `hovercraft.obj`
A OBJ model that:
- Renders the hovercraft design via CAD.

### `tests.ino`
An Arduino sketch that:
- Interfaces with all critical components of the hovercraft.
- Runs tests on each component and facilitates debugging & troubleshooting.
  
## Requirements

### MATLAB or Octave

### Roblox Studio

### Arduino kit
- Arduino board (e.g., Arduino Nano)
- ATMega328p board
- Required electronics: motors, fans, sensors (IMU, ultrasonic, servo motors, etc.)



