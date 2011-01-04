#!/bin/bash

# Wrapper script for maven
[ -z $DYNAMO_HOME ] && echo "DYNAMO_HOME not set" && exit 1

os=`uname | awk '{ print tolower($0) }'`

export PATH=$DYNAMO_HOME/bin:$PATH
export PATH=$DYNAMO_HOME/ext/bin:$DYNAMO_HOME/ext/bin/${os}:$PATH
export LD_LIBRARY_PATH=$DYNAMO_HOME/lib:$DYNAMO_HOME/ext/lib/${os}
export PYTHONPATH=$DYNAMO_HOME/lib/python:$DYNAMO_HOME/ext/lib/python

ln -sfn $DYNAMO_HOME ../com.dynamo.cr.common/DYNAMO_HOME
ln -sfn $DYNAMO_HOME ../com.dynamo.cr.contenteditor/DYNAMO_HOME

$DYNAMO_HOME/ext/share/maven/bin/mvn $@
