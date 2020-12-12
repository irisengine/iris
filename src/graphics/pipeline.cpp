#include "graphics/pipeline.h"

#include <memory>
#include <tuple>
#include <vector>

#include "graphics/stage.h"

namespace iris
{

Pipeline::Pipeline(std::unique_ptr<Stage> stage)
    : stages_()
{
    stages_.emplace_back(std::move(stage));
}

Pipeline::Pipeline(std::vector<std::unique_ptr<Stage>> stages)
    : stages_(std::move(stages))
{
}

std::vector<std::unique_ptr<Stage>> &Pipeline::stages()
{
    return stages_;
}

}
