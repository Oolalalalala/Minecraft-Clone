#include "pch.h"
#include "VertexArray.h"

namespace Olala {

    static GLenum CastToOpenGLBaseType(ShaderDataType type)
    {
        switch (type)
        {
        case ShaderDataType::Float:
        case ShaderDataType::Float2:
        case ShaderDataType::Float3:
        case ShaderDataType::Float4:
        case ShaderDataType::Mat3:
        case ShaderDataType::Mat4:
            return GL_FLOAT;
        case ShaderDataType::Int:
        case ShaderDataType::Int2:
        case ShaderDataType::Int3:
        case ShaderDataType::Int4:
            return GL_INT;
        case ShaderDataType::Bool:
            return GL_BOOL;
        }
        std::cout << "Shader data type not supported\n";
        return 0;
    }

    VertexArray::VertexArray()
    {
        glCreateVertexArrays(1, &m_RendererID);
    }

    VertexArray::~VertexArray()
    {
        glDeleteVertexArrays(1, &m_RendererID);
    }

    void VertexArray::Bind() const
    {
        glBindVertexArray(m_RendererID);
    }

    void VertexArray::Unbind() const
    {
        glBindVertexArray(0);
    }

    void VertexArray::SetLayout(const VertexBufferLayout& layout)
    {
        if (layout.GetElements().empty()) OLA_CORE_ERROR("Layout has no elements");

        glBindVertexArray(m_RendererID);
        int attributeIndex = 0;

        for (const auto& element : layout)
        {
            switch (element.DataType)
            {
            case ShaderDataType::Float:
            case ShaderDataType::Float2:
            case ShaderDataType::Float3:
            case ShaderDataType::Float4:
            {
                glVertexAttribFormat(attributeIndex, element.GetComponentCount(), CastToOpenGLBaseType(element.DataType),
                    element.Normalized, element.Offset);
                glVertexAttribBinding(attributeIndex, 0);
                glEnableVertexAttribArray(attributeIndex);
                attributeIndex++;
                break;
            }
            case ShaderDataType::Int:
            case ShaderDataType::Int2:
            case ShaderDataType::Int3:
            case ShaderDataType::Int4:
            case ShaderDataType::Bool:
            {
                glVertexAttribIFormat(attributeIndex, element.GetComponentCount(), CastToOpenGLBaseType(element.DataType),
                    element.Offset);
                glVertexAttribBinding(attributeIndex, 0);
                glEnableVertexAttribArray(attributeIndex);
                attributeIndex++;
                break;
            }
            case ShaderDataType::Mat3:
            case ShaderDataType::Mat4:
            {
                uint8_t count = element.GetComponentCount();
                for (uint8_t i = 0; i < count; i++)
                {
                    glVertexAttribFormat(attributeIndex, count, CastToOpenGLBaseType(element.DataType),
                        element.Normalized, element.Offset + sizeof(float) * count * i);
                    glVertexBindingDivisor(attributeIndex, 1);
                    glVertexAttribBinding(attributeIndex, 0); // TODO : set binding to 1 ?   &    modify the stride 
                    glEnableVertexAttribArray(attributeIndex);
                    attributeIndex++;
                }
                break;
            }
            default:
                std::cout << "Unknown shader data type\n";
            }
        }

        glBindVertexArray(0);
        m_Layout = layout;
    }

    void VertexArray::AddBuffer(Ref<VertexBuffer> vertexBuffer, Ref<IndexBuffer> indexBuffer)
    {
        vertexBuffer->m_VertexArray = this;
        m_Buffers.emplace_back(vertexBuffer, indexBuffer);
    }

    void VertexArray::ClearBuffers()
    {
        for (auto& [vertexBuffer, indexBuffer] : m_Buffers)
            vertexBuffer->m_VertexArray = nullptr;
        m_Buffers.clear();
    }

    Ref<VertexArray> VertexArray::Create()
    {
        return CreateRef<VertexArray>();
    }

}