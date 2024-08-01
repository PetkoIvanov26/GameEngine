#include "game_engine_ui.h"

namespace lve {
    GameEngineUI::GameEngineUI(GLFWwindow* window, LveDevice& device, VkRenderPass renderPass , std::unique_ptr<LveDescriptorPool>& globalPool)
        : window(window), lveDevice(device), renderPass(renderPass), selectedObjectIndex(-1), globalPool(globalPool) {
        Initialize();
    }

    GameEngineUI::~GameEngineUI()
    {
        Shutdown();
    }

    void GameEngineUI::Initialize() {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForVulkan(window, true);

        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = lveDevice.getInstance();
        init_info.PhysicalDevice = lveDevice.getPhysicalDevice();
        init_info.Device = lveDevice.device();
        init_info.QueueFamily = lveDevice.getQueueFamilyIndices().graphicsFamily;
        init_info.Queue = lveDevice.graphicsQueue();
        init_info.PipelineCache = VK_NULL_HANDLE;
        init_info.DescriptorPool = globalPool->getPool(); // Use the descriptor pool from LveDescriptorPool
        init_info.MinImageCount = 2;
        init_info.ImageCount = 3;
        init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        init_info.Allocator = nullptr;
        init_info.CheckVkResultFn = nullptr;
        init_info.RenderPass = renderPass;

        ImGui_ImplVulkan_Init(&init_info);

        VkCommandBuffer command_buffer = lveDevice.beginSingleTimeCommands();
        ImGui_ImplVulkan_CreateFontsTexture();
        lveDevice.endSingleTimeCommands(command_buffer);
        ImGui_ImplVulkan_DestroyFontsTexture();
    }

    void GameEngineUI::Shutdown() {
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void GameEngineUI::RenderUI(LveGameObject::Map& gameObjects, VkCommandBuffer commandBuffer) {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Object Manipulator");

        if (ImGui::Button("Insert Object")) {
            auto newObj = LveGameObject::createGameObject();
            gameObjects.emplace(newObj.getId(), std::move(newObj));
        }

        if (ImGui::BeginListBox("Objects")) {
            int index = 0;
            for (auto& [id, obj] : gameObjects) {
                if (ImGui::Selectable(std::to_string(id).c_str(), selectedObjectIndex == index)) {
                    selectedObjectIndex = index;
                }
                ++index;
            }
            ImGui::EndListBox();
        }

        if (selectedObjectIndex != -1) {
            int index = 0;
            for (auto& [id, obj] : gameObjects) {
                if (index == selectedObjectIndex) {
                    ImGui::Text("Selected Object: %d", id);
                    ImGui::SliderFloat("X", &obj.transform.translation.x, -10.0f, 10.0f);
                    ImGui::SliderFloat("Y", &obj.transform.translation.y, -10.0f, 10.0f);
                    ImGui::SliderFloat("Z", &obj.transform.translation.z, -10.0f, 10.0f);
                    ImGui::SliderFloat("Scale X", &obj.transform.scale.x, 0.1f, 10.0f);
                    ImGui::SliderFloat("Scale Y", &obj.transform.scale.y, 0.1f, 10.0f);
                    ImGui::SliderFloat("Scale Z", &obj.transform.scale.z, 0.1f, 10.0f);
                    ImGui::SliderFloat("Rotation X", &obj.transform.rotation.x, 0.0f, 360.0f);
                    ImGui::SliderFloat("Rotation Y", &obj.transform.rotation.y, 0.0f, 360.0f);
                    ImGui::SliderFloat("Rotation Z", &obj.transform.rotation.z, 0.0f, 360.0f);
                    break;
                }
                ++index;
            }
        }

        ImGui::End();

        ImGui::Render();
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
    }
}