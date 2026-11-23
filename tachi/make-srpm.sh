#!/bin/bash
# Build the tachi SRPM from the current git HEAD (needs git and rpm-build).
# usage: tachi/make-srpm.sh [topdir]    default topdir: ../tachi-rpmbuild
set -euo pipefail
top=$(git rev-parse --show-toplevel)
cd "$top"
[ -z "$(git status --porcelain)" ] || { echo "make-srpm: working tree is not clean" >&2; exit 1; }
out=$(realpath -m "${1:-$top/../tachi-rpmbuild}")
commit=$(git rev-parse HEAD)
short=$(git rev-parse --short=8 HEAD)
date=$(TZ=UTC git log -1 --date=format-local:%Y%m%d --format=%cd HEAD)
cldate=$(TZ=UTC git log -1 --date=format-local:'%a %b %d %Y' --format=%cd HEAD)
describe=$(git describe --abbrev=4 --tags HEAD)
# Commits made on the same UTC day as HEAD: makes same-day builds sort in order
snapnum=$(TZ=UTC git log --date=format-local:%Y%m%d --format=%cd HEAD | grep -cx "$date")
mkdir -p "$out/SOURCES" "$out/SPECS" "$out/SRPMS"
git archive --format=tar.gz --prefix="newsboat-$short/" -o "$out/SOURCES/newsboat-$short.tar.gz" HEAD
cp tachi/*.patch "$out/SOURCES/"
sed -e "s/@COMMIT@/$commit/g" -e "s/@SHORT@/$short/g" -e "s/@DATE@/$date/g" \
    -e "s/@CLDATE@/$cldate/g" -e "s/@DESCRIBE@/$describe/g" -e "s/@SNAPNUM@/$snapnum/g" \
    tachi/newsboat.spec.in > "$out/SPECS/newsboat.spec"
rpmbuild -bs --define "_topdir $out" "$out/SPECS/newsboat.spec"
