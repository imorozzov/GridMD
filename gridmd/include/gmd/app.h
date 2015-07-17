// Library initialization

#ifndef GRIMD_APP_H
#define GRIMD_APP_H


inline bool gmdInitialize(int argc = 0, char **argv = NULL) {
  (void) argc; (void) argv;
  return true;
}

inline void gmdUninitialize() {}

class gmdInitializer
{
public:
  gmdInitializer(int argc = 0, char **argv = NULL) {
    gmdInitialize(argc, argv);
  }

  ~gmdInitializer() {
    gmdUninitialize();
  }
};


#endif
