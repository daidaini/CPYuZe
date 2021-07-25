#pragma once

#include "comm.h"

class Stock
{
public:
	Stock(const string& key) : key_(key)
	{

	}
	const string& key()
	{
		return key_;
	}

private:
	string key_;
};

class StockFactory : public enable_shared_from_this<StockFactory>
{
public:
	StockFactory() = default;
	StockFactory(const StockFactory&) = delete;  //boost::noncopyable
	StockFactory operator=(const StockFactory&) = delete;

	shared_ptr<Stock> get(const string& key)
	{
		shared_ptr<Stock> pStock;
		lock_guard<mutex> guradr(mutex_);
		weak_ptr<Stock>& wkStock = stocks_[key];
		pStock = wkStock.lock();
		if (pStock == nullptr)
		{
			pStock.reset(
				new Stock(key),
				std::bind(&StockFactory::weakDeleteCallback,
					weak_ptr<StockFactory>(shared_from_this()),
					placeholders::_1)
			);
		}
		return nullptr;
	}
private:
	static void weakDeleteCallback(const weak_ptr<StockFactory>& wkFactory,
		Stock* stock)
	{
		shared_ptr<StockFactory> factory(wkFactory.lock());
		if (factory)
		{
			factory->removeStock(stock);
		}
		delete stock;
	}
	
	void removeStock(Stock* stock)
	{
		if (stock)
		{
			lock_guard<mutex> guard(mutex_);
			stocks_.erase(stock->key());
		}
	}
private:
	mutable mutex mutex_;
	map<string, weak_ptr<Stock>> stocks_;
};


void tesLongLifeFactory()
{
	shared_ptr<StockFactory> factory = make_shared<StockFactory>();
	{
		shared_ptr<Stock> stock = factory->get("NYSE:IBM");
		shared_ptr<Stock> stock2 = factory->get("NYSE:IBM");
		assert(stock == stock2);
		//stock destructs here
	}
	//factory destructs here
}

void testShrotLifeFactory()
{
	shared_ptr<Stock> stock;
	{
		shared_ptr<StockFactory> factory = make_shared<StockFactory>();
		stock = factory->get("NYSE:IBM");
		shared_ptr<Stock> stock2 = factory->get("NYSE:IBM");
		assert(stock == stock2);
		//factory destructs here
	}
	//stock destructs here
}