
#pragma once
#include <JuceHeader.H>

namespace Gui
{
	class VerticalMeter : public Component
	{
	public:
		void paint(Graphics& g) override
		{
		
			auto bounds = getLocalBounds().toFloat();
			g.setColour(Colours::white.withBrightness(0.4f));
			g.fillRoundedRectangle(bounds, 5.f);

			g.setColour(Colours::white);
			//map level from

			const auto scaledX =
			jmap(level, -60.f, +6.f, 0.f, static_cast<float>(getHeight()));
			g.fillRoundedRectangle(bounds.removeFromBottom(scaledX), 5.f);
		}
		void setlevel(const float value) { level = value; }

	private:
		float level = -60.f;
		
	};
}
