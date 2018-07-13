#include "ai/layers.hpp"

#include <iostream>

#include <Eigen/Core>

int main()
{
    std::vector<double> v_stl{1.0, 2.0, -1.0, 1.0, -0.0, 0.0, -100.0};
    Eigen::VectorXd v_eig = Eigen::Map<Eigen::VectorXd>(&v_stl[0], v_stl.size());
    Relu<7, 7> relu_eig{};

    Eigen::VectorXd relu_eig_forward = relu_eig.forward(v_eig);
    std::cout << relu_eig_forward.transpose() << std::endl;
    std::cout << relu_eig.backward(relu_eig_forward).transpose() << std::endl;

    return 0;
}
