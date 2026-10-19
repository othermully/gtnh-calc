#ifndef CONF_HPP
#define CONF_HPP

#include <cstdlib>
#include <iostream>
#include <ncurses.h>

#include "boiler.hpp"

namespace config{


static void print_menu(){

    std::cout << "Working properly.\n";

    double total_steam_output{};

    for (const auto& [k,v] : boiler::boiler_map) {


        int n;
        std::cout << "How many " << k << " do you have: " << std::endl;
        std::cin >> n;

        // If the user has that boiler
        if (n > 0){
            total_steam_output += v.steam_out;
        }
    }

    std::cout << "Your total steam output is: " << total_steam_output << std::endl;

    exit(EXIT_SUCCESS);
}

};

#endif 
