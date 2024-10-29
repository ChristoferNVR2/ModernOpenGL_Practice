#include <GL/glew.h> // Must be included before GLFW
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <csignal>

#include "Renderer.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "ObjLoader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

void DrawAxes(const glm::mat4& proj, const glm::mat4& view) {
    static constexpr float axisVertices[] = {
        // X axis (red)
        0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        10.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        // Y axis (green)
        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 10.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        // Z axis (blue)
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 10.0f, 0.0f, 0.0f, 1.0f
    };

    VertexArray va;
    VertexBuffer vb(axisVertices, sizeof(axisVertices));

    VertexBufferLayout layout;
    layout.Push<float>(3); // Position
    layout.Push<float>(3); // Color
    va.AddBuffer(vb, layout);

    auto model = glm::mat4(1.0f);
    glm::mat4 mvp = proj * view * model;

    Shader axisShader("res/shaders/Axis.shader");
    axisShader.Bind();
    axisShader.SetUniformMat4f("u_MVP", mvp);

    va.Bind();
    glDrawArrays(GL_LINES, 0, 6);
}

int main() {
    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Hello World", nullptr, nullptr);


    if (!window) {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK) {
        std::cerr << "Error initializing GLEW!" << std::endl;
        return -1;
    }

    std::cout << glGetString(GL_VERSION) << std::endl;

    // glEnable(GL_DEPTH_TEST);
    // glDepthFunc(GL_LESS);
    // glDisable(GL_CULL_FACE);


    {
        ObjLoader loader;
        if (!loader.Load("res/models/centaurwarrior.obj")) {
            std::cerr << "Failed to load .obj file." << std::endl;
            return -1;
        }
        // if (!loader.Load("res/models/uploads_files_2787791_Mercedes+Benz+GLS+580.obj")) {
        //     std::cerr << "Failed to load .obj file." << std::endl;
        //     return -1;
        // }

        const auto& vertices = loader.GetVertices();
        const auto& indices = loader.GetIndices();

        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        VertexArray va;
        VertexBuffer vb(vertices.data(), vertices.size() * sizeof(Vertex));

        VertexBufferLayout layout;
        layout.Push<float>(3); // Position
        layout.Push<float>(2); // Texture coordinates
        layout.Push<float>(3); // Normal
        va.AddBuffer(vb, layout);

        IndexBuffer ib(indices.data(), indices.size());

        glm::mat4 proj = glm::perspective(glm::radians(60.0f), 1920.0f / 1080.0f, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

        Shader shader("res/shaders/Basic.shader");
        shader.Bind();
        shader.SetUniform4f("u_Color", 1.0f, 1.0f, 1.0f, 1.0f); // White color

        // shader.SetUniform3f("lightPos", 0.0f, 500.0f, 0.0f);

        // // Light properties (adjust these based on your scene's brightness)
        // shader.SetUniform3f("lightAmbient", 0.2f, 0.0f, 0.0f);   // Low ambient red light
        // shader.SetUniform3f("lightDiffuse", 0.8f, 0.0f, 0.0f);   // Stronger diffuse red
        // shader.SetUniform3f("lightSpecular", 0.1f, 0.1f, 0.1f);  // Minimal specular
        //
        // // Material properties for matte red
        // shader.SetUniform3f("matAmbient", 0.2f, 0.0f, 0.0f);     // Soft ambient red
        // shader.SetUniform3f("matDiffuse", 0.6f, 0.0f, 0.0f);     // Strong diffuse red
        // shader.SetUniform3f("matSpecular", 0.1f, 0.1f, 0.1f);    // Very low specular for matte look
        // shader.SetUniform1f("matShininess", 100.0f);              // Low shininess for matte finish

        // Light properties (adjust these based on your scene's brightness)
        shader.SetUniform3f("lightPos", 2.0f, 5.0f, 5.0f);
        shader.SetUniform3f("lightAmbient", .5f, .5f, .5f);
        shader.SetUniform3f("lightDiffuse", .5f, .5f, .5f);
        shader.SetUniform3f("lightSpecular", 1.0f, 1.0f, 1.0f);

        // Set material properties
        shader.SetUniform3f("matAmbient", 0.7f, 0.7f, 0.7f);
        shader.SetUniform3f("matDiffuse", 0.8f, 0.8f, 0.8f);
        shader.SetUniform3f("matSpecular", 1.0f, 1.0f, 1.0f);
        shader.SetUniform1f("matShininess", 100.0f);

        va.Unbind();
        shader.Unbind();
        vb.Unbind();
        ib.Unbind();

        Renderer renderer;

        ImGui::CreateContext();
        ImGui_ImplGlfwGL3_Init(window, true);
        ImGui::StyleColorsDark();

        glm::vec3 translation(0, 0, 0);
        glm::vec3 cameraPos(5.0f, 5.0f, 5.0f);
        glm::vec3 cameraFront(-1.0f, -1.0f, -1.0f);
        glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);
        float scale = 1.0f;

        bool isDragging = false;
        bool isCameraDragging = false;
        bool isCameraRotating = false;
        double lastMouseX, lastMouseY;
        float fov = 60.0f;

        while (!glfwWindowShouldClose(window)) {
            renderer.Clear();

            DrawAxes(proj, view);

            ImGui_ImplGlfwGL3_NewFrame();

            {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0, -1.5, 0)) * glm::translate(glm::mat4(1.0f), translation) * glm::scale(glm::mat4(1.0f), glm::vec3(scale)); // ONLY FOR THE CAR
                // glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.8, -1, 3.9)) * glm::translate(glm::mat4(1.0f), translation) * glm::scale(glm::mat4(1.0f), glm::vec3(scale));
                glm::mat4 mvp = proj * view * model;
                shader.Bind();
                shader.SetUniformMat4f("u_MVP", mvp);
                shader.SetUniformMat4f("u_Model", model);
                shader.SetUniformMat4f("u_View", view);
                shader.SetUniform3f("viewPos", cameraPos.x, cameraPos.y, cameraPos.z);

                renderer.Draw(va, ib, shader);
            }

            {
                ImGui::SliderFloat3("Translation", &translation.x, -5.0f, 5.0f);
                ImGui::SliderFloat("Scale", &scale, 0.1f, 10.0f);
                ImGui::SliderFloat3("Camera Position", &cameraPos.x, -10.0f, 10.0f);
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            }

            // Capture mouse wheel input
            float mouseWheel = ImGui::GetIO().MouseWheel;
            if (mouseWheel != 0.0f) {
                if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
                    cameraPos.y += mouseWheel;
                } else if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
                    fov -= mouseWheel * 2.0f; // Adjust sensitivity as needed
                    if (fov < 1.0f) fov = 1.0f;
                    if (fov > 90.0f) fov = 90.0f;
                    proj = glm::perspective(glm::radians(fov), 1920.0f / 1080.0f, 0.1f, 100.0f);
                } else {
                    translation.y += mouseWheel;
                }
            }

            // Handle mouse dragging
            if (ImGui::IsMouseClicked(0)) {
                if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
                    isCameraDragging = true;
                } else if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
                    isCameraRotating = true;
                } else {
                    isDragging = true;
                }
                glfwGetCursorPos(window, &lastMouseX, &lastMouseY);
            }

            if (ImGui::IsMouseReleased(0)) {
                isDragging = false;
                isCameraDragging = false;
                isCameraRotating = false;
            }

            if (isDragging) {
                double mouseX, mouseY;
                glfwGetCursorPos(window, &mouseX, &mouseY);
                double deltaX = mouseX - lastMouseX;
                double deltaY = mouseY - lastMouseY;
                translation.x += static_cast<float>(deltaX) * 0.01f; // Adjust sensitivity as needed
                translation.z += static_cast<float>(deltaY) * 0.01f; // Adjust sensitivity as needed
                lastMouseX = mouseX;
                lastMouseY = mouseY;
            }

            if (isCameraDragging) {
                double mouseX, mouseY;
                glfwGetCursorPos(window, &mouseX, &mouseY);
                double deltaX = mouseX - lastMouseX;
                double deltaY = mouseY - lastMouseY;
                cameraPos.x -= static_cast<float>(deltaX) * 0.01f; // Adjust sensitivity as needed
                cameraPos.z += static_cast<float>(deltaY) * 0.01f; // Adjust sensitivity as needed
                lastMouseX = mouseX;
                lastMouseY = mouseY;
            }

            if (isCameraRotating) {
                double mouseX, mouseY;
                glfwGetCursorPos(window, &mouseX, &mouseY);
                double deltaX = mouseX - lastMouseX;
                double deltaY = mouseY - lastMouseY;

                float angleX = static_cast<float>(deltaX) * 0.01f; // Adjust sensitivity as needed
                float angleY = static_cast<float>(deltaY) * 0.01f; // Adjust sensitivity as needed

                glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angleX, cameraUp) * glm::rotate(glm::mat4(1.0f), angleY, glm::cross(cameraFront, cameraUp));
                cameraPos = glm::vec3(rotation * glm::vec4(cameraPos, 1.0f));
                cameraFront = glm::normalize(-cameraPos);

                lastMouseX = mouseX;
                lastMouseY = mouseY;
            }

            view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

            ImGui::Render();
            ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }
    }

    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}
