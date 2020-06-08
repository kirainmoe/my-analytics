CGIC=-L./lib/cgic/ -lcgic
MYSQL=-L/usr/lib/x86_64-linux-gnu/ -l:libmysqlclient.a -lpthread -lz -lm -ldl -lssl -lcrypto
CURL=-lcurl
CFLAGS=
CPPFLAGS=-g --std=c++11
CC=gcc
AR=ar
RANLIB=ranlib

all: init_database.cgi record.cgi inject_script.cgi
	make clean

init_database.cgi: init_database.o lib/mysql/mysql.o lib/cgic/libcgic.a
	g++ init_database.o lib/mysql/mysql.o -o init_database.cgi ${CGIC} ${MYSQL} ${CPPFLAGS}

record.cgi: record.o lib/mysql/mysql.o lib/cgic/libcgic.a lib/request/request.o lib/ipdb/ipdb.o
	g++ record.o lib/ipdb/ipdb.o lib/request/request.o lib/mysql/mysql.o -o record.cgi ${CGIC} ${MYSQL} ${CPPFLAGS} ${CURL}

inject_script.cgi: inject_script.o lib/cgic/libcgic.a
	g++ inject_script.o -o inject_script.cgi ${CGIC} ${CPPFLAGS}

init_database.o: init_database.cpp
	g++ -c init_database.cpp ${CPPFLAGS}

record.o: record.cpp
	g++ -c record.cpp ${CPPFLAGS}

inject_script.o: inject_script.cpp
	g++ -c inject_script.cpp ${CPPFLAGS}

# mysql
lib/mysql/mysql.o: lib/mysql/MySQL.cpp
	g++ -o lib/mysql/mysql.o -c lib/mysql/MySQL.cpp ${MYSQL} ${CPPFLAGS}

# curl
lib/request/request.o: lib/request/request.cpp
	g++ -o lib/request/request.o -c lib/request/request.cpp ${CURL} ${CPPFLAGS}

# cgic library
lib/cgic/libcgic.a: lib/cgic/cgic.o lib/cgic/cgic.h
	rm -f lib/cgic/libcgic.a
	$(AR) rc lib/cgic/libcgic.a lib/cgic/cgic.o
	$(RANLIB) lib/cgic/libcgic.a	

# ipip.net 地区数据库
lib/ipdb/ipdb.o: lib/ipdb/ipdb.cpp
	g++ -o lib/ipdb/ipdb.o -c lib/ipdb/ipdb.cpp ${CPPFLAGS}

# 将编译后的文件复制到 Apache Docker 目录中
deploy: record.cgi init_database.cgi
	rm -f data/app/cgi-bin/*
	cp record.cgi data/app/cgi-bin/record.cgi
	cp init_database.cgi data/app/cgi-bin/init_database.cgi
	cp inject_script.cgi data/app/cgi-bin/inject_script.cgi
	cp assets/example.js data/app/cgi-bin/example.js
	cp assets/ipipfree.ipdb data/app/cgi-bin/ipipfree.ipdb

# 清理编译的文件
clean:
	rm -f lib/cgic/libcgic.a lib/cgic/cgic.o
	rm -f lib/mysql/mysql.o
	rm -f record.o init_database.o inject_script.o

clean-all:
	make clean
	rm -f record.cgi init_database.cgi inject_script.cgi