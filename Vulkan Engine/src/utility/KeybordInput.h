#pragma once
#include "core/ethereal_window.h"
#include "ECS/ethereal_entity.h"

namespace ethereal {

	class CameraController {
    public:
        struct KeyMappings {
            int moveLeft = GLFW_KEY_A;
            int moveRight = GLFW_KEY_D;
            int moveForward = GLFW_KEY_W;
            int moveBackward = GLFW_KEY_S;
            int moveUp = GLFW_KEY_SPACE;
            int moveDown = GLFW_KEY_LEFT_SHIFT;
            int lookLeft = GLFW_KEY_LEFT;
            int lookRight = GLFW_KEY_RIGHT;
            int lookUp = GLFW_KEY_UP;
            int lookDown = GLFW_KEY_DOWN;
        };

        void moveInPlaneXZ(GLFWwindow* window, float dt, Entity& gameobject);

        KeyMappings keys{};
        float moveSpeed{ 10.f };
        float lookSpeed{ 2.3f };
	};
  };