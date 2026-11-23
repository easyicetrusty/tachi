# tachi

tachi is a public fork of [newsboat](https://newsboat.org), the RSS/Atom
feed reader for the text console. It carries two fixes on top of upstream
and removes three features that can run commands from outside data.

Anyone may use this code under newsboat's MIT licence. The upstream
`LICENSE` file and the authors' copyright notices are kept as they are.
tachi is not affiliated with, endorsed by or supported by the newsboat
project. Please do not report tachi problems to upstream newsboat.

The program it installs is still called `newsboat`, with the same config,
urls file and cache, so an existing setup keeps working.

## What is different from upstream

Branch `main` is upstream master at commit `e234ad07` plus two changes.

1. Crash fix for upstream newsboat issue 2101.
   Newsboat could crash when the feed list moved while a reload was running.
   `ListWidgetBackend` and `Stfl::Form` now take a lock, so the reload
   threads and the UI no longer touch the same list or form at once.
   Two commits.
2. Faster ignore rules for upstream newsboat issue 345.
   Ignore-article rules are now applied once per feed, not once per item.
   With many rules, feeds load faster. Three commits, the first two add tests.

The `#2101` and `#345` in commit messages refer to those upstream issue
numbers, not to issues in this repository.

tachi is now independent software. It does not track upstream newsboat
and will not be rebased on newer upstream releases. Upstream tags are kept
for history.

## Removed features

tachi removes features that let a file or a feed from outside run commands
on your computer. They are gone from the code, not just switched off.

- OPML import. `newsboat -i` and `--import-from-opml` print
  "Error: OPML import is not supported in tachi" and exit with a failure
  code. `urls-source "opml"` (with `opml-url`) stops newsboat at start-up
  with an error. Add feeds by writing their URLs into the urls file.
  OPML export (`-e`, `--export-to-opml`, `--export-to-opml2`) still works.
- `exec:` feeds, which ran a shell command and read its output as a feed.
- `filter:` feeds, which piped a download through a shell command.

A line starting with `exec:` or `filter:` in the urls file is skipped with
an error that names the file and line, and its command is never run. The
other feeds load as before. `query:` feeds, http(s) feeds, `browser`,
macros, `ignore-article` and podboat work as before.

## Build the RPM

The packaging lives in `tachi/`. The spec is based on Fedora's newsboat
spec, built from a `git archive` of the current commit, with Fedora's three
packaging patches and one small patch that sets the version string.

On a Fedora 45 build host or container (this is how tachi1 and tachi2 were
built):

    sudo dnf install git-core rpm-build cargo-rpm-macros
    tachi/make-srpm.sh ~/tachi-rpmbuild
    rpmbuild -br --define "_topdir $HOME/tachi-rpmbuild" ~/tachi-rpmbuild/SPECS/newsboat.spec
    # only if rpmbuild -br exited 11 (missing build dependencies):
    sudo dnf builddep ~/tachi-rpmbuild/SRPMS/newsboat-*.buildreqs.nosrc.rpm
    rpmbuild -ba --define "_topdir $HOME/tachi-rpmbuild" ~/tachi-rpmbuild/SPECS/newsboat.spec

`mock -r fedora-45-x86_64 --rebuild` on the SRPM should also work, but it
was not tried for tachi1 or tachi2.

The version is `2.45.1^<date>.<n>git<commit>` with release
`1.<dist>.tachi2`. `<n>` counts the commits made on `<date>` (UTC), so two
builds from the same day still sort in commit order: rpm compares the
version before the release, and the commit hash alone sorts at random.
tachi1 used `2.45.1^<date>git<commit>`; any `<date>.<n>` version sorts
above it. All tachi versions sort above Fedora's 2.45.1 and below the next
upstream release.
`newsboat -v` shows the git describe of the commit plus `-tachi`.
