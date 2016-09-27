#ifndef    __LIBRARY_TOOL_DATE_HH__
#define    __LIBRARY_TOOL_DATE_HH__

#include  <iostream>
#include  <ctime>

namespace fwk {
  /**
   *  \class Date Library/Tool/Date.hh
   *  \brief a class containing static methods concerning the time.
   */
  class Date {
  public:
    /**
     *  \brief cross-platform inplementation of the UNIX gettimeofday function.
     *
     *  On UNIX, simply does a gettimeofday.
     *  \return the return value of gettimeofday, or 0 on Windows.
     */
    static int  gettimeofday(struct timeval *);

    /**
     *  \brief returns the time in hh:mm:ss:µµµµµµ format.
     *  \return a string representing the current time.
     */
    static const std::string  getTime(void);
  };
}

#endif    /* __LIBRARY_TOOL_DATE_HH__ */
