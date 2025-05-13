#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cmath>
#include <format>
#include <cstring>
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

        friend Color from_hsb(int hue, int saturation, int brightness);

        constexpr bool operator==(const Color &rhs) const {
            return red == rhs.red && green == rhs.green && blue == rhs.blue;
        }

        constexpr bool operator!=(const Color &rhs) const {
            return !(*this == rhs);
        }

        std::string to_string_hex() const {
            return std::format("#{:2x}{:2x}{:2x}", red, green, blue);
        }

        std::string to_string_hsb() const {
            // Refer to
            // https://stackoverflow.com/questions/6614792/fast-optimized-and-accurate-rgb-hsb-conversion-code-in-c#answer-72451462
            double r = red;
            double g = green;
            double b = blue;
            double K = 0.f;

            if (g < b) {
                std::swap(g, b);
                K = -1.f;
            }

            if (r < g) {
                std::swap(r, g);
                K = -2.f / 6.f - K;
            }

            double chroma = r - std::min(g, b);
            double h = std::abs(K + (g - b) / (6.f * chroma + 1e-20f));
            double s = chroma / (r + 1e-20f);
            double v = r;
            return std::format("(h={:f}, s={:f}, b={:f})", h, s, v);
        }

        std::string to_string_rgb() const {
            return std::format("(r={:d}, g={:d}, b={:d})", red, green, blue);
        }

        std::string to_string() const {
            return to_string_hex();
        }

        constexpr Color inverse() const {
            return {static_cast<uint8_t>(255 - red), static_cast<uint8_t>(255 - green), static_cast<uint8_t>(255 - blue)};
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

        constexpr Style(Color bg_color, Color fg_color, int attributes = RESET)
            : bg_color(bg_color), fg_color(fg_color), attributes(attributes) {}

        constexpr Style reverse() const {
            return {fg_color, bg_color, attributes};
        }

        constexpr Style inverse() const {
            return {bg_color.inverse(), fg_color.inverse()};
        }

        std::string to_string() const;

        constexpr bool operator==(const Style &rhs) const {
            return bg_color == rhs.bg_color && fg_color == rhs.fg_color && attributes == rhs.attributes;
        }

        constexpr bool operator!=(const Style &rhs) const {
            return !(rhs == *this);
        }

        const static Style DEFAULT;
    };

    class Console {
      public:
        /**
         * @return true if the terminal is open, false otherwise
         */
        static bool is_terminal_open();

        /**
         * Initializes the console
         */
        static void init();

        /**
         * Restores old console configurations
         */
        static void restore();

        /**
         * @return size of the console (height, width)
         */
        static std::pair<size_t, size_t> size();

        /**
         * Clears the console
         */
        static void clear();

        /**
         * Sets the style of the console
         * @param style the specified style
         */
        static void style(const Style &style);

        /**
         * Moves the cursor of the console to position (x,y) of the console
         * @param x x coord
         * @param y y coord
         */
        static void gotoxy(size_t x, size_t y);

        /**
         * Sets the cell of the console at position (x,y)
         * @param x x coord
         * @param y y coord
         * @param value value to set
         * @param style style of the cell
         */
        static void set_cell(size_t x, size_t y, wchar_t value, Style style);
    };

    constexpr inline Color from_rgb(uint8_t red, uint8_t green, uint8_t blue) {
        return Color{red, green, blue};
    }

    constexpr inline Color from_hex(uint32_t hex_code) {
        return Color{static_cast<uint8_t>((hex_code >> 16) & 0xff), static_cast<uint8_t>((hex_code >> 8) & 0xff),
                     static_cast<uint8_t>(hex_code & 0xff)};
    }

    inline Color from_hsb(int hue, int saturation, int brightness) {
        // Refer to https://en.wikipedia.org/wiki/HSL_and_HSV#HSV_to_RGB_alternative
        int h = hue % 360;
        double s = saturation / 100.;
        double b = brightness / 100.;

        const auto f = [=](const int n) {
            const double k = fmod(n + h / 60, 6);
            return b - b * s * std::max(0., std::min({k, 4 - k, 1.}));
        };

        return Color{static_cast<uint8_t>(f(5)), static_cast<uint8_t>(f(3)), static_cast<uint8_t>(f(1))};
    }

    inline std::string fg(Color color) {
        return std::format("\x1b[38;2;{:d};{:d};{:d}m", color.red, color.green, color.blue);
    }

    inline std::string bg(Color color) {
        return std::format("\x1b[48;2;{:d};{:d};{:d}m", color.red, color.green, color.blue);
    }

    inline std::string attr(int attributes) {
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

    inline constexpr Color Alice_Blue = color::from_hex(0xF0F8FF);
    inline constexpr Color Antique_White = color::from_hex(0xFAEBD7);
    inline constexpr Color Aqua = color::from_hex(0x00FFFF);
    inline constexpr Color Aquamarine = color::from_hex(0x7FFFD4);
    inline constexpr Color Azure = color::from_hex(0xF0FFFF);
    inline constexpr Color Beige = color::from_hex(0xF5F5DC);
    inline constexpr Color Bisque = color::from_hex(0xFFE4C4);
    inline constexpr Color Black = color::from_hex(0x000000);
    inline constexpr Color Blanched_Almond = color::from_hex(0xFFEBCD);
    inline constexpr Color Blue = color::from_hex(0x0000FF);
    inline constexpr Color Blue_Violet = color::from_hex(0x8A2BE2);
    inline constexpr Color Brown = color::from_hex(0xA52A2A);
    inline constexpr Color Burlywood = color::from_hex(0xDEB887);
    inline constexpr Color Cadet_Blue = color::from_hex(0x5F9EA0);
    inline constexpr Color Chartreuse = color::from_hex(0x7FFF00);
    inline constexpr Color Chocolate = color::from_hex(0xD2691E);
    inline constexpr Color Coral = color::from_hex(0xFF7F50);
    inline constexpr Color Cornflower_Blue = color::from_hex(0x6495ED);
    inline constexpr Color Cornsilk = color::from_hex(0xFFF8DC);
    inline constexpr Color Crimson = color::from_hex(0xDC143C);
    inline constexpr Color Cyan = color::from_hex(0x00FFFF);
    inline constexpr Color Dark_Blue = color::from_hex(0x00008B);
    inline constexpr Color Dark_Cyan = color::from_hex(0x008B8B);
    inline constexpr Color Dark_Goldenrod = color::from_hex(0xB8860B);
    inline constexpr Color Dark_Gray = color::from_hex(0xA9A9A9);
    inline constexpr Color Dark_Green = color::from_hex(0x006400);
    inline constexpr Color Dark_Khaki = color::from_hex(0xBDB76B);
    inline constexpr Color Dark_Magenta = color::from_hex(0x8B008B);
    inline constexpr Color Dark_Olive_Green = color::from_hex(0x556B2F);
    inline constexpr Color Dark_Orange = color::from_hex(0xFF8C00);
    inline constexpr Color Dark_Orchid = color::from_hex(0x9932CC);
    inline constexpr Color Dark_Red = color::from_hex(0x8B0000);
    inline constexpr Color Dark_Salmon = color::from_hex(0xE9967A);
    inline constexpr Color Dark_Sea_Green = color::from_hex(0x8FBC8F);
    inline constexpr Color Dark_Slate_Blue = color::from_hex(0x483D8B);
    inline constexpr Color Dark_Slate_Gray = color::from_hex(0x2F4F4F);
    inline constexpr Color Dark_Turquoise = color::from_hex(0x00CED1);
    inline constexpr Color Dark_Violet = color::from_hex(0x9400D3);
    inline constexpr Color Deep_Pink = color::from_hex(0xFF1493);
    inline constexpr Color Deep_Sky_Blue = color::from_hex(0x00BFFF);
    inline constexpr Color Dim_Gray = color::from_hex(0x696969);
    inline constexpr Color Dodger_Blue = color::from_hex(0x1E90FF);
    inline constexpr Color Firebrick = color::from_hex(0xB22222);
    inline constexpr Color Floral_White = color::from_hex(0xFFFAF0);
    inline constexpr Color Forest_Green = color::from_hex(0x228B22);
    inline constexpr Color Fuchsia = color::from_hex(0xFF00FF);
    inline constexpr Color Gainsboro = color::from_hex(0xDCDCDC);
    inline constexpr Color Ghost_White = color::from_hex(0xF8F8FF);
    inline constexpr Color Gold = color::from_hex(0xFFD700);
    inline constexpr Color Goldenrod = color::from_hex(0xDAA520);
    inline constexpr Color Gray = color::from_hex(0xBEBEBE);
    inline constexpr Color Web_Gray = color::from_hex(0x808080);
    inline constexpr Color Green = color::from_hex(0x00FF00);
    inline constexpr Color Web_Green = color::from_hex(0x008000);
    inline constexpr Color Green_Yellow = color::from_hex(0xADFF2F);
    inline constexpr Color Honeydew = color::from_hex(0xF0FFF0);
    inline constexpr Color Hot_Pink = color::from_hex(0xFF69B4);
    inline constexpr Color Indian_Red = color::from_hex(0xCD5C5C);
    inline constexpr Color Indigo = color::from_hex(0x4B0082);
    inline constexpr Color Ivory = color::from_hex(0xFFFFF0);
    inline constexpr Color Khaki = color::from_hex(0xF0E68C);
    inline constexpr Color Lavender = color::from_hex(0xE6E6FA);
    inline constexpr Color Lavender_Blush = color::from_hex(0xFFF0F5);
    inline constexpr Color Lawn_Green = color::from_hex(0x7CFC00);
    inline constexpr Color Lemon_Chiffon = color::from_hex(0xFFFACD);
    inline constexpr Color Light_Blue = color::from_hex(0xADD8E6);
    inline constexpr Color Light_Coral = color::from_hex(0xF08080);
    inline constexpr Color Light_Cyan = color::from_hex(0xE0FFFF);
    inline constexpr Color Light_Goldenrod = color::from_hex(0xFAFAD2);
    inline constexpr Color Light_Gray = color::from_hex(0xD3D3D3);
    inline constexpr Color Light_Green = color::from_hex(0x90EE90);
    inline constexpr Color Light_Pink = color::from_hex(0xFFB6C1);
    inline constexpr Color Light_Salmon = color::from_hex(0xFFA07A);
    inline constexpr Color Light_Sea_Green = color::from_hex(0x20B2AA);
    inline constexpr Color Light_Sky_Blue = color::from_hex(0x87CEFA);
    inline constexpr Color Light_Slate_Gray = color::from_hex(0x778899);
    inline constexpr Color Light_Steel_Blue = color::from_hex(0xB0C4DE);
    inline constexpr Color Light_Yellow = color::from_hex(0xFFFFE0);
    inline constexpr Color Lime = color::from_hex(0x00FF00);
    inline constexpr Color Lime_Green = color::from_hex(0x32CD32);
    inline constexpr Color Linen = color::from_hex(0xFAF0E6);
    inline constexpr Color Magenta = color::from_hex(0xFF00FF);
    inline constexpr Color Maroon = color::from_hex(0xB03060);
    inline constexpr Color Web_Maroon = color::from_hex(0x800000);
    inline constexpr Color Medium_Aquamarine = color::from_hex(0x66CDAA);
    inline constexpr Color Medium_Blue = color::from_hex(0x0000CD);
    inline constexpr Color Medium_Orchid = color::from_hex(0xBA55D3);
    inline constexpr Color Medium_Purple = color::from_hex(0x9370DB);
    inline constexpr Color Medium_Sea_Green = color::from_hex(0x3CB371);
    inline constexpr Color Medium_Slate_Blue = color::from_hex(0x7B68EE);
    inline constexpr Color Medium_Spring_Green = color::from_hex(0x00FA9A);
    inline constexpr Color Medium_Turquoise = color::from_hex(0x48D1CC);
    inline constexpr Color Medium_Violet_Red = color::from_hex(0xC71585);
    inline constexpr Color Midnight_Blue = color::from_hex(0x191970);
    inline constexpr Color Mint_Cream = color::from_hex(0xF5FFFA);
    inline constexpr Color Misty_Rose = color::from_hex(0xFFE4E1);
    inline constexpr Color Moccasin = color::from_hex(0xFFE4B5);
    inline constexpr Color Navajo_White = color::from_hex(0xFFDEAD);
    inline constexpr Color Navy_Blue = color::from_hex(0x000080);
    inline constexpr Color Old_Lace = color::from_hex(0xFDF5E6);
    inline constexpr Color Olive = color::from_hex(0x808000);
    inline constexpr Color Olive_Drab = color::from_hex(0x6B8E23);
    inline constexpr Color Orange = color::from_hex(0xFFA500);
    inline constexpr Color Orange_Red = color::from_hex(0xFF4500);
    inline constexpr Color Orchid = color::from_hex(0xDA70D6);
    inline constexpr Color Pale_Goldenrod = color::from_hex(0xEEE8AA);
    inline constexpr Color Pale_Green = color::from_hex(0x98FB98);
    inline constexpr Color Pale_Turquoise = color::from_hex(0xAFEEEE);
    inline constexpr Color Pale_Violet_Red = color::from_hex(0xDB7093);
    inline constexpr Color Papaya_Whip = color::from_hex(0xFFEFD5);
    inline constexpr Color Peach_Puff = color::from_hex(0xFFDAB9);
    inline constexpr Color Peru = color::from_hex(0xCD853F);
    inline constexpr Color Pink = color::from_hex(0xFFC0CB);
    inline constexpr Color Plum = color::from_hex(0xDDA0DD);
    inline constexpr Color Powder_Blue = color::from_hex(0xB0E0E6);
    inline constexpr Color Purple = color::from_hex(0xA020F0);
    inline constexpr Color Web_Purple = color::from_hex(0x800080);
    inline constexpr Color Rebecca_Purple = color::from_hex(0x663399);
    inline constexpr Color Red = color::from_hex(0xFF0000);
    inline constexpr Color Rosy_Brown = color::from_hex(0xBC8F8F);
    inline constexpr Color Royal_Blue = color::from_hex(0x4169E1);
    inline constexpr Color Saddle_Brown = color::from_hex(0x8B4513);
    inline constexpr Color Salmon = color::from_hex(0xFA8072);
    inline constexpr Color Sandy_Brown = color::from_hex(0xF4A460);
    inline constexpr Color Sea_Green = color::from_hex(0x2E8B57);
    inline constexpr Color Seashell = color::from_hex(0xFFF5EE);
    inline constexpr Color Sienna = color::from_hex(0xA0522D);
    inline constexpr Color Silver = color::from_hex(0xC0C0C0);
    inline constexpr Color Sky_Blue = color::from_hex(0x87CEEB);
    inline constexpr Color Slate_Blue = color::from_hex(0x6A5ACD);
    inline constexpr Color Slate_Gray = color::from_hex(0x708090);
    inline constexpr Color Snow = color::from_hex(0xFFFAFA);
    inline constexpr Color Spring_Green = color::from_hex(0x00FF7F);
    inline constexpr Color Steel_Blue = color::from_hex(0x4682B4);
    inline constexpr Color Tan = color::from_hex(0xD2B48C);
    inline constexpr Color Teal = color::from_hex(0x008080);
    inline constexpr Color Thistle = color::from_hex(0xD8BFD8);
    inline constexpr Color Tomato = color::from_hex(0xFF6347);
    inline constexpr Color Turquoise = color::from_hex(0x40E0D0);
    inline constexpr Color Violet = color::from_hex(0xEE82EE);
    inline constexpr Color Wheat = color::from_hex(0xF5DEB3);
    inline constexpr Color White = color::from_hex(0xFFFFFF);
    inline constexpr Color White_Smoke = color::from_hex(0xF5F5F5);
    inline constexpr Color Yellow = color::from_hex(0xFFFF00);
    inline constexpr Color Yellow_Green = color::from_hex(0x9ACD32);

    inline constexpr Style Style::DEFAULT = Style(color::Black, color::White);
}    // namespace color