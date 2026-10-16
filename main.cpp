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
#include "include/conf.hpp"
#include "include/machine.hpp"
#include "include/boiler.hpp"
#include "include/calc_functions.hpp"

// Domain math: (Steam Age specific) (20tps)
//
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

constexpr int g_days_to_seconds = 86400;
constexpr int g_hours_to_seconds = 3600;


// TODO: Fix fuel consumption errors, different type of fuels can be used, and different machine consume differently
enum class FuelType{
    Coal,
    Charcoal,
    Creosote,
    Solar,
    Lava,
};


void setup_config_file(std::string file_name){
    std::ifstream f(file_name);

    if (!f.is_open()) {
        std::cerr << "Error opening file: " << file_name << "." << std::endl;
        exit(EXIT_FAILURE);
    }

    else {
        // TODO: Add in placeholders for user inputs
        std::cout << "File found!!" << std::endl;

        config::print_menu();

        exit(EXIT_SUCCESS);
    }

}

void print_summary(boiler::Boiler& b, int n_boilers,
                   double actual_output, double coal_req,
                   double excess_steam, double water_consumption,
                   std::string_view time)
{
    std::cout << "Boiler requirement: " << n_boilers << " " << b.type << "s.\n";

    int multiplier;

    if (time.compare("sec") == 0){
        multiplier = 1;
    }

    else if (time.compare("hour") == 0) {
        multiplier = g_hours_to_seconds;
    }

    else if (time.compare("days") == 0) {
        multiplier = g_days_to_seconds;
    }

    std::cout
        << "Actual output: "      << std::fixed << std::setprecision(3) << actual_output * multiplier << "L/" << time << " \n"
        << "Coal requirement: "   << coal_req * 240  << " per " << time << " for " << n_boilers << " boilers. \n"
        << "Excess steam: "       << excess_steam * multiplier << "L/" << time << " \n"
        << "Water consumption: "  << water_consumption * multiplier << "L/" << time << "\n"
        << "\n\n";
}

void summary_calculation_helper(boiler::Boiler& boiler, double target, std::string_view time){
    int n_boilers            = calc_functions::compute_boiler_requirement(boiler, target);
    double actual_output     = calc_functions::compute_actual_output(n_boilers, boiler);
    double coal_req          = calc_functions::compute_coal_requirement(n_boilers);
    double excess_steam      = calc_functions::compute_excess_steam(target, actual_output);
    double water_consumption = calc_functions::compute_water_consumption(actual_output);

    print_summary(boiler, n_boilers, actual_output, coal_req, excess_steam, water_consumption, time);
};

int main(int argc, char* argv[]){

    // TODO: Should also probably move the arg parser into its own header file
    argparse::ArgumentParser program( "mc-calc" );

    {
        auto& group = program.add_mutually_exclusive_group();
        // TODO: Get this working, add a file generation function, take in user input, then file reading function
        group.add_argument("--setup")
            .help("Generate a config file of your current setup to run calculations.")
            .flag();

        //auto& group = program.add_mutually_exclusive_group(true);
        group.add_argument("--distance")
            .help("Calculate block distance based on provided chunk count")
            //.default_value(0)
            .scan<'i', int>();

        group.add_argument("--list-boilers")
            .help("View list of all boiler types")
            .default_value(false)
            .implicit_value(true);

        group.add_argument("--list-machines")
            .help("View list of all machine types")
            .flag();
    }

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


    // attach steam command to main parser
    program.add_subparser(steam_command);

    try {
        program.parse_args(argc, argv);
    }
    catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;

        return EXIT_FAILURE;
    }

    if (program["list-boilers"] == true) {
        boiler::list_boilers(boiler::boiler_map);
    }

    if (program["list-machines"] == true){
        machine::list_machines(machine::machine_map);
    }

    if (program["setup"] == true){
        std::cout << "Called setup function\n";
        std::string file_name { "mc.conf" };

        setup_config_file(file_name);

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
            for (const auto& [k, v] : boiler::boiler_map) {
                summary_calculation_helper(boiler::boiler_map[k], target, time);
            }

            return 0;
        }

        else if (boiler == "LPRailcraftBoiler" || boiler == "HPRailcraftBoiler") {

            // Making sure we get the right boiler 
            boiler::Boiler b; 
            if (boiler == "LPRailcraftBoiler")  b = boiler::boiler_map["LPRailcraftBoiler"];
            if (boiler == "HPRailcraftBoiler")  b = boiler::boiler_map["HPRailcraftBoiler"];

            // Get user to input number of tanks, used to multiply default steam output
            int n_tanks{};
            std::cout << "Enter the number of Railcraft Boiler tanks: ";

            try {
                std::cin >> n_tanks;
                // Railcraft boilers steam output is multiplied by number of tanks
                b.steam_out *= n_tanks;

            } catch (const std::exception& e) {
                std::cerr << e.what() << std::endl;
            }

            // compute values
            std::cout << "Calculating for " << boiler << " " << "with " << n_tanks << " tanks.\n";
            summary_calculation_helper(b, target, time);

            return EXIT_SUCCESS;
        }

        boiler::Boiler b          = boiler::boiler_map[boiler];
        summary_calculation_helper(b, target, time);

        return EXIT_SUCCESS;
    }

    if (program.present<int>("--distance").has_value() != 0) {
        int chunks = program.present<int>("--distance").value();
        int distance_in_blocks = calc_functions::calculate_distance(chunks);

        std::cout << distance_in_blocks << " blocks.\n";
    }

    return 0;
}
