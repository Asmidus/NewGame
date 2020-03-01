#include "Systems.h"
#include "Components.h"
#include "entt/entt.hpp"
#include "Sprite.h"
#include "EventManager.h"
#include "InputManager.h"
#include "SpriteBatch.h"
#include "Camera.h"
#include "Program.h"
#include "Global.h"
//#include "AABBTree.h"

#include <iostream>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/vector_angle.hpp>
#include <algorithm>
#include <execution>



void Systems::drawSprites(SpriteBatch* batch) {
	updateAnimations();
	//render lights
	//for (auto& entity : Global::registry.group<Light>(entt::get<Transform>)) {
	//	auto [light, transform] = Global::registry.get<Light, Transform>(entity);
	//	_program->unuse();
	//	_lightEngine.Begin(light, transform);
	//	//TODO draw occluders ONCE and then translate the image for each light so we only draw occluders twice as opposed to
	//	//once per light and once again in the loop below  (this assumes a constant shadow resolution across light sizes)
	//	for (auto& entity : Global::registry.view<entt::tag<"Occluder"_hs>>()) {
	//		auto [sprite, pt] = Global::registry.get<Sprite, Transform>(entity);
	//		_lightEngine.DrawHull(&light, &transform, &sprite, &pt);
	//	}
	//	_lightEngine.End();
	//	_lightEngine.CreateShadows(&light);
	//	_camera->view();
	//	_program->use();
	//	_lightEngine.Draw(&light, &transform);
	//}
	//glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);

	//glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_ALPHA);
	batch->begin(GlyphSortType::BACK_TO_FRONT);
	Global::registry.group<Sprite>(entt::get<Transform>/*, entt::exclude<entt::tag<"Bright"_hs>>*/).each([=](auto entity, auto& sprite, auto& transform) {
		//TextureManager::Draw(sprite.texture, sprite.src, transform.rect, &transform.center, transform.angle, sprite.color);
		glm::vec4 t = glm::vec4(transform.rect.x, transform.rect.y, transform.rect.w, transform.rect.h);
		glm::vec4 u = sprite.getUV();
		if (transform.angle) {
			batch->draw(t, u, sprite.texture, transform.z, sprite.color, transform.angle, transform.center);
		} else {
			batch->draw(t, u, sprite.texture, transform.z, sprite.color);
		}
		auto text = Global::registry.try_get<Text>(entity);
		if (text) {
			text->dest.x = transform.rect.x + text->offset.x;
			text->dest.y = transform.rect.y + text->offset.y;
			t = glm::vec4(text->dest.x, text->dest.y, text->dest.w, text->dest.h);
			u = glm::vec4(0, 0, 1, 1);
			batch->draw(t, u, text->texture, transform.z, text->color);
			//TextureManager::DrawText(text->texture, text->dest);
		}
	});

	//batch->begin(GlyphSortType::BACK_TO_FRONT);
	//Global::registry.group<>(entt::get<Sprite, Transform, entt::tag<"Bright"_hs>>).each([batch](auto entity, auto& sprite, auto& transform, auto tag) {
	//	//TextureManager::Draw(sprite.texture, sprite.src, transform.rect, &transform.center, transform.angle, sprite.color);
	//	glm::vec4 t = glm::vec4(transform.rect.x, transform.rect.y, transform.rect.w, transform.rect.h);
	//	glm::vec4 u = sprite.getUV();
	//	if (transform.angle) {
	//		batch->draw(t, u, sprite.texture, transform.z, sprite.color, transform.angle, transform.center);
	//	} else {
	//		batch->draw(t, u, sprite.texture, transform.z, sprite.color);
	//	}
	//	//auto text = Global::registry.try_get<Text>(entity);
	//	//if (text) {
	//	//	text->dest.x = transform.rect.x + text->offset.x;
	//	//	text->dest.y = transform.rect.y + text->offset.y;
	//	//	TextureManager::DrawText(text->texture, text->dest);
	//	//}
	//});
	//batch->end();
	//batch->renderBatch();

	Global::registry.group<>(entt::get<Text>, entt::exclude<Transform>).each(
		[=](auto entity, auto& text) {
			//text->dest.x = transform.rect.x + text->offset.x;
			//text->dest.y = transform.rect.y + text->offset.y;
			auto t = glm::vec4(text.dest.x, text.dest.y, text.dest.w, text.dest.h);
			auto u = glm::vec4(0, 0, 1, 1);
			batch->draw(t, u, text.texture, 0, text.color);
		});
	batch->end();
	batch->renderBatch();
}

void Systems::updateAnimations() {
	Global::registry.group<>(entt::get<Sprite, Animation>).each(
		[this](auto& sprite, auto& animation) {
			if (animation.active) {
				animation.timeSinceLastFrame += _dt;
				if (animation.timeSinceLastFrame > animation.frameTime[animation.currentAnimation]) {
					unsigned int currFrame = 1 + (sprite.src.x / sprite.src.w);
					if (currFrame > animation.frames[animation.currentAnimation]) {
						currFrame = 0;
					}
					sprite.src.x = sprite.src.w * currFrame;
					sprite.src.y = sprite.src.h * animation.currentAnimation;
					animation.timeSinceLastFrame = 0;
				}
			} else {
				sprite.src.x = 0;
			}
		});
}

void Systems::moveEntities() {
	auto group = Global::registry.group<Transform, Velocity>();
	std::for_each(std::execution::par_unseq, group.begin(), group.end(), [=](auto entity) {
		auto [transform, velocity] = group.get<Transform, Velocity>(entity);
		//if (!velocity.constant) {
			glm::vec2 deltaVel = glm::vec2(0, 0);
			//if the entity is trying to accelerate in its given direction, apply that acceleration
			if (velocity.currAccel != 0 && glm::length(velocity.direction)) {
				deltaVel = glm::normalize(velocity.direction) * (velocity.currAccel);
				//if the entity is not trying to accelerate, decelerate it by its deceleration amount
			} else if ( !velocity.constant && velocity.decel && glm::length(velocity.currVel) > 0) {
				deltaVel = glm::normalize(velocity.currVel) * -(velocity.decel);
			}
			//apply the change in velocity
			velocity.currVel += (deltaVel * _dt);
			//if the entity's net speed is faster than the max, cap it
			if (glm::length(velocity.currVel) > velocity.maxSpeed) {
				velocity.currVel = glm::normalize(velocity.currVel) * velocity.maxSpeed;
				//if the entity's net speed is essentially zero, reset the current velocity vector
			} else if (glm::length(velocity.currVel) < velocity.decel * _dt) {
				velocity.currVel = glm::vec2(0, 0);
			}
		//}
		if (velocity.angular) {
			velocity.direction = glm::rotate<float>(velocity.direction, velocity.angular * _dt);
			glm::normalize(velocity.direction);
			transform.angle = glm::angle(velocity.direction, glm::vec2(1, 0));
			//make sure we can get angles larger than 180 degrees
			if (velocity.direction.y < 0) transform.angle *= -1;
		}
		//update the transform of the entity
		transform.updatePos(velocity.currVel * 120.0f * _dt);
	});
	//Global::registry.view<Transform>().each(
	//	[this](auto& transform) {
	//	if (transform.rect.x < -transform.rect.w) {
	//		transform.rect.x = Global::gameWidth;
	//	} else if (transform.rect.x > Global::gameWidth) {
	//		transform.rect.x = -transform.rect.w;
	//	}
	//	if (transform.rect.y < -transform.rect.h) {
	//		transform.rect.y = Global::gameHeight;
	//	} else if (transform.rect.y > Global::gameHeight) {
	//		transform.rect.y = -transform.rect.h;
	//	}
	//});
}

void Systems::checkCollisions() {
	std::shared_ptr<std::mutex> m = std::make_shared<std::mutex>();

	auto enemies = Global::registry.group<entt::tag<"Enemy"_hs>>(entt::get<Transform, Collider>);
	auto group = Global::registry.view<entt::tag<"Player"_hs>, Transform, Collider>();
	auto all = Global::registry.group<Collider>(entt::get<Transform>);
	//for(auto entity : all) {
	//	Global::collisionTree.updateObject(entity);
	//}
	//all.each([=](auto & entity1, auto & col1, auto & trans1) {
	//	auto collisions = Global::collisionTree.queryOverlaps(entity1);
	//	std::for_each(std::execution::par_unseq, collisions.begin(), collisions.end(), [=](auto entity2) {
	//		//if (entity1 != entity2) {
	//			auto [trans2, col2] = all.get<Transform, Collider>(entity2);
	//			glm::vec2 e1Pos = glm::vec2(trans1.center.x * trans1.rect.w + trans1.rect.x,
	//										trans1.center.y * trans1.rect.h + trans1.rect.y);
	//			glm::vec2 e2Pos = glm::vec2(trans2.center.x * trans2.rect.w + trans2.rect.x,
	//										trans2.center.y * trans2.rect.h + trans2.rect.y);
	//			if (glm::length(e1Pos - e2Pos) < col1.radius + col2.radius) {
	//				//auto cooldowns1 = Global::registry.try_get<Cooldown>(entity1);
	//				//auto cooldowns2 = Global::registry.try_get<Cooldown>(entity2);
	//				std::lock_guard<std::mutex> lock{ *m };
	//				_events->registerEvent(Event(Event::Type::collision, { entity1, entity2 }));
	//			}
	//		//}
	//	});
	//});
	//See if a simple 4-tile quad collision algorithm would be more efficient than brute force
	//if (true) {
		//static const glm::vec2 quadDims[4] = { glm::vec2(0, 0), glm::vec2(Global::gameWidth / 2, 0), glm::vec2(0, Global::gameHeight / 2), glm::vec2(Global::gameWidth / 2, Global::gameHeight / 2) };
		//static const std::vector<unsigned int> quads = { 0, 1, 2, 3 };
		//auto colliders = Global::registry.group<Collider>(entt::get<Transform>);
		//std::for_each(std::execution::par_unseq, quads.begin(), quads.end(), [=](auto i) {
		//	std::vector<const entt::entity*> quadrant;
		//	auto test = &quadrant;
		//	//For some reason the debug compiler needs quadDims to be used in this lambda so it can be used in the next one
		//	//hopefully this line is just optimized out in release
		//	quadDims;
		//	std::shared_ptr<std::mutex> m2 = std::make_shared<std::mutex>();
		//	for (auto& entity : colliders) {
		//		auto& transform = Global::registry.get<Transform>(entity);
		//		if (transform.rect.x + transform.rect.w > quadDims[i].x && transform.rect.x < quadDims[i].x + Global::gameWidth / 2 &&
		//			transform.rect.y + transform.rect.h > quadDims[i].y && transform.rect.y < quadDims[i].y + Global::gameHeight / 2) {
		//			std::lock_guard<std::mutex> lock{ *m2 };
		//			(*test).push_back(&entity);
		//		}
		//	}
		//	for (unsigned int j = 0; j < quadrant.size(); j++) {
		//		auto entity1 = quadrant[j];
		//		//if (Global::registry.has<entt::tag<"Player"_hs>>(*entity1)) {
		//			auto [trans1, col1] = Global::registry.get<Transform, Collider>(*entity1);
		//			for (unsigned int k = j+1; k < quadrant.size(); k++) {
		//				auto entity2 = quadrant[k];
		//				//if (Global::registry.has<entt::tag<"Enemy"_hs>>(*entity2)) {
		//					auto [trans2, col2] = Global::registry.get<Transform, Collider>(*entity2);
		//					if (col1.circular) {
		//						glm::vec2 e1Pos = glm::vec2(trans1.center.x * trans1.rect.w + trans1.rect.x,
		//													trans1.center.y * trans1.rect.h + trans1.rect.y);
		//						glm::vec2 e2Pos = glm::vec2(trans2.center.x * trans2.rect.w + trans2.rect.x,
		//													trans2.center.y * trans2.rect.h + trans2.rect.y);
		//						if (glm::length(e1Pos - e2Pos) < col1.radius + col2.radius) {
		//							auto cooldowns1 = Global::registry.try_get<Cooldown>(*entity1);
		//							auto cooldowns2 = Global::registry.try_get<Cooldown>(*entity2);
		//							if ((!cooldowns1 || cooldowns1->trigger(Event::Type::collision)) &&
		//								(!cooldowns2 || cooldowns2->trigger(Event::Type::collision))) {
		//								std::lock_guard<std::mutex> lock{ *m };
		//								_events->registerEvent(Event(Event::Type::collision, { *entity1, *entity2 }));
		//							}
		//						}
		//					}
		//				//}
		//			}
		//		//}
		//	}
		//});
	//} else {
	for (unsigned int i = 0; i < all.size(); ++i) {
		entt::entity entity1 = all[i];
		auto [trans1, col1] = all.get<Transform, Collider>(entity1);
		for (unsigned int j = i + 1; j < all.size(); ++j) {
			entt::entity entity2 = all[j];
			auto [trans2, col2] = all.get<Transform, Collider>(entity2);
			glm::vec2 e1Pos = glm::vec2(trans1.center.x * trans1.rect.w + trans1.rect.x,
										trans1.center.y * trans1.rect.h + trans1.rect.y);
			glm::vec2 e2Pos = glm::vec2(trans2.center.x * trans2.rect.w + trans2.rect.x,
										trans2.center.y * trans2.rect.h + trans2.rect.y);
			if (glm::length(e1Pos - e2Pos) < col1.radius + col2.radius) {
				//auto cooldowns1 = Global::registry.try_get<Cooldown>(entity1);
				//auto cooldowns2 = Global::registry.try_get<Cooldown>(entity2);
				std::lock_guard<std::mutex> lock{ *m };
				_events->registerEvent(Event(Event::Type::collision, { entity1, entity2 }));
			}
		}
	}
		//all.each([=](auto& entity1,/* auto tag,*/ auto& col1, auto& trans1) {
		//	std::for_each(std::execution::par_unseq, all.begin(), all.end(), [=](auto entity2) {
		//		if (entity1 != entity2) {
		//			auto [trans2, col2] = all.get<Transform, Collider>(entity2);
		//			glm::vec2 e1Pos = glm::vec2(trans1.center.x * trans1.rect.w + trans1.rect.x,
		//										trans1.center.y * trans1.rect.h + trans1.rect.y);
		//			glm::vec2 e2Pos = glm::vec2(trans2.center.x * trans2.rect.w + trans2.rect.x,
		//										trans2.center.y * trans2.rect.h + trans2.rect.y);
		//			if (glm::length(e1Pos - e2Pos) < col1.radius + col2.radius) {
		//				//auto cooldowns1 = Global::registry.try_get<Cooldown>(entity1);
		//				//auto cooldowns2 = Global::registry.try_get<Cooldown>(entity2);
		//				std::lock_guard<std::mutex> lock{ *m };
		//				_events->registerEvent(Event(Event::Type::collision, { entity1, entity2 }));
		//			}
		//		}
		//	});
		//});
	//}
}

void Systems::checkInput(Camera* currCam) {
	//decrement any cooldowns for all entities before checking input
	Global::registry.view<Cooldown>().each(
		[this](auto & cooldown) {
			cooldown.decrementCooldowns(_dt);
		});
	SDL_Event evnt;
	while (SDL_PollEvent(&evnt)) {
		switch (evnt.type) {
		case SDL_MOUSEMOTION:
			_inputs->setMouseCoords(evnt.motion.x, evnt.motion.y);
			break;
		case SDL_KEYDOWN:
			_inputs->pressKey(evnt.key.keysym.sym);
			break;
		case SDL_KEYUP:
			_inputs->releaseKey(evnt.key.keysym.sym);
			break;
		case SDL_MOUSEBUTTONDOWN:
			_inputs->pressKey(evnt.button.button);
			break;
		case SDL_MOUSEBUTTONUP:
			_inputs->releaseKey(evnt.button.button);
			break;
		}
	}
	//For all mouse events, find every mouse listener
	//and find out what event is tied to the button that is pressed and queue the event
	auto mouseView = Global::registry.view<MouseListener>();
	for (auto entity : mouseView) {
		for (auto key : _inputs->getPressedKeys()) {
			if (mouseView.get(entity).map.find(key) != mouseView.get(entity).map.end()) {
				auto cooldowns = Global::registry.try_get<Cooldown>(entity);
				auto eventType = mouseView.get(entity).map[key];
				if (cooldowns && !cooldowns->trigger(eventType)) {
					continue;
				}
				_events->registerEvent(Event(eventType, entity, currCam->convertScreenToWorld(_inputs->getMouseCoords())));
			}
		}
	}

	Global::registry.view<KeyListener>().each([=](auto& listener) {
		for (auto key : _inputs->getPressedKeys()) {
			if (listener.map.find(key) != listener.map.end()) {
				auto action = listener.map[key];
				if (listener.enabled[key]) {
					listener.enabled[key] = action(true);
				}
			}
		}
		for (auto key : _inputs->getReleasedKeys()) {
			if (listener.map.find(key) != listener.map.end()) {
				auto action = listener.map[key];
				action(false);
				listener.enabled[key] = true;
			}
		}
	});
	_inputs->refresh();
}

void Systems::checkLifetimes() {
	Global::registry.view<Lifetime>().each([this](auto & entity, auto & lifetime) {
		lifetime.timeLeft -= _dt;
		if (lifetime.timeLeft <= 0) {
			Global::registry.destroy(entity);
		}
	});
	Global::registry.view<Health>().each([this](auto & entity, auto & health) {
		if (health.current <= 0) {
			Global::registry.destroy(entity);
		} else if(Global::registry.has<entt::tag<"SpriteHealth"_hs>>(entity)) {
			auto& sprite = Global::registry.get<Sprite>(entity);
			sprite.color = glm::vec3(255, 255 * health.current / health.max, 255 * health.current / health.max);
		}
	});
}

void Systems::spawnAsteroids() {
	Global::registry.view<AsteroidSpawner>().each(
		[this](auto& entity, auto& spawner) {
			if (Global::registry.view<entt::tag<"Enemy"_hs>>().empty()) {
				for (int i = 0; i < spawner.numAsteroids; i++) {
					AssetManager::createBasicEnemy();
				}
				spawner.numAsteroids += spawner.increment;
			}
		});
}