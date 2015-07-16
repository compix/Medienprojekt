#pragma once
#include <string>
#include <enet/enet.h>
#include <memory.h>
#include <assert.h>
#include <stdint.h>
#include <functional>

namespace NetCode
{
	using namespace std;

	template<class _Ty> struct is_bool : false_type {};
	template<> struct is_bool<bool> : true_type{};
	template<class _Ty> struct is_string : false_type {};
	template<> struct is_string<string> : true_type{};

	template <typename MT>
	class MessageWriter
	{
	public:
		MessageWriter(int capacity)
			: m_position(0), m_limit(capacity)
		{
			m_data = new uint8_t[capacity];
		}

		~MessageWriter()
		{
			delete[] m_data;
		}

		void init(MT type)
		{
			m_position = 0;
			write<MT>(type);
		}

		ENetPacket *createPacket(enet_uint32 flags)
		{
			return enet_packet_create(m_data, m_position, flags);
		}

		template <typename T>
		void write(const typename enable_if<!is_bool<T>::value && !is_string<T>::value, T>::type &value)
		{
			static_assert(is_floating_point<T>::value || is_integral<T>::value || is_enum<T>::value, "Get can only take integer and float types");
			size_t size = sizeof(value);
			assert(m_position + size <= m_limit);
			memcpy(m_data + m_position, &value, size);
			m_position += size;
		}

		template <typename T>
		void write(const typename enable_if<is_bool<T>::value, bool>::type &value)
		{
			write<uint8_t>(value ? 1 : 0);
		}

		template <typename T>
		void write(const typename enable_if<is_string<T>::value, string>::type &value)
		{
			size_t length = value.size();
			write<uint16_t>(length);
			assert(m_position + length <= m_limit);
			memcpy(m_data + m_position, value.c_str(), length);
			m_position += length;
			m_data[m_position++] = 0;
		}

	private:
		uint8_t *m_data;
		size_t m_position;
		size_t m_limit;
	};

	template <typename MT>
	class MessageReader
	{
	public:

		MessageReader(const uint8_t *data, size_t limit)
			: m_data(data), m_limit(limit), m_position(0)
		{
			m_type = read<MT>();
		}

		MT getType() { return m_type; }

		template <typename T>
		typename enable_if<!is_bool<T>::value && !is_string<T>::value, T>::type read()
		{
			static_assert(is_floating_point<T>::value || is_integral<T>::value || is_enum<T>::value, "Get can only take integer and float types");
			size_t size = sizeof(T);
			assert(m_position + size <= m_limit);
			T value;
			memcpy(&value, m_data + m_position, size);
			m_position += size;
			return value;
		}

		template <typename T>
		typename enable_if<is_bool<T>::value, bool>::type read()
		{
			return read<uint8_t>() != 0;
		}

		template <typename T>
		typename enable_if<is_string<T>::value, string>::type read()
		{
			uint16_t length = read<uint16_t>();
			assert(m_position + length <= m_limit);
			const char *result = reinterpret_cast<const char *>(m_data + m_position);
			m_position += length + 1;
			return string(result, length);
		}

	private:
		const uint8_t *m_data;
		size_t m_position;
		size_t m_limit;
		MT m_type;
	};

	template <typename MT>
	class MessageHandler
	{
	public:
		using Callback = function<void(MessageReader<MT> &reader, ENetEvent &event)>;

		void setCallback(MT index, Callback callback)
		{
			assert(index < MT::COUNT);
			m_callbacks[static_cast<uint16_t>(index)] = callback;
		}

		template <typename T>
		void setCallback(MT index, void (T::*callback)(MessageReader<MT> &reader, ENetEvent &event), T* const classPtr)
		{
			assert(index < MT::COUNT);
			m_callbacks[static_cast<uint16_t>(index)] = std::bind(callback, classPtr, std::placeholders::_1, std::placeholders::_2);
		}

		void handleMessage(ENetEvent &event) const
		{
			MessageReader<MT> reader(event.packet->data, event.packet->dataLength);
			MT type = reader.getType();
			uint16_t _type = static_cast<uint16_t>(type);
			assert(_type >= 0 && type < MT::COUNT && m_callbacks[_type]);
			m_callbacks[_type](reader, event);
		}

	private:
		Callback m_callbacks[static_cast<uint16_t>(MT::COUNT)];
	};
}
