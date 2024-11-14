#define GLM_ENABLE_EXPERIMENTAL

#include <GL/glew.h> // Must be included before GLFW
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <csignal>
#include <map>

#include "Renderer.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp> // Include for quaternion functionality
#include <glm/gtx/quaternion.hpp> // Include for mat4_cast
#include <glm/gtc/type_ptr.hpp> // Include for make_mat4


#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// Bone structure to store transformation info
struct Bone {
    std::string name;
    glm::mat4 offsetMatrix;
    glm::mat4 finalTransformation;
};

// Animation structure to store animation data
struct Animation {
    double duration;
    double ticksPerSecond;
    std::vector<Bone> bones;
    std::map<std::string, aiNodeAnim*> channels;
};

// Function to find a bone by name
Bone* FindBone(std::vector<Bone>& bones, const std::string& name) {
    for (auto& bone : bones) {
        if (bone.name == name) {
            return &bone;
        }
    }
    return nullptr;
}

// Function to interpolate between keyframes
glm::mat4 InterpolateTransform(const aiNodeAnim* channel, double time) {
    // Interpolate position
    aiVector3D position(0, 0, 0);
    if (channel->mNumPositionKeys > 1) {
        for (unsigned int i = 0; i < channel->mNumPositionKeys - 1; i++) {
            if (time < channel->mPositionKeys[i + 1].mTime) {
                float t = (time - channel->mPositionKeys[i].mTime) / (channel->mPositionKeys[i + 1].mTime - channel->mPositionKeys[i].mTime);
                position = channel->mPositionKeys[i].mValue + t * (channel->mPositionKeys[i + 1].mValue - channel->mPositionKeys[i].mValue);
                break;
            }
        }
    } else {
        position = channel->mPositionKeys[0].mValue;
    }

    // Interpolate rotation
    aiQuaternion rotation(1, 0, 0, 0);
    if (channel->mNumRotationKeys > 1) {
        for (unsigned int i = 0; i < channel->mNumRotationKeys - 1; i++) {
            if (time < channel->mRotationKeys[i + 1].mTime) {
                float t = (time - channel->mRotationKeys[i].mTime) / (channel->mRotationKeys[i + 1].mTime - channel->mRotationKeys[i].mTime);
                aiQuaternion::Interpolate(rotation, channel->mRotationKeys[i].mValue, channel->mRotationKeys[i + 1].mValue, t);
                break;
            }
        }
    } else {
        rotation = channel->mRotationKeys[0].mValue;
    }

    // Interpolate scaling
    aiVector3D scaling(1, 1, 1);
    if (channel->mNumScalingKeys > 1) {
        for (unsigned int i = 0; i < channel->mNumScalingKeys - 1; i++) {
            if (time < channel->mScalingKeys[i + 1].mTime) {
                float t = (time - channel->mScalingKeys[i].mTime) / (channel->mScalingKeys[i + 1].mTime - channel->mScalingKeys[i].mTime);
                scaling = channel->mScalingKeys[i].mValue + t * (channel->mScalingKeys[i + 1].mValue - channel->mScalingKeys[i].mValue);
                break;
            }
        }
    } else {
        scaling = channel->mScalingKeys[0].mValue;
    }

    glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(position.x, position.y, position.z));
    glm::mat4 rotationMatrix = glm::mat4_cast(glm::quat(rotation.w, rotation.x, rotation.y, rotation.z));
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(scaling.x, scaling.y, scaling.z));

    return translation * rotationMatrix * scale;
}

// Function to update bone transformations
void UpdateBoneTransformations(const aiScene* scene, Animation& animation, double time, std::vector<glm::mat4>& boneTransforms) {
    for (auto& bone : animation.bones) {
        const aiNodeAnim* channel = animation.channels[bone.name];
        if (channel) {
            bone.finalTransformation = InterpolateTransform(channel, time);
        }
    }

    // Collect bone transformations
    boneTransforms.clear();
    for (const auto& bone : animation.bones) {
        boneTransforms.push_back(bone.finalTransformation);
    }
}

// Function to apply bone transformations to the vertices
void ApplyBoneTransformations(const std::vector<Bone>& bones, std::vector<float>& vertices) {
    // This is a simplified example, you need to implement the actual bone transformation application
    for (size_t i = 0; i < vertices.size(); i += 8) {
        glm::vec4 position(vertices[i], vertices[i + 1], vertices[i + 2], 1.0f);
        glm::vec4 transformedPosition = bones[0].finalTransformation * position;
        vertices[i] = transformedPosition.x;
        vertices[i + 1] = transformedPosition.y;
        vertices[i + 2] = transformedPosition.z;
    }
}

void DrawAxes(const glm::mat4& proj, const glm::mat4& view) {
    static constexpr float axisVertices[] = {
        // X axis (red)
        0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        150.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        // Y axis (green)
        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 150.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        // Z axis (blue)
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 150.0f, 0.0f, 0.0f, 1.0f
    };

    VertexArray va;
    VertexBuffer vb(axisVertices, sizeof(axisVertices));

    VertexBufferLayout layout;
    layout.Push<float>(3); // Position
    layout.Push<float>(3); // Color
    va.AddBuffer(vb, layout);

    auto model = glm::mat4(1.0f);
    glm::mat4 mvp = proj * view * model;

    // Shader axisShader("res/shaders/Axis.shader");
    Shader axisShader("/home/chrisvega/CLionProjects/ModernOpenGL/res/shaders/Axis.shader");
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
    glDepthFunc(GL_LESS);
    glDisable(GL_CULL_FACE);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    {
        Assimp::Importer importer;
        // const aiScene* scene = importer.ReadFile("/home/chrisvega/CLionProjects/ModernOpenGL/res/models/centaurwarrior.obj", aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        // const aiScene* scene = importer.ReadFile("/home/chrisvega/CLionProjects/ModernOpenGL/res/models/uploads_files_2787791_Mercedes+Benz+GLS+580.obj", aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        // const aiScene* scene = importer.ReadFile("/home/chrisvega/CLionProjects/ModernOpenGL/res/models/pilot.fbx",
        //                                   aiProcess_Triangulate |
        //                                   aiProcess_FlipUVs |
        //                                   aiProcess_CalcTangentSpace);

        // TODO: RECOVER THIS FUNCTIONALITY
        // const aiScene* scene = importer.ReadFile("/home/chrisvega/CLionProjects/ModernOpenGL/res/models/pilot.fbx",
        //                                   aiProcess_Triangulate |
        //                                   aiProcess_JoinIdenticalVertices |
        //                                   aiProcess_SortByPType);

        const aiScene* scene = importer.ReadFile("/home/chrisvega/CLionProjects/ModernOpenGL/res/models/Walking.fbx",
                                                 aiProcess_Triangulate |
                                                 aiProcess_FlipUVs |
                                                 aiProcess_CalcTangentSpace);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
            return -1;
        }

        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        aiMesh* mesh = scene->mMeshes[0];
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            vertices.push_back(mesh->mVertices[i].x);
            vertices.push_back(mesh->mVertices[i].y);
            vertices.push_back(mesh->mVertices[i].z);

            if (mesh->mTextureCoords[0]) {
                vertices.push_back(mesh->mTextureCoords[0][i].x);
                vertices.push_back(mesh->mTextureCoords[0][i].y);
            } else {
                vertices.push_back(0.0f);
                vertices.push_back(0.0f);
            }

            vertices.push_back(mesh->mNormals[i].x);
            vertices.push_back(mesh->mNormals[i].y);
            vertices.push_back(mesh->mNormals[i].z);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }

        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        VertexArray va;
        VertexBuffer vb(vertices.data(), vertices.size() * sizeof(float));

        VertexBufferLayout layout;
        layout.Push<float>(3); // Position
        layout.Push<float>(2); // Texture coordinates
        layout.Push<float>(3); // Normal
        layout.Push<int>(4);
        layout.Push<float>(4);
        va.AddBuffer(vb, layout);

        IndexBuffer ib(indices.data(), indices.size());

        glm::mat4 proj = glm::perspective(glm::radians(60.0f), 1920.0f / 1080.0f, 0.1f, 10000.0f);
        glm::mat4 view = glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

        Shader shader("/home/chrisvega/CLionProjects/ModernOpenGL/res/shaders/Basic.shader");
        shader.Bind();
        shader.SetUniform4f("u_Color", 1.0f, 1.0f, 1.0f, 1.0f); // White color

        // Light properties (adjust these based on your scene's brightness)
        shader.SetUniform3f("lightPos", 2.0f, 5.0f, 5.0f);
        shader.SetUniform3f("lightAmbient", .5f, .5f, .5f);
        shader.SetUniform3f("lightDiffuse", .5f, .5f, .5f);
        shader.SetUniform3f("lightSpecular", 1.0f, 1.0f, 1.0f);

        // Set material properties
        shader.SetUniform3f("matAmbient", 0.7f, 0.7f, 0.7f);
        shader.SetUniform3f("matDiffuse", 0.8f, 0.8f, 0.8f);
        shader.SetUniform3f("matSpecular", 1.0f, 1.0f, 1.0f);
        shader.SetUniform1f("matShininess", 32.0f);

        va.Unbind();
        shader.Unbind();
        vb.Unbind();
        ib.Unbind();

        Renderer renderer;

        ImGui::CreateContext();
        ImGui_ImplGlfwGL3_Init(window, true);
        ImGui::StyleColorsDark();

        glm::vec3 translation(0, 0, 0);
        glm::vec3 cameraPos(75.0f, 75.0f, 75.0f);
        glm::vec3 cameraFront(-1.0f, -1.0f, -1.0f);
        glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        float scale = 1.0f;

        bool isDragging = false;
        bool isCameraDragging = false;
        bool isCameraRotating = false;
        double lastMouseX, lastMouseY;
        float fov = 50.0f;

        // Load animation data
        Animation animation;
        if (scene->mAnimations[0]) {
            aiAnimation* aiAnim = scene->mAnimations[0];
            animation.duration = aiAnim->mDuration;
            animation.ticksPerSecond = aiAnim->mTicksPerSecond != 0 ? aiAnim->mTicksPerSecond : 25.0;

            for (unsigned int i = 0; i < aiAnim->mNumChannels; i++) {
                aiNodeAnim* channel = aiAnim->mChannels[i];
                animation.channels[channel->mNodeName.data] = channel;
            }

            for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
                aiMesh* mesh = scene->mMeshes[i];
                for (unsigned int j = 0; j < mesh->mNumBones; j++) {
                    aiBone* aiBone = mesh->mBones[j];
                    Bone bone;
                    bone.name = aiBone->mName.data;
                    bone.offsetMatrix = glm::transpose(glm::make_mat4(&aiBone->mOffsetMatrix.a1));
                    animation.bones.push_back(bone);
                }
            }
        }

        double animationTime = 0.0;
        std::vector<glm::mat4> boneTransforms(100); // Adjust the size as needed

        while (!glfwWindowShouldClose(window)) {
            renderer.Clear();

            DrawAxes(proj, view);

            ImGui_ImplGlfwGL3_NewFrame();

            {
                // glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0, -1.5, 0)) * glm::translate(glm::mat4(1.0f), translation) * glm::scale(glm::mat4(1.0f), glm::vec3(scale));
                glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0, -100.5, 0)) * glm::translate(glm::mat4(1.0f), translation) * glm::scale(glm::mat4(1.0f), glm::vec3(scale)); // ONLY FOR THE PILOT
                // glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.8, -1, 3.9)) * glm::translate(glm::mat4(1.0f), translation) * glm::scale(glm::mat4(1.0f), glm::vec3(scale)); // ONLY FOR THE CAR
                glm::mat4 mvp = proj * view * model;
                shader.Bind();
                shader.SetUniformMat4f("u_MVP", mvp);
                shader.SetUniformMat4f("u_Model", model);
                shader.SetUniformMat4f("u_View", view);
                shader.SetUniform3f("viewPos", cameraPos.x, cameraPos.y, cameraPos.z);

                // Update animation time
                animationTime += ImGui::GetIO().DeltaTime * animation.ticksPerSecond;
                if (animationTime > animation.duration) {
                    animationTime = 0.0;
                }

                // Update bone transformations
                UpdateBoneTransformations(scene, animation, animationTime, boneTransforms);

                // Pass bone transformations to the shader
                for (size_t i = 0; i < boneTransforms.size(); ++i) {
                    shader.SetUniformMat4f("u_BoneTransforms[" + std::to_string(i) + "]", boneTransforms[i]);
                }

                // Apply bone transformations to the vertices
                ApplyBoneTransformations(animation.bones, vertices);

                // Update vertex buffer with transformed vertices
                vb.Bind();
                glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());

                renderer.Draw(va, ib, shader);
            }

            {
                ImGui::SliderFloat3("Translation", &translation.x, -50.0f, 50.0f);
                ImGui::SliderFloat("Scale", &scale, 0.1f, 10.0f);
                ImGui::SliderFloat3("Camera Position", &cameraPos.x, -10.0f, 10.0f);
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            }

            // Capture mouse wheel input
            float mouseWheel = ImGui::GetIO().MouseWheel;
            // TODO: RECOVER THIS FUNCTIONALITY
            // if (mouseWheel != 0.0f) {
            //     if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
            //         cameraPos.y += mouseWheel;
            //     } else if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
            //         fov -= mouseWheel * 2.0f; // Adjust sensitivity as needed
            //         if (fov < 1.0f) fov = 1.0f;
            //         if (fov > 1500.0f) fov = 150.0f;
            //         proj = glm::perspective(glm::radians(fov), 1920.0f / 1080.0f, 0.1f, 1000.0f);
            //     } else {
            //         translation.y += mouseWheel;
            //     }
            // }

            // ONLY FOR TESTING
            if (mouseWheel != 0.0f) {
                fov -= mouseWheel * 2.0f; // Adjust sensitivity as needed
                if (fov < 1.0f) fov = 1.0f;
                if (fov > 1500.0f) fov = 1500.0f;
                proj = glm::perspective(glm::radians(fov), 1920.0f / 1080.0f, 0.1f, 10000.0f);
            }

            // TODO: RECOVER THIS FUNCTIONALITY
            // Handle mouse dragging
            // if (ImGui::IsMouseClicked(0)) {
            //     if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
            //         isCameraDragging = true;
            //     } else if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
            //         isCameraRotating = true;
            //     } else {
            //         isDragging = true;
            //     }
            //     glfwGetCursorPos(window, &lastMouseX, &lastMouseY);
            // }
            if (ImGui::IsMouseClicked(0)) {
                isCameraRotating = true;
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
