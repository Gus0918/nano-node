#include <nano/node/payment_observer_processor.hpp>

#include <nano/node/json_payment_observer.hpp>

nano::payment_observer_processor::payment_observer_processor (nano::node_observers::blocks_t & blocks)
{
	blocks.add ([this](std::shared_ptr<nano::block> block_a, nano::account const & account_a, nano::uint128_t const &, bool) {
		observer_action (account_a);
	});
}

void nano::payment_observer_processor::observer_action (nano::account const & account_a)
{
	std::shared_ptr<nano::json_payment_observer> observer;
	{
		std::lock_guard<std::mutex> lock (mutex);
		auto existing (payment_observers.find (account_a));
		if (existing != payment_observers.end ())
		{
			observer = existing->second;
		}
	}
	if (observer != nullptr)
	{
		observer->observe ();
	}
}

void nano::payment_observer_processor::add (nano::account const & account_a, std::shared_ptr<nano::json_payment_observer> payment_observer_a)
{
	std::lock_guard<std::mutex> lock (mutex);
	assert (payment_observers.find (account_a) == payment_observers.end ());
	payment_observers[account_a] = payment_observer_a;
}

void nano::payment_observer_processor::erase (nano::account & account_a)
{
	std::lock_guard<std::mutex> lock (mutex);
	assert (payment_observers.find (account_a) != payment_observers.end ());
	payment_observers.erase (account_a);
}