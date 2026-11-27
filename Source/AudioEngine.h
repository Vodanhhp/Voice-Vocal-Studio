#pragma once

#include <JuceHeader.h>
#include <atomic>

class AudioEngine final : public juce::AudioIODeviceCallback
{
public:
    AudioEngine();
    ~AudioEngine() override;

    bool start();
    void stop();
    juce::String getStatus() const;
    juce::String getDeviceName() const;

    void setInputGainDb(float v) noexcept { inputGainDb.store(v); }
    void setMicVolume(float v) noexcept { micVolume.store(v); }
    void setOutputVolume(float v) noexcept { outputVolume.store(v); }
    void setBassDb(float v) noexcept { bassDb.store(v); }
    void setMidDb(float v) noexcept { midDb.store(v); }
    void setTrebleDb(float v) noexcept { trebleDb.store(v); }
    void setThresholdDb(float v) noexcept { thresholdDb.store(v); }
    void setRatio(float v) noexcept { ratio.store(v); }
    void setDeEsser(float v) noexcept { deEsser.store(v); }
    void setNoiseGate(float v) noexcept { noiseGate.store(v); }
    void setLimiterDb(float v) noexcept { limiterDb.store(v); }

    void setMixerEnabled(bool v) noexcept { mixerEnabled.store(v); }
    void setEqEnabled(bool v) noexcept { eqEnabled.store(v); }
    void setCompressorEnabled(bool v) noexcept { compressorEnabled.store(v); }
    void setCleanupEnabled(bool v) noexcept { cleanupEnabled.store(v); }
    void setProcessingEnabled(bool v) noexcept { processingEnabled.store(v); }
    void setBypass(bool v) noexcept { bypass.store(v); }

    float getInputPeak() const noexcept { return inputPeak.load(); }
    float getOutputPeak() const noexcept { return outputPeak.load(); }

    void audioDeviceIOCallback(const float* const* inputChannelData,
                               int numInputChannels,
                               float* const* outputChannelData,
                               int numOutputChannels,
                               int numSamples) override;
    void audioDeviceAboutToStart(juce::AudioIODevice* device) override;
    void audioDeviceStopped() override;

private:
    struct ChannelState
    {
        float lowState = 0.0f;
        float midLowState = 0.0f;
        float midHighState = 0.0f;
        float highState = 0.0f;
        float compressorEnvelope = 0.0f;
        float deEsserEnvelope = 0.0f;
        float gateEnvelope = 0.0f;
    };

    float processSample(float input, ChannelState& state, double sampleRate,
                        float inputGain, float micGain, float outputGain,
                        float bass, float mid, float treble,
                        float threshold, float compRatio, float deEss,
                        float gate, float limiter) noexcept;

    juce::AudioDeviceManager deviceManager;
    juce::String status { "Chua khoi dong audio" };
    double currentSampleRate = 44100.0;
    ChannelState channelStates[2];

    std::atomic<float> inputGainDb { 0.0f }, micVolume { 75.0f }, outputVolume { 80.0f };
    std::atomic<float> bassDb { 2.0f }, midDb { 0.0f }, trebleDb { 1.0f };
    std::atomic<float> thresholdDb { -18.0f }, ratio { 3.0f };
    std::atomic<float> deEsser { 25.0f }, noiseGate { 15.0f }, limiterDb { -1.0f };
    std::atomic<bool> mixerEnabled { true }, eqEnabled { true };
    std::atomic<bool> compressorEnabled { true }, cleanupEnabled { true };
    std::atomic<bool> processingEnabled { false }, bypass { false };
    std::atomic<float> inputPeak { 0.0f }, outputPeak { 0.0f };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioEngine)
};
