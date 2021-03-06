//
// Created by Bridger Holt on 12/3/18.
//

#ifndef LOCK_MACHINE_LOCK_MACHINE_H
#define LOCK_MACHINE_LOCK_MACHINE_H

#include <mutex>
#include <unordered_map>


class LockMachine
{
	public:
		using PtrType = void const *;
		using MutexType = std::mutex;
		using MapType = std::unordered_map<PtrType, MutexType>;
		using LockType = std::lock_guard<MutexType>;



		LockMachine() noexcept(noexcept(MapType())) = default;


		template <class T, class Func>
		void lockFunc(T const & obj, Func && func)
		{
			lockFunc(ptrCast(obj), std::forward<Func>(func));
		}

		template <class Func>
		void lockFunc(PtrType objPtr, Func && func)
		{
			lock(objPtr);
			func();
		}

		template <class T>
		LockType lock(T const & obj)
		{
			return lock(ptrCast(obj));
		}

		LockType lock(PtrType objPtr)
		{
			MapIt it;
			bool exists;

			{
				auto lock = lockMap();
				std::tie(it, exists) = getMutex(objPtr);
			}

			if (!exists)
			{
				it = addObjectPtr(objPtr).first;
			}

			return LockType(it->second);
		}


		template <class T>
		bool addObject(T const & obj)
		{
			return addObject(&obj);
		}

		bool addObject(PtrType objPtr)
		{
			return addObjectPtr(objPtr).second;
		}


		template <class T>
		bool hasObject(T const & obj) const
		{
			return hasObject(&obj);
		}

		bool hasObject(PtrType objPtr) const
		{
			//auto lock = lockMap();
			return mutexMap_.find(objPtr) != mutexMap_.end();
		}





	private:
		using MapIt = typename MapType::iterator;
		using ItResult = std::pair<MapIt, bool>;

		MapType mutexMap_;
		MutexType mapMutex_;

		template <class T>
		PtrType ptrCast(T const & obj)
		{
			return static_cast<PtrType>(&obj);
		}

		std::lock_guard<MutexType> lockMap()
		{
			return std::lock_guard<MutexType>(mapMutex_);
		}

		ItResult addObjectPtr(PtrType objPtr)
		{
			lockMap();
			return mutexMap_.emplace(
				std::piecewise_construct,
				std::forward_as_tuple(objPtr),
				std::make_tuple());
		}


		ItResult getMutex(PtrType objPtr)
		{
			auto it = mutexMap_.find(objPtr);
			return {it, it != mutexMap_.end()};
		}


};

#endif //LOCK_MACHINE_LOCK_MACHINE_H
