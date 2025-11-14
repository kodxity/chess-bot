#include "nnue.hpp"
#include <cmath>
#include <iostream>

extern "C" {
#include "incbin.h"
}

// Embed the binary file
INCBIN(NetworkBin, NNUE_PATH);


const Network* g_net = nullptr;
void init_eval() {
    g_net = loadNetwork();
    if (!g_net) {
        std::cerr << "NNUE weights failed to load!\n";
        std::abort();
    }
    std::cout << "NNUE loaded successfully.\n";
}
// ============================================================
// Utility
// ============================================================
inline int32_t screlu(int16_t x) {
    return std::max<int32_t>(0, static_cast<int32_t>(x));
}

// ============================================================
// Accumulator methods
// ============================================================
void Accumulator::clear() {
    std::fill(vals, vals + HIDDEN_SIZE, 0);
}



void Accumulator::add_feature(const Network* net, size_t feature_idx) {
    const int16_t* src = net->feature_weights[feature_idx];
    for (int i = 0; i < HIDDEN_SIZE; i++)
        vals[i] += src[i];
}

void Accumulator::remove_feature(const Network* net, size_t feature_idx) {
    const int16_t* src = net->feature_weights[feature_idx];
    for (int i = 0; i < HIDDEN_SIZE; i++)
        vals[i] -= src[i];
}

// ============================================================
// Core NNUE logic
// ============================================================


const Network* loadNetwork() {
    assert(gNetworkBinSize == sizeof(Network) && "weights.bin size mismatch with Network struct");
    return reinterpret_cast<const Network*>(gNetworkBinData);
}

void init_accumulator(Accumulator& acc, const Network* net) {
    for (int i = 0; i < HIDDEN_SIZE; i++)
        acc.vals[i] = net->feature_bias[i];
}

int32_t evaluate(const Network* net, const Accumulator& us, const Accumulator& them) {
    int32_t output = 0;

    // Sum side-to-move
    for (int i = 0; i < HIDDEN_SIZE; i++)
        output += screlu(us.vals[i]) * static_cast<int32_t>(net->output_weights[i]);

    // Sum opponent
    for (int i = 0; i < HIDDEN_SIZE; i++)
        output += screlu(them.vals[i]) * static_cast<int32_t>(net->output_weights[HIDDEN_SIZE + i]);

    // Quantization reduction
    output /= QA;
    output += static_cast<int32_t>(net->output_bias);

    // Apply eval scaling and normalize
    output = output * SCALE / (QA * QB);

    return output;
}
