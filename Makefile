CGIC=-L./lib/cgic/ -lcgic
MYSQL=-L/usr/lib/x86_64-linux-gnu/ -l:libmysqlclient.a -lpthread -lz -lm -ldl -lssl -lcrypto
CFLAGS=
CPPFLAGS=-g --std=c++11
CC=gcc
AR=ar
RANLIB=ranlib

all: init_database.cgi record.cgi inject_script.cgi analytics.cgi

init_database.cgi: init_database.o lib/mysql/mysql.o lib/cgic/libcgic.a
	g++ init_database.o lib/mysql/mysql.o -o init_database.cgi ${CGIC} ${MYSQL} ${CPPFLAGS}

record.cgi: record.o lib/mysql/mysql.o lib/cgic/libcgic.a lib/ipdb/ipdb.o
	g++ record.o lib/ipdb/ipdb.o lib/mysql/mysql.o -o record.cgi ${CGIC} ${MYSQL} ${CPPFLAGS}

inject_script.cgi: inject_script.o lib/cgic/libcgic.a
	g++ inject_script.o -o inject_script.cgi ${CGIC} ${CPPFLAGS}

analytics.cgi: analytics.o lib/cgic/libcgic.a lib/mysql/mysql.o
	g++ analytics.o lib/cgic/libcgic.a lib/mysql/mysql.o -o analytics.cgi  ${CGIC} ${MYSQL} ${CPPFLAGS}

analytics.o: analytics.cpp
	g++ -c analytics.cpp ${CPPFLAGS}

init_database.o: init_database.cpp
	g++ -c init_database.cpp ${CPPFLAGS}

record.o: record.cpp
	g++ -c record.cpp ${CPPFLAGS}

inject_script.o: inject_script.cpp
	g++ -c inject_script.cpp ${CPPFLAGS}

# mysql
lib/mysql/mysql.o: lib/mysql/MySQL.cpp
	g++ -o lib/mysql/mysql.o -c lib/mysql/MySQL.cpp ${MYSQL} ${CPPFLAGS}

# cgic library
lib/cgic/libcgic.a: lib/cgic/cgic.o lib/cgic/cgic.h
	rm -f lib/cgic/libcgic.a
	$(AR) rc lib/cgic/libcgic.a lib/cgic/cgic.o
	$(RANLIB) lib/cgic/libcgic.a	

# ipip.net 地区数据库
lib/ipdb/ipdb.o: lib/ipdb/ipdb.cpp
	g++ -o lib/ipdb/ipdb.o -c lib/ipdb/ipdb.cpp ${CPPFLAGS}

# 将编译后的文件复制到 Apache Docker 目录中
deploy: record.cgi init_database.cgi inject_script.cgi analytics.cgi
	rm -f data/app/cgi-bin/*
	cp record.cgi data/app/cgi-bin/record.cgi
	cp init_database.cgi data/app/cgi-bin/init_database.cgi
	cp inject_script.cgi data/app/cgi-bin/inject_script.cgi
	cp analytics.cgi data/app/cgi-bin/analytics.cgi
	cp assets/example.js data/app/cgi-bin/example.js
	cp assets/ipipfree.ipdb data/app/cgi-bin/ipipfree.ipdb

# 清理编译的文件
clean:
	rm -f lib/cgic/libcgic.a lib/cgic/cgic.o
	rm -f lib/mysql/mysql.o
	rm -f record.o init_database.o inject_script.o analytics.o

clean-all:
	make clean
	rm -f record.cgi init_database.cgi inject_script.cgi analytics.cgi

clean-deploy:
	make clean-all
	rm -f data/app/cgi-bin/*