#pragma once
#include "sde.hpp"
#include "payoff.hpp"
#include "random.hpp"
#include <cmath>
#include <omp.h> // Thư viện OpenMP cho đa luồng

// =========================================================
// LỚP ENGINE MONTE CARLO (Cỗ máy tính toán lõi)
// =========================================================
class MonteCarloEngine {
private:
    const SDE& sde;          // Tham chiếu hằng đến đối tượng SDE (VD: GBM)
    const Payoff& payoff;    // Tham chiếu hằng đến đối tượng Payoff (VD: Call/Put)
    double r;                // Lãi suất phi rủi ro (dùng để chiết khấu)
    double T;                // Thời gian đến hạn (tính bằng năm)

public:
    // Constructor nạp các module vào engine
    MonteCarloEngine(const SDE& sde_, const Payoff& payoff_, double r_, double T_)
        : sde(sde_), payoff(payoff_), r(r_), T(T_) {}

    // Hàm chạy mô phỏng chính
    double simulate(unsigned int num_paths, unsigned int num_steps) const {
        double dt = T / num_steps;      // Kích thước bước thời gian (Delta t)
        double sum_payoffs = 0.0;       // Tổng tiền trả thưởng của tất cả kịch bản

        // Bật chế độ đa luồng của OpenMP
        #pragma omp parallel
        {
            // [QUAN TRỌNG] Mỗi luồng (thread) phải có một bộ sinh số ngẫu nhiên RIÊNG BIỆT.
            // Nếu dùng chung sẽ gây lỗi Race Condition (tranh chấp bộ nhớ).
            // omp_get_thread_num() lấy ID của luồng hiện tại để tạo ra hạt giống (seed) khác nhau.
            MersenneTwisterRNG thread_rng(1234 + omp_get_thread_num());

            // Chia đều tổng số đường dẫn (num_paths) cho các luồng CPU hiện có.
            // reduction(+:sum_payoffs) giúp cộng gộp an toàn kết quả từ các luồng vào biến tổng.
            #pragma omp for reduction(+:sum_payoffs)
            for (unsigned int i = 0; i < num_paths; ++i) {
                
                double S = sde.getInitialState(); // Lấy giá S_0 ban đầu
                double t = 0.0;

                // Vòng lặp rời rạc hóa Euler-Maruyama cho một kịch bản giá
                for (unsigned int j = 0; j < num_steps; ++j) {
                    double Z = thread_rng.getStandardNormal();
                    
                    // Công thức S_{t+dt} = S_t + Drift * dt + Diffusion * sqrt(dt) * Z
                    S = S + sde.drift(S, t) * dt + sde.diffusion(S, t) * std::sqrt(dt) * Z;
                    t += dt;
                }

                // Tính tiền bồi thường cho kịch bản này và cộng vào tổng
                sum_payoffs += payoff(S);
            }
        } // Kết thúc vùng tính toán song song

        // Tính trung bình mẫu và chiết khấu về hiện tại (Risk-Neutral Discounting)
        return std::exp(-r * T) * (sum_payoffs / num_paths);
    }
};