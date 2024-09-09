#pragma once

// <https://github.com/czinn/perlin>

double interpolate(double a, double b, double x);

double noise1d(int x, int octave, int seed);
double noise2d(int x, int y, int octave, int seed);
double noise3d(int x, int y, int z, int octave, int seed);

double smooth1d(double x, int octave, int seed);
double smooth2d(double x, double y, int octave, int seed);
double smooth3d(double x, double y, double z, int octave, int seed);

double pnoise1d(double x, double persistence, int octaves, int seed);
double pnoise2d(double x, double y, double persistence, int octaves, int seed);
double pnoise3d(double x, double y, double z, double persistence, int octaves, int seed);

float perlin2D_octaves(float x, float y, int octaves, float persistence);
// float fbm(float x, float y, int octaves, float gain, float lacunarity);
// double fbm(double x, double y, int octaves);
double fbm3D(double x, double y, double z, int octaves, double lacunarity, double gain);
double fbm2D(double x, double y, int octaves, double lacunarity, double gain);
float perlin2d(float x, float y, float freq, int depth);
void initPermutation();
double ken_noise_vanilla(double x, double y, double z);
double ken_noise(double x, double y, double z, int depth, double freq);

double sigmoid(double n, double factor);

typedef enum {
  NOISE_QUALITY_FAST,
  NOISE_QUALITY_STD,
  NOISE_QUALITY_BEST,
} NoiseQuality;

double GradientNoise3D(double fx, double fy, double fz, int ix, int iy, int iz, int seed);
double GradientCoherentNoise3D(double x, double y, double z, int seed, NoiseQuality noiseQuality);
double PerlinNoise3D(double x, double y, double z, int seed, int octaves, double persistence);
