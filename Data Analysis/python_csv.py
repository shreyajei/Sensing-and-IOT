import serial
import csv
import time

# Openin the serial connection
ser = serial.Serial('COM6', 115200, timeout=1)  

# Creating new filename using the current timestamp
timestamp = time.strftime('%Y-%m-%d_%H-%M-%S')  
filename = f'sensor_data_{timestamp}.csv'

# Create the CSV file and write the header
with open(filename, 'w', newline='') as csvfile:
    writer = csv.writer(csvfile)
    # Write the header to the CSV file
    writer.writerow(['Timestamp', 'roll', 'pitch', 'yaw', 'flex'])

    sample_count = 0
    while sample_count < 400:  # Collect 400 samples
        try:
            # Read a line from serial
            line = ser.readline().decode('utf-8', errors='ignore').strip()

            if line:
                # Parsing the line into individual values
                parts = line.split(' | ')
                if len(parts) == 4:
                    roll = parts[0].split(':')[1].strip()
                    pitch = parts[1].split(':')[1].strip()
                    yaw = parts[2].split(':')[1].strip()
                    flex = parts[3].split(':')[1].strip()

                    timestamp = time.strftime('%Y-%m-%d %H:%M:%S')

                    # Write to CSV
                    writer.writerow([timestamp, roll, pitch, yaw, flex])

                    # Print the sample data
                    print(f"Sample {sample_count + 1}: Roll={roll}, Pitch={pitch}, Yaw={yaw}, Flex={flex}")

                    sample_count += 1
                else:
                    print(f"Invalid data format: {line}")
        except Exception as e:
            print(f"Error reading from serial: {e}")
            break

ser.close()
print(f"Data collection completed. {sample_count} samples saved to {filename}.")
