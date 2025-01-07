#ifndef VELOCITY_SETTINGS_HPP
#define VELOCITY_SETTINGS_HPP

#include "../utils/common.hpp"

namespace spade {
    /**
     * Represents VM settings
     */
    struct Settings {
        string VERSION = "1.0";
        string LANG_NAME = "spade";
        string VM_NAME = "velocity";
        string INFO_STRING = VERSION + " " + LANG_NAME + " " + VM_NAME;

        std::set<string> inbuiltTypes = {
                ".array",
                ".bool",
                ".char",
                ".float",
                ".int",
                ".string",
        };

        string libPath;
        vector<fs::path> modPath;
    };
}

#endif //VELOCITY_SETTINGS_HPP
