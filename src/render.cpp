#include "tetro.h"
#include "game.h"
#include "menu.h"
#include <iostream>
#include <iomanip>
#include <sstream>

#include <SFML/Graphics.hpp>
namespace tetriskl {
    const sf::Color background_color = sf::Color(0x141414ff);
    const sf::Color text_color = sf::Color(0xe6e6e6ff);
    const sf::Color inactive_text_color = sf::Color(0x9e9e9eff);

    void CellGrid::draw(sf::RenderTarget &target, sf::RenderStates states) const {
        sf::Vector2u grid_size = this->size();
        for (std::size_t y = 0; y < grid_size.y; y++) {
            for (std::size_t x = 0; x < grid_size.x; x++) {
                sf::RectangleShape rect;
                Cell cell = (*this)[sf::Vector2u(x, y)];
                rect.setSize(sf::Vector2f(1.f, 1.f));
                rect.setPosition(x, y);
                rect.setFillColor(cell_colors[(int)cell]);
                rect.setOutlineThickness(0.03f);
                rect.setOutlineColor(outline_color);
                target.draw(rect, states);
            }
        }
    }

    void Tetris::draw(sf::RenderTarget& target, sf::RenderStates states) const {
        target.clear(background_color);
        const ConstGridView visible_cells(cells, Tetris::cells_render_start, cells.size());

        sf::Vector2f view_size = target.getView().getSize();
        sf::Vector2f view_center = view_size/2.f;
        sf::Transform cells_scale;
        cells_scale.scale(Tetris::tile_scale, Tetris::tile_scale);
        float cells_width = visible_cells.size().x + Tetris::next_piece_box_size;
        float cells_height = visible_cells.size().y;
        sf::Vector2f cells_drawcenter = cells_scale * (sf::Vector2f(cells_width, cells_height)/2.f);
        sf::RenderStates cstates = states;
        cstates.transform.translate(view_center - cells_drawcenter);
        cstates.transform *= cells_scale;

        // draw main grid/game over display
        if (!game_over) {
            target.draw(visible_cells, cstates);

            // draw falling piece
            if (falling_piece_active) {
                sf::RenderStates falling_piece_states = cstates;
                falling_piece_states.transform.translate(sf::Vector2f(falling_piece_pos)
                                                         - sf::Vector2f(Tetris::cells_render_start));

                target.draw(falling_piece, falling_piece_states);
            }
        } else {
            sf::Text game_over_text("Game over!", *this->font, Tetris::text_render_size);
            game_over_text.setFillColor(text_color);
            sf::FloatRect game_over_text_local_bounds = game_over_text.getLocalBounds();
            float game_over_text_scale = Tetris::game_over_text_size/game_over_text_local_bounds.height;
            game_over_text.setScale(game_over_text_scale, game_over_text_scale);
            sf::FloatRect game_over_text_bounds = game_over_text.getGlobalBounds();
            sf::Vector2f game_over_text_pos = sf::Vector2f(visible_cells.size())/2.f
                - sf::Vector2f(game_over_text_bounds.width, game_over_text_bounds.height)/2.f
                - sf::Vector2f(game_over_text_bounds.left, game_over_text_bounds.height);
            game_over_text.setPosition(game_over_text_pos);
            target.draw(game_over_text, cstates);
        }

        // draw next piece display

        // the box around the piece
        sf::RenderStates next_piece_box_states = cstates;
        next_piece_box_states.transform.translate(sf::Vector2f(visible_cells.size().x, 0));
        sf::RectangleShape next_piece_box;
        next_piece_box.setSize(sf::Vector2f(Tetris::next_piece_box_size, Tetris::next_piece_box_size));
        next_piece_box.setOutlineColor(outline_color);
        next_piece_box.setOutlineThickness(0.03f);
        next_piece_box.setFillColor(sf::Color::Transparent);
        target.draw(next_piece_box, next_piece_box_states);

        // the piece itself
        sf::RenderStates next_piece_states = next_piece_box_states;
        next_piece_states.transform.translate(next_piece_box.getSize()/2.f - sf::Vector2f(next_piece.size())/2.f);
        target.draw(next_piece, next_piece_states);

        // draw score display
        sf::RenderStates score_display_states = next_piece_box_states;
        score_display_states.transform.translate(0, next_piece_box.getSize().y);

        // convert score to string
        std::stringstream score_string;
        score_string << std::setw(6) << std::setfill('0') << score;

        // create sf::Text object
        sf::Text score_text(score_string.str(), *this->font, Tetris::text_render_size);
        score_text.setFillColor(text_color);
        sf::FloatRect score_text_local_bounds = score_text.getLocalBounds();
        float score_text_scale = Tetris::score_text_size/score_text_local_bounds.height;
        score_text.setScale(score_text_scale, score_text_scale);
        sf::FloatRect score_text_bounds = score_text.getGlobalBounds();

        // create box
        sf::RectangleShape score_text_box;
        score_text_box.setSize(sf::Vector2f(Tetris::next_piece_box_size,
                                            score_text_bounds.height + 2.f * vertical_score_padding));
        score_text_box.setOutlineColor(outline_color);
        score_text_box.setOutlineThickness(0.03f);
        score_text_box.setFillColor(sf::Color::Transparent);

        // draw box
        target.draw(score_text_box, score_display_states);

        // draw score
        score_text.setPosition(score_text_box.getSize()/2.f
                               - sf::Vector2f(score_text_bounds.width, score_text_bounds.height)/2.f
                               - sf::Vector2f(score_text_bounds.left, score_text_bounds.top));
        target.draw(score_text, score_display_states);
    }

    sf::FloatRect MenuAction::get_bounds() const {
        sf::Text active_indicator(">", *this->font, MenuAction::character_size);
        sf::Text action_text(this->text(), *this->font, MenuAction::character_size);

        sf::FloatRect active_indicator_bounds = active_indicator.getGlobalBounds();
        sf::FloatRect action_text_bounds = action_text.getGlobalBounds();

        float width = active_indicator_bounds.width + action_text_bounds.width + MenuAction::indicator_padding;
        float height = std::max(active_indicator_bounds.height, action_text_bounds.height) + 2*MenuAction::vertical_padding;
        return sf::FloatRect(0, 0, width, height);
    }

    void MenuAction::draw(sf::RenderTarget &target, sf::RenderStates states) const {
        sf::Text active_indicator(">", *this->font, MenuAction::character_size);
        sf::Text action_text(this->text(), *this->font, MenuAction::character_size);

        sf::Color color = this->active ? text_color : inactive_text_color;
        active_indicator.setFillColor(color);
        action_text.setFillColor(color);

        sf::FloatRect active_indicator_bounds = active_indicator.getGlobalBounds();
        sf::FloatRect action_text_bounds = action_text.getGlobalBounds();
        sf::FloatRect menu_action_bounds = this->get_bounds();

        active_indicator.setPosition(0, menu_action_bounds.height/2.f - action_text_bounds.height/2.f);
        action_text.setPosition(active_indicator_bounds.width + indicator_padding,
                                menu_action_bounds.height/2.f - action_text_bounds.height/2.f);

        if (this->active) target.draw(active_indicator, states);
        target.draw(action_text, states);
    }

    void Menu::draw(sf::RenderTarget &target, sf::RenderStates states) const {
        target.clear(background_color);
        std::size_t page = active_item_idx / Menu::actions_per_page;
        std::size_t action_range_start_idx = page * Menu::actions_per_page;
        std::size_t action_range_end_idx = std::min(action_range_start_idx + Menu::actions_per_page, actions.size());

        sf::Text title((this->title == nullptr) ? "" : this->title, *this->font, Menu::title_size);
        title.setFillColor(text_color);
        sf::FloatRect title_bounds = title.getGlobalBounds();
        float title_height = title_bounds.height + Menu::title_padding;

        float width = 0;
        float height = 0;
        if (this->title != nullptr) {
            height += title_height;
            width = std::max(width, title_bounds.width);
        }
        for (std::size_t i = action_range_start_idx; i < action_range_end_idx; i++) {
            actions[i]->set_font(*font);
            sf::FloatRect bounds = actions[i]->get_bounds();
            width = std::max(width, bounds.width);
            height += bounds.height;
        }

        sf::Vector2f view_size = target.getView().getSize();
        states.transform.translate(view_size/2.f - sf::Vector2f(width, height)/2.f);


        if (this->title != nullptr) {
            target.draw(title, states);
            states.transform.translate(0, title_height);
        }
        for (std::size_t i = action_range_start_idx; i < action_range_end_idx; i++) {
            sf::FloatRect bounds = actions[i]->get_bounds();
            target.draw(*actions[i], states);
            states.transform.translate(0, bounds.height);
        }

    }
}
