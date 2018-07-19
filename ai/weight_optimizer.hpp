#pragma once

#include <Eigen/Core>

class WeightOptimizer
{
public:
    WeightOptimizer() = default;
    virtual Eigen::MatrixXd updateWeight(
        const Eigen::MatrixXd& w,
        const Eigen::MatrixXd& w_grad) const = 0;
    // TODO デバッグ用
    virtual Eigen::MatrixXd updateWeight(
        const Eigen::MatrixXd& w,
        const Eigen::MatrixXd& w_grad,
        const double& lambda) const = 0;
};

// 勾配降下法: Gradient Descent
class GD : public WeightOptimizer
{
public:
    /*
     * @param lambda 学習係数
     */
    explicit GD(const double& lambda) : WeightOptimizer{}, m_lambda(lambda) {}

    Eigen::MatrixXd updateWeight(
        const Eigen::MatrixXd& w,
        const Eigen::MatrixXd& w_grad) const override
    {
        return w - m_lambda * w_grad;
    }
    // TODO デバッグ用
    Eigen::MatrixXd updateWeight(
        const Eigen::MatrixXd& w,
        const Eigen::MatrixXd& w_grad,
        const double& lambda) const
    {
        return w - lambda * w_grad;
    }

private:
    double m_lambda{};
};
