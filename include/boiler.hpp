#ifndef BOILER_HPP
#define BOILER_HPP

#include <ostream>
#include <iostream>
#include <unordered_map>
#include <string>

namespace boiler{

enum class BoilerType{
    Unknown,
    SmallCoalBoiler,
    LargeCoalBoiler,
    SimpleSolarBoiler,
    AdvancedSolarBoiler,
    ReinforcedLavaBoiler,
    LPRailcraftBoiler,
    HPRailcraftBoiler,
};

struct Boiler{
    BoilerType   type{};
    double       steam_out{};
    double       water_in{};
    double       fuel_consumption{};
};

static Boiler SmallCoalBoiler      { BoilerType::SmallCoalBoiler, 120.00, 0.75 };
static Boiler LargeCoalBoiler      { BoilerType::LargeCoalBoiler, 300.00, 1.88 };
static Boiler SimpleSolarBoiler    { BoilerType::SimpleSolarBoiler , 120.00, 0.75 };
static Boiler AdvancedSolarBoiler  { BoilerType::AdvancedSolarBoiler, 360.00, 2.25 };
static Boiler ReinforcedLavaBoiler { BoilerType::ReinforcedLavaBoiler, 600.00, 3.75 };

// Multiply default steam out by n_tanks to get steam output at max heat
static Boiler LPRailcraftBoiler    { BoilerType::LPRailcraftBoiler, 200.00, 1.25};
static Boiler HPRailcraftBoiler    { BoilerType::HPRailcraftBoiler, 800, 5.0};

static std::unordered_map<std::string, Boiler> boiler_map{
    {"SmallCoalBoiler", SmallCoalBoiler},
    {"LargeCoalBoiler", LargeCoalBoiler},
    {"SimpleSolarBoiler", SimpleSolarBoiler},
    {"AdvancedSolarBoiler", AdvancedSolarBoiler},
    {"ReinforcedLavaBoiler", ReinforcedLavaBoiler},
    {"LPRailcraftBoiler", LPRailcraftBoiler},
    {"HPRailcraftBoiler", HPRailcraftBoiler}
};

// Overload std::cout for boiler types to string
static std::ostream& operator<< (std::ostream& out, const BoilerType& bt){
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
            break;
        default:
            out << "Unknown Boiler type...\n";
            break;
    }
    return out;
}

static BoilerType get_boiler_type(std::string& boiler){
    if (boiler == "SmallCoalBoiler")           return BoilerType::SmallCoalBoiler;
    else if (boiler == "LargeCoalBoiler")      return BoilerType::LargeCoalBoiler;
    else if (boiler == "SimpleSolarBoiler")    return BoilerType::SimpleSolarBoiler;
    else if (boiler == "AdvancedSolarBoiler")  return BoilerType::AdvancedSolarBoiler;
    else if (boiler == "ReinforcedLavaBoiler") return BoilerType::ReinforcedLavaBoiler;
    else if (boiler == "LPRailcraftBoiler")    return BoilerType::LPRailcraftBoiler;
    else if (boiler == "HPRailcraftBoiler")    return BoilerType::HPRailcraftBoiler;
    return BoilerType::Unknown;
}

static void list_boilers(std::unordered_map<std::string, boiler::Boiler>& boilers){
    for (const auto& [k, v] : boilers){
        std::cout << v.type << '\n';
    }
}

};

#endif
