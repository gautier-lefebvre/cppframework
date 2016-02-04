#ifdef    _WIN32
# include  <sys/timeb.h>
#else
# include  <sys/time.h>
#endif

#include  <sstream>
#include  <iomanip>

#include  "Library/Tool/Date.hh"

using namespace fwk;

int  Date::gettimeofday(struct timeval *tp) {
#ifdef  _WIN32
  struct _timeb timebuffer;
  _ftime(&timebuffer);
  tp->tv_sec = timebuffer.time;
  tp->tv_usec = timebuffer.millitm * 1000;
#else
  return ::gettimeofday(tp, nullptr);
#endif
  return 0;
}

const std::string Date::getTime(void) {
  struct timeval  t;
  struct tm    *tm;
  std::stringstream  ss;

  Date::gettimeofday(&t);
  tm = localtime(&t.tv_sec);
  ss << std::setw(2) << std::setfill('0') << tm->tm_hour << ":";
  ss << std::setw(2) << std::setfill('0') << tm->tm_min << ":";
  ss << std::setw(2) << std::setfill('0') << tm->tm_sec << ":";
  ss << std::setw(6) << std::setfill('0') << t.tv_usec;
  return ss.str();
}
