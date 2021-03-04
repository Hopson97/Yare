#pragma once

#include <array>
#include <glm/glm.hpp>

class ViewFrustum {
    struct Plane {
        float distanceToPoint(const glm::vec3& point) const;

        float distanceToOrigin;
        glm::vec3 normal;
    };

  public:
    void update(const glm::mat4& projViewMatrix) noexcept;

    bool terrainInView(glm::vec2 position) const noexcept;

  private:
    std::array<Plane, 6> m_planes;
};