# Yêu cầu phiên bản CMake tối thiểu (3.14+ hỗ trợ pybind11 rất tốt)
cmake_minimum_required(VERSION 3.14)

# Khai báo tên dự án và ngôn ngữ sử dụng
project(FastMCPricer VERSION 1.0 LANGUAGES CXX)

# Ép sử dụng chuẩn C++17 (Cần thiết cho các thư viện sinh số ngẫu nhiên và smart pointers hiện đại)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------------------------------
# 1. TỐI ƯU HÓA HIỆU NĂNG (COMPILER FLAGS)
# ---------------------------------------------------------------------------
# -O3: Mức tối ưu hóa cao nhất về tốc độ của trình biên dịch
# -march=native: Tối ưu tập lệnh riêng cho kiến trúc CPU máy bạn đang chạy (AVX, SSE)
# -Wall -Wextra: Bật các cảnh báo để viết code C++ an toàn, tránh lỗi rò rỉ bộ nhớ
if(MSVC)
    add_compile_options(/O2 /arch:AVX2 /W4) # Dành cho compiler của Windows (Visual Studio)
else()
    add_compile_options(-O3 -march=native -Wall -Wextra -ffast-math) # Dành cho Linux/GCC/Clang
endif()

# ---------------------------------------------------------------------------
# 2. TÌM KIẾM CÁC THƯ VIỆN PHỤ THUỘC (DEPENDENCIES)
# ---------------------------------------------------------------------------
# Tìm OpenMP để xử lý đa luồng (Multi-threading) cho vòng lặp Monte Carlo
find_package(OpenMP REQUIRED)

# Tìm pybind11 để tạo cầu nối C++ và Python
# (Yêu cầu bạn đã cài pybind11 qua pip: pip install pybind11)
# --- BẮT ĐẦU ĐOẠN MÃ MỚI ---
# Gọi Python để tự động tìm đường dẫn của pybind11
execute_process(
    COMMAND python -c "import pybind11; print(pybind11.get_cmake_dir())"
    OUTPUT_VARIABLE pybind11_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Sau khi có đường dẫn, yêu cầu CMake tìm package
# --- CẬP NHẬT TỪ ĐÂY ---
# Bật tính năng FindPython mới để tắt cảnh báo CMP0148 của CMake
set(PYBIND11_FINDPYTHON ON)

# Sau khi có đường dẫn (từ lệnh execute_process bên trên), yêu cầu CMake tìm package
find_package(pybind11 REQUIRED)

# ---------------------------------------------------------------------------
# 3. ĐỊNH NGHĨA MODULE PYTHON (BUILD TARGET)
# ---------------------------------------------------------------------------
# Vì tất cả logic đã được viết inline trong các file .hpp, 
# chúng ta chỉ cần biên dịch file pybind_wrapper.cpp là đủ.
pybind11_add_module(fastmc_engine 
    bindings/pybind_wrapper.cpp
)
# --- ĐẾN ĐÂY ---

# ---------------------------------------------------------------------------
# 4. LIÊN KẾT & CẤU HÌNH ĐƯỜNG DẪN (LINKING & INCLUDES)
# ---------------------------------------------------------------------------
# Khai báo thư mục chứa file Header (.hpp) để compiler tìm thấy
target_include_directories(fastmc_engine PRIVATE include)

# Liên kết thư viện đa luồng OpenMP vào module
if(OpenMP_CXX_FOUND)
    target_link_libraries(fastmc_engine PRIVATE OpenMP::OpenMP_CXX)
endif()