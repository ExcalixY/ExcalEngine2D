#include "ExcalCore/ECS/Systems/SpriteRenderer.hpp"

#include <SDL3/SDL_rect.h>

#include <cmath>
#include <glm/ext/matrix_float3x3.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>

#include "ExcalCore/ECS/Components/ObjectEnabledComponent.hpp"
#include "ExcalCore/ECS/Components/SpriteRenderer.hpp"
#include "ExcalCore/ECS/Components/Transform.hpp"
#include "ExcalCore/ECS/ECSRegistry.hpp"
#include "ExcalCore/ECS/Entity.hpp"
#include "ExcalCore/Rendering/Renderer2D.hpp"

namespace ExcalCore::ECSSystems {

void IssueDrawCommands(ECSRegistry& ecs, Renderer2D& renderer) {
  ecs.Each<WorldTransform, SpriteRenderer, ObjectEnabledComponent>(
    [&renderer](
      Entity,
      const WorldTransform* t,
      SpriteRenderer* s,
      ObjectEnabledComponent*
    ) {
      if (!s->texture) {
        return;
      }

      const glm::mat3& m = t->worldMatrix;

      glm::vec2 position{m[2][0], m[2][1]};
      glm::vec2 scale{
        glm::length(glm::vec2(m[0])), glm::length(glm::vec2(m[1]))
      };
      float angle = std::atan2(m[0][1], m[0][0]);

      glm::vec2 source_size = s->source ? glm::vec2{s->source->w, s->source->h}
                                        : s->texture->GetSize();
      glm::vec2 size = source_size * scale;
      glm::vec2 pivot_px = s->pivot * size;

      renderer.SubmitDrawCommand({
        .texture = s->texture.get(),
        .source = s->source,
        .destination =
          {position.x - pivot_px.x, position.y - pivot_px.y, size.x, size.y},
        .rotation = glm::degrees(angle),
        .pivot = SDL_FPoint{pivot_px.x, pivot_px.y},
        .flip = s->flip,
        .color = s->color,
        .layer = s->layer,
      });
    }
  );
}

}
