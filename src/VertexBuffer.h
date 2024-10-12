//
// Created by chrisvega on 10/11/24.
//

#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H

class VertexBuffer {
private:
    unsigned int m_RendererID{};
public:
    VertexBuffer(const void* data, unsigned int size);
    ~VertexBuffer();

    void Bind() const;

    void Unbind() const;
};

#endif //VERTEXBUFFER_H
