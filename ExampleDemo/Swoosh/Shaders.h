#pragma once
#include <Swoosh\EmbedGLSL.h>

static auto MOVING_TEXTURE_SHADER = GLSL
(
  110,
  uniform sampler2D texture;
  uniform sampler2D pattern;
  uniform float progress;

  void main()
  {
    vec4 pixel = texture2D(texture, vec2(gl_TexCoord[0].xy));
    vec4 other = texture2D(pattern, vec2(gl_TexCoord[0].x, gl_TexCoord[0].y + progress));

    vec4 color = gl_Color * pixel;

    if (pixel.a > 0.0) {
      color = gl_Color * mix(pixel, other, 0.5);
    }

    gl_FragColor = color;
  }
);


static auto MONOTONE_SHADER = GLSL
(
  110,
  uniform sampler2D texture;
  uniform float amount;

  void main()
  {
    vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);
    float avg = (pixel.r + pixel.g + pixel.b) / 3.0;
    pixel = mix(pixel, vec4(avg, avg, avg, pixel.a), amount);
    gl_FragColor = gl_Color * pixel;
  }
);

static auto STATUS_BAR_SHADER = GLSL
(
  110,
  uniform sampler2D texture;
  uniform float hp;
  uniform float xp;

  void main()
  {
    // We encode darkened pixels in the alpha channel
    vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);

    // hp is the red color coded pixels in the sample
    if (pixel.g == 0.0 && pixel.b == 0.0) {
      float normal = (hp*137.0) + (1.0 - hp)*255.0;
      normal = normal / 255.0;

      if (pixel.r > normal) {
        pixel = vec4(0.0, 1.0*pixel.a, 0.0, 1.0);
      }
      else {
        pixel = vec4(0.5*pixel.a, 0.5*pixel.a, 0.5*pixel.a, 1.0);
      }
    }
    else if (pixel.r == 0.0 && pixel.g == 0.0) {
      // xp is the blue color coded pixels in the sample

      float normal = (xp*12.0) + (1.0 - xp)*255.0;
      normal = normal / 255.0;

      if (pixel.b > normal) {
        pixel = vec4(0.04*pixel.a, 1.0*pixel.a, 0.95*pixel.a, 1.0);
      }
      else {
        pixel = vec4(0.5*pixel.a, 0.5*pixel.a, 0.5*pixel.a, 1.0);
      }
    }

    gl_FragColor = gl_Color * pixel;
  }
);

static auto WHITE_SHADER = GLSL
(
  110,
  uniform sampler2D texture;
  uniform float opacity;

  void main()
  {
    vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);
    vec4 color = gl_Color * pixel;
    color = vec4(1.0, 1.0, 1.0, color.a)*opacity + (1.0 - opacity)*color;
    gl_FragColor = color;
  }
);