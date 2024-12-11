#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

// Flex sensor pin
const int flexPin = 33;

// average filter variables
const int FILTER_SIZE = 10;
int flexValueHistory[FILTER_SIZE];
int flexValueHistoryIndex = 0;
int flexValueSum = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  mpu.initialize();
  
  if (mpu.testConnection()) {
    Serial.println("MPU6050 connected!");
  } else {
    Serial.println("Connection failed.");
  }

  // Initialising the flex sensor history array
  for (int i = 0; i < FILTER_SIZE; i++) {
    flexValueHistory[i] = 0;
  }
}

void loop() {
  int16_t ax, ay, az;
  int16_t gx, gy, gz;
  
  // Read flex sensor value and map it from 0-1023 to 0-100
  int flexValue = map(analogRead(flexPin), 0, 1023, 0, 100);

  // Apply moving average filter
  flexValueSum -= flexValueHistory[flexValueHistoryIndex];
  flexValueHistory[flexValueHistoryIndex] = flexValue;
  flexValueSum += flexValue;
  flexValueHistoryIndex = (flexValueHistoryIndex + 1) % FILTER_SIZE;
  int filteredFlexValue = flexValueSum / FILTER_SIZE;

  // Read accelerometer and gyroscope values
  mpu.getAcceleration(&ax, &ay, &az);
  mpu.getRotation(&gx, &gy, &gz);
  
  // Convert gyroscope values to degrees/second
  float roll = gx / 131.0;    
  float pitch = gy / 131.0;
  float yaw = gz / 131.0;

  // Send data to the Serial Plotter
  Serial.print("Roll:");    Serial.print(roll);
  Serial.print(",");        
  Serial.print("Pitch:");   Serial.print(pitch);
  Serial.print(",");
  Serial.print("Yaw:");     Serial.print(yaw);
  Serial.print(",");
  Serial.print("Flex:");    Serial.println(filteredFlexValue);

  delay(50);
}