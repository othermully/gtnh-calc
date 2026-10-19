#ifndef CALC_FUNCTIONS_HPP
#define CALC_FUNCTIONS_HPP

#include <cmath>
#include <cstdint>

#include "boiler.hpp"

constexpr int g_chunk = 16;

namespace calc_functions{

static int calculate_distance(const uint16_t chunks){
    return chunks * g_chunk;
}

static double compute_actual_output(const int n_boilers, const boiler::Boiler& b){
    return b.steam_out * n_boilers;
}

//TODO Am i retarded? - Yes
static double compute_coal_requirement(int n_boilers)
{
    // 1 coal at max heat = 360 seconds of steam, 120L/s (small boiler) * 360 = 43,200Ls/coal
    return (static_cast<double>(n_boilers) / 360) * 360; // always going to return n_boilers lol
    //return (actual_out / rate_per_boiler);
}

static double compute_water_consumption(const double actual_out){
    // water:steam = 1:160
    return actual_out / 160;
}

static double compute_excess_steam(const double steam_target,
                            const double actual_output)
{
    return actual_output - steam_target;
}

static double compute_boiler_requirement(const boiler::Boiler& b, const double steam_target){
    // Get ceiling value for required boiler count
    int n_boilers = std::ceil(steam_target / b.steam_out);
    return n_boilers;
}

};

#endif
