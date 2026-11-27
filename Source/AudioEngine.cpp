#include "AudioEngine.h"
#include <cmath>

namespace
{
    constexpr float pi = 3.14159265358979323846f;
    float dbToGain(float db) noexcept { return std::pow(10.0f, db / 20.0f); }
    float clamp01(float v) noexcept { return juce::jlimit(0.0f, 1.0f, v); }
}

AudioEngine::AudioEngine() = default;

AudioEngine::~AudioEngine()
{
    stop();
}

bool AudioEngine::start()
{
    if (deviceManager.getCurrentAudioDevice() != nullptr)
        return true;

    auto error = deviceManager.initialise(1, 2, nullptr, true);
    if (error.isNotEmpty())
    {
        status = "Loi mo thiet bi: " + error;
        return false;
    }

    deviceManager.addAudioCallback(this);
    status = "Audio dang chay";
    return true;
}

void AudioEngine::stop()
{
    deviceManager.removeAudioCallback(this);
    deviceManager.closeAudioDevice();
    status = "Audio da dung";
}

juce::String AudioEngine::getStatus() const
{
    return status;
}

juce::String AudioEngine::getDeviceName() const
{
    auto* device = deviceManager.getCurrentAudioDevice();
    return device != nullptr ? device->getName() : "Chua co thiet bi";
}

void AudioEngine::audioDeviceAboutToStart(juce::AudioIODevice* device)
{
    currentSampleRate = device != nullptr ? device->getCurrentSampleRate() : 44100.0;
    channelStates[0] = {};
    channelStates[1] = {};
    status = "Audio dang chay - " + juce::String((int) currentSampleRate) + " Hz";
}

void AudioEngine::audioDeviceStopped()
{
    status = "Audio da dung";
}

float AudioEngine::processSample(float x, ChannelState& s, double sampleRate,
                                 float inputGain, float micGain, float outputGain,
                                 float bass, float mid, float treble,
                                 float threshold, float compRatio, float deEss,
                                 float gate, float limiter) noexcept
{
    x *= inputGain * micGain;

    // Three broad tone controls: low shelf, mid bell approximation, high shelf.
    const float lowAlpha = 1.0f - std::exp(-2.0f * pi * 180.0f / (float) sampleRate);
    const float midAlpha = 1.0f - std::exp(-2.0f * pi * 1100.0f / (float) sampleRate);
    const float highAlpha = 1.0f - std::exp(-2.0f * pi * 4800.0f / (float) sampleRate);
    s.lowState += lowAlpha * (x - s.lowState);
    s.midLowState += midAlpha * (x - s.midLowState);
    s.midHighState += highAlpha * (x - s.midHighState);
    s.highState += highAlpha * (x - s.highState);

    const float lowBand = s.lowState;
    const float midBand = s.midLowState - s.midHighState;
    const float highBand = x - s.highState;
    const float body = x - lowBand - midBand - highBand;
    x = body + lowBand * dbToGain(bass) + midBand * dbToGain(mid) +
        highBand * dbToGain(treble);

    // Gentle compressor with smoothed peak envelope.
    const float attack = std::exp(-1.0f / ((float) sampleRate * 0.010f));
    const float release = std::exp(-1.0f / ((float) sampleRate * 0.120f));
    const float magnitude = std::abs(x);
    const float coeff = magnitude > s.compressorEnvelope ? attack : release;
    s.compressorEnvelope = coeff * s.compressorEnvelope + (1.0f - coeff) * magnitude;
    const float levelDb = 20.0f * std::log10(juce::jmax(s.compressorEnvelope, 0.00001f));
    if (levelDb > threshold)
    {
        const float compressedDb = threshold + (levelDb - threshold) / juce::jmax(1.0f, compRatio);
        x *= dbToGain(compressedDb - levelDb);
    }

    // High-frequency de-esser: attenuate the sibilant band when its envelope rises.
    const float sibilant = x - s.highState;
    const float deCoeff = std::exp(-1.0f / ((float) sampleRate * 0.004f));
    s.deEsserEnvelope = deCoeff * s.deEsserEnvelope + (1.0f - deCoeff) * std::abs(sibilant);
    const float deAmount = clamp01(deEss / 100.0f);
    const float deReduction = clamp01((s.deEsserEnvelope - 0.025f) * 7.0f) * deAmount * 0.65f;
    x -= sibilant * deReduction;

    // Soft expander/noise gate. This is not spectral noise removal.
    const float gateCoeff = std::exp(-1.0f / ((float) sampleRate * 0.025f));
    s.gateEnvelope = gateCoeff * s.gateEnvelope + (1.0f - gateCoeff) * std::abs(x);
    const float gateAmount = clamp01(gate / 100.0f);
    const float gateFloor = 0.0015f + gateAmount * 0.018f;
    const float gateGain = s.gateEnvelope < gateFloor
        ? juce::jmap(clamp01(s.gateEnvelope / gateFloor), 0.12f, 1.0f)
        : 1.0f;
    x *= gateGain;

    // Soft saturation adds a little density without hard clipping.
    x = std::tanh(x * 1.08f) / std::tanh(1.08f);

    // Output ceiling.
    const float ceiling = dbToGain(limiter);
    x = ceiling * std::tanh((x * outputGain) / juce::jmax(ceiling, 0.001f));
    return x;
}

void AudioEngine::audioDeviceIOCallback(const float* const* inputs, int numInputs,
                                        float* const* outputs, int numOutputs,
                                        int numSamples)
{
    const bool active = processingEnabled.load() && !bypass.load();
    const bool mixOn = mixerEnabled.load();
    const bool eqOn = eqEnabled.load();
    const bool compOn = compressorEnabled.load();
    const bool cleanOn = cleanupEnabled.load();

    const float inGain = dbToGain(inputGainDb.load());
    const float micGain = juce::jlimit(0.0f, 1.5f, micVolume.load() / 100.0f);
    const float outGain = juce::jlimit(0.0f, 1.5f, outputVolume.load() / 100.0f);
    const float bass = eqOn ? bassDb.load() : 0.0f;
    const float mid = eqOn ? midDb.load() : 0.0f;
    const float treble = eqOn ? trebleDb.load() : 0.0f;
    const float threshold = compOn ? thresholdDb.load() : 0.0f;
    const float compRatio = compOn ? ratio.load() : 1.0f;
    const float deEss = cleanOn ? deEsser.load() : 0.0f;
    const float gate = cleanOn ? noiseGate.load() : 0.0f;
    const float limiter = cleanOn ? limiterDb.load() : 0.0f;

    float inPeak = 0.0f, outPeak = 0.0f;
    for (int ch = 0; ch < numOutputs; ++ch)
    {
        auto* out = outputs[ch];
        if (out == nullptr) continue;
        const int inputChannel = numInputs > 0 ? juce::jmin(ch, numInputs - 1) : -1;
        const float* in = inputChannel >= 0 ? inputs[inputChannel] : nullptr;

        for (int i = 0; i < numSamples; ++i)
        {
            const float dry = in != nullptr ? in[i] : 0.0f;
            inPeak = juce::jmax(inPeak, std::abs(dry));
            float y = dry;
            if (active)
            {
                if (mixOn)
                    y = processSample(dry, channelStates[juce::jmin(ch, 1)], currentSampleRate,
                                      inGain, micGain, outGain, bass, mid, treble,
                                      threshold, compRatio, deEss, gate, limiter);
                else
                    y = dry * outGain;
            }
            out[i] = y;
            outPeak = juce::jmax(outPeak, std::abs(y));
        }
    }
    inputPeak.store(inPeak);
    outputPeak.store(outPeak);
}
