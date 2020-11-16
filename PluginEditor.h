/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "CpuMeter.h"
#include <JuceHeader.h>
#include "PluginProcessor.h"


//==============================================================================
/**
*/
class CascadetutoPluginAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    CascadetutoPluginAudioProcessorEditor (CascadetutoPluginAudioProcessor&);
    ~CascadetutoPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;


private:
	CpuMeter mcpum;
	Image mBackgroundImage;

	//----------------------------------------------------------------------
	ComboBox cbox1;
	ComboBox cbox2;
	ComboBox cbox3;
	ComboBox cbox4;

	TextButton cb1mute;
	TextButton cb1solo;
	TextButton cb1bypass;

	// Do i declare the good class ?
	//AudioParameterChoice mApChoice;
	//----------------------------------------------------------------------

    CascadetutoPluginAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CascadetutoPluginAudioProcessorEditor)
};
