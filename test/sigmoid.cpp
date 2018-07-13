#include "ai/layers.hpp"

#include <iostream>

#include <Eigen/Core>

int main()
{
    std::vector<double> v_stl{1.0, 2.0, -1.0, 1.0, -0.0, 0.0, -100.0};
    Eigen::VectorXd v_eig = Eigen::Map<Eigen::VectorXd>(&v_stl[0], v_stl.size());
    Sigmoid<7, 7> sigmoid_eig{};

    Eigen::VectorXd sigmoid_eig_forward = sigmoid_eig.forward(v_eig);
    std::cout << sigmoid_eig_forward.transpose() << std::endl;
    std::cout << sigmoid_eig.backward(sigmoid_eig_forward).transpose() << std::endl;

    return 0;
}
