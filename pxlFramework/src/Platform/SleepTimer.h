#pragma once

#ifdef _WIN32
    #include "Windows/WindowsHighResSleepTimer.h"
#endif

#ifdef linux
    #include "Linux/LinuxHighResSleepTimer.h"
#endif