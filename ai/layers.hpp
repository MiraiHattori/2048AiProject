#pragma once

#include <cmath>
#include <functional>
#include <iostream>
#include <numeric>
#include <Eigen/Core>
#include "util/random.hpp"


template <typename TIN, typename TOUT>
class ActivationLayer
{
public:
    explicit ActivationLayer() = default;
    virtual TOUT forward(const TIN&) = 0;
    virtual TIN backward(const TOUT&) = 0;
};

template <std::size_t INPUT_SIZE_, std::size_t OUTPUT_SIZE_>
class Relu : public ActivationLayer<Eigen::VectorXd, Eigen::VectorXd>
{
public:
    explicit Relu() {
        static_assert(INPUT_SIZE_ > 0, "Error in class Relu; INPUT_SIZE <= 0");
        static_assert(OUTPUT_SIZE_ > 0, "Error in class Relu; OUTPUT_SIZE <= 0");
    }

    Eigen::VectorXd forward(const Eigen::VectorXd& x)
    {
        m_inverse_mask = Eigen::VectorXd(INPUT_SIZE);
        Eigen::VectorXd out = x;
        // std::transform使うとgとstd::vector<double>で実装が分かれてしまう
        for (std::size_t i = 0; i < static_cast<std::size_t>(out.size()); i++) {
            m_inverse_mask[i] = (x[i] >= 0.0 ? 1.0 : 0.0);
            out[i] *= m_inverse_mask[i];
        }
        return out;
    }

    Eigen::VectorXd backward(const Eigen::VectorXd& dout)
    {
        Eigen::VectorXd dx = dout;
        for (std::size_t i = 0; i < INPUT_SIZE_; i++) {
            dx[i] *= m_inverse_mask[i];
        }
        return dx;
    }

    static constexpr std::size_t INPUT_SIZE = INPUT_SIZE_;
    static constexpr std::size_t OUTPUT_SIZE = OUTPUT_SIZE_;

private:
    Eigen::VectorXd m_inverse_mask;
};

template <std::size_t INPUT_SIZE_, std::size_t OUTPUT_SIZE_>
class Sigmoid : public ActivationLayer<Eigen::VectorXd, Eigen::VectorXd>
{
public:
    explicit Sigmoid() {
        static_assert(INPUT_SIZE_ > 0, "Error in class Sigmoid; INPUT_SIZE <= 0");
        static_assert(OUTPUT_SIZE_ > 0, "Error in class Sigmoid; OUTPUT_SIZE <= 0");
    }

    Eigen::VectorXd forward(const Eigen::VectorXd& x)
    {
        Eigen::VectorXd out = Eigen::VectorXd(OUTPUT_SIZE_);
        for (std::size_t i = 0; i < INPUT_SIZE_; i++) {
            out[i] = 1.0 / (1.0 + std::exp(-x[i]));
        }
        m_out = out;
        return out;
    }

    Eigen::VectorXd backward(const Eigen::VectorXd& dout)
    {
        Eigen::VectorXd dx = Eigen::VectorXd(INPUT_SIZE_);
        for (std::size_t i = 0; i < INPUT_SIZE_; i++) {
            dx[i] = dout[i] * (1.0 - m_out[i]) * m_out[i];
        }
        return dx;
    }

    static constexpr std::size_t INPUT_SIZE = INPUT_SIZE_;
    static constexpr std::size_t OUTPUT_SIZE = OUTPUT_SIZE_;

private:
    Eigen::VectorXd m_out = Eigen::VectorXd::Zero(OUTPUT_SIZE_);
};

// Eigen前提
template <std::size_t INPUT_SIZE_, std::size_t OUTPUT_SIZE_>
class Affine : public ActivationLayer<Eigen::VectorXd, Eigen::VectorXd>
{
public:
    explicit Affine()
    {
        static_assert(INPUT_SIZE_ > 0, "Error in class Affine; INPUT_SIZE <= 0");
        static_assert(OUTPUT_SIZE_ > 0, "Error in class Affine; OUTPUT_SIZE <= 0");
        // initialize w and b
        for (std::size_t i = 0; i < OUTPUT_SIZE_; i++) {
            for (std::size_t j = 0; j < INPUT_SIZE_; j++) {
                m_w(i, j) = Util::randUniform<double>(-0.05, 0.05);
            }
        }
        m_b = Eigen::VectorXd::Zero(OUTPUT_SIZE_);
        m_dw = Eigen::MatrixXd::Zero(OUTPUT_SIZE_, INPUT_SIZE_);
        m_db = Eigen::VectorXd::Zero(OUTPUT_SIZE_);
    }

    Eigen::VectorXd forward(const Eigen::VectorXd& x)
    {
        m_x = x;
        Eigen::VectorXd out = m_w * x + m_b;
        return out;
    }
    Eigen::VectorXd backward(const Eigen::VectorXd& dout)
    {
        Eigen::VectorXd dx = dout * m_x.transpose();
        m_dw = m_x.transpose() * dout;
        m_db = dout;
        return dx;
    }
    const Eigen::MatrixXd& w() const
    {
        return m_w;
    }
    Eigen::MatrixXd& wRef()
    {
        return m_w;
    }
    const Eigen::MatrixXd& b() const
    {
        return m_b;
    }
    Eigen::MatrixXd& bRef()
    {
        return m_b;
    }

private:
    static constexpr std::size_t INPUT_SIZE = INPUT_SIZE_;
    static constexpr std::size_t OUTPUT_SIZE = OUTPUT_SIZE_;
    Eigen::VectorXd m_x;
    Eigen::MatrixXd m_w = Eigen::MatrixXd::Zero(OUTPUT_SIZE_, INPUT_SIZE_);
    Eigen::VectorXd m_b = Eigen::VectorXd::Zero(OUTPUT_SIZE_);
    Eigen::MatrixXd m_dw = Eigen::MatrixXd::Zero(OUTPUT_SIZE_, INPUT_SIZE_);
    Eigen::VectorXd m_db = Eigen::VectorXd::Zero(OUTPUT_SIZE_);
};

template <int INPUT_SIZE_, int OUTPUT_SIZE_>
Eigen::VectorXd softmax(const Eigen::VectorXd& x)
{
    Eigen::VectorXd exp_x = Eigen::VectorXd(OUTPUT_SIZE_);
    double max_elem_x = std::numeric_limits<double>::min();
    for (std::size_t i = 0; i < INPUT_SIZE_; i++) {
        if (max_elem_x < x[i]) {
            max_elem_x = x[i];
        }
    }
    for (std::size_t i = 0; i < INPUT_SIZE_; i++) {
        exp_x[i] = std::exp(x[i] - max_elem_x);
    }
    double exp_x_sum = 0.0;
    for (std::size_t i = 0; i < OUTPUT_SIZE_; i++) {
        exp_x_sum += exp_x[i];
    }
    for (std::size_t i = 0; i < OUTPUT_SIZE_; i++) {
        exp_x[i] /= exp_x_sum;
    }
    return exp_x;
}

// 教師あり
template <std::size_t INPUT_SIZE_, std::size_t OUTPUT_SIZE_>
Eigen::VectorXd crossEntropyError(const Eigen::VectorXd& y, const Eigen::VectorXd& t, const double& epsilon = 1e-7)
{
    Eigen::VectorXd sum(OUTPUT_SIZE_);
    for (std::size_t i = 0; i < static_cast<std::size_t>(t.size()); i++) {
        sum[i] = -std::log(y[i] + epsilon);
    }
    return sum;
}


template <std::size_t INPUT_SIZE_, std::size_t OUTPUT_SIZE_>
class SoftmaxWithLoss
{
public:
    explicit SoftmaxWithLoss()
    {
        static_assert(INPUT_SIZE_ > 0, "Error in class SoftMaxWithLoss; INPUT_SIZE <= 0");
        static_assert(OUTPUT_SIZE_ > 0, "Error in class SoftMaxWithLoss; OUTPUT_SIZE <= 0");
    }

    Eigen::VectorXd forward(const Eigen::VectorXd& x, const Eigen::VectorXd& t)
    {
        m_t = t;
        m_y = softmax<INPUT_SIZE_, OUTPUT_SIZE_>(x);
        m_loss = crossEntropyError<INPUT_SIZE_, OUTPUT_SIZE_>(m_y, m_t);
        return m_loss;
    }

    Eigen::VectorXd backward(const Eigen::VectorXd& /* dout = 1 */)
    {
        Eigen::VectorXd dx(INPUT_SIZE);

        for (std::size_t i = 0; i < OUTPUT_SIZE; i++) {
            dx[i] = (m_y[i] - m_t[i]);
        }
        return dx;
    }

    static constexpr std::size_t INPUT_SIZE = INPUT_SIZE_;
    static constexpr std::size_t OUTPUT_SIZE = OUTPUT_SIZE_;

private:
    Eigen::VectorXd m_loss = Eigen::VectorXd::Zero(INPUT_SIZE_);
    Eigen::VectorXd m_y = Eigen::VectorXd::Zero(INPUT_SIZE_);
    Eigen::VectorXd m_t = Eigen::VectorXd::Zero(INPUT_SIZE_);
};
