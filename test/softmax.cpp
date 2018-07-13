#include "ai/layers.hpp"

#include <iostream>

#include <Eigen/Core>

int main()
{
    std::vector<double> v_stl{1.0, 5.0, -1.0, 1.0, -0.0, 0.0, -100.0};
    Eigen::VectorXd v_eig = Eigen::Map<Eigen::VectorXd>(&v_stl[0], v_stl.size());

    std::cout << softmax<7, 7>(v_eig).transpose() << std::endl;

    return 0;
}
