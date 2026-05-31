/* api header devel file
 * vim: set noet tw=80 ts=4: */
#define _POSIX_C_SOURCE 200809L

#ifdef HOST
  #define FUNC(ret, name, ...) ret name(__VA_ARGS__)
#elif defined(LINK)
  #define FUNC(ret, name, ...) LINK(ret, name, __VA_ARGS__)
#else
  #define FUNC(ret, name, ...) ret (*name)(__VA_ARGS__) = NULL
#endif

FUNC(int, testfunc, void);

#undef FUNC
