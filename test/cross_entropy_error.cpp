#include "ai/layers.hpp"

#include <iostream>

#include <Eigen/Core>

int main()
{
    std::vector<double> y_stl{0.22, 0.27, 0.17, 0.21, 0.09, 0.04, 0.0};
    std::vector<double> t_stl{1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    Eigen::VectorXd y_eig = Eigen::Map<Eigen::VectorXd>(&y_stl[0], y_stl.size());
    Eigen::VectorXd t_eig = Eigen::Map<Eigen::VectorXd>(&t_stl[0], t_stl.size());

    std::cout << crossEntropyError<7, 7>(y_eig, t_eig).transpose() << std::endl;

    return 0;
}
