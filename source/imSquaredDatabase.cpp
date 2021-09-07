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
                PROP_TEST_STR(figure, sound)
                PROP_TEST_INT(figure, score)
                PROP_TEST_INT(figure, hits)
                PROP_TEST(emit_count, emit_count = lexical_cast<int>(line, 1))
                PROP_TEST(color,
                          {
                              std::vector<std::string> splitted = split(line, " ");
                              if (splitted.size() == 4)
                              {
                                  figure.color.r = re::lexical_cast<int>(splitted[0], 0);
                                  figure.color.g = re::lexical_cast<int>(splitted[1], 0);
                                  figure.color.b = re::lexical_cast<int>(splitted[2], 0);
                                  figure.color.a = re::lexical_cast<int>(splitted[3], 0);
                              }
                          })
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
        logDbg("Squared", sfmt("score: %d", figure.score));
        logDbg("Squared", sfmt("hits: %d", figure.hits));
        logDbg("Squared", sfmt("color: %d %d %d %d", 
                               figure.color.r, 
                               figure.color.g,
                               figure.color.b, 
                               figure.color.a));
        logDbg("Squared", sfmt("sound: %s", figure.sound.c_str()));
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
                PROP_TEST_FLT(level, bonus[0])
                PROP_TEST_FLT(level, bonus[1])
                PROP_TEST_FLT(level, bonus[2])
                PROP_TEST_FLT(level, score_factor[0])
                PROP_TEST_FLT(level, score_factor[1])
                PROP_TEST_FLT(level, score_factor[2])
                PROP_TEST_FLT(level, speed[0])
                PROP_TEST_FLT(level, speed[1])
                PROP_TEST_FLT(level, speed[2])
                PROP_TEST_FLT(level, clear_factor[0])
                PROP_TEST_FLT(level, clear_factor[1])
                PROP_TEST_FLT(level, clear_factor[2])
                PROP_TEST_FLT(level, speed_increment_per_second[0])
                PROP_TEST_FLT(level, speed_increment_per_second[1])
                PROP_TEST_FLT(level, speed_increment_per_second[2])
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

            //
            // set default values
            //
            if (level.score_factor[0] == FLT_N_INIT)
                level.score_factor[0] = 1.0;
            if (level.score_factor[1] == FLT_N_INIT)
                level.score_factor[1] = level.score_factor[0];
            if (level.score_factor[2] == FLT_N_INIT)
                level.score_factor[2] = level.score_factor[1];

            if (level.speed[0] == FLT_N_INIT)
                level.speed[0] = 1.0;
            if (level.speed[1] == FLT_N_INIT)
                level.speed[1] = level.speed[0];
            if (level.speed[2] == FLT_N_INIT)
                level.speed[2] = level.speed[1];

            if (level.speed_increment_per_second[0] == FLT_N_INIT)
                level.speed_increment_per_second[0] = 0.0;
            if (level.speed_increment_per_second[1] == FLT_N_INIT)
                level.speed_increment_per_second[1] = level.speed_increment_per_second[0];
            if (level.speed_increment_per_second[2] == FLT_N_INIT)
                level.speed_increment_per_second[2] = level.speed_increment_per_second[1];

            if (level.clear_factor[0] == FLT_N_INIT)
                level.clear_factor[0] = 0.5;
            if (level.clear_factor[1] == FLT_N_INIT)
                level.clear_factor[1] = level.clear_factor[0];
            if (level.clear_factor[2] == FLT_N_INIT)
                level.clear_factor[2] = level.clear_factor[1];

            if (level.bonus[0] == FLT_N_INIT)
                level.bonus[0] = 0.5;
            if (level.bonus[1] == FLT_N_INIT)
                level.bonus[1] = level.bonus[0];
            if (level.bonus[2] == FLT_N_INIT)
                level.bonus[2] = level.bonus[1];

            //
            // compute max expected scores
            //
            int multiplier = 1;
            for (int hard = 0; hard != max_hardness; ++hard)
            {
                level.max_expected_score[hard] = 0;
                level.max_expected_score_with_multiplier[hard] = 0;
            }

            for (auto& figure: level.figures)
            {
                for (int hard = 0; hard != max_hardness; ++hard)
                {
                    level.max_expected_score[hard] +=
                        (float)m_figures[figure].markedCount *
                        (float)m_figures[figure].score *
                        (float)level.score_factor[hard];

                    level.max_expected_score_with_multiplier[hard] +=
                        (float)m_figures[figure].markedCount *
                        (float)m_figures[figure].score *
                        (float)level.score_factor[hard] *
                        (float)multiplier;
                }
                multiplier++;
            }
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
            logDbg("Squared", sfmt("speed: %.3f %.3f %.3f", 
                                    level.speed[0], 
                                    level.speed[1], 
                                    level.speed[2]));
            logDbg("Squared", sfmt("speed_increment_per_second: %.3f %.3f %.3f",
                                   level.speed_increment_per_second[0], 
                                   level.speed_increment_per_second[1],
                                   level.speed_increment_per_second[2]));
            logDbg("Squared", sfmt("speed_max: %.3f", level.speed_max));
            logDbg("Squared", sfmt("total_figures: %d", level.total_figures));
            logDbg("Squared", sfmt("clear_factor: %.3f %.3f %.3f", 
                                    level.clear_factor[0],
                                    level.clear_factor[1], 
                                    level.clear_factor[2]));
            logDbg("Squared", sfmt("score_factor: %.3f %.3f %.3f", 
                                    level.score_factor[0],
                                    level.score_factor[1], 
                                    level.score_factor[2]));
            logDbg("Squared", sfmt("bonus: %.3f %.3f %.3f", 
                                    level.bonus[0], 
                                    level.bonus[1], 
                                    level.bonus[2]));
            logDbg("Squared", sfmt("max_expected_score: %d %d %d", 
                                    level.max_expected_score[0],
                                    level.max_expected_score[1], 
                                    level.max_expected_score[2]));
            logDbg("Squared", sfmt("max_expected_score_with_multiplier: %d %d %d",
                                   level.max_expected_score_with_multiplier[0],
                                   level.max_expected_score_with_multiplier[1],
                                   level.max_expected_score_with_multiplier[2]));

            for (auto& figure : level.figures)
                if (figure != -1)
                    logDbg("Squared", sfmt("figure [%d %s]", figure, m_figures[figure].name.c_str()));
            
        }
    }
}