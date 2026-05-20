#include <array>
#include <cctype>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace {

constexpr std::size_t kMaxPasswordLen = 40;
constexpr int kGlyphW = 5;
constexpr int kGlyphH = 7;
constexpr int kGlyphSpacingX = 1;
constexpr int kGlyphSpacingY = 1;
constexpr int kPadX = 2;
constexpr int kPadY = 2;

using Glyph = std::array<const char*, kGlyphH>;

const std::unordered_map<char, Glyph> kFont = {
    {'A', {".###.", "#...#", "#...#", "#####", "#...#", "#...#", "#...#"}},
    {'B', {"####.", "#...#", "#...#", "####.", "#...#", "#...#", "####."}},
    {'C', {".###.", "#...#", "#....", "#....", "#....", "#...#", ".###."}},
    {'D', {"####.", "#...#", "#...#", "#...#", "#...#", "#...#", "####."}},
    {'E', {"#####", "#....", "#....", "####.", "#....", "#....", "#####"}},
    {'F', {"#####", "#....", "#....", "####.", "#....", "#....", "#...."}},
    {'G', {".###.", "#...#", "#....", "#.###", "#...#", "#...#", ".###."}},
    {'H', {"#...#", "#...#", "#...#", "#####", "#...#", "#...#", "#...#"}},
    {'I', {"#####", "..#..", "..#..", "..#..", "..#..", "..#..", "#####"}},
    {'J', {"#####", "...#.", "...#.", "...#.", "...#.", "#..#.", ".##.."}},
    {'K', {"#...#", "#..#.", "#.#..", "##...", "#.#..", "#..#.", "#...#"}},
    {'L', {"#....", "#....", "#....", "#....", "#....", "#....", "#####"}},
    {'M', {"#...#", "##.##", "#.#.#", "#.#.#", "#...#", "#...#", "#...#"}},
    {'N', {"#...#", "##..#", "#.#.#", "#..##", "#...#", "#...#", "#...#"}},
    {'O', {".###.", "#...#", "#...#", "#...#", "#...#", "#...#", ".###."}},
    {'P', {"####.", "#...#", "#...#", "####.", "#....", "#....", "#...."}},
    {'Q', {".###.", "#...#", "#...#", "#...#", "#.#.#", "#..#.", ".##.#"}},
    {'R', {"####.", "#...#", "#...#", "####.", "#.#..", "#..#.", "#...#"}},
    {'S', {".####", "#....", "#....", ".###.", "....#", "....#", "####."}},
    {'T', {"#####", "..#..", "..#..", "..#..", "..#..", "..#..", "..#.."}},
    {'U', {"#...#", "#...#", "#...#", "#...#", "#...#", "#...#", ".###."}},
    {'V', {"#...#", "#...#", "#...#", "#...#", "#...#", ".#.#.", "..#.."}},
    {'W', {"#...#", "#...#", "#...#", "#.#.#", "#.#.#", "##.##", "#...#"}},
    {'X', {"#...#", "#...#", ".#.#.", "..#..", ".#.#.", "#...#", "#...#"}},
    {'Y', {"#...#", "#...#", ".#.#.", "..#..", "..#..", "..#..", "..#.."}},
    {'Z', {"#####", "....#", "...#.", "..#..", ".#...", "#....", "#####"}},

    {'0', {".###.", "#...#", "#..##", "#.#.#", "##..#", "#...#", ".###."}},
    {'1', {"..#..", ".##..", "..#..", "..#..", "..#..", "..#..", ".###."}},
    {'2', {".###.", "#...#", "....#", "...#.", "..#..", ".#...", "#####"}},
    {'3', {"####.", "....#", "....#", ".###.", "....#", "....#", "####."}},
    {'4', {"...#.", "..##.", ".#.#.", "#..#.", "#####", "...#.", "...#."}},
    {'5', {"#####", "#....", "#....", "####.", "....#", "....#", "####."}},
    {'6', {".###.", "#....", "#....", "####.", "#...#", "#...#", ".###."}},
    {'7', {"#####", "....#", "...#.", "..#..", ".#...", ".#...", ".#..."}},
    {'8', {".###.", "#...#", "#...#", ".###.", "#...#", "#...#", ".###."}},
    {'9', {".###.", "#...#", "#...#", ".####", "....#", "....#", ".###."}},

    {' ', {".....", ".....", ".....", ".....", ".....", ".....", "....."}},
    {'.', {".....", ".....", ".....", ".....", ".....", "..#..", "..#.."}},
    {',', {".....", ".....", ".....", ".....", ".....", "..#..", ".#..."}},
    {':', {".....", "..#..", "..#..", ".....", "..#..", "..#..", "....."}},
    {';', {".....", "..#..", "..#..", ".....", "..#..", ".#...", "....."}},
    {'-', {".....", ".....", ".....", ".###.", ".....", ".....", "....."}},
    {'_', {".....", ".....", ".....", ".....", ".....", ".....", "#####"}},
    {'+', {".....", "..#..", "..#..", ".###.", "..#..", "..#..", "....."}},
    {'=', {".....", ".###.", ".....", ".###.", ".....", ".....", "....."}},
    {'/', {"....#", "...#.", "..#..", ".#...", "#....", ".....", "....."}},
    {'\\', {"#....", ".#...", "..#..", "...#.", "....#", ".....", "....."}},
    {'!', {"..#..", "..#..", "..#..", "..#..", "..#..", ".....", "..#.."}},
    {'?', {".###.", "#...#", "....#", "...#.", "..#..", ".....", "..#.."}},
    {'#', {".#.#.", "#####", ".#.#.", ".#.#.", "#####", ".#.#.", ".#.#."}},
    {'@', {".###.", "#...#", "#.###", "#.#.#", "#.###", "#....", ".###."}},
    {'$', {"..#..", ".####", "#.#..", ".###.", "..#.#", "####.", "..#.."}},
    {'*', {".....", "#.#.#", ".###.", "#####", ".###.", "#.#.#", "....."}},
    {'(', {"...#.", "..#..", ".#...", ".#...", ".#...", "..#..", "...#."}},
    {')', {".#...", "..#..", "...#.", "...#.", "...#.", "..#..", ".#..."}},
    {'[', {".###.", ".#...", ".#...", ".#...", ".#...", ".#...", ".###."}},
    {']', {".###.", "...#.", "...#.", "...#.", "...#.", "...#.", ".###."}},
    {'{', {"...#.", "..#..", "..#..", ".#...", "..#..", "..#..", "...#."}},
    {'}', {".#...", "..#..", "..#..", "...#.", "..#..", "..#..", ".#..."}},
    {'\'', {"..#..", "..#..", "..#..", ".....", ".....", ".....", "....."}},
    {'"', {".#.#.", ".#.#.", ".#.#.", ".....", ".....", ".....", "....."}},
    {'<', {"...#.", "..#..", ".#...", "#....", ".#...", "..#..", "...#."}},
    {'>', {".#...", "..#..", "...#.", "....#", "...#.", "..#..", ".#..."}},
    {'|', {"..#..", "..#..", "..#..", "..#..", "..#..", "..#..", "..#.."}},
};

const Glyph kFallback = {"#####", "#...#", "...#.", "..#..", ".....", "..#..", "....."};

bool IsAllowedChar(unsigned char ch) {
    if (std::isalnum(ch) != 0) {
        return true;
    }

    static const std::string kAllowed = " .,:;-_+=/\\!?@#$*()[]{}'\"<>|";
    return kAllowed.find(static_cast<char>(ch)) != std::string::npos;
}

std::string NormalizeForRender(std::string text) {
    for (char& c : text) {
        const unsigned char uc = static_cast<unsigned char>(c);
        if (std::islower(uc) != 0) {
            c = static_cast<char>(std::toupper(uc));
        }
    }
    return text;
}

std::vector<uint8_t> Encode(const std::string& plain) {
    std::vector<uint8_t> out;
    out.reserve(plain.size());

    for (std::size_t i = 0; i < plain.size(); ++i) {
        const uint8_t key = static_cast<uint8_t>(0x5A ^ ((i * 37U + 13U) & 0xFFU));
        out.push_back(static_cast<uint8_t>(plain[i]) ^ key);
    }

    return out;
}

std::string Decode(const std::vector<uint8_t>& encoded) {
    std::string out;
    out.reserve(encoded.size());

    for (std::size_t i = 0; i < encoded.size(); ++i) {
        const uint8_t key = static_cast<uint8_t>(0x5A ^ ((i * 37U + 13U) & 0xFFU));
        out.push_back(static_cast<char>(encoded[i] ^ key));
    }

    return out;
}

std::string ToHex(const std::vector<uint8_t>& bytes) {
    std::ostringstream oss;
    for (std::size_t i = 0; i < bytes.size(); ++i) {
        if (i > 0) {
            oss << ' ';
        }
        oss << std::hex << std::uppercase << std::setw(2) << std::setfill('0')
            << static_cast<int>(bytes[i]);
    }
    return oss.str();
}

const Glyph& FindGlyph(char c) {
    const auto it = kFont.find(c);
    if (it != kFont.end()) {
        return it->second;
    }
    return kFallback;
}

void DrawGlyph(std::vector<std::string>& px, int x, int y, char c) {
    const Glyph& g = FindGlyph(c);
    for (int row = 0; row < kGlyphH; ++row) {
        for (int col = 0; col < kGlyphW; ++col) {
            if (g[row][col] == '#') {
                px[y + row][x + col] = '#';
            }
        }
    }
}

std::vector<std::string> RenderTerminalToPixels(const std::vector<std::string>& lines) {
    std::size_t maxLen = 0;
    for (const std::string& line : lines) {
        if (line.size() > maxLen) {
            maxLen = line.size();
        }
    }

    const int width = static_cast<int>(2 * kPadX + maxLen * (kGlyphW + kGlyphSpacingX));
    const int height = static_cast<int>(2 * kPadY + lines.size() * (kGlyphH + kGlyphSpacingY));

    std::vector<std::string> pixels(height, std::string(width, '.'));

    for (std::size_t i = 0; i < lines.size(); ++i) {
        const int y = kPadY + static_cast<int>(i) * (kGlyphH + kGlyphSpacingY);
        for (std::size_t j = 0; j < lines[i].size(); ++j) {
            const int x = kPadX + static_cast<int>(j) * (kGlyphW + kGlyphSpacingX);
            DrawGlyph(pixels, x, y, lines[i][j]);
        }
    }

    return pixels;
}

void PrintFramedPixels(const std::vector<std::string>& px) {
    if (px.empty()) {
        return;
    }

    const std::size_t w = px[0].size();
    std::cout << '+' << std::string(w, '-') << "+\n";
    for (const std::string& row : px) {
        std::cout << '|' << row << "|\n";
    }
    std::cout << '+' << std::string(w, '-') << "+\n";
}

}  // namespace

int main() {
    std::cout << "Podaj haslo/slowo (max 40, ASCII podstawowe):\n> ";

    std::string input;
    std::getline(std::cin, input);

    if (input.empty()) {
        std::cerr << "Blad: pusty napis.\n";
        return 1;
    }

    if (input.size() > kMaxPasswordLen) {
        std::cerr << "Blad: przekroczono limit 40 znakow.\n";
        return 1;
    }

    for (unsigned char ch : input) {
        if (!IsAllowedChar(ch)) {
            std::cerr << "Blad: wykryto niedozwolony znak (tylko ASCII podstawowe).\n";
            return 1;
        }
    }

    const std::vector<uint8_t> encoded = Encode(input);
    const std::string decoded = Decode(encoded);

    const std::string hexData = ToHex(encoded);

    std::cout << "\nZakodowane bajty (hex):\n" << hexData << "\n";

    std::vector<std::string> terminalLines;
    terminalLines.push_back(NormalizeForRender(decoded));

    const std::vector<std::string> pixelMatrix = RenderTerminalToPixels(terminalLines);

    std::cout << "\nWirtualny ekran (tekst):\n";
    for (const std::string& line : terminalLines) {
        std::cout << line << "\n";
    }

    std::cout << "\nMacierz pikseli (# = zapalony, . = zgaszony):\n";
    std::cout << "Rozmiar: " << pixelMatrix[0].size() << " x " << pixelMatrix.size() << " px\n";
    PrintFramedPixels(pixelMatrix);

    return 0;
}
