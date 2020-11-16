/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/
#include "CpuMeter.h"
#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
CascadetutoPluginAudioProcessorEditor::CascadetutoPluginAudioProcessorEditor (CascadetutoPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
	// Main BG large Image (1250x250)
	mBackgroundImage = juce::ImageCache::getFromMemory(BinaryData::rackU_procesor_1250_v2render_png, BinaryData::rackU_procesor_1250_v2render_pngSize);

	// ADD Cpu Metering -> s <float>
	addAndMakeVisible(mcpum);

	//----------------------------------------------------------------------
	// ADD first combobox > Param > Choice processor node
	cbox1.setText("SLOT 1");
	cbox1.addItem("Bypass", 1);
	cbox1.addSectionHeading("Alex Keshlosky");
	cbox1.addItem("Alex EQ", 2);
	cbox1.addItem("Alex Delay", 3);
	cbox1.addSeparator();
	cbox1.addItem("Alex Filter", 4);
	cbox1.addItem("Alex Disto", 5);
	// ########################################## EDIT
	//map = mApChoice(choices);
	//cbox1.addItemList(mApChoice, 6);

	// ##########################################
	cbox1.setJustificationType(Justification::centred);
	cbox1.setSelectedId(1);
	addAndMakeVisible(cbox1);

	cb1mute.setButtonText("Mute");
	addAndMakeVisible(cb1mute);

	cb1solo.setButtonText("Solo");
	addAndMakeVisible(cb1solo);

	cb1bypass.setButtonText("Bypass");
	addAndMakeVisible(cb1bypass);

	//----------------------------------------------------------------------

	cbox2.setText("SLOT 2");
	cbox2.setJustificationType(Justification::centred);
	addAndMakeVisible(cbox2);

	cbox3.setText("SLOT 3");
	cbox3.setJustificationType(Justification::centred);
	addAndMakeVisible(cbox3);

	cbox4.setText("SLOT 4");
	cbox4.setJustificationType(Justification::centred);
	addAndMakeVisible(cbox4);
	//----------------------------------------------------------------------


	


	// SET Plugin Size
    setSize (1250, 250);
}

CascadetutoPluginAudioProcessorEditor::~CascadetutoPluginAudioProcessorEditor()
{
}

//==============================================================================
void CascadetutoPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
	// FILL the plugin background with transparent color, TO-DELETE later
	g.fillAll(juce::Colours::transparentBlack);
	// DRAW the background image
	g.drawImageAt(mBackgroundImage, 0, 0);
}

void CascadetutoPluginAudioProcessorEditor::resized()
{
	// PLACE Bottom line component
	mcpum.setBounds(0, 235, 600, 15);

	// PLACE Combobox 1 + Buttons
	cbox1.setBounds(110, 150, 120, 20);
	cb1mute.setBounds(110, 130, 40, 15);
	cb1solo.setBounds(150, 130, 40, 15);
	cb1bypass.setBounds(190, 130, 40, 15);

	cbox2.setBounds(250, 150, 120, 20);
	cbox3.setBounds(390, 150, 120, 20);
	cbox4.setBounds(530, 150, 120, 20);

	
}

