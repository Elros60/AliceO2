#!/bin/bash
TARGET_DIR=$1
LOCAL_DIR=$2
#SET=$3
# RUN=$4
PASS=$3

NEW_DIR=$PASS
mkdir -p $LOCAL_DIR/$NEW_DIR

for RUN_DEBUG in 523897 523821 523797 523792 523789 523788 523786 523783 523779 523731 523728 523677 523671 523669 523559 523541 523441 523401 523399 523397 523309 523308 523306 523298 523186 523182 523148 523142 523141
do
	cd $LOCAL_DIR/$NEW_DIR
	mkdir -p $RUN_DEBUG
	cd $RUN_DEBUG
	echo "Start processing $RUN_DEBUG..."

	COPY_DIR=$TARGET_DIR/$RUN_DEBUG/apass2_debug
	echo "Start copying from $COPY_DIR ..."
	CTF_LIST=`alien_ls $COPY_DIR`
	echo "Found jobs:"
	echo $CTF_LIST
	for CTF in $CTF_LIST
	do
		mkdir -p $CTF
		echo "Processing $CTF"
			
		EPN_LIST=`alien_ls $COPY_DIR/$CTF | grep o2_ctf*`
		echo "Found EPN:"
		echo $EPN_LIST
		for EPN in $EPN_LIST
		do
			for ITEM in log_archive.zip mchtracks.root stderr.log stdout.log muontracks.root
			do
				echo "Copying $ITEM ..."
				alien_cp alien:$COPY_DIR/$CTF/$EPN/$ITEM file:$LOCAL_DIR/$NEW_DIR/$RUN_DEBUG/$CTF/$ITEM
				echo "$ITEM copied."
			done
		done

	done

done

cd $LOCAL_DIR/$NEW_DIR
hadd -f mchtracks.root */*/mchtracks.root
hadd -f muontracks.root */*/muontracks.root