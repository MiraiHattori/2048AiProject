#include "ai/layers.hpp"

#include <iostream>

#include <Eigen/Core>

int main()
{
    std::vector<double> y_stl{0.22, 0.27, 0.17, 0.21, 0.09, 0.04, 0.0};
    std::vector<double> t_stl{1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    Eigen::VectorXd y_eig = Eigen::Map<Eigen::VectorXd>(&y_stl[0], y_stl.size());
    Eigen::VectorXd t_eig = Eigen::Map<Eigen::VectorXd>(&t_stl[0], t_stl.size());

    SoftmaxWithLoss<7, 7> softmax_with_loss_eig{};

    Eigen::VectorXd softmax_with_loss_eig_forward = softmax_with_loss_eig.forward(y_eig, t_eig);
    std::cout << softmax_with_loss_eig_forward.transpose() << std::endl;
    std::cout << softmax_with_loss_eig.backward(softmax_with_loss_eig_forward).transpose() << std::endl;

    return 0;
}
