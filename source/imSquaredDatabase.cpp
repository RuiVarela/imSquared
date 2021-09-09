#include "Project.h"

using namespace re;

#define PROP_TEST(name, code)                 \
    else if (re::startsWith(line, #name ":")) \
    {                                         \
        re::replace(line, #name ":", "");     \
        re::trim(line);                       \
        code;                                 \
    }
#define PROP_TEST_INT(obj, name) PROP_TEST(name, obj.name = re::lexical_cast<int>(line, 0))
#define PROP_TEST_FLT(obj, name) PROP_TEST(name, obj.name = re::lexical_cast<float>(line, 0))
#define PROP_TEST_STR(obj, name) PROP_TEST(name, obj.name = line)

int imSquared::indexFromFigureName(std::string const &name)
{
    for (int i = 0; i != m_figures.size(); ++i)
    {
        if (m_figures[i].name == name)
            return i;
    }

    return -1;
}

void imSquared::appendSyntheticDatabase()
{
    int const char_width = 5;
    int const char_height = 8;

    std::vector<std::array<int, char_width> > font{
        {0x00, 0x00, 0x00, 0x00, 0x00}, //
        {0x2f, 0x00, 0x00, 0x00, 0x00}, // !
        {0x03, 0x00, 0x03, 0x00, 0x00}, // "
        {0x14, 0x3e, 0x14, 0x3e, 0x14}, // #
        {0x2e, 0x6a, 0x2b, 0x3a, 0x00}, // $
        {0x26, 0x12, 0x08, 0x24, 0x32}, // %
        {0x1c, 0x17, 0x15, 0x34, 0x00}, // &
        {0x03, 0x00, 0x00, 0x00, 0x00}, // '
        {0x1e, 0x21, 0x00, 0x00, 0x00}, // (
        {0x21, 0x1e, 0x00, 0x00, 0x00}, // )
        {0x22, 0x08, 0x1c, 0x08, 0x22}, // *
        {0x08, 0x1c, 0x08, 0x00, 0x00}, // +
        {0x40, 0x20, 0x00, 0x00, 0x00}, // ,
        {0x08, 0x08, 0x00, 0x00, 0x00}, // -
        {0x20, 0x00, 0x00, 0x00, 0x00}, // .
        {0x20, 0x10, 0x08, 0x04, 0x02}, // /
        {0x3f, 0x21, 0x21, 0x3f, 0x00}, // 0
        {0x01, 0x3f, 0x00, 0x00, 0x00}, // 1
        {0x3d, 0x25, 0x25, 0x27, 0x00}, // 2
        {0x25, 0x25, 0x25, 0x3f, 0x00}, // 3
        {0x07, 0x04, 0x04, 0x3f, 0x00}, // 4
        {0x27, 0x25, 0x25, 0x3d, 0x00}, // 5
        {0x3f, 0x25, 0x25, 0x3d, 0x00}, // 6
        {0x01, 0x39, 0x05, 0x03, 0x00}, // 7
        {0x3f, 0x25, 0x25, 0x3f, 0x00}, // 8
        {0x27, 0x25, 0x25, 0x3f, 0x00}, // 9
        {0x28, 0x00, 0x00, 0x00, 0x00}, // :
        {0x40, 0x28, 0x00, 0x00, 0x00}, // ;
        {0x04, 0x0a, 0x11, 0x00, 0x00}, // <
        {0x14, 0x14, 0x00, 0x00, 0x00}, // =
        {0x11, 0x0a, 0x04, 0x00, 0x00}, // >
        {0x01, 0x2d, 0x05, 0x07, 0x00}, // ?
        {0x3f, 0x21, 0x3d, 0x25, 0x1f}, // @
        {0x3f, 0x09, 0x09, 0x3f, 0x00}, // A
        {0x3f, 0x25, 0x27, 0x3c, 0x00}, // B
        {0x3f, 0x21, 0x21, 0x21, 0x00}, // C
        {0x3f, 0x21, 0x21, 0x1e, 0x00}, // D
        {0x3f, 0x25, 0x25, 0x25, 0x00}, // E
        {0x3f, 0x05, 0x05, 0x05, 0x00}, // F
        {0x3f, 0x21, 0x25, 0x3d, 0x00}, // G
        {0x3f, 0x04, 0x04, 0x3f, 0x00}, // H
        {0x21, 0x3f, 0x21, 0x00, 0x00}, // I
        {0x38, 0x20, 0x21, 0x3f, 0x01}, // J
        {0x3f, 0x04, 0x04, 0x3b, 0x00}, // K
        {0x3f, 0x20, 0x20, 0x20, 0x00}, // L
        {0x3f, 0x01, 0x3f, 0x01, 0x3f}, // M
        {0x3f, 0x02, 0x04, 0x3f, 0x00}, // N
        {0x3f, 0x21, 0x21, 0x3f, 0x00}, // O
        {0x3f, 0x09, 0x09, 0x0f, 0x00}, // P
        {0x3f, 0x21, 0x31, 0x3f, 0x00}, // Q
        {0x3f, 0x09, 0x39, 0x2f, 0x00}, // R
        {0x27, 0x25, 0x25, 0x3d, 0x00}, // S
        {0x01, 0x01, 0x3f, 0x01, 0x01}, // T
        {0x3f, 0x20, 0x20, 0x3f, 0x00}, // U
        {0x0f, 0x10, 0x30, 0x1f, 0x00}, // V
        {0x3f, 0x20, 0x3f, 0x20, 0x3f}, // W
        {0x3b, 0x04, 0x04, 0x3b, 0x00}, // X
        {0x0f, 0x08, 0x38, 0x0f, 0x00}, // Y
        {0x31, 0x29, 0x25, 0x23, 0x00}, // Z
        {0x3f, 0x21, 0x00, 0x00, 0x00}, // [
        {0x20, 0x10, 0x08, 0x04, 0x02}, // "\"
        {0x21, 0x3f, 0x00, 0x00, 0x00}, // ]
        {0x02, 0x01, 0x01, 0x02, 0x00}, // ^
        {0x20, 0x20, 0x00, 0x00, 0x00}, // _
        {0x01, 0x02, 0x00, 0x00, 0x00}, // `
        {0x38, 0x24, 0x24, 0x3c, 0x00}, // a
        {0x3f, 0x24, 0x24, 0x3c, 0x00}, // b
        {0x3c, 0x24, 0x24, 0x24, 0x00}, // c
        {0x3c, 0x24, 0x24, 0x3f, 0x00}, // d
        {0x3c, 0x2c, 0x2c, 0x2c, 0x00}, // e
        {0x04, 0x3f, 0x05, 0x00, 0x00}, // f
        {0xbc, 0xa4, 0xa4, 0xfc, 0x00}, // g
        {0x3f, 0x04, 0x04, 0x3c, 0x00}, // h
        {0x3d, 0x00, 0x00, 0x00, 0x00}, // i
        {0x80, 0xfd, 0x00, 0x00, 0x00}, // j
        {0x3f, 0x08, 0x08, 0x34, 0x00}, // k
        {0x3f, 0x00, 0x00, 0x00, 0x00}, // l
        {0x3c, 0x04, 0x3c, 0x04, 0x3c}, // m
        {0x3c, 0x04, 0x04, 0x3c, 0x00}, // n
        {0x3c, 0x24, 0x24, 0x3c, 0x00}, // o
        {0xfc, 0x24, 0x24, 0x3c, 0x00}, // p
        {0x3c, 0x24, 0x24, 0xfc, 0x00}, // q
        {0x3c, 0x08, 0x04, 0x00, 0x00}, // r
        {0x2c, 0x2c, 0x2c, 0x3c, 0x00}, // s
        {0x04, 0x3f, 0x24, 0x00, 0x00}, // t
        {0x3c, 0x20, 0x20, 0x3c, 0x00}, // u
        {0x0c, 0x10, 0x30, 0x1c, 0x00}, // v
        {0x3c, 0x20, 0x3c, 0x20, 0x3c}, // w
        {0x34, 0x08, 0x08, 0x34, 0x00}, // x
        {0xbc, 0xa0, 0xa0, 0xfc, 0x00}, // y
        {0x24, 0x34, 0x2c, 0x24, 0x00}, // z
        {0x04, 0x3f, 0x21, 0x00, 0x00}, // {
        {0x3f, 0x00, 0x00, 0x00, 0x00}, // |
        {0x21, 0x3f, 0x04, 0x00, 0x00}, // }
        {0x01, 0x02, 0x02, 0x01, 0x00}, // ~
        {0x00, 0x00, 0x00, 0x00, 0x00}};

    std::vector<std::string> render(char_height, std::string(char_width, ' '));

    for (size_t b = 0; b != font.size(); ++b)
    {
        auto &bytes = font[b];
        char space = ' ';
        char letter = space + b;

        for (int j = 0; j < char_width; j++)
        {
            char char_byte = bytes[j];
            for (int i = 0; i < char_height; i++)
            {
                bool is_set = (char_byte & (1 << i));
                render[i][j] = is_set ? 'X' : '-';
            }
        }

        Figure figure;
        figure.name = sfmt("char_%s", letter);
        figure.lines = render;
        m_figures.push_back(figure);
    }
}

static std::vector<std::string> trim(std::vector<std::string> input)
{

    int min_x = std::numeric_limits<int>::max();
    int max_x = std::numeric_limits<int>::min();

    int min_y = std::numeric_limits<int>::max();
    int max_y = std::numeric_limits<int>::min();

    for (int y = 0; y != int(input.size()); ++y)
    {
        auto& line = input[y];
        for (int x = 0; x != int(line.size()); ++x)
        {
            if (line[x] == 'X') {
                if (x > max_x) max_x = x;
                if (x < min_x) min_x = x;

                if (y > max_y) max_y = y;
                if (y < min_y) min_y = y;
            }
        }
    }

    if (min_x == std::numeric_limits<int>::max() || min_y == std::numeric_limits<int>::max())  {
        return input;
    }

    // trim vertically
    {
        auto first = input.begin() + min_y;
        auto last = input.begin() + max_y;
        input = std::vector<std::string>(first, ++last);
    }

    // trim horizontally
    for (auto& line : input) 
    {
        auto first = line.begin() + min_x;
        auto last = line.begin() + max_x;
        line = std::string(first, ++last);
    }

    return input;
}

void imSquared::loadDatabase()
{
    //
    // load figures
    //
    appendSyntheticDatabase();

    {
        std::ifstream buffer("resources/square.fig");
        if (buffer)
        {
            int emit_count = 1;
            Figure figure;

            std::string line;
            while (!std::getline(buffer, line).eof())
            {
                trim(line);
                if (line.empty())
                    continue;

                if (line == "#")
                {
                    m_figures.push_back(figure);
                    figure = Figure();
                    emit_count = 1;
                }
                PROP_TEST_STR(figure, name)
                PROP_TEST(emit_count, emit_count = lexical_cast<int>(line, 1))
                else
                {
                    for (int i = 0; i != emit_count; ++i)
                        figure.lines.push_back(line);
                }
            }
        }
        else
        {
            logDbg("Squared", "Unable to load figure file");
        }
    }

    for (auto &figure : m_figures)
    {
        figure.lines = trim(figure.lines);

        figure.markedCount = 0;
        figure.width = 0;
        figure.height = figure.lines.size();

        for (auto &line : figure.lines)
        {
            if ((int)line.size() > figure.width)
                figure.width = line.size();

            for (int x = 0; x < figure.width; x++)
                if (line[x] == 'X')
                    figure.markedCount++;
        }
    }

    //
    // debug
    //
    // logDbg("Squared", "************** FIGURES **************");
    // for (auto& figure : m_figures)
    // {
    //     logDbg("Squared", "****************************");
    //     logDbg("Squared", sfmt("name: %s", figure.name.c_str()));
    //     logDbg("Squared", sfmt("width: %d", figure.width));
    //     logDbg("Squared", sfmt("height: %d", figure.height));
    //     logDbg("Squared", sfmt("markedCount: %d", figure.markedCount));

    //     for (auto& line : figure.lines)
    //         logDbg("Squared", sfmt("[%s]", line.c_str()));
    // }

    //
    // load levels
    //
    {
        std::ifstream buffer("resources/square.lvl");
        if (buffer)
        {
            Level level;

            std::string line;

            while (!std::getline(buffer, line).eof())
            {
                trim(line);
                if (line.empty())
                    continue;

                if (line == "#")
                {
                    m_levels.push_back(level);
                    level = Level();
                }
                PROP_TEST_STR(level, type)
                PROP_TEST_INT(level, figure_spacing)
                PROP_TEST_FLT(level, bonus)
                PROP_TEST_FLT(level, speed)
                PROP_TEST_FLT(level, speed_increment_per_second)
                PROP_TEST_FLT(level, speed_max)
                PROP_TEST_INT(level, total_figures)
                PROP_TEST_STR(level, message)
                PROP_TEST(figure, level.figure_names.push_back(line))      
            }
        }

        for (auto &level : m_levels)
            if (level.type == "text") 
                for (auto& ch : level.message) {
                    std::string name = std::string("char_") + ch;
                    level.figure_names.push_back(name);
                }
        

        for (auto &level : m_levels)
            for (auto &name : level.figure_names)
            {
                int index = indexFromFigureName(name);
                if (index != -1)
                    level.figures.push_back(index);
                else
                    logErr("Simon", sfmt("Figure not found [%s] for level!", name));
            }

        for (auto &level : m_levels)
        {
            if (level.type != "random puzzle")
                level.total_figures = level.figures.size();

            if (level.type == "continuous")
                level.figure_spacing = 0;
        }

        //
        // debug
        //
        logDbg("Squared", "************** LEVELS **************");
        int count = 0;
        for (auto &level : m_levels)
        {
            logDbg("Squared", sfmt("****************************"));
            logDbg("Squared", sfmt("name: %d", count++));
            logDbg("Squared", sfmt("type: %s", level.type.c_str()));
            logDbg("Squared", sfmt("figure_spacing: %d", level.figure_spacing));
            logDbg("Squared", sfmt("speed: %.3f", level.speed));
            logDbg("Squared", sfmt("speed_increment_per_second: %.3f", level.speed_increment_per_second));
            logDbg("Squared", sfmt("speed_max: %.3f", level.speed_max));
            logDbg("Squared", sfmt("total_figures: %d", level.total_figures));
            logDbg("Squared", sfmt("bonus: %.3f", level.bonus));

            for (auto &figure : level.figures)
                if (figure != -1)
                    logDbg("Squared", sfmt("figure [%d %s]", figure, m_figures[figure].name.c_str()));
        }
    }
}