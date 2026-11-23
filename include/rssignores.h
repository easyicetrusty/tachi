#ifndef NEWSBOAT_RSSIGNORES_H_
#define NEWSBOAT_RSSIGNORES_H_

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "configactionhandler.h"
#include "matcher.h"
#include "rssitem.h"

namespace newsboat {

class Regex;
class RssFeed;

class RssIgnores : public ConfigActionHandler {
public:
	RssIgnores() {}
	~RssIgnores() override {}
	void handle_action(std::string_view action,
		const std::vector<std::string>& params) override;
	void dump_config(std::vector<std::string>& config_output) const override;
	bool matches(RssItem* item);

	/// Removes items that match any `ignore-article` rule from the feed.
	///
	/// Works out once which rules apply to the feed's URL, then checks
	/// each item against those rules only. An item is kept if a rule can't
	/// be evaluated for it. Doesn't lock `feed.item_mutex`.
	void filter_feed(RssFeed& feed);

	bool matches_lastmodified(const std::string& url);
	bool matches_resetunread(const std::string& url);

private:
	bool matches_expr(std::shared_ptr<Matcher> expr, RssItem* item);
	std::vector<std::shared_ptr<Matcher>> rules_for_feed(
			const std::string& feedurl) const;
	bool matches_any(const std::vector<std::shared_ptr<Matcher>>& rules,
		RssItem* item);

	struct RegexIgnore {
		std::string pattern;
		std::shared_ptr<Regex> regex;
		std::shared_ptr<Matcher> expr;
	};

	std::vector<RegexIgnore> regex_ignores;
	std::unordered_multimap<std::string, std::shared_ptr<Matcher>> non_regex_ignores;
	std::vector<std::string> ignores_lastmodified;
	std::vector<std::string> resetflag;

	static const std::string REGEX_PREFIX;
};

} // namespace newsboat

#endif /* NEWSBOAT_RSSIGNORES_H_ */
