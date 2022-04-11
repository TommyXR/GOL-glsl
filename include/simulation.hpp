#pragma once

#include "glu.hpp"


using namespace glu;


class Simulation {
  public:
    struct Parameters {
        int iterations_per_second = 1;
        float randomize_density = 0.5;
    };

    Parameters params;

  private:
};