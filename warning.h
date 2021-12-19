#ifndef __WARNING_H__
#define __WARNING_H__

#define ERROR printf("%s: In function '%s'\n%s:%d: error: ", __FILE__, __func__, __FILE__, __LINE__);
#define WARNING printf("%s: In function '%s'\n%s:%d: warning: ", __FILE__, __func__, __FILE__, __LINE__);

#endif

