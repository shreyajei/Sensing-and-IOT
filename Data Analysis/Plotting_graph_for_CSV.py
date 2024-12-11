import pandas as pd
import matplotlib.pyplot as plt

# Loading csv file
data = pd.read_csv('incorrect_nochin2.csv')

# Convert 'roll', 'pitch', 'yaw', and 'flex' columns to numeric values
data['roll'] = pd.to_numeric(data['roll'], errors='coerce')
data['pitch'] = pd.to_numeric(data['pitch'], errors='coerce')
data['yaw'] = pd.to_numeric(data['yaw'], errors='coerce')
data['flex'] = pd.to_numeric(data['flex'], errors='coerce')

# Drop any rows that have no values in roll, pitch, yaw, or flex columns
data.dropna(subset=['roll', 'pitch', 'yaw', 'flex'], inplace=True)

# Convert the index to a NumPy array for matplotlib
index_array = data.index.to_numpy()

plt.figure(figsize=(10, 8))

# plotting data
plt.subplot(4, 1, 1)
plt.plot(index_array, data['roll'].to_numpy(), label='Roll', color='b')
plt.title('Roll Over Samples')
plt.xlabel('Sample Index')
plt.ylabel('Roll (degrees)')
plt.grid(True)

plt.subplot(4, 1, 2)
plt.plot(index_array, data['pitch'].to_numpy(), label='Pitch', color='g')
plt.title('Pitch Over Samples')
plt.xlabel('Sample Index')
plt.ylabel('Pitch (degrees)')
plt.grid(True)

plt.subplot(4, 1, 3)
plt.plot(index_array, data['yaw'].to_numpy(), label='Yaw', color='c')
plt.title('Yaw Over Samples')
plt.xlabel('Sample Index')
plt.ylabel('Yaw (degrees)')
plt.grid(True)

plt.subplot(4, 1, 4)
plt.plot(index_array, data['flex'].to_numpy(), label='Flex', color='r')
plt.title('Flex Sensor Value Over Samples')
plt.xlabel('Sample Index')
plt.ylabel('Flex')
plt.grid(True)

plt.tight_layout()

plt.show()
