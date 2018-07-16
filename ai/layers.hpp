#pragma once

#include <cmath>
#include <functional>
#include <numeric>

#include <Eigen/Core>

#include "ai/weight_optimizer.hpp"
#include "util/random.hpp"


class ActivationLayer
{
public:
    explicit ActivationLayer() = default;
    virtual Eigen::VectorXd forward(const Eigen::VectorXd&) = 0;
    virtual Eigen::VectorXd backward(const Eigen::VectorXd&) = 0;
};

template <std::size_t INPUT_SIZE_, std::size_t OUTPUT_SIZE_>
class Relu : public ActivationLayer
{
public:
    explicit Relu()
    {
        static_assert(INPUT_SIZE_ > 0, "Error in class Relu; INPUT_SIZE <= 0");
        static_assert(OUTPUT_SIZE_ > 0, "Error in class Relu; OUTPUT_SIZE <= 0");
    }

    Eigen::VectorXd forward(const Eigen::VectorXd& x)
    {
        Eigen::VectorXd out = x;
        // std::transform使うとgとstd::vector<double>で実装が分かれてしまう
        for (std::size_t i = 0; i < OUTPUT_SIZE_; i++) {
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

    /*
     * 内部パラメータを学習可能か
     * (学習可能な場合、updateWeight(const std::shared_ptr<WeightOptimizer>& optimizer)関数が必要)
     */
    static constexpr bool HAS_WEIGHT = false;

private:
    Eigen::VectorXd m_inverse_mask = Eigen::VectorXd::Zero(INPUT_SIZE_);
};

template <std::size_t INPUT_SIZE_, std::size_t OUTPUT_SIZE_>
class Sigmoid : public ActivationLayer
{
public:
    explicit Sigmoid()
    {
        static_assert(INPUT_SIZE_ > 0, "Error in class Sigmoid; INPUT_SIZE <= 0");
        static_assert(OUTPUT_SIZE_ > 0, "Error in class Sigmoid; OUTPUT_SIZE <= 0");
    }

    Eigen::VectorXd forward(const Eigen::VectorXd& x)
    {
        Eigen::VectorXd out = Eigen::VectorXd::Zero(OUTPUT_SIZE_);
        for (std::size_t i = 0; i < INPUT_SIZE_; i++) {
            out[i] = 1.0 / (1.0 + std::exp(-x[i]));
        }
        m_out = out;
        return out;
    }

    Eigen::VectorXd backward(const Eigen::VectorXd& dout)
    {
        Eigen::VectorXd dx = Eigen::VectorXd::Zero(INPUT_SIZE_);
        for (std::size_t i = 0; i < INPUT_SIZE_; i++) {
            dx[i] = dout[i] * (1.0 - m_out[i]) * m_out[i];
        }
        return dx;
    }

    /*
     * 内部パラメータを学習可能か
     * (学習可能な場合、updateWeight(const std::shared_ptr<WeightOptimizer>& optimizer)関数が必要)
     */
    static constexpr bool HAS_WEIGHT = false;

private:
    Eigen::VectorXd m_out = Eigen::VectorXd::Zero(OUTPUT_SIZE_);
};

// Eigen前提
template <std::size_t INPUT_SIZE_, std::size_t OUTPUT_SIZE_>
class Affine : public ActivationLayer
{
public:
    explicit Affine()
    {
        static_assert(INPUT_SIZE_ > 0, "Error in class Affine; INPUT_SIZE <= 0");
        static_assert(OUTPUT_SIZE_ > 0, "Error in class Affine; OUTPUT_SIZE <= 0");
        // initialize w and b
        for (std::size_t i = 0; i < OUTPUT_SIZE_; i++) {
            for (std::size_t j = 0; j < INPUT_SIZE_; j++) {
                m_w(i, j) = Util::randUniform<double>(-1, 1);
            }
        }
    }

    Eigen::VectorXd forward(const Eigen::VectorXd& x)
    {
        m_x = x;
        Eigen::VectorXd out = m_w * m_x + m_b;
        return out;
    }
    Eigen::VectorXd backward(const Eigen::VectorXd& dout)
    {
        Eigen::VectorXd dx = m_w.transpose() * dout;
        m_dw = dout * m_x.transpose();
        m_db = dout;
        return dx;
    }

    // updateWeight関数があると重みがupdateできるようになる
    void updateWeight(const std::shared_ptr<WeightOptimizer>& optimizer)
    {
        m_w = optimizer->updateWeight(m_w, m_dw);
        m_b = optimizer->updateWeight(m_b, m_db);
    }

    const Eigen::MatrixXd& w() const { return m_w; }
    const Eigen::MatrixXd& dw() const { return m_dw; }
    const Eigen::VectorXd& b() const { return m_b; }
    const Eigen::VectorXd& db() const { return m_db; }

    /*
     * 内部パラメータを学習可能か
     * (学習可能な場合、updateWeight(const std::shared_ptr<WeightOptimizer>& optimizer)関数が必要)
     */
    static constexpr bool HAS_WEIGHT = true;

private:
    Eigen::VectorXd m_x;
    Eigen::MatrixXd m_w = Eigen::MatrixXd::Zero(OUTPUT_SIZE_, INPUT_SIZE_);
    Eigen::VectorXd m_b = Eigen::VectorXd::Zero(OUTPUT_SIZE_);
    Eigen::MatrixXd m_dw = Eigen::MatrixXd::Zero(OUTPUT_SIZE_, INPUT_SIZE_);
    Eigen::VectorXd m_db = Eigen::VectorXd::Zero(OUTPUT_SIZE_);
};

template <int INPUT_SIZE_, int OUTPUT_SIZE_>
Eigen::VectorXd softmax(const Eigen::VectorXd& x)
{
    Eigen::VectorXd exp_x = Eigen::VectorXd::Zero(OUTPUT_SIZE_);
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
Eigen::VectorXd crossEntropyError(const Eigen::VectorXd& y,
    const Eigen::VectorXd& t,
    const double& epsilon = 1e-7)
{
    Eigen::VectorXd sum = Eigen::VectorXd::Zero(OUTPUT_SIZE_);
    for (std::size_t i = 0; i < OUTPUT_SIZE_; i++) {
        sum[i] = -(t[i] * std::log(y[i] + epsilon) + (1 - t[i]) * std::log(1 - y[i]));
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
        return crossEntropyError<INPUT_SIZE_, OUTPUT_SIZE_>(m_y, m_t);
    }

    Eigen::VectorXd backward(const Eigen::VectorXd& /* dout = 1 */)
    {
        return m_y - m_t;
    }

    static constexpr std::size_t OUTPUT_SIZE = OUTPUT_SIZE_;

    /*
     * 内部パラメータを学習可能か
     * (学習可能な場合、updateWeight(const std::shared_ptr<WeightOptimizer>& optimizer)関数が必要)
     */
    static constexpr bool HAS_WEIGHT = false;

private:
    Eigen::VectorXd m_y = Eigen::VectorXd::Zero(INPUT_SIZE_);
    Eigen::VectorXd m_t = Eigen::VectorXd::Zero(INPUT_SIZE_);
};
