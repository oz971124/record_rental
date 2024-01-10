#include "connection.h"
#include <iostream>

Connection::Connection()
{
	conn = mysql_init(NULL);
	mysql_options(conn, MYSQL_OPT_CONNECT_TIMEOUT, &timeout_sec);
	conn_result = mysql_real_connect(conn, IP, USER, PASSWORD, DB_NAME, PORT_NUMBER, NULL, 0);

	if (conn_result == NULL)
	{
		printf("DB Connection Fail\n");
	}
	else
	{
		printf("DB Connection Success\n");
	}
}

Connection::~Connection()
{
	printf("Connection FINISH\n");
}