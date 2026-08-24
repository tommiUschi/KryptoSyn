
#include <foleys_gui_magic/foleys_gui_magic.h>
#include "HelpTextItem.h"

HelpTextItem::HelpTextItem (foleys::MagicGUIBuilder& builder, const juce::ValueTree& node)
        : foleys::GuiItem (builder, node)
{
        heplTextComponent.initialise();
        //colour = myNode.getProperty ("text", "TextEditor");;
        myNode = node;
        // make the raw JUCE component visible
        addAndMakeVisible (editor);
        backGroundColor = juce::Colour(0xffdcd3bf);
        colour = juce::Colour(0xff111111);
        auto textStr = juce::String(myNode.getProperty ("text", "Text"));
        auto myText = textStr.isNotEmpty() ? juce::String (textStr) : "Text";
        heplTextComponent.setTextEditCustomColors (backGroundColor, colour);
        heplTextComponent.setText(myText);
        addAndMakeVisible (heplTextComponent);
}

void HelpTextItem::update()
{

}

juce::Component* HelpTextItem::getWrappedComponent()
{
        return &heplTextComponent;
}




