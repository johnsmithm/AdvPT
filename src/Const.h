#ifndef _CONST_H_
#define _CONST_H_

/** @brief  factor for fixed point numbers */
const int FP_FACTOR = 10000;

/** @brief  Amount of energy that is increased every timestep for each unit/building */
const int DEFAULT_ENERGY_INCREASE = 0.5625 * FP_FACTOR;

/** @brief  Amount of vespene gas that is increased every timestep per mining worker */
const int DEFAULT_WORKER_GAS_INCREASE = 0.35 * FP_FACTOR;

/** @brief  Amount of minerals that is increased every timestep per mining worker */
const int DEFAULT_WORKER_MINERAL_INCREASE = 0.7 * FP_FACTOR;

/** @brief  Amount of minerals at the start of the Game */
const int INITIAL_MINERAL_AMOUNT = 50 * FP_FACTOR;

/** @brief  Amount of minerals at the start of the Game */
const int INITIAL_WORKER_COUNT = 6;


/** @brief  Amount of energy required to do a chronoboost */
const int BOOST_REQUIRED_ENERGY = 25 * FP_FACTOR;

/** @brief  Chronoboost's duration in timesteps */
const int BOOST_ACTION_TIME = 20;


/** @brief  MULE's mining power in working units */
const int MULE_MINING_POWER = 4;

/** @brief  Amount of energy required for calldown MULE */
const int MULE_REQUIRED_ENERGY = 50 * FP_FACTOR;

/** @brief  MULE's life span in timesteps */
const int MULE_ACTION_TIME = 90;


/** @brief  Larva slots per hatchery/lair/hive */
const int LARVA_SLOTS = 3;

/** @brief  number of timesteps a larva needs to spawn */
const int LARVA_SPAWN_TIME = 15;

/** @brief  Amount of energy required for queen larva spawn */
const int QUEEN_EGGS_REQUIRED_ENERGY = 25;

/** @brief  Time till larvae spawn from eggs */
const int QUEEN_EGGS_SPAWN_TIME = 40;

/** @brief  Amount of QueenAction's eggs/larvae */
const int QUEEN_EGGS_AMOUNT = 4;

/** @brief  Maximum number of larva slots per hatchery/lair/hive */
const int MAX_LARVA_SLOTS = 19;

#endif
