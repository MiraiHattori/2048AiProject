#include "ai/layers.hpp"

#include <iostream>

#include <Eigen/Core>


int main()
{
    std::vector<double> x_stl{1.0, 2.0};
    Eigen::VectorXd x_eig = Eigen::Map<Eigen::VectorXd>(&x_stl[0], x_stl.size());

    Affine<2, 6> affine_eig{};


    Eigen::VectorXd affine_eig_forward = affine_eig.forward(x_eig);
    std::cout << affine_eig_forward.transpose() << std::endl;
    std::cout << affine_eig.backward(affine_eig_forward).transpose() << std::endl;

    return 0;
}
