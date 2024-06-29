import socket
import threading
import DoBotArm as Dbt

# Function to bundle common operations
def move_dobot(ctrlBot, x, y):
    ctrlBot.moveArmXY(x, y)

# Function to set the origin (home position) of the DoBot
def set_origin(ctrlBot):
    homeX, homeY, homeZ = 250, 0, 50
    ctrlBot.moveArmXY(homeX, homeY)
    print("Origin set at position: ({}, {})".format(homeX, homeY))

# Function to map haptic coordinates to DoBot coordinates
def map_haptic_to_dobot(haptic_x, haptic_y):
    # Mapping haptic x from range [-130, 130] to DoBot x range [320, 175]
    dobot_x = 320 + (haptic_x - (-130)) * (175 - 320) / (130 - (-130))
    # Mapping haptic y from range [-84, 113] to DoBot y range [190, -190]
    dobot_y = 190 + (haptic_y - (-84)) * (-190 - 190) / (113 - (-84))
    return dobot_x, dobot_y

# Function to handle incoming socket connections and move the DoBot
def handle_client(conn, addr, ctrlBot):
    with conn:
        print(f"Connected by {addr}")
        while True:
            data = conn.recv(1024)
            if not data:
                break
            try:
                decoded_data = data.decode().strip()
                print(f"Received raw data: '{decoded_data}'")
                # Extracting relevant information
                parts = decoded_data.split(' ')
                if len(parts) >= 3:
                    haptic_x, haptic_y, _haptic_z = parts[1:4]
                    button = parts[-1].split(',')[0]  # Extracting the button value
                    haptic_x, haptic_y, button = map(float, [haptic_x, haptic_y, button])
                    print(f"Received haptic coordinates: x={haptic_x}, y={haptic_y}, button={button}")
                    if button == 1:
                        set_origin(ctrlBot)
                    elif button == 2:
                        dobot_x, dobot_y = map_haptic_to_dobot(haptic_x, haptic_y)
                        print(f"Mapped to DoBot coordinates: x={dobot_x}, y={dobot_y}")
                        move_dobot(ctrlBot, dobot_x, dobot_y)
                        print(f"Robot moved to position: ({dobot_x}, {dobot_y})")
                    else:
                        print("Unknown button value. Ignoring.")
                else:
                    raise ValueError("Insufficient data parts received.")
            except ValueError as e:
                print(f"Invalid data format: {e}")
            except Exception as e:
                print(f"An error occurred: {e}")

# Function to start the server
def start_server(host='127.0.0.1', port=65432):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind((host, port))
        s.listen()
        print(f"Listening on {host}:{port}")
        
        homeX, homeY, homeZ = 250, 0, 50
        ctrlBot = Dbt.DoBotArm(homeX, homeY, homeZ)  # Create DoBot Class Object with home position x, y, z
        ctrlBot.moveArmXY(homeX, homeY)  # Move to home position initially
        print("Robot moved to home position: ({}, {})".format(homeX, homeY))

        while True:
            conn, addr = s.accept()
            client_thread = threading.Thread(target=handle_client, args=(conn, addr, ctrlBot))
            client_thread.start()

if __name__ == "__main__":
    start_server()
