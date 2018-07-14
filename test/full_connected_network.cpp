#include "ai/full_connected_network.hpp"
#include "ai/layers.hpp"

int main()
{
    Eigen::VectorXd x = Eigen::VectorXd::Zero(2);
    x << 1, 0;
    FullConnectedNetwork<SoftmaxWithLoss<2, 2>, Affine<2, 3>, Relu<3, 3>, Affine<3, 2>> net{};
    net.backProp(x, x);
    return 0;
}
