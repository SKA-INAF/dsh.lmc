#/bin/sh

CMD="TANGO_HOST=$TANGO_HOST TANGO_ROOT=$TANGO_ROOT PATH=$PATH LD_LIBRARY_PATH=$LD_LIBRARY_PATH OMNI_ROOT=$OMNI_ROOT ZEROMQ_ROOT=$ZEROMQ_ROOT MYSQL_USER=$MYSQL_USER
MYSQL_PASSWORD=$MYSQL_PASSWORD MYSQL_HOST=$MYSQL_HOST /etc/init.d/tango-db start"
#echo $CMD
#sudo `$CMD`