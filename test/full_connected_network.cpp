#include "ai/full_connected_network.hpp"
#include "ai/layers.hpp"

int main()
{
    Eigen::VectorXd x = Eigen::VectorXd(16);
    x << 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 12, 11;
    FullConnectedNetwork<SoftmaxWithLoss<4, 4>,
        Affine<16, 20>, Relu<20, 20>, Affine<20, 4>>
        net{};
    std::cout << net.predict(x) << std::endl;
    return 0;
}
