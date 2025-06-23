
# Hovercraft Build w/ Maze solving algorithm

This project contains two main coding components:
- A Roblox Lua script to compute and visualize the center of mass of a hovercraft model.
- An Arduino code to control the hovercraft's hardware components such as motors and sensors.

# Project Structure
├── hovercraft_model            RBLX file for the 3D model representation of the hovercraft

├── centerofmass_script.lua     Lua script for computing and visualizing center of mass in Roblox Studio

├── hovercraft_code.ino         Arduino code for controlling hovercraft components (thrusters, sensors)

└── README.md                   Project documentation


# Overview

centerofmass_script.lua
  
  A Roblox Lua script that:
  - Calculates the center of mass of a hovercraft based on individual part masses and positions.
  - Creates a visual marker at the calculated center of mass (Neon white cube in the workspace).

# hovercraft.ino
  
  An Arduino sketch that:
  - Interfaces with the hovercraft’s motors, sensors, and other electronics.
  - Handles signals to operate the thrust and lift fans, manage balance, and possibly collect data from sensors (ultrasonics, IMU, etc).

# Requirements
  Roblox Studio
  Arduino
  Motors, fans, sensors (IMU, ultrasonic, IR, etc..)

# Future Improvements
- Implement more advanced control algorithms in Arduino (e.g., PID controllers).
- Integrate more Ultrasonic sensors instead of IR for accuracy when threading long distances.
