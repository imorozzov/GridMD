#include <gmd/threadpool.h>

std::map<const std::type_info*, gmdRedirectorBase*, gmdThreadPool::TypeInfoComparator> gmdThreadPool::mRedirectorsMap;
