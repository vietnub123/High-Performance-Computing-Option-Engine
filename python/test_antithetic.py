import sys
import os
import time
import math
from scipy.stats import norm

# Setup đường dẫn C++
current_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(current_dir, '..', 'build'))
mingw_bin_path = r"D:\pearl\c\bin"
if os.path.exists(mingw_bin_path) and hasattr(os, 'add_dll_directory'):
    os.add_dll_directory(mingw_bin_path)

import fastmc_engine

def black_scholes_call(S0, K, T, r, sigma):
    d1 = (math.log(S0 / K) + (r + 0.5 * sigma**2) * T) / (sigma * math.sqrt(T))
    d2 = d1 - sigma * math.sqrt(T)
    return S0 * norm.cdf(d1) - K * math.exp(-r * T) * norm.cdf(d2)

def main():
    S0, K, r, sigma, T = 100.0, 100.0, 0.05, 0.20, 1.0
    num_paths, num_steps = 1_000_000, 252

    # Giá đích
    bs_price = black_scholes_call(S0, K, T, r, sigma)
    
    # Khởi tạo C++ Engine
    gbm = fastmc_engine.GBM(S0, r, sigma)
    call = fastmc_engine.PayoffCall(K)
    engine = fastmc_engine.MonteCarloEngine(gbm, call, r, T)

    print(f"MỤC TIÊU BLACK-SCHOLES: {bs_price:.6f}\n")
    print("-" * 65)
    print(f"{'Phương pháp':<20} | {'Giá mô phỏng':<15} | {'Sai số tuyệt đối':<15}")
    print("-" * 65)

    # 1. Chạy MC Chuẩn
    mc_price = engine.simulate(num_paths, num_steps)
    print(f"{'Standard MC':<20} | {mc_price:<15.6f} | {abs(mc_price - bs_price):<15.6f}")

    # 2. Chạy MC Antithetic
    anti_price = engine.simulate_antithetic(num_paths, num_steps)
    print(f"{'Antithetic MC':<20} | {anti_price:<15.6f} | {abs(anti_price - bs_price):<15.6f}")
    print("-" * 65)

if __name__ == "__main__":
    main()