#include <array>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <iomanip>
#include <ios>
#include <iostream>
#include <locale>
#include <ostream>
#include <string>
#include <unordered_map>
#include <fstream>
#include "include/argparse/argparse.hpp"

// Domain math: (Steam Age specific) (20tps)
// Water tank L/s = 25 x Humidity x Environmental Modifiers
// - Rain 300%
// - No sky access 50%
// - Snowing 50%

// Small coal boiler:
// Steam output: 120L/s, Tank(buffer) = 16,000L
// Water in: 0.75L/s, Tank(buffer) = 16,000L
// Max Heat: 500°C, Heat up rate: +1°C per 12 ticks

// Large coal boiler:
// Steam output: 300L/s, Tank(buffer) = 32,000L
// Water in: 1.88L/s, Tank(buffer) = 32,000L
// Max Heat: 1000°C, Heat up rate: +1°C per 12 ticks

// Simple solar boiler:
// Steam output: 120L/s, Tank(buffer) = 16,000L
// Water in: 0.75L/s, Tank(buffer) = 16,000L
// Max Heat: 500°C, Heat up rate: +5°C per 12 ticks

// Advanced solar boiler:
// Steam output: 360L/s, Tank(buffer) = 32,000L,
// Water in: 2.25L/s, Tank(buffer) = 32,000L
// Max Heat: 500°C, Heat up rate: +5°C per 12 ticks

// Reinforced lava boiler:
// 600L/s, Tank(buffer) = 32,000L
// Water in: 3.75L/s, Tank(buffer) = 32,000L
// Max Heat: 500°C, Heat up rate: +1°C per 12 ticks

// Water is converted to steam at a 1:160 ratio
// Coal boilers consume 10 ticks of fuel every time their heat increases by +1°C 
// Lava boilers consume 3L of lava every time its heat increaes by +1°C 
// Boilers heating process, 1 charcoal last 96 seconds, at max heat, it lasts 360 seconds 


constexpr int g_chunk = 16;
constexpr int g_days_to_seconds = 86400;
constexpr int g_hours_to_seconds = 3600;


// TODO: Add Railcraft boiler as boiler type, uses different formulas, its also a multiblock structure
enum class BoilerType{
    SmallCoalBoiler,
    LargeCoalBoiler,
    SimpleSolarBoiler,
    AdvancedSolarBoiler,
    ReinforcedLavaBoiler,
    LPRailcraftBoiler,
    HPRailcraftBoiler,
};

BoilerType get_boiler_type(std::string boiler){
    if (boiler == "SmallCoalBoiler") {
        return BoilerType::SmallCoalBoiler;
    }
    else if (boiler == "LargeCoalBoiler")
        return BoilerType::LargeCoalBoiler;
    else if (boiler == "SimpleSolarBoiler"){
        return BoilerType::SimpleSolarBoiler;
    }
    else if (boiler == "AdvancedSolarBoiler") {
        return BoilerType::AdvancedSolarBoiler;
    }
    else if (boiler == "ReinforcedLavaBoiler") {
        return BoilerType::ReinforcedLavaBoiler;
    }
    else if (boiler == "LPRailcraftBoiler"){
        return BoilerType::LPRailcraftBoiler;
    }
    else if (boiler == "HPRailcraftBoiler") {
        return BoilerType::HPRailcraftBoiler;
    }

    return BoilerType::SmallCoalBoiler;
}

struct Boiler{
    BoilerType   type{};
    double       steam_out{};
    double       water_in{};
//    float       coal_in{};
};

// Overload std::cout for boiler types to string
std::ostream& operator<< (std::ostream& out, const BoilerType& bt){
    switch (bt) {
        case BoilerType::SmallCoalBoiler:
            out << "Small Coal Boiler";
            break;
        case BoilerType::LargeCoalBoiler:
            out << "Large Coal Boiler";
            break;
        case BoilerType::SimpleSolarBoiler:
            out << "Simple Solar Boiler";
            break;
        case BoilerType::AdvancedSolarBoiler:
            out << "Advanced Solar Boiler";
            break;
        case BoilerType::ReinforcedLavaBoiler:
            out << "Reinforced Lava Boiler";
            break;
        case BoilerType::LPRailcraftBoiler:
            out << "Low Pressure Railcraft Boiler";
            break;
        case BoilerType::HPRailcraftBoiler:
            out << "High Pressure Railcraft Boiler";
        default:
            out << "Unknown Boiler type...\n";
            break;
    }
    return out;
}


int calculate_distance(const uint16_t chunks){
    return chunks * g_chunk;
}

double compute_actual_output(const int n_boilers, const Boiler& b){
    return b.steam_out * n_boilers;
}

//TODO Am i retarded?
double compute_coal_requirement(int n_boilers)
{
    // 1 coal at max heat = 360 seconds of steam, 120L/s (small boiler) * 360 = 43,200Ls/coal
    return (static_cast<double>(n_boilers) / 360) * 360;
    //return (actual_out / rate_per_boiler);
}

double compute_water_consumption(const double actual_out){
    // water:steam = 1:160
    return actual_out / 160;
}

double compute_excess_steam(const double steam_target,
                            const double actual_output)
{
    return actual_output - steam_target;
}

double compute_boiler_requirement(const Boiler& b, const double steam_target){
    // Get ceiling value for required boiler count
    int n_boilers = std::ceil(steam_target / b.steam_out);
    return n_boilers;
}

void list_boilers(std::unordered_map<std::string, Boiler>& boilers){
    for (const auto& [k, v] : boilers){
        std::cout << v.type << '\n';
    }
}

void print_summary(Boiler& b, int n_boilers,
                   double actual_output, double coal_req,
                   double excess_steam, double water_consumption,
                   std::string_view time)
{
    std::cout << "Boiler requirement: " << n_boilers << " " << b.type << "s.\n";
    if (time.compare("sec") == 0){
        std::cout 
            << "Actual output: "      << std::fixed << std::setprecision(3) << actual_output << "L/s \n"
            << "Coal requirement: "   << coal_req          << " per 6 mintues for " << n_boilers <<  " boilers. \n"
            << "Excess steam: "       << excess_steam      << "L/s \n"
            << "Water consumption: "  << water_consumption << "L/s\n"
            << "\n\n";
    }
    else if (time.compare("hour") == 0) {
        std::cout
            << "Actual output: "      << std::fixed << std::setprecision(3) << actual_output * g_hours_to_seconds << "L/hour \n"
            << "Coal requirement: "   << coal_req * 10            << " per hour for " << n_boilers << " boilers. \n"
            << "Excess steam: "       << excess_steam * g_hours_to_seconds << "L/hour \n"
            << "Water consumption: "  << water_consumption * g_hours_to_seconds << "L/hour\n"
            << "\n\n";
    }

    else if (time.compare("days") == 0) {
        std::cout
            << "Actual output: "      << std::fixed << std::setprecision(3) << actual_output * g_days_to_seconds << "L/day \n"
            << "Coal requirement: "   << coal_req * 240            << " per day for " << n_boilers << " boilers. \n"
            << "Excess steam: "       << excess_steam * g_days_to_seconds << "L/day \n"
            << "Water consumption: "  << water_consumption * g_days_to_seconds << "L/day\n"
            << "\n\n";
    }
}

int main(int argc, char* argv[]){

    // Assuming all boilers are at max heat
                                // Type                        S/s     Water
    Boiler SmallCoalBoiler      { BoilerType::SmallCoalBoiler, 120.00, 0.75 };
    Boiler LargeCoalBoiler      { BoilerType::LargeCoalBoiler, 300.00, 1.88 };
    Boiler SimpleSolarBoiler    { BoilerType::SimpleSolarBoiler , 120.00, 0.75 };
    Boiler AdvancedSolarBoiler  { BoilerType::AdvancedSolarBoiler, 360.00, 2.25 };
    Boiler ReinforcedLavaBoiler { BoilerType::ReinforcedLavaBoiler, 600.00, 3.75 };
    
    // Multiply default steam out by n_tanks to get steam output at max heat
    Boiler LPRailcraftBoiler    { BoilerType::LPRailcraftBoiler, 200.00, 1.25};
    Boiler HPRailcraftBoiler    { BoilerType::HPRailcraftBoiler, 800, 5};


    std::unordered_map<std::string, Boiler> boiler_map;
    boiler_map["SmallCoalBoiler"]      = SmallCoalBoiler;
    boiler_map["LargeCoalBoiler"]      = LargeCoalBoiler;
    boiler_map["SimpleSolarBoiler"]    = SimpleSolarBoiler;
    boiler_map["AdvancedSolarBoiler"]  = AdvancedSolarBoiler;
    boiler_map["ReinforcedLavaBoiler"] = ReinforcedLavaBoiler;
    boiler_map["LPRailcraftBoiler"]    = LPRailcraftBoiler;
    boiler_map["HPRailcraftBoiler"]    = HPRailcraftBoiler;

    argparse::ArgumentParser program( "mc-calc" );

    //auto& group = program.add_mutually_exclusive_group(true);
    program.add_argument("--distance")
        .help("Calculate block distance based on provided chunk count")
        //.default_value(0)
        .scan<'i', int>();

    program.add_argument("--list-boilers")
        .help("View list of all boiler types")
        .default_value(false)
        .implicit_value(true);

    // steam sub-command
    argparse::ArgumentParser steam_command("steam");
    steam_command.add_description("Calculate boiler requirement for desired Steam L/s");
    steam_command.add_argument(("--target"))
        .help("Specify a desired steam liters/second output")
        .scan<'g', double>();

    // TODO: Get rid of default value, should be an optional arg?
    steam_command.add_argument("--type")
        .default_value(std::string("ALL"))
        .help("Specify a boiler type.");

    steam_command.add_argument("--time")
        .default_value(std::string("sec"))
        .help("Specify output in liters/s or liters/h (e.g. --time hour, --time sec");

    // TODO: Get this working, add a file generation function, take in user input, then file reading function
    steam_command.add_argument("--setup")
        .help("Generate a config file of your current setup to run calculations.")
        .default_value(false)
        .implicit_value(true);


    // attach steam command to main parser
    program.add_subparser(steam_command);

    try {
        program.parse_args(argc, argv);
    }
    catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return 1;
    }

    if (program["list-boilers"] == true) {
        list_boilers(boiler_map);
    }

    if (steam_command.present<double>("--target").has_value() != 0) {
        std::cout.imbue(std::locale("")); // automatically add commas to numbers

        // steam calculator here
        double target          = steam_command.present<double>("--target").value();
        std::string time       = steam_command.get<std::string>("--time");

        std::cout << "Target steam/s: " << std::fixed << std::setprecision(3) << target << std::endl;

        int n_boilers{};
        double actual_output{};
        double coal_req{};
        double excess_steam{};
        double water_consumption{};

        std::string boiler     = steam_command.get<std::string>("--type");

        if (boiler == "ALL") {
            for (const auto& [k, v] : boiler_map) {
                n_boilers         = compute_boiler_requirement(v, target);
                actual_output     = compute_actual_output(n_boilers, v);
                coal_req          = compute_coal_requirement(n_boilers);
                excess_steam      = compute_excess_steam(target, actual_output);
                water_consumption = compute_water_consumption(actual_output);
                print_summary(boiler_map[k], n_boilers, actual_output, coal_req, excess_steam, water_consumption, time);
            }
            return 0;
        }

        else if (boiler == "LPRailcraftBoiler" || boiler == "HPRailcraftBoiler") {

            // Making sure we get the right boiler 
            Boiler b; 
            if (boiler == "LPRailcraftBoiler")  b = boiler_map["LPRailcraftBoiler"];
            if (boiler == "HPRailcraftBoiler")  b = boiler_map["HPRailcraftBoiler"];

            // Get user to input number of tanks, used to multiply default steam output
            int n_tanks;
            std::cout << "Enter the number of Railcraft Boiler tanks: ";

            try {
                std::cin >> n_tanks;
                b.steam_out *= n_tanks;

            } catch (const std::exception& e) {
                std::cerr << e.what() << std::endl;
            }

            // compute values
            std::cout << "Calculating for " << boiler << " " << "with " << n_tanks << " tanks.\n";

            n_boilers         = compute_boiler_requirement(b, target);
            actual_output     = compute_actual_output(n_boilers, b);
            coal_req          = compute_coal_requirement(n_boilers);
            excess_steam      = compute_excess_steam(target, actual_output);
            water_consumption = compute_water_consumption(actual_output);
            print_summary(b, n_boilers, actual_output, coal_req, excess_steam, water_consumption, time);

            return EXIT_SUCCESS;
        }

        Boiler b          = boiler_map[boiler];
        n_boilers         = compute_boiler_requirement(b, target);
        actual_output     = compute_actual_output(n_boilers, b);
        coal_req          = compute_coal_requirement(n_boilers);
        excess_steam      = compute_excess_steam(target, actual_output);
        water_consumption = compute_water_consumption(actual_output);
        print_summary(b, n_boilers, actual_output, coal_req, excess_steam, water_consumption, time);

        return EXIT_SUCCESS;
    }

    if (program.present<int>("--distance").has_value() != 0) {
        int chunks = program.present<int>("--distance").value();
        int distance_in_blocks = calculate_distance(chunks);

        std::cout << distance_in_blocks << " blocks.\n";
    }

    return 0;
}
