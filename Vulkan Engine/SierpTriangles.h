//#pragma once
//
//void Application::sierpinski(
//	std::vector<EtherealModel::Vertex>& vertices,
//	int depth,
//	glm::vec2 left,
//	glm::vec2 right,
//	glm::vec2 top) {
//	if (depth <= 0) {
//		EtherealModel::Vertex a = { {top}, {1.0f, 0.0f, 0.0f} };
//		vertices.push_back(EtherealModel::Vertex(top, { 1.0f, 0.0f, 0.0f }));
//		vertices.push_back(EtherealModel::Vertex(right, { 0.0f, 1.0f, 0.0f }));
//		vertices.push_back(EtherealModel::Vertex(left, { 0.0f, 0.0f, 1.0f }));
//	}
//	else {
//		auto leftTop = 0.5f * (left + top);
//		auto rightTop = 0.5f * (right + top);
//		auto leftRight = 0.5f * (left + right);
//		sierpinski(vertices, depth - 1, left, leftRight, leftTop);
//		sierpinski(vertices, depth - 1, leftRight, right, rightTop);
//		sierpinski(vertices, depth - 1, leftTop, rightTop, top);
//	}
//}

//sierpinski(vertices, 5, glm::vec2(-0.5f, 0.5f), glm::vec2(0.5f, 0.5f), glm::vec2(0.0f, -0.5f));