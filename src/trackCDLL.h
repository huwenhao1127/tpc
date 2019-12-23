#include <armadillo>
#include "trackC.h"

#ifndef TRACKPOINTCLUSTER_TRACKCDLL_H
#define TRACKPOINTCLUSTER_TRACKCDLL_H
#ifdef TRACKCDLL
#define TRACKCDLL extern "C" _declspec(dllimport)
#else
#define TRACKCDLL extern "C" _declspec(dllexport)
#endif

extern "C" char * model_wrap(float *x, float *y, int length, int a);
extern "C" char * direction_test_wrap(float *x, float *y, int length);

#endif
