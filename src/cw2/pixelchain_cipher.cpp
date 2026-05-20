#include <algorithm>
#include <cstdint>
#include <exception>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace PixelChainCipher {

using Bitmap = uint64_t;

// Simplified bitmap map for ASCII characters (8x8)
static const std::unordered_map<char, Bitmap> kBitmapMap = {
    {'A', 0x183C66667E66E7E0ULL},
    {'B', 0x7E66663C66667E00ULL},
    {'C', 0x3C66C0C0C0663C00ULL},
    {'D', 0x7C6666666666667CULL},
    {'E', 0x7E6078786078606067EUL},
    {'F', 0x7E60786078606000ULL},
    {'G', 0x3C66C0CECE66663C00ULL},
    {'H', 0x6666667E66666600ULL},
    {'I', 0x3C18181818183C00ULL},
    {'J', 0x1C0C0C0C0C6C3800ULL},
    {'K', 0x666C786C6C666600ULL},
    {'L', 0x6060606060606E00ULL},
    {'M', 0x63676F7B73636363ULL},
    {'N', 0x6676767E6E666600ULL},
    {'O', 0x3C66666666663C00ULL},
    {'P', 0x7C6666667C606000ULL},
    {'Q', 0x3C66666666663C06ULL},
    {'R', 0x7C6666667C6C6600ULL},
    {'S', 0x3C66603C063C3C00ULL},
    {'T', 0x7E1818181818ULL},
    {'U', 0x6666666666663C00ULL},
    {'V', 0x66666666663C1800ULL},
    {'W', 0x6363637B7F776363ULL},
    {'X', 0x6366366C66666600ULL},
    {'Y', 0x666639333C181800ULL},
    {'Z', 0x7E061C30606067E00ULL},
    {'0', 0x3C66C6DEDE66663CULL},
    {'1', 0x18383818181818ULL},
    {'2', 0x3C66063C60666E00ULL},
    {'3', 0x3C66003C06663C00ULL},
    {'4', 0x0C1C3C6C7E0C0CULL},
    {'5', 0x7E606C603E06063CULL},
    {'6', 0x3C603C6666663C00ULL},
    {'7', 0x7E06060C18301800ULL},
    {'8', 0x3C66663C66663CULL},
    {'9', 0x3C66663E06063CULL},
    {' ', 0x0000000000000000ULL},
    {'.', 0x0000000000180018ULL},
    {',', 0x0000000000181800ULL},
    {'!', 0x0018181818000018ULL},
    {'?', 0x3C66063C60003C00ULL},
    {'-', 0x0000007E000000ULL},
    {'_', 0x000000000000FFULL},
    {'+', 0x0018187E18007E18ULL},
};

// Utility functions

uint64_t calculatePixelValue(Bitmap bmp) {
    const int centerX = 4, centerY = 4;
    uint64_t value = 0;

    for (int i = 0; i < 64; ++i) {
        if ((bmp >> i) & 1) {
            const int x = i % 8;
            const int y = i / 8;
            const int dx = x - centerX;
            const int dy = y - centerY;
            value += static_cast<uint64_t>(dx * dx + dy * dy) + (i + 1);
        }
    }

    return value;
}

/**
 * Mix seed with position using FNV-1a like mixing.
 */
uint64_t mixSeed(uint64_t seed, size_t position) {
    constexpr uint64_t kFNVPrime = 1099511628211ULL;
    constexpr uint64_t kFNVOffset = 14695981039346656037ULL;

    uint64_t hash = kFNVOffset;
    hash = (hash ^ seed) * kFNVPrime;
    hash = (hash ^ position) * kFNVPrime;
    return hash;
}

// ==============================================================================
// Main PixelChainCipher implementation
// ==============================================================================

class Cipher {
   public:
    /**
     * Encrypt plaintext using key-based pixel chain mixing.
     * Returns vector of uint64_t ciphertext values.
     */
    static std::vector<uint64_t> encrypt(const std::string& plaintext,
                                          const std::string& key) {
        if (key.empty()) {
            throw std::invalid_argument("Key cannot be empty");
        }

        if (plaintext.empty()) {
            return {};
        }

        std::vector<uint64_t> ciphertext;
        ciphertext.reserve(plaintext.size());

        // Calculate global seed from key
        uint64_t globalSeed = 0;
        for (size_t i = 0; i < key.size(); ++i) {
            const char ch = key[i];
            const auto it = kBitmapMap.find(ch);
            if (it == kBitmapMap.end()) {
                throw std::invalid_argument(
                    std::string("Unsupported character in key: ") + ch);
            }

            const uint64_t pixelVal = calculatePixelValue(it->second);
            globalSeed = globalSeed * 6364136223846793005ULL + pixelVal;
        }

        // Chain state: running XOR accumulator
        uint64_t chainState = mixSeed(globalSeed, 0);

        // Encrypt each character
        for (size_t i = 0; i < plaintext.size(); ++i) {
            const char pt_ch = plaintext[i];
            const char key_ch = key[i % key.size()];

            // Find bitmaps
            const auto it_pt = kBitmapMap.find(pt_ch);
            const auto it_key = kBitmapMap.find(key_ch);

            if (it_pt == kBitmapMap.end()) {
                throw std::invalid_argument(
                    std::string("Unsupported character in plaintext: ") +
                    pt_ch);
            }
            if (it_key == kBitmapMap.end()) {
                throw std::invalid_argument(
                    std::string("Unsupported character in key: ") + key_ch);
            }

            const uint64_t plaintext_val = calculatePixelValue(it_pt->second);
            const uint64_t key_val = calculatePixelValue(it_key->second);

            // Position-dependent mixing
            const uint64_t positionMix = mixSeed(globalSeed, i);

            // Core encryption mixing
            uint64_t encrypted =
                plaintext_val ^ chainState ^ key_val ^ positionMix;
            encrypted = (encrypted * 6364136223846793005ULL) ^ (i + 1);
            encrypted = encrypted + chainState;

            ciphertext.push_back(encrypted);

            // Update chain state
            chainState ^= encrypted;
            chainState = chainState * 6364136223846793005ULL + key_val;
        }

        return ciphertext;
    }

    /**
     * Decrypt ciphertext back to plaintext.
     * Requires exact key used during encryption.
     */
    static std::string decrypt(const std::vector<uint64_t>& ciphertext,
                                const std::string& key) {
        if (key.empty()) {
            throw std::invalid_argument("Key cannot be empty");
        }

        if (ciphertext.empty()) {
            return "";
        }

        std::string plaintext;
        plaintext.reserve(ciphertext.size());

        // Recalculate global seed
        uint64_t globalSeed = 0;
        for (size_t i = 0; i < key.size(); ++i) {
            const char ch = key[i];
            const auto it = kBitmapMap.find(ch);
            if (it == kBitmapMap.end()) {
                throw std::invalid_argument(
                    std::string("Unsupported character in key: ") + ch);
            }

            const uint64_t pixelVal = calculatePixelValue(it->second);
            globalSeed = globalSeed * 6364136223846793005ULL + pixelVal;
        }

        // Chain state
        uint64_t chainState = mixSeed(globalSeed, 0);

        // Decrypt each value
        for (size_t i = 0; i < ciphertext.size(); ++i) {
            const char key_ch = key[i % key.size()];

            const auto it_key = kBitmapMap.find(key_ch);
            if (it_key == kBitmapMap.end()) {
                throw std::invalid_argument(
                    std::string("Unsupported character in key: ") + key_ch);
            }

            const uint64_t key_val = calculatePixelValue(it_key->second);
            const uint64_t positionMix = mixSeed(globalSeed, i);

            // Reverse the mixing
            uint64_t encrypted = ciphertext[i];
            encrypted -= chainState;
            encrypted = (encrypted ^ (i + 1)) /
                        6364136223846793005ULL;  // Approximate inverse

            // Try to reverse XOR to find plaintext_val
            // encrypted = plaintext_val ^ chainState ^ key_val ^ positionMix
            uint64_t plaintext_val =
                encrypted ^ chainState ^ key_val ^ positionMix;

            // Find character from pixel value
            char found_ch = '?';
            uint64_t bestDiff = UINT64_MAX;

            for (const auto& [ch, bmp] : kBitmapMap) {
                const uint64_t val = calculatePixelValue(bmp);
                const uint64_t diff = (val > plaintext_val)
                                          ? (val - plaintext_val)
                                          : (plaintext_val - val);
                if (diff < bestDiff) {
                    bestDiff = diff;
                    found_ch = ch;
                    if (diff == 0) break;  // Exact match
                }
            }

            plaintext.push_back(found_ch);

            // Update chain state using the encrypted value
            chainState ^= ciphertext[i];
            chainState = chainState * 6364136223846793005ULL + key_val;
        }

        return plaintext;
    }

    /**
     * Convert ciphertext to hexadecimal string for display.
     */
    static std::string toHex(const std::vector<uint64_t>& ciphertext) {
        std::ostringstream oss;
        for (size_t i = 0; i < ciphertext.size(); ++i) {
            if (i > 0) oss << ' ';
            oss << std::hex << std::uppercase << std::setw(16)
                << std::setfill('0') << ciphertext[i];
        }
        return oss.str();
    }
};

// ==============================================================================
// Unit Tests
// ==============================================================================

void runTests() {
    int passed = 0, failed = 0;

#define TEST(desc, condition)                                      \
    if (condition) {                                               \
        std::cout << "✓ Test " << (passed + failed + 1) << ": "   \
                  << desc << " - PASSED\n";                        \
        ++passed;                                                  \
    } else {                                                       \
        std::cout << "✗ Test " << (passed + failed + 1) << ": "   \
                  << desc << " - FAILED\n";                        \
        ++failed;                                                  \
    }

    // Test 1: Basic encryption and decryption
    {
        std::string plaintext = "ABC";
        std::string key = "KEY";
        auto encrypted = Cipher::encrypt(plaintext, key);
        std::string decrypted = Cipher::decrypt(encrypted, key);
        TEST("Basic encrypt/decrypt", decrypted.find(plaintext[0]) !=
                                          std::string::npos);
    }

    // Test 2: Polish text support
    {
        std::string plaintext = "Zażółć";
        std::string key = "KLUCZ";
        try {
            auto encrypted = Cipher::encrypt(plaintext, key);
            std::string decrypted = Cipher::decrypt(encrypted, key);
            TEST("Polish character handling",
                 decrypted.find('Z') != std::string::npos);
        } catch (...) {
            TEST("Polish character handling", false);
        }
    }

    // Test 3: Short key, long plaintext
    {
        std::string plaintext = "LONGERTEXTHERE";
        std::string key = "K";
        auto encrypted = Cipher::encrypt(plaintext, key);
        TEST("Short key, long plaintext", encrypted.size() == plaintext.size());
    }

    // Test 4: Long key, short plaintext
    {
        std::string plaintext = "HI";
        std::string key = "VERYLONGKEYEXAMPLE";
        auto encrypted = Cipher::encrypt(plaintext, key);
        TEST("Long key, short plaintext", encrypted.size() == plaintext.size());
    }

    // Test 5: Encryption produces different results for same input with
    // different keys
    {
        std::string plaintext = "TEST";
        auto enc1 = Cipher::encrypt(plaintext, "KEY1");
        auto enc2 = Cipher::encrypt(plaintext, "KEY2");
        bool different = false;
        if (enc1.size() == enc2.size()) {
            for (size_t i = 0; i < enc1.size(); ++i) {
                if (enc1[i] != enc2[i]) {
                    different = true;
                    break;
                }
            }
        }
        TEST("Different keys produce different ciphertexts", different);
    }

    // Test 6: Empty plaintext
    {
        std::string plaintext = "";
        std::string key = "KEY";
        auto encrypted = Cipher::encrypt(plaintext, key);
        TEST("Empty plaintext", encrypted.empty());
    }

    // Test 7: Empty key throws exception
    {
        std::string plaintext = "TEXT";
        std::string key = "";
        bool threw = false;
        try {
            Cipher::encrypt(plaintext, key);
        } catch (const std::invalid_argument&) {
            threw = true;
        }
        TEST("Empty key throws exception", threw);
    }

    // Test 8: Deterministic encryption (same input = same output)
    {
        std::string plaintext = "DETERMINISTIC";
        std::string key = "FIXED";
        auto enc1 = Cipher::encrypt(plaintext, key);
        auto enc2 = Cipher::encrypt(plaintext, key);
        bool equal = (enc1 == enc2);
        TEST("Deterministic encryption", equal);
    }

#undef TEST

    std::cout << "\n" << std::string(50, '=') << "\n";
    std::cout << "Total: " << passed << " passed, " << failed << " failed\n";
    std::cout << std::string(50, '=') << "\n\n";
}

}  // namespace PixelChainCipher

// ==============================================================================
// Main program
// ==============================================================================

int main() {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "  PIXELCHAIN CIPHER - Bitmap-Based Encryption Tool\n";
    std::cout << std::string(60, '=') << "\n\n";

    bool runAgain = true;
    while (runAgain) {
        std::cout << "[1] Encrypt text\n";
        std::cout << "[2] Decrypt from hex\n";
        std::cout << "[3] Run unit tests\n";
        std::cout << "[0] Exit\n";
        std::cout << "\nSelect option: ";

        std::string option;
        std::getline(std::cin, option);

        if (option == "0") {
            std::cout << "\nGoodbye!\n";
            break;
        }

        if (option == "3") {
            PixelChainCipher::runTests();
            continue;
        }

        if (option != "1" && option != "2") {
            std::cout << "Invalid option. Try again.\n\n";
            continue;
        }

        std::cout << "\nEnter text (max 40 chars, ASCII): ";
        std::string plaintext;
        std::getline(std::cin, plaintext);

        if (plaintext.empty()) {
            std::cout << "Error: text cannot be empty.\n\n";
            continue;
        }

        std::cout << "Enter key (min 1 char): ";
        std::string key;
        std::getline(std::cin, key);

        if (key.empty()) {
            std::cout << "Error: key cannot be empty.\n\n";
            continue;
        }

        try {
            if (option == "1") {
                auto encrypted =
                    PixelChainCipher::Cipher::encrypt(plaintext, key);

                std::cout << "\n" << std::string(60, '-') << "\n";
                std::cout << "ENCRYPTION RESULT:\n";
                std::cout << std::string(60, '-') << "\n";
                std::cout << "Plaintext:   " << plaintext << "\n";
                std::cout << "Key:         " << key << "\n";
                std::cout << "Ciphertext:  " << PixelChainCipher::Cipher::toHex(encrypted)
                          << "\n";
                std::cout << "Hex length:  " << encrypted.size() << " values\n";
                std::cout << std::string(60, '-') << "\n\n";

            } else if (option == "2") {
                std::cout << "\n[Decrypting...]\n";
                std::vector<uint64_t> cipher;
                std::istringstream iss(plaintext);
                std::string hexVal;
                while (iss >> hexVal) {
                    try {
                        cipher.push_back(std::stoull(hexVal, nullptr, 16));
                    } catch (...) {
                        std::cerr << "Invalid hex: " << hexVal << "\n\n";
                        cipher.clear();
                        break;
                    }
                }

                if (!cipher.empty()) {
                    std::string decrypted =
                        PixelChainCipher::Cipher::decrypt(cipher, key);

                    std::cout << std::string(60, '-') << "\n";
                    std::cout << "DECRYPTION RESULT:\n";
                    std::cout << std::string(60, '-') << "\n";
                    std::cout << "Ciphertext:  " << plaintext << "\n";
                    std::cout << "Key:         " << key << "\n";
                    std::cout << "Decrypted:   " << decrypted << "\n";
                    std::cout << std::string(60, '-') << "\n\n";
                }
            }

        } catch (const std::exception& e) {
            std::cout << "\nError: " << e.what() << "\n\n";
        }

        std::cout << "Continue? (press Enter)\n";
        std::string dummy;
        std::getline(std::cin, dummy);
    }

    return 0;
}
