#pragma once

#include <tuple>
#include <Eigen/Core>
#include "ai/layers.hpp"

namespace
{

template <int I>
struct WrapInt {
    WrapInt() = default;
    static constexpr int value = I;
    static constexpr int value_plus_1 = I + 1;
    static constexpr int value_minus_1 = I - 1;
};

// std::tuple_element<i, std::tuple<HIDDEN_LAYERS...>>::typeでi番目の型を取得できる
template <typename INT, typename LAST_LAYER, typename... HIDDEN_LAYERS>
struct WrapFunc_ {
    static Eigen::VectorXd
    predict_(
        const Eigen::VectorXd& x,
        const Eigen::VectorXd& teacher,
        const std::tuple<HIDDEN_LAYERS...>& layers,
        const LAST_LAYER& last_layer)
    {
        typename std::tuple_element<INT::value, std::tuple<HIDDEN_LAYERS...>>::type tmp = std::get<INT::value>(layers);
        return WrapFunc_<WrapInt<INT::value_plus_1>, LAST_LAYER, HIDDEN_LAYERS...>::predict_(
            tmp.forward(x), teacher, layers, last_layer);
    }
};

template <typename LAST_LAYER, typename... HIDDEN_LAYERS>
struct WrapFunc_<WrapInt<0>, LAST_LAYER, HIDDEN_LAYERS...> {
    static Eigen::VectorXd
    predict_(
        const Eigen::VectorXd& x,
        const Eigen::VectorXd teacher,
        const std::tuple<HIDDEN_LAYERS...>& layers,
        const LAST_LAYER& last_layer)
    {
        typename std::tuple_element<0, std::tuple<HIDDEN_LAYERS...>>::type tmp = std::get<0>(layers);
        return WrapFunc_<WrapInt<1>, LAST_LAYER, HIDDEN_LAYERS...>::predict_(
            tmp.forward(x), teacher, layers, last_layer);
    }
};

template <typename LAST_LAYER, typename... HIDDEN_LAYERS>
struct WrapFunc_<WrapInt<sizeof...(HIDDEN_LAYERS) - 1>, LAST_LAYER, HIDDEN_LAYERS...> {
    static Eigen::VectorXd
    predict_(
        const Eigen::VectorXd& x,
        const Eigen::VectorXd& teacher,
        const std::tuple<HIDDEN_LAYERS...>&,
        LAST_LAYER last_layer)
    {
        return last_layer.forward(x, teacher);
    }
};

template <typename LAST_LAYER, typename... HIDDEN_LAYERS>
struct WrapFunc {
    static Eigen::VectorXd predict(
        const Eigen::VectorXd& x,
        const Eigen::VectorXd& teacher,
        const std::tuple<HIDDEN_LAYERS...>& layers,
        const LAST_LAYER& last_layer)
    {
        return WrapFunc_<WrapInt<0>, LAST_LAYER, HIDDEN_LAYERS...>::predict_(
            x, teacher, layers, last_layer);
    }
};
}  // anonymous namespace

template <typename LAST_LAYER, typename... HIDDEN_LAYERS>
class FullConnectedNetwork
{
public:
    explicit FullConnectedNetwork()
    {
        this->initLayers();
    }

    void initLayers()
    {
        // initialize m_layers and m_last_layer weight and bias
    }

    Eigen::VectorXd predict(const Eigen::VectorXd& x)
    {
        Eigen::VectorXd t = x;
        // 層をfor文で回して順々に計算し取得しようとしたが、templateの闇にハマってしまった
        Eigen::VectorXd out = WrapFunc<LAST_LAYER, HIDDEN_LAYERS...>::predict(
            x, t, m_layers, m_last_layer);
        return out;
    }


private:
    static constexpr std::size_t HIDDEN_LAYER_SIZE = sizeof...(HIDDEN_LAYERS);
    std::tuple<HIDDEN_LAYERS...> m_layers{};
    LAST_LAYER m_last_layer{};
};
