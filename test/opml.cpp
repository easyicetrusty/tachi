#include "opml.h"

#include <libxml/xmlsave.h>

#include "3rd-party/catch.hpp"
#include "cache.h"
#include "rssfeed.h"
#include "test_helpers/envvar.h"
#include "test_helpers/misc.h"
#include "test_helpers/tempfile.h"

using namespace newsboat;

TEST_CASE("opml::generate creates an XML document with feed URLs in OPML format",
	"[Opml]")
{
	const auto check =
		[]
		(const FeedContainer& feedcontainer, bool version2)
	-> std::string {
		xmlDocPtr opml = opml::generate(feedcontainer, version2);

		xmlBufferPtr buffer = xmlBufferCreate();

		xmlSaveCtxtPtr context = xmlSaveToBuffer(buffer, nullptr, 0);
		xmlSaveDoc(context, opml);
		xmlSaveClose(context);

		xmlFreeDoc(opml);

		const std::string opmlText(
			reinterpret_cast<const char*>(xmlBufferContent(buffer)),
			xmlBufferLength(buffer));

		xmlBufferFree(buffer);
		buffer = nullptr;

		return opmlText;
	};

	SECTION("No feeds") {
		FeedContainer feeds;

		const std::string expectedOpmlText(
			"<?xml version=\"1.0\"?>\n"
			"<opml version=\"2.0\">"
			"<head><title>Newsboat - Exported Feeds</title></head>"
			"<body/></opml>\n");

		REQUIRE(check(feeds, true) == expectedOpmlText);
	}

	SECTION("A few feeds") {
		ConfigContainer cfg;
		auto rsscache = Cache::in_memory(cfg);
		FeedContainer feeds;

		std::shared_ptr<RssFeed> feed =
			std::make_shared<RssFeed>(rsscache.get(), "https://example.com/feed1.xml");
		feed->set_title("Feed 1");
		feed->set_link("https://example.com/feed1/");
		feeds.add_feed(std::move(feed));

		feed = std::make_shared<RssFeed>(rsscache.get(), "https://example.com/feed2.xml");
		feed->set_title("Feed 2");
		feed->set_link("https://example.com/feed2/");
		feed->set_tags({"tag", "tag,with,commas", "tag/with/slashes", "tag with spaces"});
		feeds.add_feed(std::move(feed));

		const std::string expectedOpml1Text(
			"<?xml version=\"1.0\"?>\n"
			"<opml version=\"1.0\">"
			"<head><title>Newsboat - Exported Feeds</title></head>"
			"<body>"
			"<outline type=\"rss\" "
			"xmlUrl=\"https://example.com/feed1.xml\" "
			"htmlUrl=\"https://example.com/feed1/\" "
			"title=\"Feed 1\" "
			"text=\"Feed 1\"/>"
			"<outline type=\"rss\" "
			"xmlUrl=\"https://example.com/feed2.xml\" "
			"htmlUrl=\"https://example.com/feed2/\" "
			"title=\"Feed 2\" "
			"text=\"Feed 2\"/>"
			"</body>"
			"</opml>\n");

		const std::string expectedOpml2Text(
			"<?xml version=\"1.0\"?>\n"
			"<opml version=\"2.0\">"
			"<head><title>Newsboat - Exported Feeds</title></head>"
			"<body>"
			"<outline type=\"rss\" "
			"xmlUrl=\"https://example.com/feed1.xml\" "
			"htmlUrl=\"https://example.com/feed1/\" "
			"title=\"Feed 1\" "
			"text=\"Feed 1\" "
			"category=\"\"/>"
			"<outline type=\"rss\" "
			"xmlUrl=\"https://example.com/feed2.xml\" "
			"htmlUrl=\"https://example.com/feed2/\" "
			"title=\"Feed 2\" "
			"text=\"Feed 2\" "
			"category=\"tag,tag_with_commas,tag/with/slashes,tag with spaces\"/>"
			"</body>"
			"</opml>\n");

		REQUIRE(check(feeds, false) == expectedOpml1Text);
		REQUIRE(check(feeds, true) == expectedOpml2Text);
	}
}
