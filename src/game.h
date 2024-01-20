#ifndef GAME_H_
#define GAME_H_
#include "tetro.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <utility>

namespace tetriskl {
    class Tetris: sf::Drawable {
    private:
        StaticCellGrid<10, 30> cells;
        const static sf::Vector2u cells_render_start;
        Tetromino falling_piece;
        Tetromino next_piece;
        bool falling_piece_active;
        sf::Vector2u falling_piece_pos;
        sf::Time tick_period;
        sf::Clock tick_timer;
        sf::Clock evtloop_timer;
        bool game_over;
        unsigned int score;
        bool closed;

        const static sf::Time evtloop_period;
        TetrominoProvider provider;

        const sf::Font *font;

        const static sf::Time flash_period;
        const static unsigned int flash_times;
        constexpr static float tile_scale = 20.f;
        constexpr static float next_piece_box_size = 5.f;
        constexpr static unsigned int text_render_size = 30;
        constexpr static float game_over_text_size = 1.f;
        constexpr static float score_text_size = 1.f;
        constexpr static float vertical_score_padding = 0.5f;

        bool new_piece();
        void process_key(sf::RenderWindow &rw, sf::Keyboard::Key key);
        bool move(sf::Vector2i dir);
        void reset();
        void pause(sf::RenderWindow &rw);
        void close();
        void award_points(unsigned int lines_cleared);
        void clear_lines(sf::RenderWindow &rw);
        void flash_lines(sf::RenderWindow &rw, unsigned int *lines, std::size_t num_lines);
        void tick(sf::RenderWindow &rw);
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    public:
        Tetris();
        void set_font(const sf::Font &font);
        void run(sf::RenderWindow &rw);
    };
}
#endif
