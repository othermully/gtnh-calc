#ifndef MACHINE_HPP
#define MACHINE_HPP

#include <string>
#include <unordered_map>
#include <iostream>

namespace machine {

enum class MachineType {
    Unknown,
    HPAlloySmelter,
    LPAlloySmelter,
    HPCompressor,
    LPCompressor,
    HPExtractor,
    LPExtractor,
    HPMacerator,
    LPMacerator,
    HPFurnace,
    LPFurnace,
    HPForgeHammer,
    LPForgeHammer,
};

struct Machine{
    MachineType type{};
    double steam_consumption;
};

// TODO: Need to add output values
static Machine LPMacerator         { MachineType::LPMacerator, 80.0 };
static Machine HPMacerator         { MachineType::HPMacerator, 120.0 };
static Machine LPFurnace           { MachineType::LPFurnace, 160.0 };
static Machine HPFurnace           { MachineType::HPFurnace, 320.0 };
static Machine LPAlloySmelter      { MachineType::LPAlloySmelter, 1200.0 };
static Machine HPAlloySmelter      { MachineType::HPAlloySmelter, 2400.0 };
static Machine LPCompressor        { MachineType::LPCompressor, 160.0 };
static Machine HPCompressor        { MachineType::HPCompressor, 320.0 };
static Machine LPForgeHammer       { MachineType::LPForgeHammer, 1200.0 };
static Machine HPForgeHammer       { MachineType::HPForgeHammer, 2400.0 };
static Machine LPExtractor         { MachineType::LPExtractor, 320.0 };
static Machine HPExtractor         { MachineType::HPExtractor, 640.0 };

static std::unordered_map<std::string, Machine> machine_map{
    {"LPMacerator",     LPMacerator},
    {"HPMacerator",     HPMacerator},
    {"LPFurnace",       LPFurnace},
    {"HPFurnace",       HPFurnace},
    {"LPAlloySmelter",  LPAlloySmelter},
    {"HPAlloySmelter",  HPAlloySmelter},
    {"LPCompressor",    LPCompressor},
    {"HPCompressor",    HPCompressor},
    {"LPForgeHammer",   LPForgeHammer},
    {"HPForgeHammer",   HPForgeHammer},
    {"LPExtractor",     LPExtractor},
    {"HPExtractor",     HPExtractor}
};

// Overload std::cout for machine types to string
static std::ostream& operator<< (std::ostream& out, const MachineType& mt){
    switch (mt) {
        case MachineType::LPMacerator:
            out << "Low Pressure Macerator";
            break;
        case MachineType::HPMacerator:
            out << "High Pressure Macerator";
            break;
        case MachineType::LPCompressor:
            out << "Low Pressure Compressor";
            break;
        case MachineType::HPCompressor:
            out << "High Pressure Compressor";
            break;
        case MachineType::LPExtractor:
            out << "Low Pressure Extractor";
            break;
        case MachineType::HPExtractor:
            out << "High Pressure Extractor";
            break;
        case MachineType::LPAlloySmelter:
            out << "Low Pressure Alloy Smelter";
            break;
        case MachineType::HPAlloySmelter:
            out << "High Pressure Alloy Smelter";
            break;
        case MachineType::LPForgeHammer:
            out << "Low Pressure Forge Hammer";
            break;
        case MachineType::HPForgeHammer:
            out << "High Pressure Forge Hammer";
            break;
        case MachineType::LPFurnace:
            out << "Low Pressure Furnace";
            break;
        case MachineType::HPFurnace:
            out << "High Pressure Furnace";
            break;
        default:
            out << "Unknown Machine type...\n";
            break;
    }
    return out;
}

[[maybe_unused]] static MachineType get_machine_type(std::string& machine){
    if (machine == "LPCompressor")        return MachineType::LPCompressor;
    else if (machine == "HPCompressor")   return MachineType::HPCompressor;
    else if (machine == "LPExtractor")    return MachineType::LPExtractor;
    else if (machine == "HPExtractor")    return MachineType::HPExtractor;
    else if (machine == "HPMacerator")    return MachineType::HPMacerator;
    else if (machine == "LPMacerator")    return MachineType::LPMacerator;
    else if (machine == "HPFurnace")      return MachineType::HPFurnace;
    else if (machine == "LPFurnace")      return MachineType::LPFurnace;
    else if (machine == "HPForgeHammer")  return MachineType::HPForgeHammer;
    else if (machine == "LPForgeHammer")  return MachineType::LPForgeHammer;
    else if (machine == "HPAlloySmelter") return MachineType::HPAlloySmelter;
    else if (machine == "LPAlloySmelter") return MachineType::LPAlloySmelter;
    return MachineType::Unknown;
}
};

#endif
