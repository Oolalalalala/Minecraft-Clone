#pragma once

#include <queue>
#include <mutex>

namespace Olala {

	template<typename T>
	class Pipe : public std::queue<T>
	{
	public:
		void Push(const T& value)
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			std::queue<T>::push(value);
			m_ConditionalVariable.notify_one();
		}

		T Pop()
		{
			std::unique_lock<std::mutex> lock(m_Mutex);
			while (std::queue<T>::empty())
			{
				m_ConditionalVariable.wait(lock);
			}
			T value = std::queue<T>::front();
			std::queue<T>::pop();
			lock.unlock();
			return value;
		}

		uint32_t Size()
		{
			return std::queue<T>::size();
		}

	private:
		std::mutex m_Mutex;
		std::condition_variable m_ConditionalVariable;
	};


	template<typename A, typename B>
	class Tunnel
	{
	public:
		void PushLaneA(const A& value) { m_LaneA.Push(value); }
		void PushLaneB(const B& value) { m_LaneB.Push(value); }
		A PopLaneA() { return m_LaneA.Pop(); }
		B PopLaneB() { return m_LaneB.Pop(); }
		uint32_t LaneASize() { return m_LaneA.Size(); }
		uint32_t LaneBSize() { return m_LaneB.Size(); }

	private:
		Pipe<A> m_LaneA;
		Pipe<B> m_LaneB;
	};

}