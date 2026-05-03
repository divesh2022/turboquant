import ctypes
import os
import random

# Define the QuantizedBlock structure to match the C++ definition
class QuantizedBlock(ctypes.Structure):
    _fields_ = [
        ("quantized_sign", ctypes.c_byte),
        ("mean", ctypes.c_float),
        ("scale", ctypes.c_float)
    ]

# Path to your compiled dynamic library
dll_path = r"E:\project\turboquant\turboquant.dll"

# Add the project folder and MinGW bin folder to the Windows DLL search directory
if hasattr(os, 'add_dll_directory'):
    os.add_dll_directory(r"E:\project\turboquant")
    mingw_bin_path = r"C:\ProgramData\mingw64\mingw64\bin"
    if os.path.exists(mingw_bin_path):
        os.add_dll_directory(mingw_bin_path)

if not os.path.exists(dll_path):
    raise FileNotFoundError(f"Dynamic library not found at: {dll_path}")

lib = ctypes.CDLL(dll_path)

# Link signatures
lib.create_tensor_compressor.argtypes = [ctypes.c_size_t, ctypes.c_size_t]
lib.create_tensor_compressor.restype = ctypes.c_void_p

lib.delete_tensor_compressor.argtypes = [ctypes.c_void_p]
lib.delete_tensor_compressor.restype = None

lib.compress_tensor.argtypes = [
    ctypes.c_void_p, 
    ctypes.POINTER(ctypes.c_float), 
    ctypes.POINTER(QuantizedBlock)
]
lib.compress_tensor.restype = None

lib.decompress_tensor.argtypes = [
    ctypes.c_void_p, 
    ctypes.POINTER(QuantizedBlock), 
    ctypes.POINTER(ctypes.c_float)
]
lib.decompress_tensor.restype = None


if __name__ == "__main__":
    INPUT_DIM = 4096
    TARGET_DIM = 128
    NUM_TENSORS = 10

    print(f"===========================================================")
    print(f"  Initializing TurboQuant Neural Network Tensor Pipeline   ")
    print(f"  Input Dimension: {INPUT_DIM} | Target Dimension: {TARGET_DIM}")
    print(f"===========================================================\n")

    compressor = lib.create_tensor_compressor(INPUT_DIM, TARGET_DIM)
    random.seed(42)

    total_mse = 0.0

    output_log = []
    output_log.append("===========================================================")
    output_log.append("  Initializing TurboQuant Neural Network Tensor Pipeline   ")
    output_log.append(f"  Input Dimension: {INPUT_DIM} | Target Dimension: {TARGET_DIM}")
    output_log.append("===========================================================\n")

    # Process 10 synthetic tensors
    for i in range(NUM_TENSORS):
        print(f"--- Processing Tensor Sample [{i+1}/{NUM_TENSORS}] ---")
        output_log.append(f"--- Processing Tensor Sample [{i+1}/{NUM_TENSORS}] ---")
        
        # Generate synthetic array
        input_tensor = [random.uniform(-100.0, 100.0) for _ in range(INPUT_DIM)]
        
        c_input_array = (ctypes.c_float * INPUT_DIM)(*input_tensor)
        c_output_array = (QuantizedBlock * TARGET_DIM)()

        # 1. Run Compression
        lib.compress_tensor(compressor, c_input_array, c_output_array)

        # 2. Run Decompression
        c_reconstructed_array = (ctypes.c_float * INPUT_DIM)()
        lib.decompress_tensor(compressor, c_output_array, c_reconstructed_array)
        reconstructed_list = list(c_reconstructed_array)

        # Compute error metric (MSE)
        mse = sum((o - r) ** 2 for o, r in zip(input_tensor, reconstructed_list)) / INPUT_DIM
        total_mse += mse

        # Print outputs to the terminal
        print(f"  > Original Tensor Sample    : {input_tensor[:4]} ... (Total: {len(input_tensor)} elements)")
        print(f"  > Compressed Blocks Sample  : [Sign: {c_output_array[0].quantized_sign}, Mean: {c_output_array[0].mean:.4f}, Scale: {c_output_array[0].scale:.4f}] ... (Total: {len(c_output_array)} blocks)")
        print(f"  > Reconstructed Tensor Sample: {reconstructed_list[:4]} ... (Total: {len(reconstructed_list)} elements)")
        print(f"  > Tensor Reconstruction MSE : {mse:.6f}\n")

        output_log.append(f"  > Original Tensor Sample    : {input_tensor[:4]} ... (Total: {len(input_tensor)} elements)")
        output_log.append(f"  > Compressed Blocks Sample  : [Sign: {c_output_array[0].quantized_sign}, Mean: {c_output_array[0].mean:.4f}, Scale: {c_output_array[0].scale:.4f}] ... (Total: {len(c_output_array)} blocks)")
        output_log.append(f"  > Reconstructed Tensor Sample: {reconstructed_list[:4]} ... (Total: {len(reconstructed_list)} elements)")
        output_log.append(f"  > Tensor Reconstruction MSE : {mse:.6f}\n")

    # Clean up memory
    lib.delete_tensor_compressor(compressor)

    avg_mse = total_mse / NUM_TENSORS
    print(f"===========================================================")
    print(f"  Pipeline Summary Evaluation Result                       ")
    print(f"===========================================================")
    print(f"  Average Validation MSE Score: {avg_mse:.6f}")
    print(f"===========================================================")

    output_log.append("===========================================================")
    output_log.append("  Pipeline Summary Evaluation Result                       ")
    output_log.append("===========================================================")
    output_log.append(f"  Average Validation MSE Score: {avg_mse:.6f}")
    output_log.append("===========================================================")

    with open("pipeline_output.txt", "w") as f:
        f.write("\n".join(output_log))
    print("Complete output written to pipeline_output.txt")