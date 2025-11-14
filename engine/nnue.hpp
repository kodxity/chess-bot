#pragma once
#include <cstdint>
#include <algorithm>
#include <cassert>
// ============================================================
// Constants
// ============================================================
constexpr int HIDDEN_SIZE = 128;
constexpr int SCALE = 400;
constexpr int16_t QA = 255;
constexpr int16_t QB = 64;
#ifndef NNUE_PATH
#define NNUE_PATH "quantised.bin"
#endif
// ============================================================
// Forward declarations
// ============================================================

struct Network;
struct Accumulator;
extern "C" {
    extern const unsigned char gNetworkBinData[];
    extern const unsigned int gNetworkBinSize;
}

extern const Network* g_net;     // declared only


// ============================================================
// Accumulator
// ============================================================
struct alignas(64) Accumulator {
    int16_t vals[HIDDEN_SIZE];

    void clear();
    void add_feature(const Network* net, size_t feature_idx);
    void remove_feature(const Network* net, size_t feature_idx);
    
};

// ============================================================
// Network layout (matches .bin format)
// ============================================================
struct alignas(64) Network {
    // 768 × HIDDEN_SIZE
    int16_t feature_weights[768][HIDDEN_SIZE];
    int16_t feature_bias[HIDDEN_SIZE];
    int16_t output_weights[2 * HIDDEN_SIZE];
    int16_t output_bias;
};

// ============================================================
// Core functions
// ============================================================

// Load the embedded binary weights into a Network
const Network* loadNetwork();

void init_eval();

// Initialize accumulator from network bias
void init_accumulator(Accumulator& acc, const Network* net);

// Evaluate the NNUE given both sides' accumulators
int32_t evaluate(const Network* net, const Accumulator& us, const Accumulator& them);
