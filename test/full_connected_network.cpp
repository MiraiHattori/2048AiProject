#include "ai/full_connected_network.hpp"

#include <iostream>

int main()
{
    Eigen::VectorXd x0 = Eigen::VectorXd::Zero(16);
    x0 << 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0;
    Eigen::VectorXd t0 = Eigen::VectorXd::Zero(4);
    t0 << 1, 0, 0, 0;
    Eigen::VectorXd x1 = Eigen::VectorXd::Zero(16);
    x1 << 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0;
    Eigen::VectorXd t1 = Eigen::VectorXd::Zero(4);
    t1 << 0, 1, 0, 0;
    Eigen::VectorXd x2 = Eigen::VectorXd::Zero(16);
    x2 << 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0;
    Eigen::VectorXd t2 = Eigen::VectorXd::Zero(4);
    t2 << 0, 0, 1, 0;
    Eigen::VectorXd x3 = Eigen::VectorXd::Zero(16);
    x3 << 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1;
    Eigen::VectorXd t3 = Eigen::VectorXd::Zero(4);
    t3 << 0, 0, 0, 1;
    std::shared_ptr<GD> gd = std::make_shared<GD>(0.01);
    FullConnectedNetwork<SoftmaxWithLoss<4, 4>, Affine<16, 20>, Relu<20, 20>, Affine<20, 4>> net{gd};
    constexpr bool PRINT_WEIGHT = false;
    for (int i = 0; i < 10000; i++) {
        net.learn(x0, t0, PRINT_WEIGHT);
        net.learn(x1, t1, PRINT_WEIGHT);
        net.learn(x2, t2, PRINT_WEIGHT);
        net.learn(x3, t3, PRINT_WEIGHT);
    }
    std::cout << "net.predict(x0)" << std::endl;
    std::cout << net.predict(x0) << std::endl;
    std::cout << "net.predict(x1)" << std::endl;
    std::cout << net.predict(x1) << std::endl;
    std::cout << "net.predict(x2)" << std::endl;
    std::cout << net.predict(x2) << std::endl;
    std::cout << "net.predict(x3)" << std::endl;
    std::cout << net.predict(x3) << std::endl;
    return 0;
}
