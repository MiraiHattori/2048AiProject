#include "ai/full_connected_network.hpp"

#include <iostream>

int main()
{
    Eigen::VectorXd x0 = Eigen::VectorXd::Zero(2);
    x0 << 1, 0;
    Eigen::VectorXd t0 = Eigen::VectorXd::Zero(2);
    t0 << 1, 0;
    Eigen::VectorXd x1 = Eigen::VectorXd::Zero(2);
    x1 << 0, 1;
    Eigen::VectorXd t1 = Eigen::VectorXd::Zero(2);
    t1 << 0, 1;
    std::shared_ptr<GD> gd = std::make_shared<GD>(0.01);
    FullConnectedNetwork<SoftmaxWithLoss<2, 2>, Affine<2, 10>, Relu<10, 10>, Affine<10, 2>> net{gd};
    for (int i = 0; i < 100000; i++) {
        net.learn(x0, t0);
        net.learn(x1, t1);
    }
    std::cout << "net.predict(x0)" << std::endl;
    std::cout << net.predict(x0) << std::endl;
    std::cout << "net.predict(x1)" << std::endl;
    std::cout << net.predict(x1) << std::endl;
    return 0;
}
