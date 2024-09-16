/**
 * @file Clock.h
 * @brief Defines the Clock class for precise time measurements and sleep functionality.
 * @author Simon Chan
 * @date September 15, 2024
 */

#pragma once

#ifndef __CLOCK_H__
#define __CLOCK_H__

// Include standard headers
#include <cstdint>

namespace lof {

    /**
     * @class Clock
     * @brief Provides functionality for measuring elapsed time and sleeping.
     */
    class Clock {
    private:
        int64_t m_previous_time; ///< Previous call to delta() (in microseconds).

    public:
        /**
         * @brief Constructor for Clock. Sets previous_time to current time.
         */
        Clock();

        /**
         * @brief Calculates time elapsed since last call and resets the clock.
         * @return Time elapsed in microseconds, or -1 if an error occurred.
         */
        int64_t delta();

        /**
         * @brief Calculates time elapsed since last delta() call without resetting the clock.
         * @return Time elapsed in microseconds, or -1 if an error occurred.
         */
        int64_t split() const;

        /**
         * @brief Sleeps for the specified number of microseconds.
         * @param microseconds The number of microseconds to sleep.
         */
        static void sleep(int64_t microseconds);
    };

} // end of namespace lof

#endif // __CLOCK_H__