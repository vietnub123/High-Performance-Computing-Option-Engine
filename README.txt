# FastMCPricer: High-Performance Monte Carlo Options Pricing Engine

**FastMCPricer** là một hệ thống định giá quyền chọn tài chính hiệu năng cao, kết hợp giữa sức mạnh tính toán song song của **C++** và sự linh hoạt của **Python**. Dự án sử dụng mô hình Chuyển động Brown Hình học (GBM) để mô phỏng quỹ đạo giá cổ phiếu và định giá các sản phẩm phái sinh.

---

## 🛠️ Công cụ & Công nghệ sử dụng (Tech Stack)

Dự án được xây dựng dựa trên các công nghệ tiêu chuẩn trong ngành Tài chính định lượng (Quant):

* **Ngôn ngữ:** C++17 (Tính toán lõi) & Python 3.13 (Giao diện & Scripting).
* **Đa luồng:** `OpenMP` - Tận dụng tối đa các nhân CPU để song song hóa vòng lặp Monte Carlo.
* **Cầu nối:** `pybind11` - Đóng gói mã nguồn C++ thành module Python cực kỳ mượt mà.
* **Build System:** `CMake` & `Ninja` - Quản lý quy trình biên dịch tự động và tốc độ cao.
* **Toán học:** `SciPy` - Dùng để tính toán giá trị chuẩn Black-Scholes làm thước đo sai số.

---

## 📁 Cấu trúc thư mục

```text
FastMCPricer/
├── CMakeLists.txt          # Cấu hình biên dịch dự án
├── .gitignore              # Loại bỏ file rác biên dịch khỏi Git
├── README.md               # Hướng dẫn sử dụng dự án
├── include/                # Thư viện lõi C++ (.hpp)
    ├── payoff.hpp          # Định nghĩa hàm trả thưởng (Payoff)
    ├── sde.hpp             # Mô hình phương trình vi phân ngẫu nhiên (GBM)
    ├── random.hpp          # Bộ sinh số ngẫu nhiên
    └── mc_engine.hpp       # Động cơ Monte Carlo tích hợp OpenMP
├── bindings/               # Cầu nối ngôn ngữ
│   └── pybind_wrapper.cpp  # Export các class C++ sang Python
└── python/                 # Kịch bản ứng dụng Python
    ├── main_pricer.py      # Script chạy định giá chính
    ├── test_performance.py # So sánh với công thức B-S
    ├── data_loader.py      # Dữ liệu thực tế
    └── test_antithetic.py  # Script kiểm thử kỹ thuật giảm phương sai
    
```

---

## ⚙️ Hướng dẫn Cài đặt & Biên dịch (Dành cho người mới)

Vì phần tính toán nặng được viết bằng C++, bạn cần biên dịch dự án trên máy của mình trước khi chạy.

### 1. Yêu cầu phần mềm
* Đã cài đặt **C++ Compiler** (GCC/MinGW, MSVC hoặc Clang).
* Đã cài đặt **CMake** và **Ninja**.
* Đã cài đặt **Python 3.8+**.

### 2. Cài đặt thư viện phụ thuộc
Mở Terminal tại thư mục dự án và chạy:
```bash
pip install pybind11 scipy pandas numpy yfinance
```

### 3. Biên dịch lõi C++ (Build)
Thực hiện các lệnh sau để tạo file thư viện `.pyd` (Windows) hoặc `.so` (Linux/Mac):
```bash
mkdir build
cd build
cmake -G Ninja ..
ninja
```

---

## 🚀 Cách chạy chương trình

Sau khi biên dịch thành công, bạn quay lại thư mục gốc và chạy các script Python:

**Định giá quyền chọn và so sánh kỹ thuật Antithetic:**
```bash
python python/test_antithetic.py
```

**Lưu ý:** Nếu bạn chạy trực tiếp bằng cách click đúp file hoặc nhấn F5 trong VS Code, hãy đảm bảo bạn đã chọn đúng **Python Interpreter** có chứa các thư viện đã cài đặt.

---

## 📊 Nguyên lý Toán học sơ lược

1.  **Mô hình hóa:** Sử dụng phương trình vi phân ngẫu nhiên $dS_t = rS_tdt + \sigma S_tdW_t$.
2.  **Rời rạc hóa:** Sử dụng sơ đồ Euler-Maruyama để xấp xỉ quỹ đạo giá trên máy tính.
3.  **Monte Carlo:** Mô phỏng hàng triệu kịch bản, tính trung bình Payoff và chiết khấu về hiện tại bằng $e^{-rT}$.
4.  **Tối ưu:** Sử dụng **Antithetic Variates** (biến đối ngẫu) để giảm phương sai của ước lượng, giúp kết quả chính xác hơn với cùng một số lượng vòng lặp.

---
*Dự án được phát triển cho mục đích học tập và nghiên cứu trong lĩnh vực Tài chính định lượng.*
```

---
