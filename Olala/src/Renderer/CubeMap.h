#pragma once


namespace Olala {

	class CubeMap
	{
	public:

		enum class Face
		{
			Right = 0, Left, Top, Bottom, Back, Front
		};

		CubeMap(const std::string& folder, const std::unordered_map<Face, std::string>& faces = std::unordered_map<Face, std::string>());
		~CubeMap();

		void Bind(uint32_t slot = 0);

		static Ref<CubeMap> Create(const std::string& folder, const std::unordered_map<Face, std::string>& faces = std::unordered_map<Face, std::string>());

	private:
		uint32_t m_RendererID;
	};

}