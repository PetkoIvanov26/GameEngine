#include "game_engine_ui.h"

#include<iostream>
namespace lve {
    GameEngineUI::GameEngineUI(GLFWwindow* window, LveDevice& device, VkRenderPass renderPass, std::unique_ptr<LveDescriptorPool>& globalPool)
        : window(window), lveDevice(device), renderPass(renderPass), selectedObjectIndex(-1), globalPool(globalPool) {
        try {
            Initialize();
        }
        catch (const std::exception& e) {
            std::cerr << "Error during ImGui initialization: " << e.what() << std::endl;
            throw;
        }
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

    std::vector<std::string> GameEngineUI::listModelFiles(const std::string& directory) {
        std::vector<std::string> modelFiles;
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file() && entry.path().extension() == ".obj") {
                modelFiles.push_back(entry.path().string());
            }
        }
        return modelFiles;
    }

    void GameEngineUI::RenderUI(LveGameObject::Map& gameObjects, VkCommandBuffer commandBuffer) {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Object Manipulator");

        static int selectedModel = 0;
        modelFiles = listModelFiles("models");
        if (!modelFiles.empty()) {
            static int selectedModel = 0;

            if (ImGui::BeginCombo("Select Model", modelFiles[selectedModel].c_str())) {
                for (int i = 0; i < modelFiles.size(); i++) {
                    bool isSelected = (selectedModel == i);
                    if (ImGui::Selectable(modelFiles[i].c_str(), isSelected)) {
                        selectedModel = i;
                    }
                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            if (ImGui::Button("Load Selected Model")) {
                auto newObj = LveGameObject::createGameObject();
                if (selectedModel >= 0 && selectedModel < modelFiles.size()) {
                    newObj.model = LveModel::createModelFromFile(lveDevice, modelFiles[selectedModel]);
                    gameObjects.emplace(newObj.getId(), std::move(newObj));
                }
            }
        }
        else {
            ImGui::Text("No models found in the directory.");
        }

        // Remove Selected Object
        if (selectedObjectIndex != -1 && ImGui::Button("Remove Selected Object")) {
            vkDeviceWaitIdle(lveDevice.device());

            auto it = gameObjects.begin();
            std::advance(it, selectedObjectIndex);
            if (it != gameObjects.end()) {
                // Clean up Vulkan resources associated with the object
                if (it->second.model) {
                    it->second.model->cleanup();
                }

                gameObjects.erase(it);
                selectedObjectIndex = -1; // Reset selection
            }
        }
        if (ImGui::Button("Add Point Light")) {
            auto newLight = LveGameObject::makePointLight();
            gameObjects.emplace(newLight.getId(), std::move(newLight));
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

                    if (obj.pointLight) {
                        ImGui::ColorEdit3("Light Color", reinterpret_cast<float*>(&obj.color));
                        ImGui::SliderFloat("Light Intensity", &obj.pointLight->lightIntensity, 0.0f, 10.0f);
                    }

                    ImGui::SliderFloat("X", &obj.transform.translation.x, -10.0f, 10.0f);
                    ImGui::SliderFloat("Y", &obj.transform.translation.y, -10.0f, 10.0f);
                    ImGui::SliderFloat("Z", &obj.transform.translation.z, -10.0f, 10.0f);
                    ImGui::SliderFloat("Scale X", &obj.transform.scale.x, 0.1f, 10.0f);
                    ImGui::SliderFloat("Scale Y", &obj.transform.scale.y, 0.1f, 10.0f);
                    ImGui::SliderFloat("Scale Z", &obj.transform.scale.z, 0.1f, 10.0f);
                    ImGui::SliderFloat("Rotation X", &obj.transform.rotation.x, 0.1, 3.6);
                    ImGui::SliderFloat("Rotation Y", &obj.transform.rotation.y, 0.1, 3.6);
                    ImGui::SliderFloat("Rotation Z", &obj.transform.rotation.z, 0.1, 3.6);
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