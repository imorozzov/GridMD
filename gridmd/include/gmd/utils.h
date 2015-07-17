// Sustem-level operations

#ifndef GRIMD_UTILS_H
#define GRIMD_UTILS_H

#include <gmd/string.h>
#include <gmd/arrstr.h>


enum {
    gmdEXEC_ASYNC = 0, // execute the process asynchronously
    gmdEXEC_SYNC = 1   // execute it synchronously
};

long gmdExecute(gmdChar **argv,
                gmdArrayString* output, gmdArrayString* error, int sync);

long gmdExecute(const gmdString& command,
                gmdArrayString* output, gmdArrayString* error, int sync);

inline long gmdExecute(const gmdString& command,
                       gmdArrayString& output, gmdArrayString& error) {
  return gmdExecute(command, &output, &error, gmdEXEC_SYNC);
}

inline long gmdExecute(const gmdString& command, int sync = gmdEXEC_ASYNC) {
  return gmdExecute(command, NULL, NULL, sync);
}

enum gmdKillFlags {
    gmdKILL_NOCHILDREN = 0,  // don't kill children
    gmdKILL_CHILDREN = 1     // kill children
};
enum gmdSignal {
  gmdSIGNONE = 0,            // verify if the process exists under Unix
  gmdSIGKILL = 9             // forcefully kill process
};

int gmdKill(long pid, gmdSignal sig, int *krc = NULL, int flags = 0);

void gmdMilliSleep(unsigned long milliseconds);
inline void gmdSleep(unsigned long secs) {
  gmdMilliSleep(secs*1000);
}

gmdString gmdGetUserName();
gmdString gmdGetHostName();
gmdString gmdGetOsDescription();

#endif
