#ifndef MENU_H_
#define MENU_H_

#include <cstddef>
#include <vector>
#include <memory>
#include <utility>
#include <functional>

#include <SFML/Graphics.hpp>
#include "game.h"

namespace tetriskl {
    class Menu;

    class MenuAction: public sf::Drawable {
    protected:
        const sf::Font *font;
        bool active;
        constexpr static unsigned int character_size = 20;
        constexpr static float indicator_padding = 12.f;
        constexpr static float vertical_padding = 5.f;
    public:
        MenuAction();
        virtual ~MenuAction() = default;

        void set_font(const sf::Font& font);
        void set_active(bool active);

        virtual const char *text() const = 0;
        virtual void handle_event(sf::RenderWindow& rw, Menu& menu, const sf::Event& ev) = 0;

        sf::FloatRect get_bounds() const;
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    };

    template <typename HandlerFn>
    class SimpleMenuAction: public MenuAction {
    private:
        const char *text_str;
        HandlerFn handler;
    public:
        SimpleMenuAction(const char *_text_str, HandlerFn _handler)
            : MenuAction(), text_str(_text_str), handler(_handler) {}

        ~SimpleMenuAction() override = default;

        const char *text() const override {
            return text_str;
        }
        void handle_event(sf::RenderWindow& rw, Menu& menu, const sf::Event& ev) override {
            if (ev.type == sf::Event::KeyPressed && ev.key.code == sf::Keyboard::Enter) {
                handler(rw, menu);
            }
        }
    };

    template<typename HandlerFn>
    std::unique_ptr<MenuAction> menu_action(const char *_text_str, HandlerFn _handler) {
        auto p = std::unique_ptr<MenuAction>(new SimpleMenuAction<HandlerFn>(_text_str, _handler));
        return std::move(p);
    }

    class Menu: sf::Drawable {
    public:
        using close_action_type = std::function<void(sf::RenderWindow& rw, Menu& menu)>;
    private:
        const sf::Font *font;
        const char *title;
        bool closed;
        std::size_t active_item_idx;
        mutable std::vector<std::unique_ptr<MenuAction>> actions;
        close_action_type close_action;
        constexpr static unsigned int title_size = 30;
        constexpr static float title_padding = 10;
        constexpr static std::size_t actions_per_page = 5;
    public:
        Menu();
        void close();
        Menu& set_title(const char *title) &;
        Menu& set_font(const sf::Font& font) &;
        Menu& set_close_action(close_action_type action) &;
        Menu& add_menu_item(std::unique_ptr<MenuAction> action) &;
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
        void run(sf::RenderWindow& rw);
    };
}

#endif // MENU_H_
