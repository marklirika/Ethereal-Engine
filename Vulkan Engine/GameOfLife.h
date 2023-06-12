//#pragma once
//bool** field = nullptr;
//int generation = 0;
//int scale = 2;
//int density = 1000;
//int row = 800 / scale;
//int col = 600 / scale;
//field = new bool* [row];
//for (int i = 0; i < row; i++) {
//    field[i] = new bool[col];
//    for (int j = 0; j < col; j++) {
//        field[i][j] = (rand() % density == 0);
//    }
//}
//std::shared_ptr<EtherealModel> squareModel = createSquareModel(etherealDevice);
//EtherealRenderSystem etherealRenderSystem{ etherealDevice, etherealRenderer.getSwapChainRenderPass() };
//
//while (!etherealWindow.shouldClose()) {
//    gameObjects.clear();
//    gameObjects.shrink_to_fit();
//
//    if (generation > 0) {
//        bool** newfield;
//        newfield = new bool* [row];
//        for (int i = 0; i < row; i++) {
//            newfield[i] = new bool[col];
//        }
//        for (int i = 0; i < row; i++)
//            for (int j = 0; j < col; j++) {
//                int countNeighbours = neighbours(field, i, j, row, col);
//                if (!field[i][j] && countNeighbours == 3) {
//                    newfield[i][j] = true;
//
//                }
//                else if (field[i][j] && (countNeighbours > 3) || countNeighbours < 2) {
//                    newfield[i][j] = true;
//                }
//                else {
//                    newfield[i][j] = field[i][j];
//                }
//            }
//        for (int i = 0; i < row; i++) {
//            delete[]field[i];
//        }
//        delete[]field;
//        newfield = nullptr;
//        field = newfield;
//    }
//
//    for (int i = 0; i < row; i++) {
//        for (int j = 0; j < col; j++) {
//            if (field[i][j]) {
//                auto sqr = EtherealGameObject::createGameObject();
//                sqr.transform2d.scale = glm::vec2(0.005f * scale);
//                sqr.transform2d.translation = {
//                    -1.0f + (i + 0.5f) * 2.0f / row,
//                    -1.0f + (j + 0.5f) * 2.0f / col };
//                sqr.color = glm::vec3(0.5f, 0.5f, 0.5f);
//                sqr.model = squareModel;
//                gameObjects.push_back(std::move(sqr));
//            }
//        }
//    }
//    generation++;