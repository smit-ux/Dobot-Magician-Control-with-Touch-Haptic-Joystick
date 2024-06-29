# Dobot Magician Control with Touch Haptic Joystick

This project enables the control of the Dobot Magician robotic arm using a Touch Haptic Joystick.

## Project Structure

The project consists of two main directories:

1. **getcurrentdata**: Contains a C++ project for interfacing with the Touch Haptic Joystick and sending position data to the Dobot using TCP/IP. 
   - `getcurrent.cpp`: The main C++ file for handling joystick data.

2. **Touch and Dobot Teleop**: Contains the Python API for the Dobot.
   - `dobot_interface_with_haptic.py`: A Python script for controlling the Dobot Arm with the Touch Haptic Joystick.

## How to Start

### Prerequisites

- Ensure that your Touch Haptic Joystick is connected to your system.
- Ensure that the Dobot is connected to the same system as the joystick.

### Step-by-Step Instructions

1. **Run the C++ Executable:**
   - Navigate to `getcurrentdata/x64/Release/` and run `getcurrentdata.exe`. 
   - Ensure your joystick is connected to the system before running the executable to initialize it properly.

2. **Run the Python Script:**
   - Navigate to the `Touch and Dobot Teleop` directory.
   - Open and run `dobot_interface_with_haptic.py`.
   - Upon execution, the Dobot will move to its home position as defined in the script. Once it reaches the home position, you can start controlling the Dobot using the joystick.

### Important Configuration

- Update the path to your Dobot DLL file in `DobotDLLtype.py` within the `def load()` function to ensure proper loading of the Dobot library.

## Repository Structure

```
Dobot-Magician-Control-with-Touch-Haptic-Joystick/
├── getcurrentdata/
│   ├── x64/
│   │   ├── Release/
│   │   │   ├── getcurrentdata.exe
│   ├── getcurrent.cpp
├── Touch and Dobot Teleop/
│   ├── dobot_interface_with_haptic.py
│   ├── DobotDLLtype.py
```

Feel free to contribute, report issues, or suggest improvements!
