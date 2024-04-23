#!/usr/bin/env python
"""
Serial Data Collection CSV

Collects raw data in CSV form over a serial connection and saves them to files.

Install dependencies:

    python -m pip install pyserial

The first line should be header information. Each sample should be on a newline.
Here is a raw accelerometer data sample (in m/s^2):

    accX,accY,accZ
    -0.22,0.82,10.19
    -0.05,0.77,9.63
    -0.01,1.10,8.50
    ...

The end of the sample should contain an empty line (e.g. \r\n\r\n).

Call this script as follows:

    python serial-data-collect-csv.py
    
Author: Shawn Hymel (EdgeImpulse, Inc.)
Date: June 17, 2022
License: Apache-2.0 (apache.org/licenses/LICENSE-2.0)
"""

import os
import uuid
import keyboard

# Third-party libraries
import serial
import serial.tools.list_ports
FLAG1 = False
FLAG2 = False
FLAG3 = False
def on_key_event(event):
    global FLAG1,FLAG2
    if event.name == 's':
        print("La tecla 's' ha sido pulsada.")
        # Envía el comando 'start' al puerto serie cuando se presiona la tecla 's'
        FLAG1 = True
    if event.name == 't':
        print("La tecla 't' ha sido pulsada.")
        # Envía el comando 'start' al puerto serie cuando se presiona la tecla 's'
        FLAG2 = True


# Create a file with unique filename and write CSV data to it
def write_csv(data, dir, label):

    # Keep trying if the file exists
    exists = True
    while exists:

        # Generate unique ID for file (last 12 characters from uuid4 method)
        uid = str(uuid.uuid4())[-12:]
        filename = label + "." + uid + ".csv"
        
        # Create and write to file if it does not exist
        out_path = os.path.join(dir, filename)
        if not os.path.exists(out_path):
            exists = False
            try:
                with open(out_path, 'w') as file:
                    file.write(data)
                print("Data written to:", out_path)
            except IOError as e:
                print("ERROR", e)
                return
    

    
# Parse arguments
port = 'COM13'
baud = 115200 
out_dir = 'C:/Users/Usuario/Desktop/Trabajo edge impulse/10'
label = "Puntillas"

# Configure serial port
ser = serial.Serial()
ser.port = port
ser.baudrate = baud
flag_received = False
# Attempt to connect to the serial port
try:
    ser.open()
except Exception as e:
    print("ERROR:", e)
    
    
print()
print("Connected to {} at a baud rate of {}".format(port, baud))
# Register callback function for keyboard event
keyboard.on_press(on_key_event)


# Serial receive buffer
rx_buf = b''

# Make output directory
try:
    os.makedirs(out_dir)
except FileExistsError:
    pass

# Loop forever (unless ctrl+c is captured)
try:
    while True:
        
        if(FLAG1):
            FLAG1 = False
            flag_received = True
            ser.write(b'k\n')
            print("Start")
            
        if(FLAG2):
            FLAG2 = False
            flag_received = False
            ser.write(b'w\n')
            print("Stop")
        
        # Read bytes from serial port
        if ser.in_waiting > 0 or flag_received:
            while(ser.in_waiting or flag_received):
                
                # Read bytes
                data_received = ser.read()
                
                if(FLAG1):
                    FLAG1 = False
                    flag_received = True
                    ser.write(b'k')
                    print("Start")
                    
                if(FLAG2):
                    FLAG2 = False
                    flag_received = False
                    ser.write(b'w')
                    print("Stop")
                    FLAG3 = True
                    
                if(data_received != b'k\n' and data_received != b'w\n' and flag_received):
                    
                    rx_buf += data_received
                    
                # Look for an empty line
                if FLAG3:
                    FLAG3 = False
                    print("stop2")
                    # Strip extra newlines (convert \r\n to \n)
                    buf_str = rx_buf.decode('utf-8').strip()
                    buf_str = buf_str.replace('\r', '')
                    #buf_str = buf_str.replace(',', ';')
                    # Write contents to file
                    write_csv(buf_str, out_dir, label)
                    rx_buf = b''
                    flag_received = False

# Look for keyboard interrupt (ctrl+c)
except KeyboardInterrupt:
    pass

# Close serial port
print("Closing serial port")
ser.close()