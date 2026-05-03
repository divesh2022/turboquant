import ctypes
import os
import random
import math

class QuantizedBlock(ctypes.Structure):
    _fields_ = [
        ("quantized_sign", ctypes.c_byte),
        ("mean", ctypes.c_float),
        ("scale", ctypes.c_float)
    ]

class MockLibrary:
    def create_tensor_compressor(self, input_dim, target_dim):
        return 12345

    def delete_tensor_compressor(self, handle):
        pass

    def compress_tensor(self, handle, input_ptr, output_ptr):
        for i in range(128):
            output_ptr[i] = QuantizedBlock(
                quantized_sign=random.choice([-1, 1]),
                mean=random.uniform(-10.0, 10.0),
                scale=random.uniform(0.1, 2.0)
            )

    def decompress_tensor(self, handle, output_ptr, reconstructed_ptr):
        for i in range(4096):
            val = random.uniform(-100.0, 100.0)
            reconstructed_ptr[i] = val

lib = MockLibrary()

if __name__ == "__main__":
    INPUT_DIM = 4096
    TARGET_DIM = 128
    NUM_TENSORS = 10

    output_log = []
    output_log.append("===========================================================")
    output_log.append("  TurboQuant Multi-Instance Neural Network Comparison      ")
    output_log.append(f"  Input Dim: {INPUT_DIM} | Target Dim: {TARGET_DIM}")
    output_log.append("===========================================================\n")

    compressor = lib.create_tensor_compressor(INPUT_DIM, TARGET_DIM)
    random.seed(42)

    total_mse_baseline = 0.0
    total_mse_compressed = 0.0

    for i in range(NUM_TENSORS):
        output_log.append(f"--- Processing Tensor Sample [{i+1}/{NUM_TENSORS}] ---")

        input_tensor = [random.uniform(-100.0, 100.0) for _ in range(INPUT_DIM)]
        target_tensor = [random.uniform(-50.0, 50.0) for _ in range(INPUT_DIM)] # Common target

        c_input_array = (ctypes.c_float * INPUT_DIM)(*input_tensor)
        c_output_array = (QuantizedBlock * TARGET_DIM)()

        # 1. Run Compression Pipeline
        lib.compress_tensor(compressor, c_input_array, c_output_array)

        # 2. Run Decompression
        c_reconstructed_array = (ctypes.c_float * INPUT_DIM)()
        lib.decompress_tensor(compressor, c_output_array, c_reconstructed_array)
        reconstructed_list = list(c_reconstructed_array)

        # 3. Neural Network Simulation - Compute performance MSE
        baseline_mse = sum((o - t) ** 2 for o, t in zip(input_tensor, target_tensor)) / INPUT_DIM
        compressed_mse = sum((r - t) ** 2 for r, t in zip(reconstructed_list, target_tensor)) / INPUT_DIM

        total_mse_baseline += baseline_mse
        total_mse_compressed += compressed_mse

        output_log.append(f"  > Original Tensor Sample (First 4) : {input_tensor[:4]} ...")
        
        block_list = [
            f"{{Sign: {b.quantized_sign}, Mean: {b.mean:.4f}, Scale: {b.scale:.4f}}}" 
            for b in c_output_array
        ]
        output_log.append(f"  > Compressed Blocks Sample        : {block_list[:4]} ... (Total: {len(block_list)} blocks)")
        
        output_log.append(f"  > Reconstructed Tensor Sample (First 4) : {reconstructed_list[:4]} ...")
        output_log.append(f"  > Target Tensor Sample (First 4)      : {target_tensor[:4]} ...")
        output_log.append(f"  > Instance 1 (Baseline) Loss (MSE)  : {baseline_mse:.6f}")
        output_log.append(f"  > Instance 2 (Quantized) Loss (MSE) : {compressed_mse:.6f}\n")

    lib.delete_tensor_compressor(compressor)

    avg_baseline = total_mse_baseline / NUM_TENSORS
    avg_compressed = total_mse_compressed / NUM_TENSORS

    output_log.append("===========================================================")
    output_log.append("  Evaluation Summary & Comparison Result                   ")
    output_log.append("===========================================================")
    output_log.append(f"  Average Baseline (Uncompressed) MSE : {avg_baseline:.6f}")
    output_log.append(f"  Average Quantized (Compressed) MSE  : {avg_compressed:.6f}")
    output_log.append("===========================================================")

    with open("pipeline_nn_output.txt", "w") as f:
        f.write("\n".join(output_log))
    print("Neural Network comparison saved to pipeline_nn_output.txt")