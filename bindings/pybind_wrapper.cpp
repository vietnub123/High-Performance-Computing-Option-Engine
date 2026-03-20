#include <pybind11/pybind11.h>
#include "payoff.hpp"
#include "sde.hpp"
#include "mc_engine.hpp"

namespace py = pybind11;

// =========================================================
// KHỞI TẠO MODULE PYTHON (Tên module phải khớp với CMake)
// =========================================================
PYBIND11_MODULE(fastmc_engine, m) {
    // Tài liệu miêu tả module (Docstring)
    m.doc() = "High-Performance Monte Carlo Pricing Engine built with C++ and OpenMP";

    // 1. Đóng gói các lớp Payoff (Hàm trả thưởng)
    // Cần khai báo lớp cơ sở (Payoff) trước để pybind11 hiểu cấu trúc phân cấp
    py::class_<Payoff>(m, "Payoff");
    
    // Khai báo lớp con PayoffCall kế thừa từ Payoff và bóc tách Constructor
    py::class_<PayoffCall, Payoff>(m, "PayoffCall")
        .def(py::init<double>(), py::arg("strike_price"), "Khởi tạo Quyền chọn Mua với Giá thực thi");
        
    // Khai báo lớp con PayoffPut
    py::class_<PayoffPut, Payoff>(m, "PayoffPut")
        .def(py::init<double>(), py::arg("strike_price"), "Khởi tạo Quyền chọn Bán với Giá thực thi");

    // 2. Đóng gói các lớp SDE (Động học giá)
    py::class_<SDE>(m, "SDE");
    
    // Khai báo lớp GBM kế thừa từ SDE
    py::class_<GBM, SDE>(m, "GBM")
        .def(py::init<double, double, double>(), 
             py::arg("S0"), py::arg("r"), py::arg("sigma"),
             "Khởi tạo mô hình Chuyển động Brown Hình học");

    // 3. Đóng gói Engine cốt lõi
    py::class_<MonteCarloEngine>(m, "MonteCarloEngine")
        // Bọc Constructor nhận các module vào
        .def(py::init<const SDE&, const Payoff&, double, double>(),
             py::arg("sde"), py::arg("payoff"), py::arg("r"), py::arg("T"))
             
        // Bọc hàm chạy mô phỏng chính
        .def("simulate", &MonteCarloEngine::simulate, 
             py::arg("num_paths"), py::arg("num_steps"),
             "Chạy mô phỏng Monte Carlo đa luồng");
}