#include "MysqlDB.h"
#include "../../Public/Exception.h"
#pragma comment(lib, "../Lib/libmysql.lib")

using namespace DAL;
using namespace PUBLIC;

MysqlDB::MysqlDB() : mysql_(NULL)
{
}

MysqlDB::~MysqlDB()
{
	if (mysql_)
	{
		Close();
	}
}
void MysqlDB::Open(const char* host,
				   const char* user,
				   const char* passwd,
				   const char* db,
				   unsigned int port)
{
	//分配、初始化、并返回新对象
	mysql_ = mysql_init(NULL);   
	if (mysql_ == NULL)
	{
		string errmsg = mysql_error(mysql_);  //返回描述错误的、由Null终结的字符串
		throw Exception("DB ERROR:"+errmsg);
	}

	//设置连接方式
	char reconnect = true;
	mysql_options(mysql_, MYSQL_OPT_RECONNECT, &reconnect); //如果发现连接丢失，启动或禁止与服务器的自动再连接
	mysql_options(mysql_, MYSQL_SET_CHARSET_NAME, "gbk");   //设置字符集

	//与运行在主机上的MySQL数据库引擎建立连接
	//如果连接成功，返回MYSQL*连接句柄。如果连接失败，返回NULL。
	if (!mysql_real_connect(mysql_, host, user, passwd, db, 0, NULL, 0))
	{
		string errmsg = mysql_error(mysql_);
		Close();
		throw Exception("DB ERROR:"+errmsg);
	}
}

void MysqlDB::Close()
{
	if (NULL != mysql_)
	{
		mysql_close(mysql_);  //关闭前面打开的连接
		mysql_ = NULL;
	}
}

MysqlRecordset MysqlDB::QuerySQL(const char* sql)
{
	if (mysql_query(mysql_, sql) != 0)
	{
		//int errno = mysql_errno(mysql_);
		string errmsg = mysql_error(mysql_);
		throw Exception("DB ERROR:"+errmsg);
	}

	MYSQL_RES*  mysql_res;
	mysql_res = mysql_store_result(mysql_);

	//得到查询返回的行数
	//unsigned long n = mysql_affected_rows(mysql_);	

	//指向  mysql 的查询字段集
	MYSQL_FIELD* mysql_field = NULL;
	
	MysqlRecordset rs;
	unsigned int i = 0;
	unsigned int nCols = mysql_num_fields(mysql_res);
	while ((mysql_field = mysql_fetch_field(mysql_res)) != NULL)
	{
		MysqlRecordset::FIELD field;
		field.name = mysql_field->name;
		field.index = i;
		++i;
		rs.fields_.push_back(field);
	}
 
	MYSQL_ROW mysql_row;
	while ((mysql_row = mysql_fetch_row(mysql_res)))
	{
		MysqlRecordset::ROW row(nCols);
		for (unsigned int i = 0; i< nCols; ++i)
		{
			row[i] = mysql_row[i] ? mysql_row[i] : "";
		}
		rs.rows_.push_back(row);
		
	}
	

	mysql_free_result(mysql_res);

	return rs;

}

unsigned long long MysqlDB::ExecSQL(const char* sql)
{
	if (mysql_query(mysql_, sql) != 0)
	{
		//int errno = mysql_errno(mysql_);
		string errmsg = mysql_error(mysql_);
		throw Exception("DB ERROR:"+errmsg);
	}

	return mysql_affected_rows(mysql_);

}

void MysqlDB::StartTransaction()
{
	if (mysql_query(mysql_, "START TRANSACTION") != 0)
	{
		//int errno = mysql_errno(mysql_);
		string errmsg = mysql_error(mysql_);
		throw Exception("DB ERROR:"+errmsg);
	}
}

void MysqlDB::Commit()
{
	if (mysql_query( mysql_, "COMMIT") != 0)
	{
		//int errno = mysql_errno(mysql_);
		string errmsg = mysql_error(mysql_);
		throw Exception("DB ERROR:"+errmsg);
	}
}

void MysqlDB::Rollback()
{
	if (mysql_query(mysql_, "ROLLBACK") == 0)
	{
		//int errno = mysql_errno(mysql_);
		string errmsg = mysql_error(mysql_);
		throw Exception("DB ERROR:"+errmsg);
	}
}


unsigned long long MysqlDB::GetInsertId() const
{
	return mysql_insert_id(mysql_);
}