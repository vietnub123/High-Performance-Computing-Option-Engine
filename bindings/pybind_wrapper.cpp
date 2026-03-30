#include <pybind11/pybind11.h>
#include "sde.hpp"
#include "payoff.hpp"
#include "mc_engine.hpp"

namespace py = pybind11;

PYBIND11_MODULE(fastmc_engine, m) {
    m.doc() = "Fast Monte Carlo Pricing Engine built with C++ and OpenMP";

    // 1. Đóng gói mô hình SDE
    py::class_<SDE>(m, "SDE");
    py::class_<GBM, SDE>(m, "GBM")
        .def(py::init<double, double, double>(),
             py::arg("S0"), py::arg("r"), py::arg("sigma"));

    // 2. Đóng gói Payoff
    py::class_<Payoff>(m, "Payoff");
    py::class_<PayoffCall, Payoff>(m, "PayoffCall")
        .def(py::init<double>(), py::arg("strike_price"));

    // 3. Đóng gói Engine cốt lõi (Chú ý cách nối chuỗi .def liên tiếp)
    py::class_<MonteCarloEngine>(m, "MonteCarloEngine")
        .def(py::init<const SDE&, const Payoff&, double, double>(),
             py::arg("sde"), py::arg("payoff"), py::arg("r"), py::arg("T"))
        .def("simulate", &MonteCarloEngine::simulate, 
             py::arg("num_paths"), py::arg("num_steps"),
             "Chạy mô phỏng Monte Carlo tiêu chuẩn")
        .def("simulate_antithetic", &MonteCarloEngine::simulateAntithetic, 
             py::arg("num_paths"), py::arg("num_steps"),
             "Chạy mô phỏng Monte Carlo với Antithetic Variates");
}
