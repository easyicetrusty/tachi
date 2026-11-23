#include "opml.h"

#include <algorithm>
#include <cinttypes>
#include <cstring>
#include <iostream>
#include <sstream>

#include "fileurlwriter.h"
#include "logger.h"
#include "rssfeed.h"
#include "utils.h"

namespace newsboat {

xmlDocPtr opml::generate(const FeedContainer& feedcontainer, bool version2)
{
	xmlDocPtr root = xmlNewDoc((const xmlChar*)"1.0");
	xmlNodePtr opml_node =
		xmlNewDocNode(root, nullptr, (const xmlChar*)"opml", nullptr);
	xmlSetProp(opml_node,
		(const xmlChar*)"version",
		(const xmlChar*)(version2 ? "2.0" : "1.0"));
	xmlDocSetRootElement(root, opml_node);

	xmlNodePtr head = xmlNewTextChild(
			opml_node, nullptr, (const xmlChar*)"head", nullptr);
	xmlNewTextChild(head,
		nullptr,
		(const xmlChar*)"title",
		(const xmlChar*)PROGRAM_NAME " - Exported Feeds");
	xmlNodePtr body = xmlNewTextChild(
			opml_node, nullptr, (const xmlChar*)"body", nullptr);

	for (const auto& feed : feedcontainer.get_all_feeds()) {
		if (!utils::is_special_url(feed->rssurl())) {
			std::string rssurl = feed->rssurl();
			std::string link = feed->link();
			std::string title = feed->title();

			xmlNodePtr outline = xmlNewTextChild(body,
					nullptr,
					(const xmlChar*)"outline",
					nullptr);
			xmlSetProp(outline,
				(const xmlChar*)"type",
				(const xmlChar*)"rss");
			xmlSetProp(outline,
				(const xmlChar*)"xmlUrl",
				(const xmlChar*)rssurl.c_str());
			xmlSetProp(outline,
				(const xmlChar*)"htmlUrl",
				(const xmlChar*)link.c_str());
			xmlSetProp(outline,
				(const xmlChar*)"title",
				(const xmlChar*)title.c_str());
			xmlSetProp(outline,
				(const xmlChar*)"text",
				(const xmlChar*)title.c_str());

			if (version2) {
				// OPML 2.0 supports including tags
				std::vector<std::string> tags = feed->get_tags();
				std::string opml_tags;

				bool first_tag = true;
				for (auto t : tags) {
					utils::trim(t);
					t = utils::replace_all(t, ",", "_");
					if (first_tag) {
						first_tag = false;
					} else {
						opml_tags.append(",");
					}
					opml_tags.append(t);
				}
				xmlSetProp(outline,
					(const xmlChar*)"category",
					(const xmlChar*)opml_tags.c_str());
			}
		}
	}

	return root;
}

} // namespace newsboat
