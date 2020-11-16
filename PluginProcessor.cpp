/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
using AudioGraphIOProcessor = AudioProcessorGraph::AudioGraphIOProcessor;
using Node = AudioProcessorGraph::Node;


// DEBUG TOOLS
bool customGui = false;
// ADD

//==============================================================================
CascadetutoPluginAudioProcessor::CascadetutoPluginAudioProcessor()
     : AudioProcessor (BusesProperties()
                     .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                     .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                       ),
	mainProcessor(new AudioProcessorGraph()),
	muteInput(new AudioParameterBool("mute", "Mute Input", true)),
	bypassAll(new AudioParameterBool("bypass", "Bypass All", true)),
	processorSlot1(new AudioParameterChoice("slot1", "Slot 1", processorChoices, 0)),
	processorSlot2(new AudioParameterChoice("slot2", "Slot 2", processorChoices, 0)),
	processorSlot3(new AudioParameterChoice("slot3", "Slot 3", processorChoices, 0)),
	processorSlot4(new AudioParameterChoice("slot4", "Slot 4", processorChoices, 0)),
	bypassSlot1(new AudioParameterBool("bypass1", "Bypass 1", false)),
	bypassSlot2(new AudioParameterBool("bypass2", "Bypass 2", false)),
	bypassSlot3(new AudioParameterBool("bypass3", "Bypass 3", false)),
	bypassSlot4(new AudioParameterBool("bypass4", "Bypass 4", false))
	// ADD
{
	addParameter(muteInput);
	addParameter(bypassAll);

	addParameter(processorSlot1);
	addParameter(processorSlot2);
	addParameter(processorSlot3);
	addParameter(processorSlot4);

	addParameter(bypassSlot1);
	addParameter(bypassSlot2);
	addParameter(bypassSlot3);
	addParameter(bypassSlot4);
	// ADD
}

CascadetutoPluginAudioProcessor::~CascadetutoPluginAudioProcessor()
{
}

//==============================================================================
const juce::String CascadetutoPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool CascadetutoPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool CascadetutoPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool CascadetutoPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double CascadetutoPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int CascadetutoPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int CascadetutoPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void CascadetutoPluginAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String CascadetutoPluginAudioProcessor::getProgramName (int index)
{
    return {};
}

void CascadetutoPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void CascadetutoPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock) 
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
	mainProcessor->setPlayConfigDetails(getMainBusNumInputChannels(),
		getMainBusNumOutputChannels(),
		sampleRate, samplesPerBlock);

	mainProcessor->prepareToPlay(sampleRate, samplesPerBlock);

	initialiseGraph();
}

void CascadetutoPluginAudioProcessor::releaseResources() 
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
	mainProcessor->releaseResources();
}


bool CascadetutoPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
	if (layouts.getMainInputChannelSet() == AudioChannelSet::disabled()
		|| layouts.getMainOutputChannelSet() == AudioChannelSet::disabled())
		return false;
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
	return layouts.getMainOutputChannelSet() == layouts.getMainInputChannelSet();
       
}


void CascadetutoPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...

    }

	updateGraph();

	mainProcessor->processBlock(buffer, midiMessages);
}

//==============================================================================
bool CascadetutoPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* CascadetutoPluginAudioProcessor::createEditor()
{

	// SETUP GUI DEBUG MODE
	if (customGui == true)
	{
		return new CascadetutoPluginAudioProcessorEditor(*this);
	}
	else
	{
		return new GenericAudioProcessorEditor(*this);
	}
	
}

//==============================================================================
void CascadetutoPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void CascadetutoPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CascadetutoPluginAudioProcessor();
}
