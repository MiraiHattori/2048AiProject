#pragma once
#include "ai/full_connected_network.hpp"

namespace AI
{

void init();
void initGame();
void chooseMove(const double& e_greedy_epsilon);
void printWeight();
}  // namespace AI
