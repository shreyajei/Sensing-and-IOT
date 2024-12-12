import serial
import numpy as np
import tensorflow as tf
from sklearn.preprocessing import StandardScaler
import joblib
import time
import os

# Debug: Print current working directory and check files
print("Current Working Directory:", os.getcwd())
print("Contents of current directory:", os.listdir())

# Configuration
SERIAL_PORT = 'COM6'  # Change this to match your Arduino's serial port
BAUD_RATE = 115200

# Load Model and Scaler
try:
    print("Loading model...")
    model = tf.keras.models.load_model('breastfeeding_model', compile=False)
    print("Model loaded successfully!")
except Exception as e:
    print(f"Error loading model: {e}")
    exit()

try:
    print("Loading scaler...")
    scaler = joblib.load('breastfeeding_scaler.joblib')
    print("Scaler loaded successfully!")
except FileNotFoundError:
    print("Scaler file not found!")
    scaler = StandardScaler()
    scaler.mean_ = np.array([0, 0, 0])
    scaler.scale_ = np.array([1, 1, 1])

def classify_data(roll, pitch, flex):
    """
    Classify incoming sensor data.
    :return: 1 for correct position, 0 for incorrect position.
    """
    input_data = np.array([[roll, pitch, flex]])
    try:
        input_normalized = scaler.transform(input_data)
        prediction = model.predict(input_normalized)
        return 1 if prediction[0][0] > 0.5 else 0
    except Exception as e:
        print(f"Error during classification: {e}")
        return None

def main():
    try:
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
        print(f"Connected to {SERIAL_PORT}")
        
        while True:
            if ser.in_waiting > 0:
                try:
                    line = ser.readline().decode('utf-8').strip()
                    print(f"Received: {line}")

                    parts = line.split(',')
                    if len(parts) == 5:
                        timestamp, roll, pitch, yaw, flex = parts
                        roll, pitch, flex = float(roll), float(pitch), float(flex)

                        classification = classify_data(roll, pitch, flex)
                        if classification is not None:
                            print(f"Classification: {classification}")
                            ser.write(b'1' if classification == 1 else b'0')
                
                except Exception as e:
                    print(f"Error processing data: {e}")
            
            time.sleep(0.1)

    except serial.SerialException as e:
        print(f"Error: {e}")
    finally:
        if 'ser' in locals():
            ser.close()

if __name__ == "__main__":
    main()
