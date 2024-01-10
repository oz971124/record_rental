// DB ¿¬°á
#include <mysql.h>

#pragma comment (lib, "libmariadb.lib")

const char IP[] = "127.0.0.1";
const char USER[] = "root";
const char PASSWORD[] = "1234";
const char DB_NAME[] = "RecordDB";
const int PORT_NUMBER = 3306;

class Connection
{
public:
	Connection();
	~Connection();
	MYSQL* conn;
	MYSQL* conn_result;

private:
	
	unsigned int timeout_sec = 1;
};

