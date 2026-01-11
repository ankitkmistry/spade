#pragma once

#include <cstdint>
#include <format>
#include <string>

namespace color
{
    class Color {
      public:
        uint8_t red = 0;
        uint8_t green = 0;
        uint8_t blue = 0;

      private:
        constexpr Color(const uint8_t red, const uint8_t green, const uint8_t blue) : red(red), green(green), blue(blue) {}

      public:
        constexpr Color() = default;

        friend constexpr Color from_rgb(uint8_t red, uint8_t green, uint8_t blue);

        friend constexpr Color from_hex(uint32_t hex_code);

        constexpr bool operator==(const Color &rhs) const {
            return red == rhs.red && green == rhs.green && blue == rhs.blue;
        }

        constexpr bool operator!=(const Color &rhs) const {
            return !(*this == rhs);
        }

        std::string to_string_hex() const {
            return std::format("#{:2x}{:2x}{:2x}", red, green, blue);
        }

        std::string to_string_rgb() const {
            return std::format("(r={:d}, g={:d}, b={:d})", red, green, blue);
        }

        std::string to_string() const {
            return to_string_hex();
        }
    };

    constexpr int RESET = 1;
    constexpr int BOLD = 2;
    constexpr int UNDERLINE = 4;
    constexpr int INVERSE = 8;

    class Style {
      public:
        Color bg_color;
        Color fg_color;
        int attributes = RESET;

        constexpr Style(Color bg_color, Color fg_color, int attributes = RESET) : bg_color(bg_color), fg_color(fg_color), attributes(attributes) {}

        std::string to_string() const;

        constexpr bool operator==(const Style &rhs) const {
            return bg_color == rhs.bg_color && fg_color == rhs.fg_color && attributes == rhs.attributes;
        }

        constexpr bool operator!=(const Style &rhs) const {
            return !(rhs == *this);
        }

        const static Style DEFAULT;
    };

    constexpr inline Color from_rgb(const uint8_t red, const uint8_t green, const uint8_t blue) {
        return Color{red, green, blue};
    }

    constexpr inline Color from_hex(const uint32_t hex_code) {
        return Color{static_cast<uint8_t>((hex_code >> 16) & 0xff), static_cast<uint8_t>((hex_code >> 8) & 0xff),
                     static_cast<uint8_t>(hex_code & 0xff)};
    }

    inline std::string fg(const Color color) {
        return std::format("\x1b[38;2;{:d};{:d};{:d}m", color.red, color.green, color.blue);
    }

    inline std::string bg(const Color color) {
        return std::format("\x1b[48;2;{:d};{:d};{:d}m", color.red, color.green, color.blue);
    }

    inline std::string attr(const int attributes) {
        std::string res;
        if (attributes & RESET)
            res += "\x1b[0m";
        if (attributes & BOLD)
            res += "\x1b[1m";
        if (attributes & UNDERLINE)
            res += "\x1b[4m";
        if (attributes & INVERSE)
            res += "\x1b[7m";
        return res;
    }

#define COLOR_ALICE_BLUE          (::color::from_hex(0xF0F8FF))
#define COLOR_ANTIQUE_WHITE       (::color::from_hex(0xFAEBD7))
#define COLOR_AQUA                (::color::from_hex(0x00FFFF))
#define COLOR_AQUAMARINE          (::color::from_hex(0x7FFFD4))
#define COLOR_AZURE               (::color::from_hex(0xF0FFFF))
#define COLOR_BEIGE               (::color::from_hex(0xF5F5DC))
#define COLOR_BISQUE              (::color::from_hex(0xFFE4C4))
#define COLOR_BLACK               (::color::from_hex(0x000000))
#define COLOR_BLANCHED_ALMOND     (::color::from_hex(0xFFEBCD))
#define COLOR_BLUE                (::color::from_hex(0x0000FF))
#define COLOR_BLUE_VIOLET         (::color::from_hex(0x8A2BE2))
#define COLOR_BROWN               (::color::from_hex(0xA52A2A))
#define COLOR_BURLYWOOD           (::color::from_hex(0xDEB887))
#define COLOR_CADET_BLUE          (::color::from_hex(0x5F9EA0))
#define COLOR_CHARTREUSE          (::color::from_hex(0x7FFF00))
#define COLOR_CHOCOLATE           (::color::from_hex(0xD2691E))
#define COLOR_CORAL               (::color::from_hex(0xFF7F50))
#define COLOR_CORNFLOWER_BLUE     (::color::from_hex(0x6495ED))
#define COLOR_CORNSILK            (::color::from_hex(0xFFF8DC))
#define COLOR_CRIMSON             (::color::from_hex(0xDC143C))
#define COLOR_CYAN                (::color::from_hex(0x00FFFF))
#define COLOR_DARK_BLUE           (::color::from_hex(0x00008B))
#define COLOR_DARK_CYAN           (::color::from_hex(0x008B8B))
#define COLOR_DARK_GOLDENROD      (::color::from_hex(0xB8860B))
#define COLOR_DARK_GRAY           (::color::from_hex(0xA9A9A9))
#define COLOR_DARK_GREEN          (::color::from_hex(0x006400))
#define COLOR_DARK_KHAKI          (::color::from_hex(0xBDB76B))
#define COLOR_DARK_MAGENTA        (::color::from_hex(0x8B008B))
#define COLOR_DARK_OLIVE_GREEN    (::color::from_hex(0x556B2F))
#define COLOR_DARK_ORANGE         (::color::from_hex(0xFF8C00))
#define COLOR_DARK_ORCHID         (::color::from_hex(0x9932CC))
#define COLOR_DARK_RED            (::color::from_hex(0x8B0000))
#define COLOR_DARK_SALMON         (::color::from_hex(0xE9967A))
#define COLOR_DARK_SEA_GREEN      (::color::from_hex(0x8FBC8F))
#define COLOR_DARK_SLATE_BLUE     (::color::from_hex(0x483D8B))
#define COLOR_DARK_SLATE_GRAY     (::color::from_hex(0x2F4F4F))
#define COLOR_DARK_TURQUOISE      (::color::from_hex(0x00CED1))
#define COLOR_DARK_VIOLET         (::color::from_hex(0x9400D3))
#define COLOR_DEEP_PINK           (::color::from_hex(0xFF1493))
#define COLOR_DEEP_SKY_BLUE       (::color::from_hex(0x00BFFF))
#define COLOR_DIM_GRAY            (::color::from_hex(0x696969))
#define COLOR_DODGER_BLUE         (::color::from_hex(0x1E90FF))
#define COLOR_FIREBRICK           (::color::from_hex(0xB22222))
#define COLOR_FLORAL_WHITE        (::color::from_hex(0xFFFAF0))
#define COLOR_FOREST_GREEN        (::color::from_hex(0x228B22))
#define COLOR_FUCHSIA             (::color::from_hex(0xFF00FF))
#define COLOR_GAINSBORO           (::color::from_hex(0xDCDCDC))
#define COLOR_GHOST_WHITE         (::color::from_hex(0xF8F8FF))
#define COLOR_GOLD                (::color::from_hex(0xFFD700))
#define COLOR_GOLDENROD           (::color::from_hex(0xDAA520))
#define COLOR_GRAY                (::color::from_hex(0xBEBEBE))
#define COLOR_WEB_GRAY            (::color::from_hex(0x808080))
#define COLOR_GREEN               (::color::from_hex(0x00FF00))
#define COLOR_WEB_GREEN           (::color::from_hex(0x008000))
#define COLOR_GREEN_YELLOW        (::color::from_hex(0xADFF2F))
#define COLOR_HONEYDEW            (::color::from_hex(0xF0FFF0))
#define COLOR_HOT_PINK            (::color::from_hex(0xFF69B4))
#define COLOR_INDIAN_RED          (::color::from_hex(0xCD5C5C))
#define COLOR_INDIGO              (::color::from_hex(0x4B0082))
#define COLOR_IVORY               (::color::from_hex(0xFFFFF0))
#define COLOR_KHAKI               (::color::from_hex(0xF0E68C))
#define COLOR_LAVENDER            (::color::from_hex(0xE6E6FA))
#define COLOR_LAVENDER_BLUSH      (::color::from_hex(0xFFF0F5))
#define COLOR_LAWN_GREEN          (::color::from_hex(0x7CFC00))
#define COLOR_LEMON_CHIFFON       (::color::from_hex(0xFFFACD))
#define COLOR_LIGHT_BLUE          (::color::from_hex(0xADD8E6))
#define COLOR_LIGHT_CORAL         (::color::from_hex(0xF08080))
#define COLOR_LIGHT_CYAN          (::color::from_hex(0xE0FFFF))
#define COLOR_LIGHT_GOLDENROD     (::color::from_hex(0xFAFAD2))
#define COLOR_LIGHT_GRAY          (::color::from_hex(0xD3D3D3))
#define COLOR_LIGHT_GREEN         (::color::from_hex(0x90EE90))
#define COLOR_LIGHT_PINK          (::color::from_hex(0xFFB6C1))
#define COLOR_LIGHT_SALMON        (::color::from_hex(0xFFA07A))
#define COLOR_LIGHT_SEA_GREEN     (::color::from_hex(0x20B2AA))
#define COLOR_LIGHT_SKY_BLUE      (::color::from_hex(0x87CEFA))
#define COLOR_LIGHT_SLATE_GRAY    (::color::from_hex(0x778899))
#define COLOR_LIGHT_STEEL_BLUE    (::color::from_hex(0xB0C4DE))
#define COLOR_LIGHT_YELLOW        (::color::from_hex(0xFFFFE0))
#define COLOR_LIME                (::color::from_hex(0x00FF00))
#define COLOR_LIME_GREEN          (::color::from_hex(0x32CD32))
#define COLOR_LINEN               (::color::from_hex(0xFAF0E6))
#define COLOR_MAGENTA             (::color::from_hex(0xFF00FF))
#define COLOR_MAROON              (::color::from_hex(0xB03060))
#define COLOR_WEB_MAROON          (::color::from_hex(0x800000))
#define COLOR_MEDIUM_AQUAMARINE   (::color::from_hex(0x66CDAA))
#define COLOR_MEDIUM_BLUE         (::color::from_hex(0x0000CD))
#define COLOR_MEDIUM_ORCHID       (::color::from_hex(0xBA55D3))
#define COLOR_MEDIUM_PURPLE       (::color::from_hex(0x9370DB))
#define COLOR_MEDIUM_SEA_GREEN    (::color::from_hex(0x3CB371))
#define COLOR_MEDIUM_SLATE_BLUE   (::color::from_hex(0x7B68EE))
#define COLOR_MEDIUM_SPRING_GREEN (::color::from_hex(0x00FA9A))
#define COLOR_MEDIUM_TURQUOISE    (::color::from_hex(0x48D1CC))
#define COLOR_MEDIUM_VIOLET_RED   (::color::from_hex(0xC71585))
#define COLOR_MIDNIGHT_BLUE       (::color::from_hex(0x191970))
#define COLOR_MINT_CREAM          (::color::from_hex(0xF5FFFA))
#define COLOR_MISTY_ROSE          (::color::from_hex(0xFFE4E1))
#define COLOR_MOCCASIN            (::color::from_hex(0xFFE4B5))
#define COLOR_NAVAJO_WHITE        (::color::from_hex(0xFFDEAD))
#define COLOR_NAVY_BLUE           (::color::from_hex(0x000080))
#define COLOR_OLD_LACE            (::color::from_hex(0xFDF5E6))
#define COLOR_OLIVE               (::color::from_hex(0x808000))
#define COLOR_OLIVE_DRAB          (::color::from_hex(0x6B8E23))
#define COLOR_ORANGE              (::color::from_hex(0xFFA500))
#define COLOR_ORANGE_RED          (::color::from_hex(0xFF4500))
#define COLOR_ORCHID              (::color::from_hex(0xDA70D6))
#define COLOR_PALE_GOLDENROD      (::color::from_hex(0xEEE8AA))
#define COLOR_PALE_GREEN          (::color::from_hex(0x98FB98))
#define COLOR_PALE_TURQUOISE      (::color::from_hex(0xAFEEEE))
#define COLOR_PALE_VIOLET_RED     (::color::from_hex(0xDB7093))
#define COLOR_PAPAYA_WHIP         (::color::from_hex(0xFFEFD5))
#define COLOR_PEACH_PUFF          (::color::from_hex(0xFFDAB9))
#define COLOR_PERU                (::color::from_hex(0xCD853F))
#define COLOR_PINK                (::color::from_hex(0xFFC0CB))
#define COLOR_PLUM                (::color::from_hex(0xDDA0DD))
#define COLOR_POWDER_BLUE         (::color::from_hex(0xB0E0E6))
#define COLOR_PURPLE              (::color::from_hex(0xA020F0))
#define COLOR_WEB_PURPLE          (::color::from_hex(0x800080))
#define COLOR_REBECCA_PURPLE      (::color::from_hex(0x663399))
#define COLOR_RED                 (::color::from_hex(0xFF0000))
#define COLOR_ROSY_BROWN          (::color::from_hex(0xBC8F8F))
#define COLOR_ROYAL_BLUE          (::color::from_hex(0x4169E1))
#define COLOR_SADDLE_BROWN        (::color::from_hex(0x8B4513))
#define COLOR_SALMON              (::color::from_hex(0xFA8072))
#define COLOR_SANDY_BROWN         (::color::from_hex(0xF4A460))
#define COLOR_SEA_GREEN           (::color::from_hex(0x2E8B57))
#define COLOR_SEASHELL            (::color::from_hex(0xFFF5EE))
#define COLOR_SIENNA              (::color::from_hex(0xA0522D))
#define COLOR_SILVER              (::color::from_hex(0xC0C0C0))
#define COLOR_SKY_BLUE            (::color::from_hex(0x87CEEB))
#define COLOR_SLATE_BLUE          (::color::from_hex(0x6A5ACD))
#define COLOR_SLATE_GRAY          (::color::from_hex(0x708090))
#define COLOR_SNOW                (::color::from_hex(0xFFFAFA))
#define COLOR_SPRING_GREEN        (::color::from_hex(0x00FF7F))
#define COLOR_STEEL_BLUE          (::color::from_hex(0x4682B4))
#define COLOR_TAN                 (::color::from_hex(0xD2B48C))
#define COLOR_TEAL                (::color::from_hex(0x008080))
#define COLOR_THISTLE             (::color::from_hex(0xD8BFD8))
#define COLOR_TOMATO              (::color::from_hex(0xFF6347))
#define COLOR_TURQUOISE           (::color::from_hex(0x40E0D0))
#define COLOR_VIOLET              (::color::from_hex(0xEE82EE))
#define COLOR_WHEAT               (::color::from_hex(0xF5DEB3))
#define COLOR_WHITE               (::color::from_hex(0xFFFFFF))
#define COLOR_WHITE_SMOKE         (::color::from_hex(0xF5F5F5))
#define COLOR_YELLOW              (::color::from_hex(0xFFFF00))
#define COLOR_YELLOW_GREEN        (::color::from_hex(0x9ACD32))

    inline constexpr Style Style::DEFAULT = Style(COLOR_BLACK, COLOR_WHITE);
}    // namespace color
