#ifndef    __LIBRARY_TOOL_RANDOM_HH__
#define    __LIBRARY_TOOL_RANDOM_HH__

namespace fwk {
    /**
     *  \class Random Library/Tool/Random.hh
     *  \brief a class with static methods to generate pseudo-random numbers.
     */
    class Random {
    public:
        /**
         *  \brief Sets the seed for generating pseudo-random numbers.
         *  \param seed the seed.
         */
        static void  setSeed(unsigned int seed);

        /**
         *  \brief Sets the seed to the current timestamp.
         */
        static void  init(void);

        /**
         *  \brief Generates a pseudo-random signed 32 bits integer, between two values.
         *  \param min the minimum value.
         *  \param max the maximum value.
         *  \return the random value.
         */
        static int32_t  getInt(int32_t min, int32_t max);
    };
}

#endif    /* __LIBRARY_TOOL_RANDOM_HH__ */
