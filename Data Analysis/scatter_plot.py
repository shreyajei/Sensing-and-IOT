# https://matplotlib.org/stable/api/_as_gen/matplotlib.pyplot.scatter.html
import pandas as pd
import matplotlib.pyplot as plt

# Step 1: Load and Combine the CSV Files
data1 = pd.read_csv('chintobreast_correct.csv')
data1['position'] = 'Chin to breast'
data2 = pd.read_csv('chintobreast_correctposition.csv')
data2['position'] = 'Chin to breast and nipple positioning'
data3 = pd.read_csv('incorrect_nochin1.csv')
data3['position'] = 'No Chin 1'
data4 = pd.read_csv('incorrect_tiltaway.csv')
data4['position'] = 'Tilt towards the left of the breast'
data5 = pd.read_csv('incorrect_tilttowards.csv')
data5['position'] = 'Tilt towards the right of the breast'
data6 = pd.read_csv('incorrect_nochin2.csv')
data6['position'] = 'No Chin 2'

# Step 2: Preprocessing
# Convert columns to numeric
columns_to_plot = ['roll', 'pitch', 'flex']
for df in [data1, data2, data3, data4, data5, data6]:
    for col in columns_to_plot:
        df[col] = pd.to_numeric(df[col], errors='coerce')
    df.dropna(subset=columns_to_plot, inplace=True)

# Step 3: Create Scatter Plots for Each Position
fig, axes = plt.subplots(2, 3, figsize=(13, 8))
axes_flat = axes.flatten()

for i, data in enumerate([data1, data2, data3, data4, data5, data6]):
    ax = axes_flat[i]
    position = data['position'].unique()[0]
    
    # Plot roll and pitch with hex colors
    ax.scatter(data['roll'], data['flex'], color='#8993FF', label='Roll vs flex', alpha=0.6)
    ax.scatter(data['pitch'], data['flex'], color='#2C3D4F', label='Pitch vs flex', alpha=0.6)
    ax.set_title(f"{position}")
    ax.set_xlabel('Value')
    ax.set_ylabel('Flex Value')
    ax.legend()

plt.tight_layout()
plt.show()
