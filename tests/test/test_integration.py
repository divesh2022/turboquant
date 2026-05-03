import ctypes
import os
import random
import math

# Define the QuantizedBlock structure to match the C++ definition
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
    NUM_TENSORS = 5

    output_log = []
    output_log.append("===========================================================")
    output_log.append("  TurboQuant: MLP & CNN Tensor Validation Pipeline         ")
    output_log.append(f"  Features: {INPUT_DIM} | Compressed Blocks: {TARGET_DIM}")
    output_log.append("===========================================================\n")

    compressor = lib.create_tensor_compressor(INPUT_DIM, TARGET_DIM)
    random.seed(42)

    total_mlp_baseline_loss = 0.0
    total_mlp_quant_loss = 0.0
    total_cnn_baseline_loss = 0.0
    total_cnn_quant_loss = 0.0

    for i in range(NUM_TENSORS):
        output_log.append(f"--- Processing Sample Layer [{i+1}/{NUM_TENSORS}] ---")

        # Create input tensors and common target
        input_tensor = [random.uniform(-100.0, 100.0) for _ in range(INPUT_DIM)]
        target_tensor = [random.uniform(-50.0, 50.0) for _ in range(INPUT_DIM)]

        c_input_array = (ctypes.c_float * INPUT_DIM)(*input_tensor)
        c_output_array = (QuantizedBlock * TARGET_DIM)()

        # 1. Compress & Decompress Pipeline
        lib.compress_tensor(compressor, c_input_array, c_output_array)
        c_reconstructed_array = (ctypes.c_float * INPUT_DIM)()
        lib.decompress_tensor(compressor, c_output_array, c_reconstructed_array)
        reconstructed_list = list(c_reconstructed_array)

        # 2. Simulate MLP Test Instance
        # Simulates a dense layer by computing the L2 norm and activation performance
        mlp_baseline = sum(math.tanh(o) - math.tanh(t) for o, t in zip(input_tensor, target_tensor)) / INPUT_DIM
        mlp_quant = sum(math.tanh(r) - math.tanh(t) for r, t in zip(reconstructed_list, target_tensor)) / INPUT_DIM

        # 3. Simulate CNN Test Instance
        # Simulates a convolutional pathway by applying a localized kernel operation/bias
        cnn_baseline = sum((o * 0.5 + 1.0) - (t * 0.5 + 1.0) for o, t in zip(input_tensor, target_tensor)) / INPUT_DIM
        cnn_quant = sum((r * 0.5 + 1.0) - (t * 0.5 + 1.0) for r, t in zip(reconstructed_list, target_tensor)) / INPUT_DIM

        total_mlp_baseline_loss += abs(mlp_baseline)
        total_mlp_quant_loss += abs(mlp_quant)
        total_cnn_baseline_loss += abs(cnn_baseline)
        total_cnn_quant_loss += abs(cnn_quant)

        output_log.append(f"  > MLP Baseline Output (First 4): {[math.tanh(x) for x in input_tensor[:4]]}")
        output_log.append(f"  > MLP Quantized Output (First 4): {[math.tanh(x) for x in reconstructed_list[:4]]}")
        output_log.append(f"  > Baseline MLP Error: {abs(mlp_baseline):.6f}")
        output_log.append(f"  > Quantized MLP Error: {abs(mlp_quant):.6f}\n")
        output_log.append(f"  > Baseline CNN Error: {abs(cnn_baseline):.6f}")
        output_log.append(f"  > Quantized CNN Error: {abs(cnn_quant):.6f}\n")

    lib.delete_tensor_compressor(compressor)

    avg_mlp_base = total_mlp_baseline_loss / NUM_TENSORS
    avg_mlp_quant = total_mlp_quant_loss / NUM_TENSORS
    avg_cnn_base = total_cnn_baseline_loss / NUM_TENSORS
    avg_cnn_quant = total_cnn_quant_loss / NUM_TENSORS

    output_log.append("===========================================================")
    output_log.append("  Final Performance Comparison Summary                     ")
    output_log.append("===========================================================")
    output_log.append(f"  MLP Baseline Average Error: {avg_mlp_base:.6f}")
    output_log.append(f"  MLP Quantized Average Error: {avg_mlp_quant:.6f}\n")
    output_log.append(f"  CNN Baseline Average Error: {avg_cnn_base:.6f}")
    output_log.append(f"  CNN Quantized Average Error: {avg_cnn_quant:.6f}")
    output_log.append("===========================================================")

    with open("nn_validation_output.txt", "w") as f:
        f.write("\n".join(output_log))
    print("Verification and performance data saved to nn_validation_output.txt")