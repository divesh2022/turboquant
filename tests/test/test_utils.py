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

class TurboQuantGenerativeEngine:
    def __init__(self, input_dim, target_dim):
        self.input_dim = input_dim
        self.target_dim = target_dim

    def compress(self, tensor_data):
        compressed_blocks = []
        chunk_size = self.input_dim // self.target_dim
        
        for i in range(self.target_dim):
            chunk = tensor_data[i * chunk_size : (i + 1) * chunk_size]
            mean = sum(chunk) / len(chunk)
            variance = sum((x - mean) ** 2 for x in chunk) / len(chunk)
            scale = math.sqrt(variance) if variance > 0 else 1e-5
            sign = 1 if mean >= 0 else -1
            
            compressed_blocks.append(QuantizedBlock(
                quantized_sign=sign,
                mean=mean,
                scale=scale
            ))
        return compressed_blocks

    def decompress(self, compressed_blocks):
        reconstructed = []
        chunk_size = self.input_dim // self.target_dim
        
        for block in compressed_blocks:
            for _ in range(chunk_size):
                # Apply scaling for image/video latent variations
                val = block.mean + block.quantized_sign * block.scale * random.uniform(0.9, 1.1)
                reconstructed.append(val)
        return reconstructed

if __name__ == "__main__":
    INPUT_DIM = 4096
    TARGET_DIM = 128
    
    output_log = []
    output_log.append("===========================================================")
    output_log.append("  TurboQuant Generative Hardware-Constrained Pipeline      ")
    output_log.append("===========================================================\n")
    
    engine = TurboQuantGenerativeEngine(INPUT_DIM, TARGET_DIM)
    
    # Simulate a single latent vector (e.g., from an image token or video frame)
    latent_vector = [random.uniform(-100.0, 100.0) for _ in range(INPUT_DIM)]
    
    # Compress 4096 elements to 128 blocks
    compressed_data = engine.compress(latent_vector)
    
    # Decompress back to 4096 dimensions
    reconstructed_vector = engine.decompress(compressed_data)
    
    # Compute the reconstruction loss/fidelity
    mse = sum((o - r) ** 2 for o, r in zip(latent_vector, reconstructed_vector)) / INPUT_DIM
    
    output_log.append(f"  > Processing latent dimension: {INPUT_DIM}")
    output_log.append(f"  > Output representation      : {len(compressed_data)} blocks")
    output_log.append(f"  > Reconstructed dimension    : {len(reconstructed_vector)} elements")
    output_log.append(f"  > Latent Reconstruction MSE  : {mse:.6f}\n")
    output_log.append("===========================================================")
    
    with open("generative_pipeline_output.txt", "w") as f:
        f.write("\n".join(output_log))
        
    print("TurboQuant Generative Pipeline Initialized Successfully.")
    print("Compressed State Dimensions :", len(compressed_data), "Blocks")
    print("Reconstructed Dimensions    :", len(reconstructed_vector), "Elements")
    print("Log saved to 'generative_pipeline_output.txt'")