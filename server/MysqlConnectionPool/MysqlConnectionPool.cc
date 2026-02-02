#include "MysqlConnectionPool.h"
#include "MysqlConnectionPoolLog.h"

// 线程安全的懒汉单例函数接口
MysqlConnectionPool *MysqlConnectionPool::getConnectionPool()
{
	static MysqlConnectionPool pool; // lock和unlock
	return &pool;
}

// 给外部提供接口，从连接池中获取一个可用的空闲连接
std::shared_ptr<MysqlConnection> MysqlConnectionPool::getMysqlConnection()
{
	std::unique_lock<std::mutex> lock(_queueMutex);
	while (_connectionQue.empty())
	{
		if (std::cv_status::timeout == _cv.wait_for(lock, std::chrono::milliseconds(_connectionTimeout)))
		{
			if (_connectionQue.empty())
			{
				MYSQLLOG("获取空闲连接超时了...获取连接失败!");
				return nullptr;
			}
		}
	}

	// 自定义删除函数，避免shared_ptr生命周期结束后调用delete，而是应该将连接还给连接池
	std::shared_ptr<MysqlConnection> sp(_connectionQue.front(),
										[&](MysqlConnection *pcon)
										{
											// 这里是在服务器应用线程中调用的，所以一定要考虑队列的线程安全操作
											std::unique_lock<std::mutex> lock(_queueMutex);
											pcon->refreshAliveTime(); // 刷新一下开始空闲的起始时间
											_connectionQue.push(pcon);
										});

	_connectionQue.pop();
	_cv.notify_all(); // 消费完连接以后，通知生产者线程检查一下，如果队列为空了，赶紧生产连接

	return sp;
}

// 构造函数
MysqlConnectionPool::MysqlConnectionPool()
{
	// 加载配置文件
	if (!loadConfigFile())
	{
		return;
	}

	// 创建初始数量的连接
	for (int i = 0; i < _initSize; ++i)
	{
		MysqlConnection *p = new MysqlConnection();
		p->connect(_ip, _port, _username, _password, _dbname);
		p->refreshAliveTime(); // 刷新一下开始空闲的起始时间
		_connectionQue.push(p);
		_connectionCnt++;
	}

	// 启动一个新的线程，作为连接的生产者
	std::thread produce(std::bind(&MysqlConnectionPool::produceConnectionTask, this));
	produce.detach();

	// 启动一个新的定时线程，扫描超过maxIdleTime时间的空闲连接，进行对于的连接回收
	std::thread scanner(std::bind(&MysqlConnectionPool::scannerConnectionTask, this));
	scanner.detach();
}

// 从配置文件中加载配置项
bool MysqlConnectionPool::loadConfigFile()
{
	FILE *pf = fopen("../MysqlConnectionPool/Mysql.ini", "r");
	if (pf == nullptr)
	{
		MYSQLLOG("Mysql.ini file is not exist!");
		return false;
	}

	while (!feof(pf))
	{
		char line[1024] = {0};
		fgets(line, 1024, pf);
		std::string str = line;
		int idx = str.find('=', 0);
		if (idx == -1) // 无效的配置项
		{
			continue;
		}

		// password=123456\n
		int endidx = str.find('\n', idx);
		std::string key = str.substr(0, idx);
		std::string value = str.substr(idx + 1, endidx - idx - 1);

		if (key == "ip")
			_ip = value;
		else if (key == "port")
			_port = atoi(value.c_str());
		else if (key == "username")
			_username = value;
		else if (key == "password")
			_password = value;
		else if (key == "dbname")
			_dbname = value;
		else if (key == "initSize")
			_initSize = atoi(value.c_str());
		else if (key == "maxSize")
			_maxSize = atoi(value.c_str());
		else if (key == "maxIdleTime")
			_maxIdleTime = atoi(value.c_str());
		else if (key == "connectionTimeOut")
			_connectionTimeout = atoi(value.c_str());
	}
	return true;
}

// 运行在独立的线程中，专门负责生产新连接
void MysqlConnectionPool::produceConnectionTask()
{
	for (;;)
	{
		std::unique_lock<std::mutex> lock(_queueMutex);
		while (!_connectionQue.empty())
		{
			_cv.wait(lock); // 队列不空，此处生产线程进入等待状态
		}

		// 连接数量没有到达上限，继续创建新的连接
		if (_connectionCnt < _maxSize)
		{
			MysqlConnection *p = new MysqlConnection();
			p->connect(_ip, _port, _username, _password, _dbname);
			p->refreshAliveTime(); // 刷新一下开始空闲的起始时间
			_connectionQue.push(p);
			_connectionCnt++;
		}

		// 通知消费者线程，可以消费连接了
		_cv.notify_all();
	}
}

// 扫描超过maxIdleTime时间的空闲连接，进行对于的连接回收
void MysqlConnectionPool::scannerConnectionTask()
{
	for (;;)
	{
		// 通过sleep模拟定时效果
		std::this_thread::sleep_for(std::chrono::seconds(_maxIdleTime));

		// 扫描整个队列，释放多余的连接
		std::unique_lock<std::mutex> lock(_queueMutex);
		while (_connectionCnt > _initSize)
		{
			MysqlConnection *p = _connectionQue.front();
			if (p->getAliveeTime() >= (_maxIdleTime * 1000))
			{
				_connectionQue.pop();
				_connectionCnt--;
				delete p; // 调用~Connection()释放连接
			}
			else
			{
				break; // 队头的连接没有超过_maxIdleTime，其它连接肯定没有
			}
		}
	}
}