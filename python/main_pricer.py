import sys
import os
import time

# 1. Chỉ đường cho Python tìm thấy module C++
current_dir = os.path.dirname(os.path.abspath(__file__))
build_dir = os.path.join(current_dir, '..', 'build')
sys.path.append(build_dir)

# --- THÊM ĐOẠN NÀY VÀO TRƯỚC KHI IMPORT ---
# 1.5 Cấp quyền cho Python đọc các file DLL của trình biên dịch MinGW (GCC/OpenMP)
mingw_bin_path = r"D:\pearl\c\bin"  # Đường dẫn này lấy từ log CMake của bạn
if os.path.exists(mingw_bin_path):
    if hasattr(os, 'add_dll_directory'):
        os.add_dll_directory(mingw_bin_path)
else:
    print(f"Cảnh báo: Không tìm thấy thư mục MinGW tại {mingw_bin_path}")
# ------------------------------------------

try:
    import fastmc_engine
except ImportError as e:
    print(f"Lỗi Import: {e}")
    print(f"Python không tìm thấy module tại: {build_dir}")
    sys.exit(1)

def main():
    # 2. Thiết lập các tham số thị trường (Ví dụ: Định giá Chứng quyền)
    S0 = 100.0        # Giá tài sản cơ sở hiện tại (Spot Price)
    K = 105.0         # Giá thực thi (Strike Price)
    r = 0.05          # Lãi suất phi rủi ro (Risk-free rate: 5%)
    sigma = 0.20      # Độ biến động (Volatility: 20%)
    T = 1.0           # Thời gian đáo hạn (1 năm)
    
    # Thiết lập tham số Monte Carlo
    num_paths = 1_000_000  # Chạy 1 triệu kịch bản
    num_steps = 252        # Rời rạc hóa thành 252 bước (tương đương 252 ngày giao dịch/năm)

    print("Đang khởi tạo Engine Định giá Monte Carlo...")
    
    # 3. Khởi tạo các đối tượng C++ từ Python
    gbm_model = fastmc_engine.GBM(S0=S0, r=r, sigma=sigma)
    call_option = fastmc_engine.PayoffCall(strike_price=K)
    
    # Lắp ráp vào Engine
    engine = fastmc_engine.MonteCarloEngine(sde=gbm_model, payoff=call_option, r=r, T=T)

    # 4. Kích hoạt tính toán và đo thời gian
    print(f"Bắt đầu mô phỏng {num_paths:,} đường dẫn, {num_steps} bước/đường...")
    
    start_time = time.perf_counter()
    
    # Lệnh này sẽ đánh thức C++ và chạy đa luồng ngầm bên dưới
    price = engine.simulate(num_paths=num_paths, num_steps=num_steps)
    
    end_time = time.perf_counter()
    execution_time = end_time - start_time

    # 5. In kết quả
    print("-" * 40)
    print(f"Giá lý thuyết của Quyền chọn Mua : ${price:.4f}")
    print(f"Thời gian thực thi của C++ Engine : {execution_time:.4f} giây")
    print("-" * 40)

if __name__ == "__main__":
    main()