#include "pch.h"
#include "Renderer3D.h"
#include "VertexArray.h"
#include "RenderCommand.h"
#include "UniformBuffer.h"


namespace Olala {

	Renderer3D::DirectionalLightUniformData Renderer3D::s_DirectionalLightUniformData;

	struct Renderer3DData
	{
		struct MeshData
		{
			Ref<Mesh> Mesh;
			Ref<Material> Material;
			glm::mat4 ModelMatrix;
		};

		std::vector<MeshData> MeshDataBuffer;

		Ref<VertexArray> MeshVertexArray;
		Ref<Shader> MeshShader;

		glm::mat4 ViewPerspectiveMatrix = glm::mat4(0.f);

		// Lighting
		struct DirectionalLightData
		{
			glm::vec3 Direction;
			glm::vec3 Color;
			bool CastShadow;
		};

		struct PointLightData
		{
			glm::vec3 Position;
			glm::vec3 Color;
			float Range;
			bool CastShadow;
		};

		struct SpotLightData
		{
			glm::vec3 Position;
			glm::vec3 Direction;
			glm::vec3 Color;
			float Angle;
			float Range;
			bool CastShadow;
		};

		std::vector<DirectionalLightData> DirectionalLightDataArray;
		std::vector<PointLightData> PointLightDataArray;
		std::vector<SpotLightData> SpotLightDataArray;

		Ref<UniformBuffer> DirectionalLightUniformBuffer;

		// SkyBox
		Ref<CubeMap> SkyBox;
		glm::vec4 SkyBoxColor;

		Ref<VertexArray> SkyBoxVertexArray;
		Ref<Shader> SkyBoxShader;

		// Camera
		Ref<Camera> RenderingCamera;
	};
	static Renderer3DData s_Data;


	void Renderer3D::Init()
	{
		s_Data.MeshVertexArray = VertexArray::Create();
		s_Data.MeshVertexArray->SetLayout({
			{ "a_Position" , ShaderDataType::Float3 },
			{ "a_Normal"   , ShaderDataType::Float3 },
			{ "a_TexCoord" , ShaderDataType::Float2 }
		});
		s_Data.MeshShader = Shader::Create("../Olala/Asset/Internal/Shader/Phong.glsl");
		s_Data.DirectionalLightUniformBuffer = UniformBuffer::Create(sizeof(Renderer3D::DirectionalLightUniformData), 1);

		// Skybox
		s_Data.SkyBoxVertexArray = VertexArray::Create();
		s_Data.SkyBoxVertexArray->SetLayout({
			{ "a_Position" , ShaderDataType::Float3 }
		});
		float vertices[] = { -1,  1,  1,   1,  1,  1,   1,  1, -1,   -1,  1, -1,
							 -1, -1,  1,   1, -1,  1,   1, -1, -1,   -1, -1, -1 };
		uint32_t indices[] = { 0, 2, 1,  2, 0, 3,    0, 1, 4,  4, 1, 5,   1, 2, 5,  5, 2, 6,
							   2, 3, 6,  6, 3, 7,    3, 0, 7,  7, 0, 4,   4, 5, 6,  6, 7, 4 };
		auto vertexBuffer = VertexBuffer::Create(vertices, sizeof(vertices));
		auto indexBuffer = IndexBuffer::Create(indices, 36);
		s_Data.SkyBoxVertexArray->AddBuffer(vertexBuffer, indexBuffer);
		s_Data.SkyBoxShader = Shader::Create("../Olala/Asset/Internal/Shader/SkyBox.glsl");
	}

	void Renderer3D::ShutDown()
	{
	}

	void Renderer3D::BeginScene(Ref<Camera> camera)
	{
		s_Data.RenderingCamera = camera;
		Reset();
	}

	void Renderer3D::EndScene()
	{
		Flush();
	}

	void Renderer3D::Submit(Ref<Mesh> mesh, Ref<Material> material, glm::mat4 modelMatrix)
	{
		s_Data.MeshDataBuffer.push_back({ mesh, material, modelMatrix });
	}

	void Renderer3D::AddDirectionalLight(const glm::vec3& direction, const glm::vec3& color, const float& intensity, bool castShadow)
	{
		s_Data.DirectionalLightDataArray.push_back({ direction, color * intensity, castShadow });
	}

	void Renderer3D::AddPointLight(const glm::vec3& position, const glm::vec3& color, const float& range, const float& intensity, bool castShadow)
	{
		s_Data.PointLightDataArray.push_back({ position, color * intensity, range, castShadow });
	}

	void Renderer3D::AddSpotLight(const glm::vec3& position, const glm::vec3 direction, const glm::vec3& color, const float& angle,
		const float& range, const float& intensity, bool castShadow)
	{
		s_Data.SpotLightDataArray.push_back({ position, direction, color * intensity, angle, range, castShadow });
	}

	void Renderer3D::SetSkyBox(Ref<CubeMap> skyBox, const glm::vec4& color)
	{
		s_Data.SkyBox = skyBox;
		s_Data.SkyBoxColor = color;
	}

	void Renderer3D::Reset()
	{
		s_Data.MeshDataBuffer.clear();
		s_Data.DirectionalLightDataArray.clear();
		s_Data.PointLightDataArray.clear();
		s_Data.SpotLightDataArray.clear();
	}

	void Renderer3D::Flush()
	{
		// Skybox
		if (s_Data.SkyBox)
		{
			RenderCommand::UseDepthTest(false);

			s_Data.SkyBoxShader->Bind();
			glm::mat4 viewProjection = s_Data.RenderingCamera->GetProjectionMatrix() * glm::mat4(glm::mat3(s_Data.RenderingCamera->GetViewMatrix())); // No translation for view matrix
			s_Data.SkyBoxShader->SetUniformMat4("u_ViewProjection", viewProjection);
			s_Data.SkyBoxShader->SetUniformVec4("u_Color", s_Data.SkyBoxColor);
			s_Data.SkyBoxShader->SetUniformInt("u_SkyBox", 0);
			s_Data.SkyBox->Bind(0);

			RenderCommand::DrawIndexed(s_Data.SkyBoxVertexArray);
			RenderCommand::UseDepthTest(true);
		}

		// Submit light data
		if (!s_Data.DirectionalLightDataArray.empty())
		{
			if (s_Data.DirectionalLightDataArray.size() > 1)
				OLA_WARN("Only one directional light per scene is supported");
			auto& data = s_Data.DirectionalLightDataArray[0];
			s_DirectionalLightUniformData.Direction = glm::vec4(data.Direction, data.CastShadow ? 1.f : 0.f); // w component for [castShadow] flag
			s_DirectionalLightUniformData.Color = glm::vec4(data.Color, 0.f);
			s_Data.DirectionalLightUniformBuffer->SetData(&s_DirectionalLightUniformData, sizeof(Renderer3D::DirectionalLightUniformData));
		}


		// Render meshes
		for (auto& data : s_Data.MeshDataBuffer)
		{
			s_Data.MeshVertexArray->ClearBuffers();
			s_Data.MeshVertexArray->AddBuffer(data.Mesh->GetVertexBuffer(), data.Mesh->GetIndexBuffer()); 
			data.Material->Texture->Bind(0);
			s_Data.MeshShader->Bind();
			s_Data.MeshShader->SetUniformMat4("u_Model", data.ModelMatrix);
			s_Data.MeshShader->SetUniformInt("u_Texture", 0);
			s_Data.MeshShader->SetUniformVec4("u_Color", data.Material->Color);
			//s_Data.MeshShader->SetUniformFloat("u_Metallic", data.Material->Metallic);
			//s_Data.MeshShader->SetUniformFloat("u_Roughness", data.Material->Roughness);
			RenderCommand::DrawIndexed(s_Data.MeshVertexArray);
		}		
	}

}