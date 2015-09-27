#ifndef		__LIBRARY_TOOL_RANDOM_HH__
#define		__LIBRARY_TOOL_RANDOM_HH__

class Random {
public:
	static void setSeed(unsigned int);

	static void init();

	static int32_t getInt(int32_t, int32_t);
};

#endif		/* __LIBRARY_TOOL_RANDOM_HH__ */