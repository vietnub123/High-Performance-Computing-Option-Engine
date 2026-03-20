#pragma once
#include <cmath>

// =========================================================
// LỚP CƠ SỞ TRỪU TƯỢNG (Abstract SDE)
// =========================================================
class SDE {
public:
    virtual ~SDE() = default;

    // Hàm tính phần trôi (Drift) - Xu hướng tăng trưởng kỳ vọng
    virtual double drift(double S, double t) const = 0;

    // Hàm tính phần khuếch tán (Diffusion) - Độ nhiễu/Biến động
    virtual double diffusion(double S, double t) const = 0;

    // Lấy giá trị ban đầu của tài sản S_0
    virtual double getInitialState() const = 0;
};

// =========================================================
// LỚP KẾ THỪA: Chuyển động Brown hình học (GBM)
// =========================================================
class GBM : public SDE {
private:
    double S0;      // Giá ban đầu (Initial Price)
    double r;       // Lãi suất phi rủi ro (Risk-free rate)
    double sigma;   // Độ biến động (Volatility)

public:
    // Constructor khởi tạo các tham số thị trường
    GBM(double S0_, double r_, double sigma_) 
        : S0(S0_), r(r_), sigma(sigma_) {}

    // Ghi đè hàm Drift: r * S
    // (Biến t được comment lại /*t*/ vì GBM tiêu chuẩn không phụ thuộc thời gian t, 
    // nhưng ta vẫn giữ tham số này trong interface để dự phòng cho các mô hình khác)
    double drift(double S, double /*t*/) const override {
        return r * S;
    }

    // Ghi đè hàm Diffusion: sigma * S
    double diffusion(double S, double /*t*/) const override {
        return sigma * S;
    }

    // Trả về giá ban đầu
    double getInitialState() const override {
        return S0;
    }
};