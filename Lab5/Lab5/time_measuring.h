#pragma once
#include "serial_graph.h"
#include "parallel_graph.h"

#include <chrono>

double serial_solving(double* arr, int size);

double parallel_solving(double* arr, int size);