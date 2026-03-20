#pragma once
#include <algorithm> // Cung cấp hàm std::max

// =========================================================
// LỚP CƠ SỞ TRỪU TƯỢNG (Abstract Base Class)
// =========================================================
class Payoff {
public:
    // 1. Virtual Destructor (Hàm hủy ảo): Rất quan trọng!
    // Đảm bảo khi bạn dùng smart pointer hoặc delete một con trỏ kiểu Payoff*,
    // hàm hủy của lớp con (Call/Put) sẽ được gọi đúng cách, tránh rò rỉ bộ nhớ.
    virtual ~Payoff() = default;

    // 2. Pure Virtual Function (Hàm ảo thuần túy):
    // Từ khóa 'const' báo cho compiler biết hàm này không làm thay đổi trạng thái của object.
    // 'operator()' biến object thành một Functor, cho phép gọi object như một hàm.
    virtual double operator()(double S) const = 0;
};

// =========================================================
// LỚP KẾ THỪA 1: Quyền chọn Mua (Call Option) / Chứng quyền (CW)
// =========================================================
class PayoffCall : public Payoff {
private:
    double K; // Giá thực thi (Strike Price)

public:
    // Constructor khởi tạo giá thực thi
    explicit PayoffCall(double K_) : K(K_) {}

    // Ghi đè (override) logic tính tiền: Max(S - K, 0)
    double operator()(double S) const override {
        return std::max(S - K, 0.0);
    }
};

// =========================================================
// LỚP KẾ THỪA 2: Quyền chọn Bán (Put Option)
// =========================================================
class PayoffPut : public Payoff {
private:
    double K; 

public:
    explicit PayoffPut(double K_) : K(K_) {}

    // Ghi đè logic tính tiền: Max(K - S, 0)
    double operator()(double S) const override {
        return std::max(K - S, 0.0);
    }
};