// Stopwatch timer (based on boost::timer::cpu_timer)

#ifndef GRIMD_STOPWATCH_H
#define GRIMD_STOPWATCH_H

#include <boost/version.hpp>
#if BOOST_VERSION >= 104800

#include <boost/timer/timer.hpp>
class gmdStopWatch {
protected:
  boost::timer::cpu_timer timer;

public:
  void Pause() { timer.stop(); }
  void Resume() { timer.resume(); }

  long Time() const {
    boost::timer::cpu_times times(timer.elapsed());
    return (long)(times.wall / 1000000LL);
  }

  gmdStopWatch() {}
};
#else

#include <boost/timer.hpp>
class gmdStopWatch {
protected:
  //boost::timer timer;
  std::clock_t elapsed, start;
  bool atrun;

public:
  gmdStopWatch() : elapsed(0), start(std::clock()), atrun(true) {}

  void Pause() { if(atrun) { elapsed += std::clock()-start;  atrun=false; }; }
  void Resume() { if(!atrun) { atrun=true; start=std::clock(); }; }

  long Time() const {
      return ( (atrun ? (std::clock()-start) : std::clock_t())
               + elapsed
             ) *1000L / CLOCKS_PER_SEC;
  }

};

#endif

#endif //GRIMD_STOPWATCH_H

