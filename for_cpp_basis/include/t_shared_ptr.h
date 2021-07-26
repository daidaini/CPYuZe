#pragma once

#include "comm.h"

/*copy-on-write
* 指，在更新(写)数据的时候，进行拷贝；而读取操作的时候，减小锁的范围；
* 以此来提高程序的性能。
* 按《Linux多线程服务端编程》的描述，大多数情况下，更新数据都是在原来的数据上进行，使用拷贝的方式进行数据更新的比例，还不到%1。
* 所以，作者称，这其实不算copy-on-write 而是 copy-on-ohter-reading
*/

class CustomerData
{
public:
	CustomerData()
	{
		_data = make_shared<Map>();
	}
	~CustomerData()
	{

	}
	CustomerData(const CustomerData&) = delete;
	CustomerData operator=(const CustomerData&) = delete;

	int Query(const string& customer, const string& stock)
	{
		MapPtr data = GetData();
		//data 一旦拿到，就不需要再锁了
		//读取数据的时候，只有GetData内部有锁，多线程并发读的性能较好
		//所谓读的时候引用加1 ，是由shared_ptr内部的引用计数实现。

		auto dataIt = _data->find(customer);
		if (dataIt == _data->end())
			return -1;


		return FindEntry(dataIt->second, stock);
	}

private:
	using Entry = std::pair<string, int>;
	using EntryList = std::vector<Entry>;
	//key = customer value=entries
	using Map = std::map<string, EntryList>;
	using MapPtr = shared_ptr<Map>;

	//更新customer的数据
	void Update(const string& customer, const EntryList& entries)
	{
		lock_guard<mutex> guard(_lock); //整个update都要锁

		if (!_data.unique()) //判断当前shared_ptr的引用计数，以此来判断当前是否有其他线程在读数据
		{
			//有其他线程正在query
			//拷贝原先数据 到一个新的shared_ptr;
			MapPtr newData = make_shared<Map>(*_data);

			std::swap(newData, _data);
			//这块，new一块数据，然后进行swap，是这个算法技巧的关键所在
			//_data的引用计数为什么不为1，表示，在其他线程，刚刚GetData结束，但是Query的整个动作还在进行，也就是说，query的线程已经获取了存储数据的指针，但是还没有进行读取。
			//这个时候，我们进行swap操作，用新的指针替换，不会影响原来的指针指向的内容，那么query的线程获取的数据为原先的_data，而这里swap后，用来更新数据的实际是newData
			//query的线程，数据获取结束之后，原来的_data，引用计数变为0，则在当前线程自动释放。
		}

		//更新数据
		(*_data)[customer] = entries;
	}

	static int FindEntry(const EntryList& entries, const string& stock)
	{
		//具体算法省略
		auto dst = std::find_if(entries.begin(), entries.end(), 
			[&stock](const Entry& src){
				return src.first == stock;
			});
		if (dst == entries.end())
			return -1;

		return dst->second;
	}
	
	MapPtr GetData()
	{
		lock_guard<mutex> guard(_lock);
		return _data;
	}

	std::mutex _lock;
	MapPtr _data;

};
