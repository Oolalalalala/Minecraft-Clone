#pragma once


namespace Olala {

	struct Keyframe
	{
	};

	class Animation
	{
	public:

		float GetLength() const { return m_Length; }

	private:
		std::vector<Ref<Keyframe>> m_Keyframes;
		float m_Length = 0.f;
	};

}