/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ProcessorBase.h"
#include "osc.h"
#include "gain.h"
#include "filter.h"
#include "lpfilter.h"
#include "hpfilter.h"
#include "bpfilter.h"

//==============================================================================
/**
*/
class CascadetutoPluginAudioProcessor  : public juce::AudioProcessor
{
public:
	//==============================================================================
	using AudioGraphIOProcessor = AudioProcessorGraph::AudioGraphIOProcessor;
	using Node = AudioProcessorGraph::Node;
	//ADD
    //==============================================================================
    CascadetutoPluginAudioProcessor();
    ~CascadetutoPluginAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;


private:
	//==============================================================================
	void initialiseGraph()
	{
		mainProcessor->clear();

		audioInputNode = mainProcessor->addNode(std::make_unique<AudioGraphIOProcessor>(AudioGraphIOProcessor::audioInputNode));
		audioOutputNode = mainProcessor->addNode(std::make_unique<AudioGraphIOProcessor>(AudioGraphIOProcessor::audioOutputNode));
		midiInputNode = mainProcessor->addNode(std::make_unique<AudioGraphIOProcessor>(AudioGraphIOProcessor::midiInputNode));
		midiOutputNode = mainProcessor->addNode(std::make_unique<AudioGraphIOProcessor>(AudioGraphIOProcessor::midiOutputNode));

		connectAudioNodes();
		connectMidiNodes();
	}

	void updateGraph()
	{
		bool hasChanged = false;

		Array<AudioParameterChoice*> choices{ processorSlot1,
											   processorSlot2,
											   processorSlot3,
											   processorSlot4 };

		Array<AudioParameterBool*> bypasses{ bypassSlot1,
											  bypassSlot2,
											  bypassSlot3,
											  bypassSlot4 };

		ReferenceCountedArray<Node> slots;
		slots.add(slot1Node);
		slots.add(slot2Node);
		slots.add(slot3Node);
		slots.add(slot4Node);

		for (int i = 0; i < 4; ++i)
		{
			auto& choice = choices.getReference(i);
			auto  slot = slots.getUnchecked(i);

			if (choice->getIndex() == 0)
			{
				if (slot != nullptr)
				{
					mainProcessor->removeNode(slot.get());
					slots.set(i, nullptr);
					hasChanged = true;
				}
			}
			else if (choice->getIndex() == 1)
			{
				if (slot != nullptr)
				{
					if (slot->getProcessor()->getName() == "Oscillator")
						continue;

					mainProcessor->removeNode(slot.get());
				}

				slots.set(i, mainProcessor->addNode(std::make_unique<OscillatorProcessor>()));
				hasChanged = true;
			}
			else if (choice->getIndex() == 2)
			{
				if (slot != nullptr)
				{
					if (slot->getProcessor()->getName() == "Gain")
						continue;

					mainProcessor->removeNode(slot.get());
				}

				slots.set(i, mainProcessor->addNode(std::make_unique<GainProcessor>()));
				hasChanged = true;
			}
			else if (choice->getIndex() == 3)
			{
				if (slot != nullptr)
				{
					if (slot->getProcessor()->getName() == "Filter")
						continue;

					mainProcessor->removeNode(slot.get());
				}

				slots.set(i, mainProcessor->addNode(std::make_unique<FilterProcessor>()));
				hasChanged = true;
			}
			else if (choice->getIndex() == 4)
			{
				if (slot != nullptr)
				{
					if (slot->getProcessor()->getName() == "LpFilter")
						continue;

					mainProcessor->removeNode(slot.get());
				}

				slots.set(i, mainProcessor->addNode(std::make_unique<LpFilterProcessor>()));
				hasChanged = true;
			}
			else if (choice->getIndex() == 5)
			{
				if (slot != nullptr)
				{
					if (slot->getProcessor()->getName() == "HpFilter")
						continue;

					mainProcessor->removeNode(slot.get());
				}

				slots.set(i, mainProcessor->addNode(std::make_unique<HpFilterProcessor>()));
				hasChanged = true;
			}
			else if (choice->getIndex() == 6)
			{
				if (slot != nullptr)
				{
					if (slot->getProcessor()->getName() == "BpFilter")
						continue;

					mainProcessor->removeNode(slot.get());
				}

				slots.set(i, mainProcessor->addNode(std::make_unique<BpFilterProcessor>()));
				hasChanged = true;
			}
		}

		if (hasChanged)
		{
			for (auto connection : mainProcessor->getConnections())
				mainProcessor->removeConnection(connection);

			ReferenceCountedArray<Node> activeSlots;

			for (auto slot : slots)
			{
				if (slot != nullptr)
				{
					activeSlots.add(slot);

					slot->getProcessor()->setPlayConfigDetails(getMainBusNumInputChannels(),
						getMainBusNumOutputChannels(),
						getSampleRate(), getBlockSize());
				}
			}

			if (activeSlots.isEmpty())
			{
				connectAudioNodes();
			}
			else
			{
				for (int i = 0; i < activeSlots.size() - 1; ++i)
				{
					for (int channel = 0; channel < 2; ++channel)
						mainProcessor->addConnection({ { activeSlots.getUnchecked(i)->nodeID,      channel },
														{ activeSlots.getUnchecked(i + 1)->nodeID,  channel } });
				}

				for (int channel = 0; channel < 2; ++channel)
				{
					mainProcessor->addConnection({ { audioInputNode->nodeID,         channel },
													{ activeSlots.getFirst()->nodeID, channel } });
					mainProcessor->addConnection({ { activeSlots.getLast()->nodeID,  channel },
													{ audioOutputNode->nodeID,        channel } });
				}
			}

			connectMidiNodes();

			for (auto node : mainProcessor->getNodes())
				node->getProcessor()->enableAllBuses();
		}

		for (int i = 0; i < 4; ++i)
		{
			auto  slot = slots.getUnchecked(i);
			auto& bypass = bypasses.getReference(i);

			if (slot != nullptr)
				slot->setBypassed(bypass->get());
		}

		audioInputNode->setBypassed(muteInput->get());
		audioOutputNode->setBypassed(bypassAll->get());

		slot1Node = slots.getUnchecked(0);
		slot2Node = slots.getUnchecked(1);
		slot3Node = slots.getUnchecked(2);
		slot4Node = slots.getUnchecked(3);
	}

	void connectAudioNodes()
	{
		for (int channel = 0; channel < 2; ++channel)
			mainProcessor->addConnection({ { audioInputNode->nodeID,  channel },
											{ audioOutputNode->nodeID, channel } });
	}

	void connectMidiNodes()
	{
		mainProcessor->addConnection({ { midiInputNode->nodeID,  AudioProcessorGraph::midiChannelIndex },
										{ midiOutputNode->nodeID, AudioProcessorGraph::midiChannelIndex } });
	}

	//==============================================================================
	StringArray processorChoices{ "Empty", "Oscillator", "Gain", "Filter", "LpFilter", "HpFilter", "BpFilter" };

	std::unique_ptr<AudioProcessorGraph> mainProcessor;

	AudioParameterBool* muteInput;
	AudioParameterBool* bypassAll;

	AudioParameterChoice* processorSlot1;
	AudioParameterChoice* processorSlot2;
	AudioParameterChoice* processorSlot3;
	AudioParameterChoice* processorSlot4;

	AudioParameterBool* bypassSlot1;
	AudioParameterBool* bypassSlot2;
	AudioParameterBool* bypassSlot3;
	AudioParameterBool* bypassSlot4;

	Node::Ptr audioInputNode;
	Node::Ptr audioOutputNode;
	Node::Ptr midiInputNode;
	Node::Ptr midiOutputNode;

	Node::Ptr slot1Node;
	Node::Ptr slot2Node;
	Node::Ptr slot3Node;
	Node::Ptr slot4Node;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CascadetutoPluginAudioProcessor)
};
