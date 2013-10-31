#pragma once

#include "environment.h"

class LispData;

LispData Eval(const LispData&, EnvPtr env = Env::global);

