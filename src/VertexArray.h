//
// Created by chrisvega on 10/12/24.
//

#pragma once

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

class VertexArray {
private:
    unsigned int m_RendererID{};
public:
    VertexArray();
    ~VertexArray();

    void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);

    void Bind() const;
    void Unbind() const;
};
