#pragma once
#include <random>

// =========================================================
// LỚP CƠ SỞ TRỪU TƯỢNG (Abstract RNG)
// =========================================================
class RandomGenerator {
public:
    virtual ~RandomGenerator() = default;

    // Trả về một số ngẫu nhiên tuân theo phân phối chuẩn tắc N(0, 1)
    virtual double getStandardNormal() = 0;

    // Thiết lập lại hạt giống (seed) - Cực kỳ quan trọng cho đa luồng (Multi-threading)
    virtual void setSeed(unsigned int seed) = 0;
};

// =========================================================
// LỚP KẾ THỪA: Mersenne Twister RNG
// =========================================================
class MersenneTwisterRNG : public RandomGenerator {
private:
    std::mt19937 generator;                            // Core engine sinh số nguyên đều (Uniform)
    std::normal_distribution<double> standardNormal;   // Bộ chuyển đổi sang phân phối chuẩn

public:
    // Constructor với seed mặc định (giúp dễ dàng tái tạo lại kết quả khi debug)
    explicit MersenneTwisterRNG(unsigned int seed = 1234) 
        : generator(seed), standardNormal(0.0, 1.0) {}

    // Ghi đè hàm lấy số ngẫu nhiên N(0, 1)
    double getStandardNormal() override {
        return standardNormal(generator);
    }

    // Ghi đè hàm set seed
    void setSeed(unsigned int seed) override {
        generator.seed(seed);
    }
};