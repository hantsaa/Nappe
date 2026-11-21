// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 hantsaa
#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

// Taille de conception ; l'éditeur se redimensionne en gardant ce rapport.
namespace ui { constexpr int kW = 640, kH = 520; }

// Grand bouton rectangulaire (ENGAGE, TAP) avec témoin d'état
struct NappePad : public juce::Component
{
    std::function<void()> onDown, onUp;
    std::function<bool()> lit;
    juce::String label, sub;
    void paint (juce::Graphics&) override;
    void mouseDown (const juce::MouseEvent&) override;
    void mouseUp (const juce::MouseEvent&) override;
    bool down = false;
};

// Sélecteur à deux segments lié à un paramètre booléen (false = gauche, true = droite)
struct NappeSegment : public juce::Button
{
    NappeSegment (const juce::String& left, const juce::String& right)
        : juce::Button (left + right), leftText (left), rightText (right) { setClickingTogglesState (true); }
    void paintButton (juce::Graphics&, bool over, bool down) override;
    juce::String leftText, rightText;
};

class NappeEditor : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    explicit NappeEditor (NappeProcessor&);
    ~NappeEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void mouseEnter (const juce::MouseEvent& e) override     { showReadoutFor (e); }
    void mouseDown (const juce::MouseEvent& e) override      { showReadoutFor (e); }
    void mouseDrag (const juce::MouseEvent& e) override      { showReadoutFor (e); }
    void mouseWheelMove (const juce::MouseEvent& e, const juce::MouseWheelDetails&) override { showReadoutFor (e); }

private:
    struct Knob
    {
        juce::Slider slider { juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::NoTextBox };
        juce::String name;
        int cx = 0, cy = 0;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> att;
    };

    struct KnobLook : public juce::LookAndFeel_V4
    {
        void drawRotarySlider (juce::Graphics&, int x, int y, int w, int h, float pos, float a0, float a1, juce::Slider&) override;
    };

    void timerCallback() override;
    void addKnob (Knob&, const juce::String& paramId, const juce::String& name, int cx, int cy);
    void attachRateKnob (bool sync);
    void showReadoutFor (const juce::MouseEvent&);
    juce::Rectangle<int> S (int x, int y, int w, int h) const;
    juce::Rectangle<int> lcdArea() const { return S (420, 22, 190, 36); }

    KnobLook look;
    NappeProcessor& proc;
    Knob level, size, decay, bright, noise, depth, rate, lowcut;
    std::vector<Knob*> knobs;
    NappeSegment wetMix { "WET", "MIX" }, holdToggle { "TOGGLE", "HOLD" }, freeSync { "FREE", "SYNC" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> mixAtt, momAtt, syncAtt;
    NappePad engagePad, tapPad;

    Knob* readoutKnob = nullptr;   // bouton dont l'afficheur suit la valeur
    float scale = 1.f, ox = 0.f, oy = 0.f;
    bool rateIsSync = false, lastEngaged = false, lastTapLit = false, lastHold = false;
    juce::String lcdTitle, lcdValue;
    juce::uint32 lcdUntil = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NappeEditor)
};
