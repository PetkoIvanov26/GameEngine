#pragma once
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include "lve_game_object.h"
#include <GLFW/glfw3.h>
#include "lve_descriptors.h"
#include <filesystem>
namespace lve {
    class GameEngineUI {
    public:
        GameEngineUI(GLFWwindow* window, LveDevice& device, VkRenderPass renderPass, std::unique_ptr<LveDescriptorPool>& globalPool);
        ~GameEngineUI();

        void RenderUI(LveGameObject::Map& gameObjects, VkCommandBuffer commandBuffer);
    private:
        GLFWwindow* window;
        LveDevice& lveDevice;
        VkRenderPass renderPass;
        int selectedObjectIndex;
        std::unique_ptr<LveDescriptorPool>& globalPool;
        std::vector<std::string> modelFiles; // List of model files

        void Initialize();
        void Shutdown();
        std::vector<std::string> listModelFiles(const std::string& directory);
    };
}