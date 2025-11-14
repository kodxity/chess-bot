#include "nnue.hpp"
#include <cmath>
#include <iostream>
#include "incbin.h"
extern "C" {
    INCBIN(networkWeights, NNUE_PATH);

}

// Embed the binary file



const Network* g_net = nullptr;

static Network* s_network_storage = nullptr;

void init_eval() {


    s_network_storage = new Network();
    s_network_storage->load();

    g_net = s_network_storage;
}



// ============================================================
// Utility
// ============================================================
int16_t CReLU(int16_t value, int16_t min, int16_t max)
{
    if (value <= min)
        return min;

    if (value >= max)
        return max;

    return value;
}
inline int32_t screlu(int16_t x) {
    int32_t y = CReLU(x,0,QA);
    // std::max<int32_t>(0, static_cast<int32_t>(x));
    return y*y;
}

// ============================================================
// Accumulator methods
// ============================================================
void Accumulator::clear() {
    std::fill(vals, vals + HIDDEN_SIZE, 0);
}



void Accumulator::add_feature(const Network* net, size_t feature_idx) {
    // std::cout<<"FEATURE: "<<feature_idx<<"\n";
    for (int i = 0; i < HIDDEN_SIZE; i++)
        vals[i] += net->feature_weights[feature_idx][i];
}

void Accumulator::remove_feature(const Network* net, size_t feature_idx) {
    for (int i = 0; i < HIDDEN_SIZE; i++)
        vals[i] -= net->feature_weights[feature_idx][i];
}

// ============================================================
// Core NNUE logic
// ============================================================




void Network::load() {
	char *ptr = (char *)gnetworkWeightsData;
	memcpy(feature_weights, ptr, sizeof(feature_weights));
	ptr += sizeof(feature_weights);
	memcpy(feature_bias, ptr, sizeof(feature_bias));
	ptr += sizeof(feature_bias);
	memcpy(output_weights, ptr, sizeof(output_weights));
	ptr += sizeof(output_weights);
	memcpy(&output_bias, ptr, sizeof(output_bias));
}



void init_accumulator(Accumulator& acc, const Network* net) {
    for (int i = 0; i < HIDDEN_SIZE; i++){
        acc.vals[i] = net->feature_bias[i];
        std::cout<<"FUCCK "<<acc.vals[i]<<"\n";
    }
        
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
    output *= SCALE;
    output /= (QA * QB);

    return output;
}
