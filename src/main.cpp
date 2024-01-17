#include "game.h"
#include "menu.h"
#include "dirs.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>

int main(int argc, const char *argv[]) {
    sf::RenderWindow window(sf::VideoMode(640, 480), "tetriskl");
    tetriskl::ResourceLocator locator(argc, argv);
    std::string font_path = locator.get_asset_path("font.ttf");
    sf::Font font;
    if (!font.loadFromFile(font_path)) {
        return EXIT_FAILURE;
    }
    tetriskl::Tetris game;
    game.set_font(font);
    game.run(window);
    return EXIT_SUCCESS;
}
