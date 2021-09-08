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

int imSquared::indexFromLevelName(std::string const &name)
{
    for (int i = 0; i != m_levels.size(); ++i)
    {
        if (m_levels[i].name == name)
            return i;
    }

    return -1;
}

void imSquared::loadDatabase()
{
    //
    // load figures
    //
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

    for (auto& figure : m_figures)
    {
        figure.markedCount = 0;
        figure.width = 0;
        figure.height = figure.lines.size();

        for (auto& line : figure.lines)
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
    logDbg("Squared", "************** FIGURES **************");
    for (auto& figure : m_figures)
    {
        logDbg("Squared", "****************************");
        logDbg("Squared", sfmt("name: %s", figure.name.c_str()));
        logDbg("Squared", sfmt("width: %d", figure.width));
        logDbg("Squared", sfmt("height: %d", figure.height));
        logDbg("Squared", sfmt("markedCount: %d", figure.markedCount));

        for (auto& line : figure.lines)
            logDbg("Squared", sfmt("[%s]", line.c_str()));
    }

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
                PROP_TEST_STR(level, name)
                PROP_TEST_STR(level, type)
                PROP_TEST_INT(level, figure_spacing)
                PROP_TEST_FLT(level, bonus)
                PROP_TEST_FLT(level, speed)
                PROP_TEST_FLT(level, speed_increment_per_second)
                PROP_TEST_FLT(level, speed_max)
                PROP_TEST_INT(level, total_figures)
                PROP_TEST(figure,
                          {
                              int figureIndex = indexFromFigureName(line);
                              if (figureIndex == -1)
                              {
                                  logErr("Simon", sfmt("Figure not found [%s] for level!", line.c_str()));
                              }
                              else
                              {
                                  level.figures.push_back(figureIndex);
                              }
                          })
            }
        }

        for (auto& level : m_levels)
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
        for (auto& level : m_levels)
        {
            logDbg("Squared", sfmt("****************************"));
            logDbg("Squared", sfmt("name: %s", level.name.c_str()));
            logDbg("Squared", sfmt("type: %s", level.type.c_str()));
            logDbg("Squared", sfmt("figure_spacing: %d", level.figure_spacing));
            logDbg("Squared", sfmt("speed: %.3f",  level.speed));
            logDbg("Squared", sfmt("speed_increment_per_second: %.3f", level.speed_increment_per_second));
            logDbg("Squared", sfmt("speed_max: %.3f", level.speed_max));
            logDbg("Squared", sfmt("total_figures: %d", level.total_figures));
            logDbg("Squared", sfmt("bonus: %.3f", level.bonus));

            for (auto& figure : level.figures)
                if (figure != -1)
                    logDbg("Squared", sfmt("figure [%d %s]", figure, m_figures[figure].name.c_str()));
            
        }
    }
}