#!/bin/bash

ROOT=$PWD

if [[ ! -e upstream ]]
then
  mkdir upstream
fi

START_TIME=$(date +%s)

for cfile in $(find . -name \*.[ch] | grep -v compat | grep -v upstream)
do
  # remove leading "./"
  filename=${cfile/\.\//}

  # compute distant path
  path=drivers/hid/${filename}
  if [[ "$filename" == *include* ]] # files in include/ are at the root directory
  then
    path=${filename}
  fi

  # check if output dir exists, if not, create it
  outdir=upstream/$(dirname ${filename})
  if [[ ! -e ${outdir} ]]
  then
    echo create ${outdir}
    mkdir -p ${outdir}
  fi

  TIMESTAMP="0"

  # check the timestamp of the current local upstream file, if < 15 min, do not
  # retrieve it
  if [[ -e upstream/${filename} ]]
  then
    TIMESTAMP=$(date +%s -r upstream/${filename})
  fi

  let "diff_timestamp = $START_TIME - $TIMESTAMP"
  diff_timestamp=${diff_timestamp#-}
  if [ "${diff_timestamp}" -lt "900" ]
  then
    echo keeping current version of upstream/${filename}
  else
    # retrieve the distant file
    echo -n getting ${filename}...
    wget -q http://git.kernel.org/cgit/linux/kernel/git/jikos/hid.git/plain/${path}?h=for-next -O upstream/${filename}
    result_wget=$?

    if [ ${result_wget} == 8 ]
    then
      # 404 not found
      rm upstream/${filename}
      echo FAILED
    else
      echo OK
    fi
  fi
done

for cfile in $(find upstream -name \*.[ch])
do
  # remove leading "upstream/"
  filename=${cfile/upstream\//}

  # compare the two files
  diff_output="not ok"
  diff upstream/${filename} ${filename} > /dev/null && diff_output="ok"

  if [ x"${diff_output}" == x"not ok" ]
  then
    # files do not match
    echo files ${filename} do not match
    meld upstream/${filename} ${filename}
  fi
done

