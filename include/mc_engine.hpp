#pragma once
#include "sde.hpp"
#include "payoff.hpp"
#include "random.hpp"
#include <cmath>
#include <omp.h>

class MonteCarloEngine {
private:
    const SDE& sde;
    const Payoff& payoff;
    double r;
    double T;

public:
    MonteCarloEngine(const SDE& sde_, const Payoff& payoff_, double r_, double T_)
        : sde(sde_), payoff(payoff_), r(r_), T(T_) {}

    // Hàm 1: Mô phỏng chuẩn (Đã viết)
    double simulate(unsigned int num_paths, unsigned int num_steps) const {
        double dt = T / num_steps;
        double sum_payoffs = 0.0;

        #pragma omp parallel
        {
            MersenneTwisterRNG thread_rng(1234 + omp_get_thread_num());
            #pragma omp for reduction(+:sum_payoffs)
            for (unsigned int i = 0; i < num_paths; ++i) {
                double S = sde.getInitialState();
                double t = 0.0;
                for (unsigned int j = 0; j < num_steps; ++j) {
                    double Z = thread_rng.getStandardNormal();
                    S = S + sde.drift(S, t) * dt + sde.diffusion(S, t) * std::sqrt(dt) * Z;
                    t += dt;
                }
                sum_payoffs += payoff(S);
            }
        }
        return std::exp(-r * T) * (sum_payoffs / num_paths);
    }

    // --- HÀM 2 MỚI: Mô phỏng với biến đối ngẫu (Antithetic Variates) ---
    double simulateAntithetic(unsigned int num_paths, unsigned int num_steps) const {
        double dt = T / num_steps;
        double sum_payoffs = 0.0;
        
        // Chỉ cần chạy một nửa số vòng lặp vì mỗi vòng đẻ ra 2 đường
        unsigned int half_paths = num_paths / 2;

        #pragma omp parallel
        {
            MersenneTwisterRNG thread_rng(1234 + omp_get_thread_num());

            #pragma omp for reduction(+:sum_payoffs)
            for (unsigned int i = 0; i < half_paths; ++i) {
                double S_plus = sde.getInitialState();
                double S_minus = sde.getInitialState(); // Đường lật ngược
                double t = 0.0;

                for (unsigned int j = 0; j < num_steps; ++j) {
                    double Z = thread_rng.getStandardNormal();
                    
                    // Tính song song 2 kịch bản với cùng một con số Z
                    S_plus = S_plus + sde.drift(S_plus, t) * dt + sde.diffusion(S_plus, t) * std::sqrt(dt) * Z;
                    S_minus = S_minus + sde.drift(S_minus, t) * dt + sde.diffusion(S_minus, t) * std::sqrt(dt) * (-Z);
                    
                    t += dt;
                }

                // Trung bình cộng payoff của cả đường gốc và đường đối ngẫu
                sum_payoffs += (payoff(S_plus) + payoff(S_minus)) / 2.0;
            }
        }

        return std::exp(-r * T) * (sum_payoffs / half_paths);
    }
};