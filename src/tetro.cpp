#include "tetro.h"
#include <algorithm>
#include <stdexcept>
#include <SFML/Graphics.hpp>

namespace tetriskl {
    bool CellGrid::can_place(sf::Vector2u pos, const CellGrid &tile) const {
        sf::Vector2u tile_size = tile.size();
        sf::Vector2u this_size = this->size();
        if (pos.x + tile_size.x > this_size.x) return false;
        if (pos.y + tile_size.y > this_size.y) return false;
        for (std::size_t x = 0; x < tile_size.x; x++) {
            for (std::size_t y = 0; y < tile_size.y; y++) {
                sf::Vector2u tilepos = sf::Vector2u(x, y);
                if (tile[tilepos] != Cell::N && (*this)[pos + tilepos] != Cell::N)
                    return false;
            }
        }

        return true;
    }

    void CellGrid::place(sf::Vector2u pos, const CellGrid& tile) {
        sf::Vector2u tile_size = tile.size();
        for (std::size_t x = 0; x < tile_size.x; x++) {
            for (std::size_t y = 0; y < tile_size.y; y++) {
                sf::Vector2u tilepos(x, y);
                if (tile[tilepos] != Cell::N)
                    (*this)[pos + tilepos] = tile[tilepos];
            }
        }
    }

    ConstGridView::ConstGridView(const CellGrid& _inner, sf::Vector2u _top_left, sf::Vector2u _bottom_right)
        : inner(_inner), top_left(_top_left), bottom_right(_bottom_right) {}

    Cell& ConstGridView::operator[](sf::Vector2u point) {
        throw std::logic_error("cannot use non-const operator[] with ConstGridView");
    }

    const Cell& ConstGridView::operator[](sf::Vector2u point) const {
        return inner[top_left + point];
    }

    sf::Vector2u ConstGridView::size() const {
        return bottom_right - top_left;
    }

    Tetromino::Tetromino() : grid(), unrot_size(sf::Vector2u(0, 0)), rot(Rotation::NONE), rot_origin(), rotates(false) {}
    Tetromino::Tetromino(init_list<init_list<Cell>> cg) : grid(cg), rot(Rotation::NONE), rot_origin(), rotates(false) {
        unrot_size.y = cg.size();
        unrot_size.x = std::max(cg, [] (auto &a, auto &b) { return a.size() < b.size(); }).size();
    }


    sf::Vector2u unrotate_point(Rotation rot, sf::Vector2u p, sf::Vector2u size) {
        sf::Vector2u origin = size;
        switch (rot) {
        case Rotation::NONE: return p;
        case Rotation::DEG90: return sf::Vector2u(size.x - 1 - p.y, p.x);
        case Rotation::DEG180: return sf::Vector2u(size.x - 1 - p.x, size.y - 1 -  p.y);
        case Rotation::DEG270: return sf::Vector2u(p.y, size.y -1 - p.x);
        }
        throw std::logic_error("Rotation invariant violation");
    }

    sf::Vector2u rotate_point(Rotation rot, sf::Vector2u p, sf::Vector2u size) {
        sf::Vector2u origin = size;
        switch (rot) {
        case Rotation::NONE: return p;
        case Rotation::DEG90: return sf::Vector2u(p.y, size.x - 1 - p.x);
        case Rotation::DEG180: return sf::Vector2u(size.x - 1 - p.x, size.y - 1 -  p.y);
        case Rotation::DEG270: return sf::Vector2u(size.y - 1 - p.y, p.x);
        }
        throw std::logic_error("Rotation invariant violation");
    }



    Rotation invert_rotation(Rotation rot) {
        return (rot != Rotation::NONE)
            ? (Rotation) (NUM_ROTATIONS - (int)rot)
            : rot;
    }

    Cell& Tetromino::operator[](sf::Vector2u point) {
        return grid[unrotate_point(rot, point, unrot_size)];
    }

    const Cell& Tetromino::operator[](sf::Vector2u point) const {
        return grid[unrotate_point(rot, point, unrot_size)];
    }


    sf::Vector2u Tetromino::size() const {
        switch (rot) {
        case Rotation::NONE: case Rotation::DEG180: return unrot_size;
        case Rotation::DEG90: case Rotation::DEG270: return sf::Vector2u(unrot_size.y, unrot_size.x);
        }
        throw std::logic_error("Rotation invariant violation");
    }

    void Tetromino::set_origin(sf::Vector2u origin) {
        this->rot_origin = origin;
        rotates = true;
    }

    void Tetromino::rotate_to(CellGrid &grid, sf::Vector2u &pos, Rotation new_rot) {
        if (!rotates) return;
        Rotation old_rot = rot;
        sf::Vector2u old_pos = pos;
        for (const sf::Vector2i wall_kick : {sf::Vector2i(0, 0), sf::Vector2i(1, 0), sf::Vector2i(-1, 0)}) {
            sf::Vector2u rotated_origin_pre = rotate_point(rot, rot_origin, unrot_size);
            rot = new_rot;
            sf::Vector2u rotated_origin_post = rotate_point(rot, rot_origin, unrot_size);
            sf::Vector2i new_pos = sf::Vector2i(pos + rotated_origin_pre - rotated_origin_post) + wall_kick;

            if (new_pos.x < 0 || new_pos.y < 0 || !grid.can_place(sf::Vector2u(new_pos), *this)) {
                rot = old_rot;
                pos = old_pos;
                continue;
            }

            pos = sf::Vector2u(new_pos);
            break;
        }
    }

    void Tetromino::rotate_ccw(CellGrid &grid, sf::Vector2u &pos) {
        Rotation new_rot = (Rotation) (((int) rot + 1) % NUM_ROTATIONS);
        rotate_to(grid, pos, new_rot);
    }

    void Tetromino::rotate_cw(CellGrid &grid, sf::Vector2u &pos) {
        Rotation new_rot = (rot == Rotation::NONE)
            ? Rotation::DEG270
            : (Rotation)((int)rot - 1);
        rotate_to(grid, pos, new_rot);
    }

    void TetrominoProvider::reshuffle() {
        std::shuffle(tetromino_bag.begin(), tetromino_bag.end(), rng);
        i = 0;
    }

    TetrominoProvider::TetrominoProvider() : tetromino_bag(tetrominoes), i(0) {
        std::random_device rd;
        rng = std::mt19937(rd());
        reshuffle();
    }


    Tetromino TetrominoProvider::next() {
        if (i >= tetromino_bag.size()) reshuffle();
        return tetromino_bag[i++];
    }

    array<Tetromino, NUM_TETROMINOES> make_tetromino_tbl() {
        constexpr Cell I = Cell::I;
        constexpr Cell J = Cell::J;
        constexpr Cell L = Cell::L;
        constexpr Cell O = Cell::O;
        constexpr Cell S = Cell::S;
        constexpr Cell Z = Cell::Z;
        constexpr Cell T = Cell::T;
        constexpr Cell N = Cell::N;

        array<Tetromino, NUM_TETROMINOES> tbl;
        tbl[(int)I] = {
            {I, I, I, I},
        };
        tbl[(int)I].set_origin(sf::Vector2u(1, 0));

        tbl[(int)J] = {
            {J, N, N},
            {J, J, J},
        };
        tbl[(int)J].set_origin(sf::Vector2u(1, 1));

        tbl[(int)L] = {
            {N, N, L},
            {L, L, L},
        };
        tbl[(int)L].set_origin(sf::Vector2u(1, 1));

        tbl[(int)O] = {
            {O, O},
            {O, O}
        };

        tbl[(int)S] = {
            {N, S, S},
            {S, S, N},
        };
        tbl[(int)S].set_origin(sf::Vector2u(1, 1));

        tbl[(int)Z] = {
            {Z, Z, N},
            {N, Z, Z},
        };
        tbl[(int)Z].set_origin(sf::Vector2u(1, 1));

        tbl[(int)T] = {
            {N, T, N},
            {T, T, T}
        };
        tbl[(int)T].set_origin(sf::Vector2u(1, 1));
        return tbl;
    }

    const array<Tetromino, NUM_TETROMINOES> tetrominoes = make_tetromino_tbl();


    array<sf::Color, NUM_CELLS> make_color_tbl() {
        array<sf::Color, NUM_CELLS> retval;

        retval[(int)Cell::I] = sf::Color(0x34dbebff);
        retval[(int)Cell::J] = sf::Color(0x083673ff);
        retval[(int)Cell::L] = sf::Color(0xeb8100ff);
        retval[(int)Cell::O] = sf::Color(0xffdd00ff);
        retval[(int)Cell::S] = sf::Color(0x098700ff);
        retval[(int)Cell::Z] = sf::Color(0xcc2c00ff);
        retval[(int)Cell::T] = sf::Color(0x969696ff);
        retval[(int)Cell::N] = sf::Color(0x00000000);
        return retval;
    }

    const sf::Color outline_color = sf::Color(0x6e6e6eff);
    const array<sf::Color, NUM_CELLS> cell_colors = make_color_tbl();
}
