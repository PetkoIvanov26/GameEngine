#pragma once
#include "lve_window.h"
#include "lve_device.h"
#include "lve_game_object.h"
#include "lve_renderer.h"

#include <memory>
#include <vector>
namespace lve {
	class FirstApp
	{
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		FirstApp();
		~FirstApp();

		FirstApp(const FirstApp&) = delete;
		FirstApp& operator=(const FirstApp&) = delete;

		void run();
	private:
		void loadGameObjects();

		LveWindow lveWindow{ WIDTH, HEIGHT , "Game Engine" };
		LveDevice lveDevice{ lveWindow };
		LveRenderer lveRenderer{ lveWindow ,lveDevice };
		std::vector<LveGameObject> gameObjects;
	};
}
