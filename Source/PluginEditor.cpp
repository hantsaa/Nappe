// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 hantsaa
#include "PluginEditor.h"

using namespace juce;

namespace
{
const Colour kBg     (0xff15181d);
const Colour kPanel  (0xff1d2129);
const Colour kText   (0xffe6eeec);
const Colour kDim    (0xff7d8c8a);
const Colour kAccent (0xff3cc4b0);
const Colour kTrack  (0xff2b313a);
const Colour kFace   (0xff262c35);

Font bold (float h)  { return Font (FontOptions (h, Font::bold)); }
Font plain (float h) { return Font (FontOptions (h)); }
Font mono (float h, bool b = false) { return Font (FontOptions (Font::getDefaultMonospacedFontName(), h, b ? Font::bold : Font::plain)); }

// Positions dans le repère de conception 640 x 520 : grille 4 x 2, sélecteurs, deux grands boutons
constexpr int kColX[4] = { 110, 250, 390, 530 };
constexpr int kRow1 = 172, kRow2 = 300;
constexpr int kSwY = 372;
constexpr int kPadY = 404, kPadH = 70;
}

//==============================================================================
void NappeEditor::KnobLook::drawRotarySlider (Graphics& g, int x, int y, int w, int h, float pos,
                                              float a0, float a1, Slider& s)
{
    const auto b = Rectangle<float> ((float) x, (float) y, (float) w, (float) h).reduced (4.f);
    const float R = jmin (b.getWidth(), b.getHeight()) * 0.5f;
    const auto c = b.getCentre();
    const bool hot = s.isMouseOverOrDragging() && s.isEnabled();
    const float ang = a0 + pos * (a1 - a0);
    const float ringR = R - 5.f;

    Path track; track.addCentredArc (c.x, c.y, ringR, ringR, 0.f, a0, a1, true);
    g.setColour (kTrack);
    g.strokePath (track, PathStrokeType (4.f, PathStrokeType::curved, PathStrokeType::rounded));
    if (pos > 0.001f)
    {
        Path val; val.addCentredArc (c.x, c.y, ringR, ringR, 0.f, a0, ang, true);
        g.setColour (hot ? kAccent.brighter (0.2f) : kAccent);
        g.strokePath (val, PathStrokeType (4.f, PathStrokeType::curved, PathStrokeType::rounded));
    }

    const float faceR = ringR - 9.f;
    g.setColour (hot ? kFace.brighter (0.06f) : kFace);
    g.fillEllipse (c.x - faceR, c.y - faceR, 2.f * faceR, 2.f * faceR);

    const float dr = faceR * 0.62f;
    g.setColour (kText);
    g.fillEllipse (c.x + dr * std::sin (ang) - 3.f, c.y - dr * std::cos (ang) - 3.f, 6.f, 6.f);
}

//==============================================================================
void NappePad::paint (Graphics& g)
{
    auto b = getLocalBounds().toFloat().reduced (1.f);
    const float a = isEnabled() ? 1.f : 0.4f;
    const bool on = lit && lit();

    g.setColour ((down ? kPanel.brighter (0.05f) : kPanel).withMultipliedAlpha (a));
    g.fillRoundedRectangle (b, 14.f);
    g.setColour ((on ? kAccent : kTrack).withMultipliedAlpha (a));
    g.drawRoundedRectangle (b, 14.f, 2.f);
    if (on) { g.setColour (kAccent.withAlpha (0.10f * a)); g.fillRoundedRectangle (b, 14.f); }

    auto top = b.removeFromTop (b.getHeight() * 0.58f);
    g.setColour (kText.withMultipliedAlpha (a));
    g.setFont (bold (15.f));
    g.drawText (label, top, Justification::centred);
    g.setColour (kDim.withMultipliedAlpha (a));
    g.setFont (plain (10.5f));
    g.drawText (sub, b, Justification::centred);
}

void NappePad::mouseDown (const MouseEvent&) { if (!isEnabled()) return; down = true; if (onDown) onDown(); repaint(); }
void NappePad::mouseUp (const MouseEvent&)   { if (!down) return;        down = false; if (onUp) onUp(); repaint(); }

//==============================================================================
void NappeSegment::paintButton (Graphics& g, bool over, bool)
{
    auto b = getLocalBounds().toFloat().reduced (1.f);
    const float a = isEnabled() ? 1.f : 0.4f;
    const bool on = getToggleState();
    const float r = b.getHeight() * 0.5f;

    g.setColour (kPanel.withMultipliedAlpha (a));
    g.fillRoundedRectangle (b, r);

    auto half = b.withWidth (b.getWidth() * 0.5f);
    if (on) half = half.withX (b.getCentreX());
    g.setColour (kAccent.withMultipliedAlpha (a));
    g.fillRoundedRectangle (half.reduced (2.f), r - 2.f);

    if (over) { g.setColour (kText.withAlpha (0.06f)); g.fillRoundedRectangle (b, r); }

    g.setFont (bold (11.f));
    g.setColour ((on ? kDim : kBg).withMultipliedAlpha (a));
    g.drawText (leftText, b.withWidth (b.getWidth() * 0.5f), Justification::centred);
    g.setColour ((on ? kBg : kDim).withMultipliedAlpha (a));
    g.drawText (rightText, b.withTrimmedLeft (b.getWidth() * 0.5f), Justification::centred);
}

//==============================================================================
NappeEditor::NappeEditor (NappeProcessor& p) : AudioProcessorEditor (&p), proc (p)
{
    setLookAndFeel (&look);

    addKnob (level,  "reverb",    "LEVEL",   kColX[0], kRow1);
    addKnob (size,   "character", "SIZE",    kColX[1], kRow1);
    addKnob (decay,  "length",    "DECAY",   kColX[2], kRow1);
    addKnob (bright, "air",       "BRIGHT",  kColX[3], kRow1);
    addKnob (noise,  "noise",     "NOISE",   kColX[0], kRow2);
    addKnob (depth,  "tremolo",   "DEPTH",   kColX[1], kRow2);
    addKnob (rate,   "tempo",     "RATE",    kColX[2], kRow2);
    addKnob (lowcut, "filter",    "LOW CUT", kColX[3], kRow2);

    for (auto* sw : { &wetMix, &holdToggle, &freeSync }) addAndMakeVisible (*sw);
    mixAtt  = std::make_unique<AudioProcessorValueTreeState::ButtonAttachment> (proc.apvts, "mix",  wetMix);
    momAtt  = std::make_unique<AudioProcessorValueTreeState::ButtonAttachment> (proc.apvts, "mom",  holdToggle);
    syncAtt = std::make_unique<AudioProcessorValueTreeState::ButtonAttachment> (proc.apvts, "sync", freeSync);

    auto* active = proc.apvts.getParameter ("active");
    auto isHold = [this] { return proc.apvts.getRawParameterValue ("mom")->load() > 0.5f; };
    auto isOn   = [this] { return proc.apvts.getRawParameterValue ("active")->load() > 0.5f; };

    // Passer en mode HOLD coupe l'effet : il ne doit agir que pendant l'appui
    holdToggle.onClick = [this, active]
    {
        if (holdToggle.getToggleState())
        {
            active->beginChangeGesture();
            active->setValueNotifyingHost (0.f);
            active->endChangeGesture();
        }
    };

    engagePad.label = "ENGAGE";
    engagePad.lit = isOn;
    engagePad.onDown = [active, isHold, isOn]
    {
        active->beginChangeGesture();
        active->setValueNotifyingHost (isHold() ? 1.f : (isOn() ? 0.f : 1.f));
    };
    engagePad.onUp = [active, isHold]
    {
        if (isHold()) active->setValueNotifyingHost (0.f);
        active->endChangeGesture();
    };
    addAndMakeVisible (engagePad);

    tapPad.label = "TAP";
    tapPad.sub = "tap tempo";
    tapPad.lit = [this] { return proc.lfoPhase.load() < 0.5f; };
    tapPad.onDown = [this] { proc.tap(); };
    addAndMakeVisible (tapPad);

    attachRateKnob (proc.apvts.getRawParameterValue ("sync")->load() > 0.5f);

    setResizeLimits (400, 325, 1600, 1300);
    getConstrainer()->setFixedAspectRatio ((double) ui::kW / (double) ui::kH);
    setResizable (true, true);
    setSize (ui::kW, ui::kH);
    startTimerHz (30);
}

NappeEditor::~NappeEditor() { stopTimer(); setLookAndFeel (nullptr); }

void NappeEditor::addKnob (Knob& k, const String& id, const String& name, int cx, int cy)
{
    k.name = name; k.cx = cx; k.cy = cy;
    addAndMakeVisible (k.slider);
    k.slider.setRotaryParameters (degreesToRadians (225.f), degreesToRadians (495.f), true);
    k.slider.setMouseDragSensitivity (220);
    k.slider.setPopupDisplayEnabled (false, false, nullptr);
    k.slider.addMouseListener (this, false);   // alimente l'afficheur
    knobs.push_back (&k);
    if (id != "tempo")
        k.att = std::make_unique<AudioProcessorValueTreeState::SliderAttachment> (proc.apvts, id, k.slider);
}

// RATE règle la vitesse en Hz (FREE) ou la division du tempo de l'hôte (SYNC)
void NappeEditor::attachRateKnob (bool sync)
{
    rateIsSync = sync;
    rate.att.reset();
    rate.att = std::make_unique<AudioProcessorValueTreeState::SliderAttachment> (proc.apvts, sync ? "division" : "tempo", rate.slider);
    rate.name = sync ? "DIVISION" : "RATE";
    tapPad.setEnabled (! sync);
    tapPad.sub = sync ? "host sync" : "tap tempo";
    repaint();
}

void NappeEditor::showReadoutFor (const MouseEvent& e)
{
    for (auto* k : knobs)
        if (e.eventComponent == &k->slider)
        {
            lcdTitle = k->name;
            readoutKnob = k;
            lcdValue = k->slider.getTextFromValue (k->slider.getValue());
            lcdUntil = Time::getMillisecondCounter() + 1800;
            repaint (lcdArea());
            return;
        }
}

void NappeEditor::timerCallback()
{
    const bool engaged = proc.apvts.getRawParameterValue ("active")->load() > 0.5f;
    if (engaged != lastEngaged) { lastEngaged = engaged; repaint (S (30, 404, 300, 70)); }

    const bool tapLit = proc.lfoPhase.load() < 0.5f;
    if (tapLit != lastTapLit) { lastTapLit = tapLit; repaint (S (330, 404, 280, 70)); }

    const bool hold = proc.apvts.getRawParameterValue ("mom")->load() > 0.5f;
    if (hold != lastHold)
    {
        lastHold = hold;
        engagePad.sub = hold ? "hold to play" : "press to toggle";
        repaint (S (30, 404, 300, 70));
    }

    const bool sync = proc.apvts.getRawParameterValue ("sync")->load() > 0.5f;
    if (sync != rateIsSync) attachRateKnob (sync);

    // L'afficheur suit la valeur pendant 1,8 s (y compris après un double-clic qui remet la valeur par défaut)
    if (readoutKnob != nullptr && lcdUntil != 0)
    {
        lcdValue = readoutKnob->slider.getTextFromValue (readoutKnob->slider.getValue());
        repaint (lcdArea());
    }
    if (lcdUntil != 0 && Time::getMillisecondCounter() > lcdUntil) { lcdUntil = 0; readoutKnob = nullptr; repaint (lcdArea()); }
    if (sync && lcdUntil == 0) repaint (lcdArea());   // affiche le tempo de l'hôte
}

Rectangle<int> NappeEditor::S (int x, int y, int w, int h) const
{
    return { roundToInt (ox + (float) x * scale), roundToInt (oy + (float) y * scale),
             roundToInt ((float) w * scale), roundToInt ((float) h * scale) };
}

void NappeEditor::resized()
{
    scale = jmin ((float) getWidth() / (float) ui::kW, (float) getHeight() / (float) ui::kH);
    ox = ((float) getWidth()  - (float) ui::kW * scale) * 0.5f;
    oy = ((float) getHeight() - (float) ui::kH * scale) * 0.5f;

    for (auto* k : knobs) k->slider.setBounds (S (k->cx - 48, k->cy - 48, 96, 96));

    wetMix.setBounds    (S (kColX[0] - 85, kSwY, 170, 26));
    holdToggle.setBounds (S (kColX[1] + 5, kSwY, 170, 26));
    freeSync.setBounds  (S (kColX[3] - 85, kSwY, 170, 26));
    engagePad.setBounds (S (30, kPadY, 300, kPadH));
    tapPad.setBounds    (S (340, kPadY, 270, kPadH));
}

//==============================================================================
void NappeEditor::paint (Graphics& g)
{
    g.fillAll (kBg);
    g.addTransform (AffineTransform::scale (scale).translated (ox, oy));
    const Rectangle<float> full (0.f, 0.f, (float) ui::kW, (float) ui::kH);

    g.setColour (kPanel);
    g.fillRoundedRectangle (full.reduced (12.f), 18.f);

    // en-tête
    g.setColour (kText);
    g.setFont (bold (24.f));
    g.drawText ("NAPPE", Rectangle<float> (34.f, 26.f, 240.f, 30.f), Justification::centredLeft);
    g.setColour (kDim);
    g.setFont (plain (11.f));
    g.drawText ("reverb / tremolo", Rectangle<float> (35.f, 54.f, 240.f, 16.f), Justification::centredLeft);

    // afficheur
    const auto lcd = lcdArea().toFloat();
    g.setColour (kBg); g.fillRoundedRectangle (lcd, 8.f);
    g.setColour (kTrack); g.drawRoundedRectangle (lcd, 8.f, 1.2f);
    String title, value;
    if (lcdUntil != 0)                                  { title = lcdTitle; value = lcdValue; }
    else if (rateIsSync && proc.hostBpm.load() > 0.f)   { title = "HOST"; value = String (proc.hostBpm.load(), 1) + " BPM"; }
    else if (rateIsSync)                                { title = "NO HOST TEMPO"; value = "120 BPM"; }
    else                                                { title = "READY"; }
    g.setColour (kAccent.withAlpha (0.7f)); g.setFont (mono (11.f));
    g.drawText (title, lcd.reduced (12.f, 0.f), Justification::centredLeft);
    g.setColour (kAccent); g.setFont (mono (14.f, true));
    g.drawText (value, lcd.reduced (12.f, 0.f), Justification::centredRight);

    // libellés des commandes
    g.setColour (kText.withAlpha (0.9f)); g.setFont (bold (12.f));
    for (auto* k : knobs)
    {
        const float ly = (float) k->cy - 74.f;
        g.drawText (k->name, Rectangle<float> ((float) k->cx - 70.f, ly, 140.f, 16.f), Justification::centred);
    }

    // séparateur et titres des sélecteurs
    g.setColour (kTrack); g.drawLine (40.f, 352.f, 600.f, 352.f, 1.f);
    g.setColour (kDim); g.setFont (bold (9.5f));
    g.drawText ("OUTPUT",      Rectangle<float> ((float) kColX[0] - 85.f, 354.f, 170.f, 14.f), Justification::centred);
    g.drawText ("ACTIVATION",  Rectangle<float> ((float) kColX[1] + 5.f,  354.f, 170.f, 14.f), Justification::centred);
    g.drawText ("RATE SOURCE", Rectangle<float> ((float) kColX[3] - 85.f, 354.f, 170.f, 14.f), Justification::centred);

    // pied de page
    g.setColour (kDim.withAlpha (0.6f)); g.setFont (plain (8.5f));
    g.drawText ("Nappe  -  open source  -  licence GPL-3.0", Rectangle<float> (0.f, 486.f, (float) ui::kW, 12.f), Justification::centred);
}
