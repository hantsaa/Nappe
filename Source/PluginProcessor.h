// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 hantsaa
#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "NappeDSP.h"

class NappeProcessor : public juce::AudioProcessor
{
public:
    NappeProcessor();
    ~NappeProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using juce::AudioProcessor::processBlock;   // évite le masquage de la version double (avertissement GCC/Clang)

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Nappe"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 20.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // Tap tempo (thread UI) : écrit directement le paramètre "tempo" et recale la phase du trémolo
    void tap();

    juce::AudioProcessorValueTreeState apvts;
    std::atomic<float> lfoPhase { 0.f };   // lu par l'UI (LED TAP)
    std::atomic<float> hostBpm  { 0.f };   // 0 = tempo de l'hôte inconnu

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createLayout();

    nappe::Engine engine;
    std::atomic<float>* pReverb, *pCharacter, *pLength, *pAir, *pNoise, *pFilter,
                       *pTremolo, *pTempo, *pMix, *pActive, *pSync, *pDivision;
    std::atomic<bool> resetPhaseRequest { false };
    std::atomic<bool> jumpPending { true };   // premier bloc après préparation : pas de rampe
    double tapTimes[4] = {};
    int tapCount = 0;
    double lastTapMs = 0.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NappeProcessor)
};
