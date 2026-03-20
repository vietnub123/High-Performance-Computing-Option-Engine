import sys
import os
import time
import math
import matplotlib.pyplot as plt
from scipy.stats import norm

# --- XỬ LÝ ĐƯỜNG DẪN MODULE C++ VÀ DLL ---
current_dir = os.path.dirname(os.path.abspath(__file__))
build_dir = os.path.join(current_dir, '..', 'build')
sys.path.append(build_dir)

# Cấp quyền cho Python đọc DLL của MinGW (Đổi lại đường dẫn nếu cần)
mingw_bin_path = r"D:\pearl\c\bin"
if os.path.exists(mingw_bin_path) and hasattr(os, 'add_dll_directory'):
    os.add_dll_directory(mingw_bin_path)

import fastmc_engine

# =========================================================
# 1. HÀM TÍNH NGHIỆM GIẢI TÍCH BLACK-SCHOLES
# =========================================================
def black_scholes_call(S0, K, T, r, sigma):
    """Tính giá lý thuyết của Call Option bằng công thức B-S"""
    d1 = (math.log(S0 / K) + (r + 0.5 * sigma**2) * T) / (sigma * math.sqrt(T))
    d2 = d1 - sigma * math.sqrt(T)
    
    # norm.cdf là hàm phân phối tích lũy chuẩn tắc
    call_price = S0 * norm.cdf(d1) - K * math.exp(-r * T) * norm.cdf(d2)
    return call_price

# =========================================================
# 2. KỊCH BẢN KIỂM ĐỊNH CHÍNH (MAIN TEST)
# =========================================================
def main():
    # Tham số thị trường
    S0 = 100.0
    K = 100.0         # Quyền chọn đẳng phí (At-the-money) để test độ nhạy tốt nhất
    r = 0.05
    sigma = 0.20
    T = 1.0
    num_steps = 252   # 252 ngày giao dịch

    # 1. Tính giá mục tiêu (Target Price) bằng Black-Scholes
    bs_price = black_scholes_call(S0, K, T, r, sigma)
    print(f"Giá lý thuyết Black-Scholes (Mục tiêu): {bs_price:.6f}")
    print("-" * 60)

    # 2. Chuẩn bị Engine C++
    gbm_model = fastmc_engine.GBM(S0=S0, r=r, sigma=sigma)
    call_option = fastmc_engine.PayoffCall(strike_price=K)
    engine = fastmc_engine.MonteCarloEngine(sde=gbm_model, payoff=call_option, r=r, T=T)

    # Các mốc số lượng đường dẫn để kiểm tra sự hội tụ
    path_scenarios = [10_000, 50_000, 100_000, 500_000, 1_000_000, 2_000_000]
    
    mc_prices = []
    errors = []
    times = []

    print(f"{'Số đường dẫn':<15} | {'Giá Monte Carlo':<15} | {'Sai số tuyệt đối':<15} | {'Thời gian (s)'}")
    print("-" * 60)

    # 3. Vòng lặp test các kịch bản
    for paths in path_scenarios:
        start_time = time.perf_counter()
        
        # Gọi C++ Engine chạy mô phỏng
        mc_price = engine.simulate(num_paths=paths, num_steps=num_steps)
        
        exec_time = time.perf_counter() - start_time
        error = abs(mc_price - bs_price)
        
        mc_prices.append(mc_price)
        errors.append(error)
        times.append(exec_time)
        
        print(f"{paths:<15,} | {mc_price:<15.6f} | {error:<15.6f} | {exec_time:.4f}s")

    # =========================================================
    # 3. VẼ BIỂU ĐỒ HỘI TỤ (CONVERGENCE PLOT)
    # =========================================================
    plt.figure(figsize=(10, 6))
    
    # Vẽ đường ngang thể hiện giá lý thuyết BS
    plt.axhline(y=bs_price, color='r', linestyle='--', label=f'Black-Scholes (Target: {bs_price:.4f})')
    
    # Vẽ đường tiệm cận của mô hình Monte Carlo
    plt.plot(path_scenarios, mc_prices, marker='o', color='b', label='FastMC C++ Engine')
    
    plt.title('Sự hội tụ của Monte Carlo C++ Engine so với Black-Scholes', fontsize=14)
    plt.xlabel('Số lượng đường dẫn (Paths)', fontsize=12)
    plt.ylabel('Giá Quyền Chọn (Call Price)', fontsize=12)
    plt.xscale('log') # Dùng thang đo logarit cho trục X để dễ nhìn sự thay đổi
    plt.grid(True, which="both", ls="--", alpha=0.5)
    plt.legend()
    
    # Lưu ảnh và hiển thị
    plt.tight_layout()
    plt.savefig('convergence_plot.png')
    print("\nĐã lưu biểu đồ hội tụ thành file 'convergence_plot.png'.")
    plt.show()

if __name__ == "__main__":
    main()