#pragma once
#include "lve_device.h"
#include<string>
#include<vector>
namespace lve {

	struct PipelineConfigInfo{

		std::vector<VkVertexInputBindingDescription> bindingDescriptions{};
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
		VkPipelineViewportStateCreateInfo viewportInfo;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		std::vector<VkDynamicState> dynamicStateEnables;
		VkPipelineDynamicStateCreateInfo dynamicStateInfo;
		VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
		VkRenderPass renderPass = VK_NULL_HANDLE;
		uint32_t subpass = 0;
	};
	class LvePipeline
	{
	public:
		LvePipeline(LveDevice& device , const std::string& vertFilepath,const std::string& fragFilepath , const PipelineConfigInfo& configInfo);
		~LvePipeline();
		LvePipeline(const LvePipeline&) = delete;
		LvePipeline& operator=(const LvePipeline&) = delete;

		void bind(VkCommandBuffer commandBuffer);
		static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo);
	private:
		static std::vector<char> readFile(const std::string& filepath);
		void createGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath, const PipelineConfigInfo& configInfo);
		void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);
		LveDevice& lveDevice;
		VkPipeline graphicsPipeline;
		VkShaderModule vertShaderModule;
		VkShaderModule fragShaderModule;
	};
}

