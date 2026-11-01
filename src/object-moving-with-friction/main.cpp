#include "../compile.hpp"
#define uu_internal_symbol static
#define uu_local_state static
#ifndef OS
#error OS variable must be defined
#endif
#define OS_WINDOWS (2)

#include <micros/api.h>
#include <micros/gl3.h>

// simple object moving in space with friction

// (nanovg prototypes)
BEGIN_NOWARN_BLOCK
#include "nanovg/src/nanovg.h"
END_NOWARN_BLOCK
uu_internal_symbol NVGcontext *nvg_create_context();

template <typename T, size_t N>
constexpr size_t countof(T(&)[N]) {
  return N;
}

// (moving object with friction)
extern void render_next_gl3(uint64_t time_micros,
                            struct Display display)
{
  simulating:
        uu_local_state struct {
          double ddpx;
          double ddpy;
          double dpx;
          double dpy;
          double px;
          double py;
          double friction;
        } entities[2];
        uu_local_state struct {
          double dt;
          double t;
          uint64_t t_micros;
        } simulation = { 1.0/120.0, 0.0, time_micros };
        if (simulation.t <= 0.0) {
          uu_local_state bool do_it = true;
          if (do_it) {
              do_it = false;
              entities[0].dpy = -10e-2;
              entities[0].py = 1.0;
              entities[0].px = -20e-2;
              entities[0].friction = 0.0;

              entities[1].dpy = -10e-2;
              entities[1].py = 1.0;
              entities[1].px = 20e-2;
              entities[1].friction = 0.1;
          }
        } else if (simulation.t <= 1000.0) {
        }
        double display_t = simulation.t + double(time_micros - simulation.t_micros) / 1e6;
        double dt = simulation.dt;
        if (simulation.t < display_t) {
          while (simulation.t < display_t) {
            for (size_t entity_index = 0;
                 entity_index != countof(entities);
                 ++entity_index) {
              auto& simulated = entities[entity_index];
              { // Friction:
                simulated.ddpx = -simulated.friction * simulated.dpx;
                simulated.ddpy = -simulated.friction * simulated.dpy;
              }
              simulated.dpx += simulated.ddpx * dt * 0.5;
              simulated.dpy += simulated.ddpy * dt * 0.5;
              simulated.px += simulated.dpx * dt;
              simulated.py += simulated.dpy * dt;
            }
            simulation.t += dt;
            
          }
          simulation.t_micros = time_micros;
        }
  rendering:
        uu_local_state auto vg = nvg_create_context();
        double pixel_ratio = 1.0;
        double m_to_pixels = display.framebuffer_height_px / 2.0;
        nvgBeginFrame(
          vg,
          int(display.framebuffer_width_px),
          int(display.framebuffer_height_px),
          pixel_ratio);
        nvgReset(vg);
        nvgTranslate(
          vg,
          display.framebuffer_width_px / 2.0,
          display.framebuffer_height_px / 2.0);
        nvgScale(vg, m_to_pixels, -m_to_pixels);
        nvgBeginPath(vg);
        for (size_t entity_index = 0; entity_index < countof(entities); ++entity_index) {
          auto& simulated = entities[entity_index];
          nvgCircle(vg, simulated.px, simulated.py, 3e-2);
        }
        nvgFillColor(vg, nvgRGBA(80, 89, 78, 255));
        nvgFill(vg);
        
        nvgEndFrame(vg);
}

extern void render_next_2chn_48khz_audio(uint64_t time_micros,
                int const sample_count, double left[/*sample_count*/],
                double right[/*sample_count*/])
{
        // silence
}

int main (int argc, char** argv)
{
        runtime_init();

        return 0;
}


// (nanovg)
#ifndef UU_NANOVG_DEF
#define UU_NANOVG_DEF
BEGIN_NOWARN_BLOCK
#define NANOVG_GL3_IMPLEMENTATION
#include "nanovg/src/nanovg.c"
#include "nanovg/src/nanovg.h"
#include "nanovg/src/nanovg_gl.h"
uu_internal_symbol NVGcontext *nvg_create_context()
{
  uint32_t flags = NVG_ANTIALIAS | NVG_STENCIL_STROKES;
  flags |= NVG_DEBUG;
  return nvgCreateGL3(flags);
}
END_NOWARN_BLOCK
#endif