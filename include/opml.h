#ifndef NEWSBOAT_OPML_H_
#define NEWSBOAT_OPML_H_

#include <libxml/tree.h>

#include "feedcontainer.h"

namespace newsboat {

namespace opml {
xmlDocPtr generate(const FeedContainer& feedcontainer, bool version2);
}

} // namespace newsboat

#endif /* NEWSBOAT_OPML_H_ */
