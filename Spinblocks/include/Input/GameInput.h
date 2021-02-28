#pragma once

namespace GameInput
{
	float getVerticalAxis();
	float getHorizontalAxis();

	void setVerticalAxis(float vertical);
	void setHorizontalAxis(float horizontal);
	void clearState();

	namespace
	{
		float vertical;
		float horizontal;
	}
};
