#include "menu.h"
#include <utility>
#include <SFML/Graphics.hpp>

namespace tetriskl {
    MenuAction::MenuAction() : font(nullptr), active(false) {}

    void MenuAction::set_font(const sf::Font& font) {
        this->font = &font;
    }

    void MenuAction::set_active(bool active) {
        this->active = active;
    }

    Menu::Menu() : font(nullptr),
                   title(nullptr),
                   active_item_idx(0),
                   actions(),
                   closed(false),
                   close_action([] (auto &rw, auto &menu) { menu.close(); })
    {}

    void Menu::close() {
        closed = true;
    }

    Menu& Menu::set_title(const char *title) & {
        this->title = title;
        return *this;
    }

    Menu& Menu::set_font(const sf::Font& font) & {
        this->font = &font;
        return *this;
    }

    Menu& Menu::set_close_action(close_action_type action) & {
        this->close_action = action;
        return *this;
    }

    Menu& Menu::add_menu_item(std::unique_ptr<MenuAction> action) & {
        if (this->actions.size() == active_item_idx)
            action->set_active(true);

        this->actions.push_back(std::move(action));
        return *this;
    }
    void Menu::run(sf::RenderWindow& rw) {
        closed = false;
        sf::Event ev;
        while (!closed && rw.waitEvent(ev)) {
            switch (ev.type) {
            case sf::Event::Closed:
                close_action(rw, *this);
            case sf::Event::KeyPressed:
                if (ev.key.code == sf::Keyboard::Up) {
                    actions[active_item_idx]->set_active(false);
                    active_item_idx = (active_item_idx == 0)
                        ? actions.size()
                        : active_item_idx - 1;

                    actions[active_item_idx]->set_active(true);
                    break;
                } else if (ev.key.code == sf::Keyboard::Down) {
                    actions[active_item_idx]->set_active(false);
                    active_item_idx++;
                    active_item_idx %= actions.size();
                    actions[active_item_idx]->set_active(true);
                    break;
                }
                // fallthrough
            default:
                actions[active_item_idx]->handle_event(rw, *this, ev);
                break;
            }

            rw.draw(*this);
            rw.display();
        }
    }
}
