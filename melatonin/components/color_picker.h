#pragma once

#include "juce_gui_extra/juce_gui_extra.h"
#include "melatonin_inspector/melatonin/components/overlay.h"

namespace melatonin
{

    class ColorPicker : public juce::Component
    {
    public:
        ColorPicker()
        {
            addAndMakeVisible (colorValField);
            addAndMakeVisible (colorPickerButton);

            colorValField.setJustificationType(juce::Justification::centredLeft);
            colorValField.setColour(juce::TextEditor::ColourIds::textColourId, colors::propertyValue);
            selectedColor = juce::Colours::transparentBlack;

            // we overlap the header, so let people click that as usual
            setInterceptsMouseClicks (false, true);
        }

        void paint (juce::Graphics& g) override
        {
            auto bounds = juce::Rectangle<float> ({ 32, 40, 18, 18 });

            if (selectedColor.isTransparent())
            {
                g.setColour (colors::titleTextColor);
                g.drawRoundedRectangle (bounds, 2.f, 1.f);
            }
            else
            {
                g.setColour (selectedColor);
                g.fillRoundedRectangle (bounds, 2.f);
            }
            g.setColour (colors::panelLineSeparatorColor);
            g.drawHorizontalLine (getHeight() - 1, 0, (float) getWidth()); // separator
        }

        void resized() override
        {
            colorPickerButton.setBounds (getLocalBounds()
                                             .removeFromRight (32)
                                             .removeFromTop (32)
                                             .withSizeKeepingCentre (32, 32));

            auto area = getLocalBounds();
            area.removeFromTop (29); // overlap with our header + bit of padding
            colorValField.setBounds (area.withTrimmedLeft (64));
        }

        void mouseEnter (const juce::MouseEvent& event) override
        {
            if (root == nullptr)
                return;

            auto rootPos = event.getEventRelativeTo (root).getPosition();

            image = std::make_unique<juce::Image> (root->createComponentSnapshot ({ root->getWidth(), root->getHeight() }, false));
            if (colorPickerButton.enabled)
            {
                selectedColor = image->getPixelAt (rootPos.x, rootPos.y);
                updateLabels();
            }

            root->setMouseCursor (colorPickerButton.enabled ? juce::MouseCursor::CrosshairCursor : juce::MouseCursor::NormalCursor);
        }

        void mouseMove (const juce::MouseEvent& event) override
        {
            if (root == nullptr || image == nullptr)
                return;
            auto rootPos = event.getEventRelativeTo (root).getPosition();

            if (colorPickerButton.enabled)
            {
                selectedColor = image->getPixelAt (rootPos.x, rootPos.y);
                updateLabels();
            }
        }

        void mouseExit (const juce::MouseEvent&) override
        {
            root->setMouseCursor (colorPickerButton.enabled ? juce::MouseCursor::CrosshairCursor : juce::MouseCursor::NormalCursor);
        }

        void setRootComponent (Component* rootComponent)
        {
            if (root != nullptr)
                root->removeMouseListener (this);

            root = rootComponent;

            if (root == nullptr)
            {
                selectedColor = juce::Colours::transparentBlack;
                reset();

                return;
            }

            root->addMouseListener (this, true);
            image = std::make_unique<juce::Image> (root->createComponentSnapshot ({ root->getWidth(), root->getHeight() }, false));

            reset();
        }

        void reset()
        {
            updateLabels();
            resized();
        }

    private:
        InspectorImageButton colorPickerButton { "Eyedropper", { 0, 6 }, true };
        juce::Label colorValField {
            "Color value",
        };

        std::unique_ptr<juce::Image> image;
        juce::Colour selectedColor { juce::Colours::transparentBlack };

        juce::Component* root {};

        void updateLabels()
        {
            juce::String rgbaString = juce::String::formatted ("%d, %d, %d",
                selectedColor.getRed(),
                selectedColor.getGreen(),
                selectedColor.getBlue());
            colorValField.setText (rgbaString, juce::dontSendNotification);
            colorValField.setVisible (true);

            resized();
            repaint();
        }

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ColorPicker)
    };
}
