//
// Created by chrisvega on 10/11/24.
//

#pragma once

class VertexBuffer {
private:
    unsigned int m_RendererID{};
public:
    VertexBuffer(const void* data, unsigned int size);
    ~VertexBuffer();

    void Bind() const;

    void Unbind() const;
};
