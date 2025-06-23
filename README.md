
# Hovercraft Center of Mass Calculator & Controller

This project contains two main components:
- A **Roblox Lua script** to compute and visualize the center of mass of a hovercraft model.
- An **AVC/C code** to control the hovercraft's hardware components such as motors and sensors.

## 📂 Project Structure

```
├── centerofmass_script.lua     # Lua script for computing center of mass in Roblox Studio
├── hovercraft_code.ino         # Arduino code for the hovercraft
├── hovercraft_model            # RBLX file with 3d representation of the hovercraft
└── README.md                   # Project documentation
```

## 🚀 Overview

### `centerofmass_script.lua`
A Roblox Lua script that:
- Calculates the center of mass of a hovercraft based on individual part masses and positions.
- Creates a visual marker at the calculated center of mass (Neon white cube in the workspace).

### `hovercraft_code.ino`
An Arduino sketch that:
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

#### Hovercraft Logic:
- When the IR sensor detects an obstacle, it compares the US sensors at the location.
- When a difference in distance between the left and right ultrasonic sensors is detected, the system determines the turn direction (left or right).
- It uses gyroscope data to track yaw and ensure a 90-degree turn.
- If the turn takes too long (timeout), it resets the servo to a centered position.
- When there is an opening of a maze, the servo will automatically override everything and aim for the exit.

## 🛠 Requirements

### Roblox Studio

### Arduino
- Arduino board (e.g., Arduino Nano)
- Required electronics: motors, fans, sensors (IMU, ultrasonic, servo motors, etc.)

## 💡 Future Improvements
- Implement more advanced control algorithms in Arduino (e.g., PID controllers).


