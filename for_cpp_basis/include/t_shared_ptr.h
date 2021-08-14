#pragma once

#include "comm.h"

/*copy-on-write
* ָ���ڸ���(д)���ݵ�ʱ�򣬽��п���������ȡ������ʱ�򣬼�С���ķ�Χ��
* �Դ�����߳�������ܡ�
* ����Linux���̷߳���˱�̡������������������£��������ݶ�����ԭ���������Ͻ��У�ʹ�ÿ����ķ�ʽ�������ݸ��µı�����������%1��
* ���ԣ����߳ƣ�����ʵ����copy-on-write ���� copy-on-ohter-reading
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
		//data һ���õ����Ͳ���Ҫ������
		//��ȡ���ݵ�ʱ��ֻ��GetData�ڲ����������̲߳����������ܽϺ�
		//��ν����ʱ�����ü�1 ������shared_ptr�ڲ������ü���ʵ�֡�

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

	//����customer������
	void Update(const string& customer, const EntryList& entries)
	{
		lock_guard<mutex> guard(_lock); //����update��Ҫ��

		if (!_data.unique()) //�жϵ�ǰshared_ptr�����ü������Դ����жϵ�ǰ�Ƿ��������߳��ڶ�����
		{
			//�������߳�����query
			//����ԭ������ ��һ���µ�shared_ptr;
			MapPtr newData = make_shared<Map>(*_data);

			std::swap(newData, _data);
			//��飬newһ�����ݣ�Ȼ�����swap��������㷨���ɵĹؼ�����
			//_data�����ü���Ϊʲô��Ϊ1����ʾ���������̣߳��ո�GetData����������Query�������������ڽ��У�Ҳ����˵��query���߳��Ѿ���ȡ�˴洢���ݵ�ָ�룬���ǻ�û�н��ж�ȡ��
			//���ʱ�����ǽ���swap���������µ�ָ���滻������Ӱ��ԭ����ָ��ָ������ݣ���ôquery���̻߳�ȡ������Ϊԭ�ȵ�_data��������swap�������������ݵ�ʵ����newData
			//query���̣߳����ݻ�ȡ����֮��ԭ����_data�����ü�����Ϊ0�����ڵ�ǰ�߳��Զ��ͷš�
		}

		//��������
		(*_data)[customer] = entries;
	}

	static int FindEntry(const EntryList& entries, const string& stock)
	{
		//�����㷨ʡ��
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
