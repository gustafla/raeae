#!/bin/sh
cp $TARGET.debugnovc $TARGET.stripped
sstrip $TARGET.stripped
cp $TARGET.stripped $TARGET.nounpacker
xz --format=lzma --lzma1=preset=9,lc=1,lp=0,pb=0 $TARGET.nounpacker
printf "HOME=/tmp/i;sed 1d \$0|lzcat>~;chmod +x ~;~;exit\n" >$TARGET
cat $TARGET.nounpacker.lzma >> $TARGET
chmod +x $TARGET
echo "Finished."
