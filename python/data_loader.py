import sys
import os
import time
import numpy as np
import pandas as pd
import yfinance as yf

# --- THIẾT LẬP ĐƯỜNG DẪN MODULE C++ ---
current_dir = os.path.dirname(os.path.abspath(__file__))
build_dir = os.path.join(current_dir, '..', 'build')
sys.path.append(build_dir)

# Cấp quyền đọc DLL cho MinGW
mingw_bin_path = r"D:\pearl\c\bin"
if os.path.exists(mingw_bin_path) and hasattr(os, 'add_dll_directory'):
    os.add_dll_directory(mingw_bin_path)

import fastmc_engine

# =========================================================
# 1. HÀM TÍNH TOÁN ĐỘ BIẾN ĐỘNG LỊCH SỬ (HISTORICAL VOLATILITY)
# =========================================================
def calculate_historical_volatility(ticker, start_date, end_date):
    print(f"Đang tải dữ liệu lịch sử cho mã {ticker}...")
    # Tải dữ liệu giá đóng cửa điều chỉnh (Adjusted Close)
    data = yf.download(ticker, start=start_date, end=end_date, progress=False)
    
    if data.empty:
        raise ValueError(f"Không tìm thấy dữ liệu cho {ticker}.")
    
    # Lấy chuỗi giá
    prices = data['Close'].squeeze()
    
    # 1. Tính lợi suất Logarit liên tục: R_t = ln(S_t / S_{t-1})
    log_returns = np.log(prices / prices.shift(1)).dropna()
    
    # 2. Tính phương sai mẫu và độ lệch chuẩn của lợi suất ngày
    daily_volatility = np.std(log_returns, ddof=1)
    
    # 3. Thường niên hóa (Annualized Volatility) - Giả sử 252 ngày giao dịch/năm
    annualized_volatility = daily_volatility * np.sqrt(252)
    
    # Lấy giá hiện tại (phiên giao dịch gần nhất)
    current_price = float(prices.iloc[-1])
    
    return current_price, annualized_volatility

# =========================================================
# 2. KỊCH BẢN ĐỊNH GIÁ CHỨNG QUYỀN THỰC TẾ
# =========================================================
def main():
    ticker = "AAPL"  # Mã cổ phiếu 
    
    # Lấy dữ liệu 1 năm gần nhất để ước lượng sigma
    try:
        S0, sigma = calculate_historical_volatility(ticker, start_date="2025-03-01", end_date="2026-03-01")
    except Exception as e:
        print(f"Lỗi tải dữ liệu: {e}")
        return

    print("-" * 50)
    print(f"THÔNG SỐ THỊ TRƯỜNG THỰC TẾ ({ticker}):")
    print(f"Giá cơ sở hiện tại (S0) : {S0:,.0f} USD")
    print(f"Độ biến động lịch sử (σ): {sigma * 100:.2f}%")
    
    # Thiết lập hợp đồng Chứng quyền giả định (At-the-money)
    K = S0 * 1.05     # Giá thực thi cao hơn giá hiện tại 5%
    r = 0.0371         # Lãi suất phi rủi ro (%)
    T = 0.25       # Thời gian đáo hạn: (năm)
    
    print(f"\nTHÔNG SỐ CHỨNG QUYỀN (COVERED WARRANT):")
    print(f"Giá thực thi (K)        : {K:,.0f} USD")
    print(f"Lãi suất phi rủi ro (r) : {r * 100:.2f}%")
    print(f"Thời gian đáo hạn (T)   : {T} năm")
    print("-" * 50)

    # Khởi tạo C++ Engine
    gbm_model = fastmc_engine.GBM(S0=S0, r=r, sigma=sigma)
    call_option = fastmc_engine.PayoffCall(strike_price=K)
    engine = fastmc_engine.MonteCarloEngine(sde=gbm_model, payoff=call_option, r=r, T=T)

    # Chạy mô phỏng cường độ cao
    num_paths = 2_000_000
    num_steps = int(252 * T) # Số bước bằng số ngày giao dịch còn lại

    print(f"Đang kích hoạt C++ tính toán {num_paths:,} kịch bản...")
    start_time = time.perf_counter()
    
    cw_price = engine.simulate(num_paths=num_paths, num_steps=num_steps)
    
    exec_time = time.perf_counter() - start_time

    print("\nKẾT QUẢ ĐỊNH GIÁ:")
    print(f"Giá hợp lý của Chứng quyền: {cw_price:,.2f} USD")
    print(f"Thời gian tính toán (C++):  {exec_time:.4f} giây")
    print("-" * 50)

if __name__ == "__main__":
    main()