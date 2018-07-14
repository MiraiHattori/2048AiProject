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
};

// std::tuple_element<i, std::tuple<HIDDEN_LAYERS...>>::typeでi番目の型を取得できる
template <typename INT, typename... HIDDEN_LAYERS>
struct WrapFunc_ {
    static Eigen::VectorXd predict_(
        const Eigen::VectorXd& x,
        std::tuple<HIDDEN_LAYERS...>& layers)
    {
        return WrapFunc_<WrapInt<INT::value_plus_1>, HIDDEN_LAYERS...>::predict_(
            std::get<INT::value>(layers).forward(x), layers);
    }
    static Eigen::VectorXd backProp_(
        const Eigen::VectorXd& dout,
        std::tuple<HIDDEN_LAYERS...>& layers)
    {
        return WrapFunc_<WrapInt<INT::value_plus_1>, HIDDEN_LAYERS...>::backProp_(
            std::get<INT::value>(layers).backward(dout), layers);
    }
};

template <typename... HIDDEN_LAYERS>
struct WrapFunc_<WrapInt<0>, HIDDEN_LAYERS...> {
    static Eigen::VectorXd predict_(
        const Eigen::VectorXd& x,
        std::tuple<HIDDEN_LAYERS...>& layers)
    {
        return WrapFunc_<WrapInt<1>, HIDDEN_LAYERS...>::predict_(
            std::get<0>(layers).forward(x), layers);
    }
    static Eigen::VectorXd backProp_(
        const Eigen::VectorXd& dout,
        std::tuple<HIDDEN_LAYERS...>& layers)
    {
        return WrapFunc_<WrapInt<1>, HIDDEN_LAYERS...>::backProp_(
            std::get<sizeof...(HIDDEN_LAYERS) - 1>(layers).backward(dout), layers);
    }
};

template <typename... HIDDEN_LAYERS>
struct WrapFunc_<WrapInt<sizeof...(HIDDEN_LAYERS) - 1>, HIDDEN_LAYERS...> {
    static Eigen::VectorXd predict_(
        const Eigen::VectorXd& x,
        std::tuple<HIDDEN_LAYERS...>& layers)
    {
        return std::get<sizeof...(HIDDEN_LAYERS) - 1>(layers).forward(x);
    }
    static Eigen::VectorXd backProp_(
        const Eigen::VectorXd& dout,
        std::tuple<HIDDEN_LAYERS...>& layers)
    {
        return std::get<0>(layers).backward(dout);
    }
};

template <typename... HIDDEN_LAYERS>
struct WrapFunc {
    static Eigen::VectorXd predict(
        const Eigen::VectorXd& x,
        std::tuple<HIDDEN_LAYERS...>& layers)
    {
        return WrapFunc_<WrapInt<0>, HIDDEN_LAYERS...>::predict_(x, layers);
    }
    static Eigen::VectorXd backProp(
        const Eigen::VectorXd& dout,
        std::tuple<HIDDEN_LAYERS...>& layers)
    {
        return WrapFunc_<WrapInt<0>, HIDDEN_LAYERS...>::backProp_(dout, layers);
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
        // 層をfor文で回して順々に計算し取得しようとしたが、templateの闇にハマってしまった
        Eigen::VectorXd out = WrapFunc<HIDDEN_LAYERS...>::predict(x, m_layers);
        return out;
    }

    Eigen::VectorXd loss(const Eigen::VectorXd& x, const Eigen::VectorXd& t)
    {
        // TODO L2正則化などはここで実装するとよい
        Eigen::VectorXd y = this->predict(x);
        return m_last_layer.forward(y, t);
    }

    void backProp(const Eigen::VectorXd& x, const Eigen::VectorXd& t)
    {
        // forward; disposing return value.
        this->loss(x, t);
        Eigen::VectorXd dout = Eigen::VectorXd::Ones(LAST_LAYER::OUTPUT_SIZE);
        dout = m_last_layer.backward(dout);
        dout = WrapFunc<HIDDEN_LAYERS...>::backProp(dout, m_layers);
    }


private:
    std::tuple<HIDDEN_LAYERS...> m_layers{};
    LAST_LAYER m_last_layer{};
};
