#ifndef TETRO_H_
#define TETRO_H_
#include <array>
#include <cstddef>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <stdexcept>
#include <cstdint>
#include <random>
#include <vector>

namespace tetriskl {
    using std::array;
    template <typename T>
    using init_list = std::initializer_list<T>;

    enum class Cell {
        I, J, L, O, S, Z, T, // all the tetrominoes
        N // none,
    };
    constexpr int NUM_TETROMINOES = static_cast<int>(Cell::N);
    constexpr int NUM_CELLS = NUM_TETROMINOES + 1;

    enum class Rotation {
        NONE,
        DEG90,
        DEG180,
        DEG270,
    };


    constexpr int NUM_ROTATIONS = 4;

    // Abstraktā klase CellGrid attēlo jebkādu lauciņu ar Cell tipa šūnām
    class CellGrid : public sf::Drawable {
    public:
        void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
        // metode can_place(pos, tile) pārbauda, vai lauciņa tile saturu var novietot vietā pos.
        bool can_place(sf::Vector2u pos, const CellGrid &tile) const;
        // metode place(pos, tile) novieto lauciņua tile saturu vietā pos
        void place(sf::Vector2u pos, const CellGrid& tile);
        // metode operator[](sf::Vector2u point) atgriež referenci vai const referenci uz šūnu vietā point.
        virtual Cell& operator[](sf::Vector2u point) = 0;
        virtual const Cell& operator[](sf::Vector2u point) const = 0;
        // metode size() atgriež lauciņa lielumu
        virtual sf::Vector2u size() const = 0;
    };

    template<std::size_t Columns, std::size_t Rows>
    class StaticCellGrid: public CellGrid {
    private:
        array<array<Cell, Columns>, Rows> cells;

    public:
        constexpr static unsigned int columns = Columns;
        constexpr static unsigned int rows = Rows;

        StaticCellGrid() {
            for (array<Cell, Columns>& row : this->cells)
                for (Cell& k : row)
                    k = Cell::N;
        }
        StaticCellGrid(init_list<init_list<Cell>> cells) : StaticCellGrid() {
            if (cells.size() > Rows)
                throw std::out_of_range("initializer_list exceeds size of grid");

            std::size_t y = 0;
            for (init_list<Cell> row : cells) {
                if (row.size() > Columns)
                    throw std::out_of_range("initializer_list exceeds size of grid");

                std::size_t x = 0;
                for (Cell c : row)
                    this->cells[y][x++] = c;
                y++;
            }
        }

        using iterator = typename array<array<Cell, Columns>, Rows>::iterator;
        using reverse_iterator = typename array<array<Cell, Columns>, Rows>::reverse_iterator;

        iterator begin() {
            return cells.begin();
        }

        iterator end() {
            return cells.end();
        }

        reverse_iterator rbegin() {
            return cells.rbegin();
        }

        reverse_iterator rend() {
            return cells.rend();
        }


        Cell& operator[](sf::Vector2u point) override {
            return cells[point.y][point.x];
        }

        const Cell& operator[](sf::Vector2u point) const override {
            return cells[point.y][point.x];
        }



        sf::Vector2u size() const override { return sf::Vector2u(Columns, Rows); }

    };

    class ConstGridView: public CellGrid {
    private:
        const CellGrid& inner;
        sf::Vector2u top_left;
        sf::Vector2u bottom_right;
    public:
        ConstGridView() = delete;
        ConstGridView(const CellGrid& _inner, sf::Vector2u _top_left, sf::Vector2u _bottom_right);

        Cell& operator[](sf::Vector2u point) override;
        const Cell& operator[](sf::Vector2u point) const override;

        sf::Vector2u size() const override;
    };


    class Tetromino: public CellGrid {
    private:
        StaticCellGrid<4, 4> grid;
        sf::Vector2u unrot_size;
        sf::Vector2u rot_origin;
        Rotation rot;
        bool rotates;

        void rotate_to(CellGrid &grid, sf::Vector2u &pos, Rotation new_rot);
    public:
        Tetromino();
        Tetromino(init_list<init_list<Cell>>);

        Cell& operator[](sf::Vector2u point) override;
        const Cell& operator[](sf::Vector2u point) const override;
        sf::Vector2u size() const override;

        void set_origin(sf::Vector2u origin);
        void rotate_ccw(CellGrid &grid, sf::Vector2u &pos);
        void rotate_cw(CellGrid &grid, sf::Vector2u &pos);

    };

    class TetrominoProvider {
    private:
        array<Tetromino, NUM_TETROMINOES> tetromino_bag;
        std::size_t i;
        std::mt19937 rng;
        void reshuffle();
    public:
        TetrominoProvider();
        Tetromino next();
    };


    extern const array<Tetromino, NUM_TETROMINOES> tetrominoes;
    extern const sf::Color outline_color;
    extern const array<sf::Color, NUM_CELLS> cell_colors;
}

#endif // GAME_H_
