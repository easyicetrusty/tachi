# tachi

tachi is a public fork of [newsboat](https://newsboat.org), the RSS/Atom
feed reader for the text console. It carries two fixes on top of upstream.

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

## Build the RPM

The packaging lives in `tachi/`. The spec is based on Fedora's newsboat
spec, built from a `git archive` of the current commit, with Fedora's three
packaging patches and one small patch that sets the version string.

On a Fedora 45 build host or container (this is how tachi1 was built):

    sudo dnf install git-core rpm-build cargo-rpm-macros
    tachi/make-srpm.sh ~/tachi-rpmbuild
    rpmbuild -br --define "_topdir $HOME/tachi-rpmbuild" ~/tachi-rpmbuild/SPECS/newsboat.spec
    # only if rpmbuild -br exited 11 (missing build dependencies):
    sudo dnf builddep ~/tachi-rpmbuild/SRPMS/newsboat-*.buildreqs.nosrc.rpm
    rpmbuild -ba --define "_topdir $HOME/tachi-rpmbuild" ~/tachi-rpmbuild/SPECS/newsboat.spec

`mock -r fedora-45-x86_64 --rebuild` on the SRPM should also work, but it
was not tried for tachi1.

The version is `2.45.1^<date>git<commit>` with release `1.<dist>.tachi1`,
so it sorts above Fedora's 2.45.1 and below the next upstream release.
`newsboat -v` shows the git describe of the commit plus `-tachi`.
