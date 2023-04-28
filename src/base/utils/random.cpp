#include "random.h"

#include <random>

#include <QtGlobal>

#ifdef Q_OS_WIN
#include <Windows.h>
#include <Ntsecapi.h>
#else  // Q_OS_WIN
#include <cerrno>
#include <cstdio>
#include <cstring>
#endif

#include <QString>

#include "base/global.h"
#include "base/utils/misc.h"

namespace
{
#ifdef Q_OS_WIN
    class RandomLayer
    {
    public:
        using result_type = uint32_t;

        RandomLayer()
            : m_rtlGenRandom {Utils::Misc::loadWinAPI<PRTLGENRANDOM>(u"Advapi32.dll"_qs, "SystemFunction036")}
        {
            if (!m_rtlGenRandom)
                qFatal("Failed to load RtlGenRandom()");
        }

        static constexpr result_type min()
        {
            return std::numeric_limits<result_type>::min();
        }

        static constexpr result_type max()
        {
            return std::numeric_limits<result_type>::max();
        }

        result_type operator()()
        {
            result_type buf = 0;
            const bool result = m_rtlGenRandom(&buf, sizeof(buf));
            if (!result)
                qFatal("RtlGenRandom() failed");

            return buf;
        }

    private:
        using PRTLGENRANDOM = BOOLEAN (WINAPI *)(PVOID, ULONG);
        const PRTLGENRANDOM m_rtlGenRandom;
    };
#else  // Q_OS_WIN
    class RandomLayer
    {
    public:
        using result_type = uint32_t;

        RandomLayer()
            : m_randDev {fopen("/dev/urandom", "rb")}
        {
            if (!m_randDev)
                qFatal("Failed to open /dev/urandom. Reason: %s. Error code: %d.\n", std::strerror(errno), errno);
        }

        ~RandomLayer()
        {
            fclose(m_randDev);
        }

        static constexpr result_type min()
        {
            return std::numeric_limits<result_type>::min();
        }

        static constexpr result_type max()
        {
            return std::numeric_limits<result_type>::max();
        }

        result_type operator()()
        {
            result_type buf = 0;
            const size_t size = sizeof(buf);
            const size_t result = fread(&buf, 1, size, m_randDev);
            if (result != size)
                qFatal("Failed to read from /dev/urandom. Reason: %s. Error code: %d.\n", std::strerror(errno), errno);

            return buf;
        }

    private:
        FILE *m_randDev;
    };
#endif

    std::mt19937 createRNG()
    {
        static RandomLayer rng;
        return std::mt19937(rng());
    }
}

int Random::next()
{
    static std::mt19937 rng = createRNG();
    static std::uniform_int_distribution<int> dist(0, std::numeric_limits<int>::max());
    return dist(rng);
}
