import os
import matplotlib.pyplot as plt

if __name__ == "__main__":
    file_path = r"E:\project\turboquant\output_data.txt"

    if not os.path.exists(file_path):
        raise FileNotFoundError("Output data file not found! Run test_tensor_quant.py first.")

    with open(file_path, "r") as f:
        lines = f.readlines()
        original_data = [float(x) for x in lines[0].strip().split(",")]
        reconstructed_data = [float(x) for x in lines[1].strip().split(",")]

    x_indices = list(range(len(original_data)))

    plt.figure(figsize=(14, 6))
    plt.plot(x_indices, original_data, marker='.', linestyle='-', color='#1f77b4', 
             label='Original Synthetic Data', linewidth=1.2, markersize=3, alpha=0.8)
    plt.plot(x_indices, reconstructed_data, marker='s', linestyle='--', color='#d62728', 
             label='Reconstructed Data', linewidth=1.5, markersize=2, alpha=0.9)

    plt.title('TurboQuant Compression Analysis: 1000 Value Sample (Input Dim=4096, Target Dim=64)', fontsize=13, fontweight='bold')
    plt.xlabel('Element Index', fontsize=11)
    plt.ylabel('Value', fontsize=11)
    plt.grid(True, alpha=0.3)
    plt.legend(loc='upper right')
    plt.tight_layout()

    print("Displaying dataset verification graph...")
    plt.show()