#!/bin/bash -eu

### Config
DEV_BRANCH=develop
MAIN_BRANCH=master
REPO=preesm/spider

# git root dir
DIR=$(cd `dirname $0` && echo `git rev-parse --show-toplevel`)


if [ "$#" -ne "1" ]; then
  CURRENT_VERSION=$(cat ${DIR}/VERSION)
  echo -e "usage: $0 <new version>\nNote: current version = ${CURRENT_VERSION}"
  exit 1
fi

# First check access on git (will exit on error)
echo "Testing Github permission"
git ls-remote git@github.com:${REPO}.git > /dev/null


#warning
echo "Warning: this script will delete ignored files and remove all changes in $DEV_BRANCH and $MAIN_BRANCH"
read -p "Do you want to conitnue ? [NO/yes] " ANS
LCANS=`echo "${ANS}" | tr '[:upper:]' '[:lower:]'`
[ "${LCANS}" != "yes" ] && echo "Aborting." && exit 1

NEW_VERSION=$1

CURRENT_BRANCH=$(cd `dirname $0` && echo `git branch`)
ORIG_DIR=`pwd`
TODAY_DATE=`date +%Y.%m.%d`
#change to git root dir
cd $DIR

#move to dev branch and clean repo
git stash -u
git checkout $MAIN_BRANCH
git pull
git checkout $DEV_BRANCH
git reset --hard
git clean -xdf

#update version in code and stash changes
./releng/update-version.sh $NEW_VERSION
sed -i -e "s/X\.Y\.Z/$NEW_VERSION/g" release_notes.md
sed -i -e "s/XXXX\.XX\.XX/$TODAY_DATE/g" release_notes.md
git stash

# Fix headers
./releng/fix_header_copyright_and_authors.sh
# commit fixed headers (if any)
NBCHANGES=`git status --porcelain | wc -l`
if [ $NBCHANGES -ne 0 ]; then
  git add -A
  git commit -m "[RELENG] Fix headers"
fi

# make sure integration works before deploying and pushing
git stash pop
./releng/build_and_test.sh

#commit new version in develop
git add -A
git commit -m "[RELENG] Prepare version $NEW_VERSION"

#merge in master, add tag
git checkout $MAIN_BRANCH
git merge --no-ff $DEV_BRANCH -m "merge branch '$DEV_BRANCH' for new version $NEW_VERSION"
git tag v$NEW_VERSION

sleep 0.2

#move to snapshot version in develop and push
git checkout $DEV_BRANCH
./releng/update-version.sh $NEW_VERSION-SNAPSHOT
cat release_notes.md | tail -n +3 > tmp
cat > release_notes.md << EOF
Spider Changelog
================

## Release version X.Y.Z
*XXXX.XX.XX*

### New Feature

### Changes

### Bug fix

EOF
cat tmp >> release_notes.md
rm tmp
git add -A
git commit -m "[RELENG] Move to snapshot version"

sleep 0.2

#deploy from master
git checkout $MAIN_BRANCH
./releng/deploy.sh

#push if everything went fine
git push --tags
git push
git checkout $DEV_BRANCH
git push
