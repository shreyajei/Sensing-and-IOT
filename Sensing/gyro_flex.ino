#include <Wire.h>

// MPU6050 I2C address
const int MPU = 0x68;

// Flex sensor pin
const int FLEX_PIN = 33;

// LED pin
const int ledPin = 5;

// Flex sensor threshold
const int flexThreshold = 80;

// Variables for MPU6050 data
float AccX, AccY, AccZ;
float GyroX, GyroY, GyroZ;
float accAngleX, accAngleY, gyroAngleX, gyroAngleY, gyroAngleZ;
float roll, pitch, yaw = 0;
float AccErrorX = 0, AccErrorY = 0, GyroErrorX = 0, GyroErrorY = 0, GyroErrorZ = 0;
float elapsedTime, currentTime, previousTime;
int c = 0;

// Sampling period in milliseconds (for 10Hz sampling rate, this is 100ms)
const unsigned long samplingPeriod = 100;
unsigned long previousMillis = 0;

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  
  // Initialize LED pin
  pinMode(ledPin, OUTPUT);
  
  // Initialize I2C communication
  Wire.begin();
  
  // Initialize MPU6050
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // Wake up the MPU6050
  Wire.endTransmission(true);
  
  // Configure Accelerometer
  Wire.beginTransmission(MPU);
  Wire.write(0x1C);  // ACCEL_CONFIG register
  Wire.write(0x10);  // Set to +/- 8g full scale range
  Wire.endTransmission(true);
  
  // Configure Gyroscope
  Wire.beginTransmission(MPU);
  Wire.write(0x1B);  // GYRO_CONFIG register
  Wire.write(0x10);  // Set to 1000 deg/s full scale range
  Wire.endTransmission(true);
  
  // Calculate IMU errors
  calculate_IMU_error();
  
  delay(20);
}

void loop() {
  // Check if the sampling period has passed
  if (millis() - previousMillis >= samplingPeriod) {
    previousMillis = millis();  // Update the last time the data was sampled
    
    // Read MPU6050 data
    readMPU6050();
    
    // Read Flex Sensor
    int flexValue = readFlexSensor();
    
    // Print data to Serial Monitor
    Serial.print("Roll: ");
    Serial.print(roll);
    Serial.print(" | Pitch: ");
    Serial.print(pitch);
    Serial.print(" | Yaw: ");
    Serial.print(yaw);
    Serial.print(" | Flex Sensor: ");
    Serial.println(flexValue);
    
    // Control LED based on flex sensor threshold
    if (flexValue > flexThreshold) {
      digitalWrite(ledPin, HIGH);  // Turn on LED
    } else {
      digitalWrite(ledPin, LOW);   // Turn off LED
    }
  }
}

void calculate_IMU_error() {
  // Calculate accelerometer and gyroscope error values
  c = 0;
  while (c < 200) {
    Wire.beginTransmission(MPU);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    
    AccX = (Wire.read() << 8 | Wire.read()) / 16384.0;
    AccY = (Wire.read() << 8 | Wire.read()) / 16384.0;
    AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0;
    
    // Swapped Y and Z for vertical orientation
    AccErrorX = AccErrorX + ((atan((AccZ) / sqrt(pow((AccX), 2) + pow((AccY), 2))) * 180 / PI));
    AccErrorY = AccErrorY + ((atan(-1 * (AccX) / sqrt(pow((AccZ), 2) + pow((AccY), 2))) * 180 / PI));
    c++;
  }
  
  // Divide the sum by 200 to get the error value
  AccErrorX = AccErrorX / 200;
  AccErrorY = AccErrorY / 200;
  
  // Reset counter
  c = 0;
  
  // Calculate gyroscope errors
  while (c < 200) {
    Wire.beginTransmission(MPU);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    
    GyroX = Wire.read() << 8 | Wire.read();
    GyroY = Wire.read() << 8 | Wire.read();
    GyroZ = Wire.read() << 8 | Wire.read();
    
    // Swapped Y and Z for vertical orientation
    GyroErrorX = GyroErrorX + (GyroX / 131.0);
    GyroErrorY = GyroErrorY + (GyroZ / 131.0);
    GyroErrorZ = GyroErrorZ + (GyroY / 131.0);
    c++;
  }
  
  // Divide the sum by 200 to get the error value
  GyroErrorX = GyroErrorX / 200;
  GyroErrorY = GyroErrorY / 200;
  GyroErrorZ = GyroErrorZ / 200;
}

void readMPU6050() {
  // Store previous time
  previousTime = currentTime;
  currentTime = millis();
  elapsedTime = (currentTime - previousTime) / 1000; // Convert to seconds
  
  // Read Accelerometer data
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);  // Start with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true);
  
  AccX = (Wire.read() << 8 | Wire.read()) / 16384.0;
  AccY = (Wire.read() << 8 | Wire.read()) / 16384.0;
  AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0;
  
  // Calculating Roll and Pitch with Y and Z swapped for vertical orientation
  accAngleX = (atan(AccZ / sqrt(pow(AccX, 2) + pow(AccY, 2))) * 180 / PI) - AccErrorX;
  accAngleY = (atan(-1 * AccX / sqrt(pow(AccZ, 2) + pow(AccY, 2))) * 180 / PI) - AccErrorY;
  
  // Read Gyroscope data
  Wire.beginTransmission(MPU);
  Wire.write(0x43);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true);
  
  GyroX = (Wire.read() << 8 | Wire.read()) / 131.0;
  GyroY = (Wire.read() << 8 | Wire.read()) / 131.0;
  GyroZ = (Wire.read() << 8 | Wire.read()) / 131.0;
  
  // Swapped gyro corrections for vertical orientation
  GyroX = GyroX - GyroErrorX;
  GyroY = GyroY - GyroErrorZ;  // Note the swap with GyroZ
  GyroZ = GyroZ - GyroErrorY;  // Note the swap with GyroY
  
  // Integrate gyroscope values to get angles
  gyroAngleX = gyroAngleX + GyroX * elapsedTime;
  gyroAngleY = gyroAngleY + GyroY * elapsedTime;
  yaw = yaw + GyroZ * elapsedTime;
  
  // Complementary filter - combine accelerometer and gyro angle values
  gyroAngleX = 0.90 * gyroAngleX + 0.1 * accAngleX;
  gyroAngleY = 0.90 * gyroAngleY + 0.1 * accAngleY;
  
  // Final angle values
  roll = gyroAngleX;
  pitch = gyroAngleY;
}

int readFlexSensor() {
  // Read flex sensor value
  int flexRaw = analogRead(FLEX_PIN);
  int flexMapped = map(flexRaw, 0, 1023, 0, 100);
  
  return flexMapped;
}