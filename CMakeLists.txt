cmake_minimum_required(VERSION 3.22)

# Runtime MSVC statique : le .vst3 tourne sans installer les redistribuables Visual C++
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

project(Nappe VERSION 0.1.0)

include(FetchContent)
FetchContent_Declare(JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.6
    GIT_SHALLOW ON)
FetchContent_MakeAvailable(JUCE)

set(NAPPE_FORMATS VST3 Standalone)

juce_add_plugin(Nappe
    COMPANY_NAME "Hantsa"
    PLUGIN_MANUFACTURER_CODE Hnts
    PLUGIN_CODE Nap1
    FORMATS ${NAPPE_FORMATS}
    PRODUCT_NAME "Nappe"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    VST3_CATEGORIES Fx Reverb)

target_sources(Nappe PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp)

target_compile_features(Nappe PRIVATE cxx_std_17)
target_compile_definitions(Nappe PUBLIC
    JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(Nappe
    PRIVATE juce::juce_audio_utils
    PUBLIC juce::juce_recommended_config_flags juce::juce_recommended_warning_flags)

# --- Tests du moteur (sans JUCE), exécutés par CTest sur Windows comme sur Linux ---
enable_testing()
add_executable(nappe_dsp_tests test/test_dsp.cpp)
target_compile_features(nappe_dsp_tests PRIVATE cxx_std_17)
add_test(NAME nappe_dsp COMMAND nappe_dsp_tests)
