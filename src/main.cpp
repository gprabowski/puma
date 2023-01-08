#include <interpolator.hpp>

int main() {
  pusn::interpolator sim;
  sim.init("Movement Interpolation");
  sim.main_loop();
  return 0;
}
