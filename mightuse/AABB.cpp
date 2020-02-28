#include "AABB.h"
#include "Global.h"
#include "Transform.h"

AABB::AABB(entt::entity entity) {
	auto trans = Global::registry.get<Transform>(entity);
	minX = trans.rect.x;
	minY = trans.rect.y;
	maxX = trans.rect.x + trans.rect.w;
	maxY = trans.rect.y + trans.rect.y;
	area = calculateArea();
}