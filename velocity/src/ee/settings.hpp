#ifndef VELOCITY_SETTINGS_HPP
#define VELOCITY_SETTINGS_HPP

#include <unordered_set>
#include "utils/common.hpp"

namespace spade
{
    /**
     * Represents VM settings
     */
    struct Settings {
        string VERSION = "1.0";
        string LANG_NAME = "spade";
        string VM_NAME = "velocity";
        string INFO_STRING = VERSION + " " + LANG_NAME + " " + VM_NAME;

        std::unordered_set<string> inbuilt_types = {
                "basic.bool",      // boolean type
                "basic.int",       // int type
                "basic.float",     // float type
                "basic.char",      // char type
                "basic.string",    // string type
                "basic.array",     // array type
        };

        string lib_path;
        vector<fs::path> mod_path;
    };
}    // namespace spade

#endif    //VELOCITY_SETTINGS_HPP
