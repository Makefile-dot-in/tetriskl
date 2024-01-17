#include "game.h"
#include "tetro.h"

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <iostream>
#include <utility>

namespace tetriskl {
    const sf::Vector2u Tetris::cells_render_start{0, 10};
    const sf::Time Tetris::evtloop_period = sf::seconds(1.f)/20.f;
    const sf::Time Tetris::flash_period = sf::seconds(0.1f);
    const unsigned int Tetris::flash_times = 5;

    bool Tetris::new_piece() {
        falling_piece = next_piece;
        falling_piece_pos = sf::Vector2u(3, 9);
        falling_piece_active = true;
        next_piece = provider.next();
        return cells.can_place(falling_piece_pos, falling_piece);
    }

    void Tetris::process_key(sf::RenderWindow &rw, sf::Keyboard::Key key) {
        tick_timer.restart();
        if (!game_over) {
            switch (key) {
            case sf::Keyboard::Up:
                falling_piece.rotate_ccw(cells, falling_piece_pos);
                break;
            case sf::Keyboard::Down:
                move(sf::Vector2i(0, 1));
                break;
            case sf::Keyboard::Space:
                while (move(sf::Vector2i(0, 1)));
                tick(rw);
                break;
            case sf::Keyboard::Left:
                move(sf::Vector2i(-1, 0));
                break;
            case sf::Keyboard::Right:
                move(sf::Vector2i(1, 0));
                break;
            default:;
            }
        } else {
            switch (key) {
            case sf::Keyboard::Space:
                reset();
                break;
            default:;
            }
        }
    }


    bool Tetris::move(sf::Vector2i dir) {
        sf::Vector2i new_pos = sf::Vector2i(falling_piece_pos) + dir;
        if (new_pos.x < 0 || new_pos.y < 0) return false;
        sf::Vector2u unew_pos = sf::Vector2u(new_pos);
        if (!cells.can_place(unew_pos, falling_piece)) return false;
        falling_piece_pos = unew_pos;
        return true;
    }

    void Tetris::reset() {
        Tetris new_this;
        if (this->font != nullptr)
            new_this.set_font(*this->font);

        *this = std::move(new_this);
    }

    void Tetris::award_points(unsigned int lines_cleared) {
        switch (lines_cleared) {
        case 0: break;
        case 1: score += 100; break;
        case 2: score += 300; break;
        case 3: score += 500; break;
        case 4: score += 800; break;
        default: score += 200 * lines_cleared;
        }
    }

    void Tetris::clear_lines(sf::RenderWindow &rw) {
        sf::Vector2u grid_size = cells.size();
        std::size_t num_cleared_lines = 0;
        unsigned int cleared_lines[decltype(Tetris::cells)::rows];
        auto it = cells.begin();
        for (std::size_t y = 0; y < grid_size.y; y++) {
            bool line_filled = std::all_of(it[y].begin(), it[y].end(), [] (Cell c) { return c != Cell::N; });
            if (line_filled)
                cleared_lines[num_cleared_lines++] = y;
        }

        award_points(num_cleared_lines);
        flash_lines(rw, cleared_lines, num_cleared_lines);

        for (std::size_t i = 0; i < num_cleared_lines; i++) {
            std::copy_backward(it, it + cleared_lines[i], it + cleared_lines[i] + 1);
            it->fill(Cell::N);
        }
    }

    void Tetris::flash_lines(sf::RenderWindow &rw, unsigned int *lines, std::size_t num_lines) {
        decltype(Tetris::cells) flash_buf = this->cells;
        sf::Vector2u flash_buf_size = flash_buf.size();
        for (std::size_t i = 0; i < num_lines; i++)
            for (std::size_t x = 0; x < flash_buf_size.x; x++)
                flash_buf[sf::Vector2u(x, lines[i])] = Cell::N;

        for (unsigned int i = 0; i < flash_times; i++) {
            std::swap(cells, flash_buf);
            rw.draw(*this);
            rw.display();
            sf::sleep(flash_period);
        }
    }

    void Tetris::tick(sf::RenderWindow &rw) {
        if (game_over) return;
        bool successful_fall = this->move(sf::Vector2i(0, 1));
        if (!successful_fall) {
            // piece has fallen down completely
            cells.place(falling_piece_pos, falling_piece);
            falling_piece_active = false;
            clear_lines(rw);
            if (!new_piece()) game_over = true;
        }
    }

    Tetris::Tetris()
        : cells(),
          tick_period(sf::seconds(0.5f)),
          tick_timer(),
          evtloop_timer(),
          game_over(false),
          score(0),
          provider() {
        for (int i = 0; i < 2; i++)
            new_piece();
    }

    void Tetris::set_font(const sf::Font &font) {
        this->font = &font;
    }

    void Tetris::run(sf::RenderWindow &rw) {
        while (rw.isOpen()) {
            evtloop_timer.restart();

            sf::Event ev;
            while (rw.pollEvent(ev)) {
                switch (ev.type) {
                case sf::Event::Closed:
                    rw.close();
                    break;
                case sf::Event::KeyPressed:
                    process_key(rw, ev.key.code);
                    break;
                default:;
                }
            }

            if (tick_timer.getElapsedTime() > tick_period) {
                this->tick(rw);
                tick_timer.restart();
            }

            rw.draw(*this);
            rw.display();

            sf::sleep(evtloop_period - evtloop_timer.getElapsedTime());
        }
    }
}
