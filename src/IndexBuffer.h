//
// Created by chrisvega on 10/12/24.
//

#ifndef INDEXBUFFER_H
#define INDEXBUFFER_H

class IndexBuffer {
private:
    unsigned int m_RendererID{};
    unsigned int m_count{};
public:
    IndexBuffer(const unsigned int* data, unsigned int count);
    ~IndexBuffer();

    void Bind() const;
    void Unbind() const;

    inline unsigned int GetCount() const { return m_count; }
};

#endif //INDEXBUFFER_H
