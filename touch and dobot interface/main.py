# Created by Hugo Nolte for course PA1414 - DoBot Magician Project
# 2019

import threading
import DoBotArm as Dbt

# Function to bundle common operations
def functions(ctrlBot, x, y):
    ctrlBot.moveArmXY(x, y)

# Function to set home position and then move the robot to given coordinates multiple times
def manualMode():
    homeX, homeY, homeZ = 250, 0, 50
    ctrlBot = Dbt.DoBotArm(homeX, homeY, homeZ)  # Create DoBot Class Object with home position x, y, z
    ctrlBot.moveArmXY(homeX, homeY)  # Move to home position initially
    print("Robot moved to home position: ({}, {})".format(homeX, homeY))
    
    while True:
        try:
            # Get user input for x and y coordinates
            x = float(input("Enter the X coordinate: "))
            y = float(input("Enter the Y coordinate: "))
            
            # Move the robot to the specified x, y coordinates
            functions(ctrlBot, x, y)
            print("Robot moved to position: ({}, {})".format(x, y))
        except ValueError:
            print("Invalid input. Please enter numeric values for X and Y coordinates.")

# --Main Program--
def main():
    manualMode()

main()
