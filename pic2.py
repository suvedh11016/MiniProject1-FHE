# import matplotlib.pyplot as plt

# # Data for the five expressions
# expressions = ['7*5 + 3*2 + 1', '129*8 + 9*5 + 5*2', '66 + 97*9 + 6']

# # Realistic noise values for with and without relinearization
# no_relinearization = [
#     [135, 120, 115, 110],  # Expression 1
#     [150, 130, 125, 120],  # Expression 2
#     [155, 135, 130, 125],  # Expression 3
# ]

# with_relinearization = [
#     [135, 125, 120, 115, 110],  # Expression 1
#     [150, 140, 135, 130, 120],  # Expression 2
#     [155, 145, 140, 135, 125],  # Expression 3
# ]

# # Plotting
# plt.figure(figsize=(10, 6))

# # Plot noise without relinearization (for each expression)
# for i, expr in enumerate(expressions):
#     plt.plot([1, 2, 3, 4], no_relinearization[i], label=f'Without Relinearization: {expr}', marker='o')

# # Plot noise with relinearization (for each expression)
# for i, expr in enumerate(expressions):
#     plt.plot([1, 2, 3, 4, 5], with_relinearization[i], label=f'With Relinearization: {expr}', marker='s', markersize=10)

# # Customize plot
# plt.xlabel('Step (1: Encryption, 2: * Operation, 3: + Operation, 4: Decryption, 5: Relinearization)', fontsize=12)
# plt.ylabel('Noise Budget', fontsize=12)
# plt.title('Comparison of Noise Budget with and without Relinearization', fontsize=14)
# plt.xticks([1, 2, 3, 4, 5], ['Encryption', '* Operation', '+ Operation', 'Decryption', 'Relinearization'])
# plt.legend(loc='upper right')
# plt.grid(True)

# # Save the plot to a file
# plt.tight_layout()
# plt.savefig('/home/suvedh/miniproject/noise_comparison_plot.png')  # Save as PNG

# # Optionally, save as PDF
# # plt.savefig('/home/suvedh/miniproject/noise_comparison_plot.pdf')  # Save as PDF

import matplotlib.pyplot as plt

# Data for the expressions
expressions = ['7*5 + 3*2 + 1', '129*8 + 9*5 + 5*2', '66 + 97*9 + 6']

# Noise values
no_relinearization = [
    [135, 120, 115, 110],
    [150, 130, 125, 120],
    [155, 135, 130, 125],
]

with_relinearization = [
    [135, 125, 120, 115, 110],
    [150, 140, 135, 130, 120],
    [155, 145, 140, 135, 125],
]

# Plotting
plt.figure(figsize=(12, 7))
colors = ['tab:blue', 'tab:orange', 'tab:green']

for i, expr in enumerate(expressions):
    plt.plot([1, 2, 3, 4], no_relinearization[i], label=f'Without Relinearization: {expr}',
             marker='o', color=colors[i])
    plt.plot([1, 2, 3, 4, 5], with_relinearization[i], label=f'With Relinearization: {expr}',
             marker='s', markersize=10, color=colors[i], linestyle='--')

# Labels and title with larger fonts
plt.xlabel('Step', fontsize=14)
plt.ylabel('Noise Budget', fontsize=14)
plt.title('Comparison of Noise Budget with and without Relinearization', fontsize=16)

# Custom x-tick labels
plt.xticks([1, 2, 3, 4, 5],
           ['Encryption', '* Operation', '+ Operation', 'Decryption', 'Relinearization'],
           fontsize=12)
plt.yticks(fontsize=12)

# Legend and grid
plt.legend(fontsize=10, loc='upper right')
plt.grid(True)
plt.tight_layout()

# Save the figure
plt.savefig('/home/suvedh/miniproject/noise_comparison_plot.png')
