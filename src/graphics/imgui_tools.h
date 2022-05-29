#ifndef IMGUI_TOOLS_H
#define IMGUI_TOOLS_H

#include <type_traits>
#include "imgui.h"

template <std::size_t N>
class ImVecN
{
public:
	constexpr ImVecN()
	{
		for (std::size_t i = 0; i < N; ++i)
			data[i] = 0.0f;
	}

	// FIXME: Make args be exactly N floats only !!!!
	template <typename ...Args, typename = typename std::enable_if<N == sizeof...(Args), void>::type>
	constexpr ImVecN(Args&&... args) : data{ args... } { }
	template <std::size_t NN = N, typename std::enable_if_t<NN == 2>* = nullptr>
	constexpr ImVecN(ImVec2&& other) : data{ other.x, other.y } { }
	template <std::size_t NN = N, typename std::enable_if_t<NN == 2>* = nullptr>
	constexpr ImVecN(const ImVec2& other) : data{ other.x, other.y } { }
	template <std::size_t NN = N, typename std::enable_if_t<NN == 4>* = nullptr>
	constexpr ImVecN(ImVec4&& other) : data{ other.x, other.y, other.z, other.w } { }
	template <std::size_t NN = N, typename std::enable_if_t<NN == 4>* = nullptr>
	constexpr ImVecN(const ImVec4& other) : data{ other.x, other.y, other.z, other.w } { }

	ImVecN(ImVecN&& other)
	{
		for (std::size_t i = 0; i < N; ++i)
			data[i] = other.data[i];
	}
	ImVecN(const ImVecN& other)
	{
		for (std::size_t i = 0; i < N; ++i)
			data[i] = other.data[i];
	}



	ImVecN& operator=(ImVecN&& other);
	ImVecN& operator=(const ImVecN& other);



	ImVecN& operator+=(const float other)
	{
		for (std::size_t i = 0; i < N; ++i)
			data[i] += other;

		return *this;
	}
	ImVecN& operator+=(const ImVecN& other)
	{
		for (std::size_t i = 0; i < N; ++i)
			data[i] += other[i];

		return *this;
	}
	template <std::size_t NN = N, typename std::enable_if_t<NN == 2>* = nullptr>
	ImVecN& operator+=(const ImVec2& other)
	{
		data[0] += other.x;
		data[1] += other.y;

		return *this;
	}
	template <std::size_t NN = N, typename std::enable_if_t<NN == 4>* = nullptr>
	ImVecN& operator+=(const ImVec4& other)
	{
		data[0] += other.x;
		data[1] += other.y;
		data[2] += other.z;
		data[3] += other.w;

		return *this;
	}



	ImVecN& operator-=(const float other)
	{
		for (std::size_t i = 0; i < N; ++i)
			data[i] -= other;

		return *this;
	}
	ImVecN& operator-=(const ImVecN& other)
	{
		for (std::size_t i = 0; i < N; ++i)
			data[i] -= other[i];

		return *this;
	}
	template <std::size_t NN = N, typename std::enable_if_t<NN == 2>* = nullptr>
	ImVecN& operator-=(const ImVec2& other)
	{
		data[0] -= other.x;
		data[1] -= other.y;

		return *this;
	}
	template <std::size_t NN = N, typename std::enable_if_t<NN == 4>* = nullptr>
	ImVecN& operator-=(const ImVec4& other)
	{
		data[0] -= other.x;
		data[1] -= other.y;
		data[2] -= other.z;
		data[3] -= other.w;

		return *this;
	}



	ImVecN& operator*=(const float other)
	{
		for (std::size_t i = 0; i < N; ++i)
			data[i] *= other;

		return *this;
	}
	ImVecN& operator*=(const ImVecN& other)
	{
		for (std::size_t i = 0; i < N; ++i)
			data[i] *= other[i];

		return *this;
	}
	template <std::size_t NN = N, typename std::enable_if_t<NN == 2>* = nullptr>
	ImVecN& operator*=(const ImVec2& other)
	{
		data[0] *= other.x;
		data[1] *= other.y;

		return *this;
	}
	template <std::size_t NN = N, typename std::enable_if_t<NN == 4>* = nullptr>
	ImVecN& operator*=(const ImVec4& other)
	{
		data[0] *= other.x;
		data[1] *= other.y;
		data[2] *= other.z;
		data[3] *= other.w;

		return *this;
	}



	ImVecN& operator/=(const float other)
	{
		for (std::size_t i = 0; i < N; ++i)
			data[i] /= other;

		return *this;
	}
	ImVecN& operator/=(const ImVecN& other)
	{
		for (std::size_t i = 0; i < N; ++i)
			data[i] /= other[i];

		return *this;
	}
	template <std::size_t NN = N, typename std::enable_if_t<NN == 2>* = nullptr>
	ImVecN& operator/=(const ImVec2& other)
	{
		data[0] /= other.x;
		data[1] /= other.y;

		return *this;
	}
	template <std::size_t NN = N, typename std::enable_if_t<NN == 4>* = nullptr>
	ImVecN& operator/=(const ImVec4& other)
	{
		data[0] /= other.x;
		data[1] /= other.y;
		data[2] /= other.z;
		data[3] /= other.w;

		return *this;
	}



	constexpr float& operator[](const std::size_t idx)
	{
		return data[idx];
	}
	constexpr const float& operator[](const std::size_t idx) const
	{
		return data[idx];
	}



	template <std::size_t NN = N, typename std::enable_if_t<NN == 2>* = nullptr>
	operator ImVec2()
	{
		return ImVec2(data[0], data[1]);
	}
	template <std::size_t NN = N, typename std::enable_if_t<NN == 4>* = nullptr>
	operator ImVec4()
	{
		return ImVec4(data[0], data[1], data[2], data[3]);
	}



	friend constexpr ImVecN<N> operator+(const float& first, const ImVecN<N>& second)
	{
		ImVecN<N> value;

		for (std::size_t i = 0; i < N; ++i)
			value[i] = first + second[i];

		return value;
	}
	friend constexpr ImVecN<N> operator+(const ImVecN<N>& first, const float& second)
	{
		ImVecN<N> value;

		for (std::size_t i = 0; i < N; ++i)
			value[i] = first[i] + second;

		return value;
	}
	friend constexpr ImVecN<N> operator+(const ImVecN<N>& first, const ImVecN<N>& second)
	{
		ImVecN<N> value;

		for (std::size_t i = 0; i < N; ++i)
			value[i] = first[i] + second[i];

		return value;
	}



	friend constexpr ImVecN<N> operator-(const float& first, const ImVecN<N>& second)
	{
		ImVecN<N> value;

		for (std::size_t i = 0; i < N; ++i)
			value[i] = first - second[i];

		return value;
	}
	friend constexpr ImVecN<N> operator-(const ImVecN<N>& first, const float& second)
	{
		ImVecN<N> value;

		for (std::size_t i = 0; i < N; ++i)
			value[i] = first[i] - second;

		return value;
	}
	friend constexpr ImVecN<N> operator-(const ImVecN<N>& first, const ImVecN<N>& second)
	{
		ImVecN<N> value;

		for (std::size_t i = 0; i < N; ++i)
			value[i] = first[i] - second[i];

		return value;
	}



	friend constexpr ImVecN<N> operator*(const float& first, const ImVecN<N>& second)
	{
		ImVecN<N> value;

		for (std::size_t i = 0; i < N; ++i)
			value[i] = first * second[i];

		return value;
	}
	friend constexpr ImVecN<N> operator*(const ImVecN<N>& first, const float& second)
	{
		ImVecN<N> value;

		for (std::size_t i = 0; i < N; ++i)
			value[i] = first[i] * second;

		return value;
	}
	friend constexpr ImVecN<N> operator*(const ImVecN<N>& first, const ImVecN<N>& second)
	{
		ImVecN<N> value;

		for (std::size_t i = 0; i < N; ++i)
			value[i] = first[i] * second[i];

		return value;
	}



	friend constexpr ImVecN<N> operator/(const float& first, const ImVecN<N>& second)
	{
		ImVecN<N> value;

		for (std::size_t i = 0; i < N; ++i)
			value[i] = first / second[i];

		return value;
	}
	friend constexpr ImVecN<N> operator/(const ImVecN<N>& first, const float& second)
	{
		ImVecN<N> value;

		for (std::size_t i = 0; i < N; ++i)
			value[i] = first[i] / second;

		return value;
	}
	friend constexpr ImVecN<N> operator/(const ImVecN<N>& first, const ImVecN<N>& second)
	{
		ImVecN<N> value;

		for (std::size_t i = 0; i < N; ++i)
			value[i] = first[i] / second[i];

		return value;
	}



private:
	float data[N];
};



constexpr ImVecN<2> operator+(const float& first, const ImVec2& second)
{
	return ImVecN<2> {
		first + second.x,
		first + second.y
	};
}
constexpr ImVecN<2> operator+(const ImVec2& first, const float& second)
{
	return ImVecN<2> {
		first.x + second,
		first.y + second
	};
}
constexpr ImVecN<2> operator+(const ImVec2& first, const ImVecN<2>& second)
{
	return ImVecN<2> {
		first.x + second[0],
		first.y + second[1]
	};
}
constexpr ImVecN<2> operator+(const ImVecN<2>& first, const ImVec2& second)
{
	return ImVecN<2> {
		first[0] + second.x,
		first[1] + second.y
	};
}

constexpr ImVecN<4> operator+(const float& first, const ImVec4& second)
{
	return ImVecN<4> {
		first + second.x,
		first + second.y,
		first + second.z,
		first + second.w
	};
}
constexpr ImVecN<4> operator+(const ImVec4& first, const float& second)
{
	return ImVecN<4> {
		first.x + second,
		first.y + second,
		first.z + second,
		first.w + second
	};
}
constexpr ImVecN<4> operator+(const ImVec4& first, const ImVecN<4>& second)
{
	return ImVecN<4> {
		first.x + second[0],
		first.y + second[1],
		first.z + second[2],
		first.w + second[3]
	};
}
constexpr ImVecN<4> operator+(const ImVecN<4>& first, const ImVec4& second)
{
	return ImVecN<4> {
		first[0] + second.x,
		first[1] + second.y,
		first[2] + second.z,
		first[3] + second.w,
	};
}



constexpr ImVecN<2> operator-(const float& first, const ImVec2& second)
{
	return ImVecN<2> {
		first - second.x,
		first - second.y
	};
}
constexpr ImVecN<2> operator-(const ImVec2& first, const float& second)
{
	return ImVecN<2> {
		first.x - second,
		first.y - second
	};
}
constexpr ImVecN<2> operator-(const ImVec2& first, const ImVecN<2>& second)
{
	return ImVecN<2> {
		first.x - second[0],
		first.y - second[1]
	};
}
constexpr ImVecN<2> operator-(const ImVecN<2>& first, const ImVec2& second)
{
	return ImVecN<2> {
		first[0] - second.x,
		first[1] - second.y
	};
}

constexpr ImVecN<4> operator-(const float& first, const ImVec4& second)
{
	return ImVecN<4> {
		first - second.x,
		first - second.y,
		first - second.z,
		first - second.w
	};
}
constexpr ImVecN<4> operator-(const ImVec4& first, const float& second)
{
	return ImVecN<4> {
		first.x - second,
		first.y - second,
		first.z - second,
		first.w - second
	};
}
constexpr ImVecN<4> operator-(const ImVec4& first, const ImVecN<4>& second)
{
	return ImVecN<4> {
		first.x - second[0],
		first.y - second[1],
		first.z - second[2],
		first.w - second[3]
	};
}
constexpr ImVecN<4> operator-(const ImVecN<4>& first, const ImVec4& second)
{
	return ImVecN<4> {
		first[0] - second.x,
		first[1] - second.y,
		first[2] - second.z,
		first[3] - second.w,
	};
}



constexpr ImVecN<2> operator*(const float& first, const ImVec2& second)
{
	return ImVecN<2> {
		first * second.x,
		first * second.y
	};
}
constexpr ImVecN<2> operator*(const ImVec2& first, const float& second)
{
	return ImVecN<2> {
		first.x * second,
		first.y * second
	};
}
constexpr ImVecN<2> operator*(const ImVec2& first, const ImVecN<2>& second)
{
	return ImVecN<2> {
		first.x * second[0],
		first.y * second[1]
	};
}
constexpr ImVecN<2> operator*(const ImVecN<2>& first, const ImVec2& second)
{
	return ImVecN<2> {
		first[0] * second.x,
		first[1] * second.y
	};
}

constexpr ImVecN<4> operator*(const float& first, const ImVec4& second)
{
	return ImVecN<4> {
		first * second.x,
		first * second.y,
		first * second.z,
		first * second.w
	};
}
constexpr ImVecN<4> operator*(const ImVec4& first, const float& second)
{
	return ImVecN<4> {
		first.x * second,
		first.y * second,
		first.z * second,
		first.w * second
	};
}
constexpr ImVecN<4> operator*(const ImVec4& first, const ImVecN<4>& second)
{
	return ImVecN<4> {
		first.x * second[0],
		first.y * second[1],
		first.z * second[2],
		first.w * second[3]
	};
}
constexpr ImVecN<4> operator*(const ImVecN<4>& first, const ImVec4& second)
{
	return ImVecN<4> {
		first[0] * second.x,
		first[1] * second.y,
		first[2] * second.z,
		first[3] * second.w,
	};
}



constexpr ImVecN<2> operator/(const float& first, const ImVec2& second)
{
	return ImVecN<2> {
		first / second.x,
		first / second.y
	};
}
constexpr ImVecN<2> operator/(const ImVec2& first, const float& second)
{
	return ImVecN<2> {
		first.x / second,
		first.y / second
	};
}
constexpr ImVecN<2> operator/(const ImVec2& first, const ImVecN<2>& second)
{
	return ImVecN<2> {
		first.x / second[0],
		first.y / second[1]
	};
}
constexpr ImVecN<2> operator/(const ImVecN<2>& first, const ImVec2& second)
{
	return ImVecN<2> {
		first[0] / second.x,
		first[1] / second.y
	};
}

constexpr ImVecN<4> operator/(const float& first, const ImVec4& second)
{
	return ImVecN<4> {
		first / second.x,
		first / second.y,
		first / second.z,
		first / second.w
	};
}
constexpr ImVecN<4> operator/(const ImVec4& first, const float& second)
{
	return ImVecN<4> {
		first.x / second,
		first.y / second,
		first.z / second,
		first.w / second
	};
}
constexpr ImVecN<4> operator/(const ImVec4& first, const ImVecN<4>& second)
{
	return ImVecN<4> {
		first.x / second[0],
		first.y / second[1],
		first.z / second[2],
		first.w / second[3]
	};
}
constexpr ImVecN<4> operator/(const ImVecN<4>& first, const ImVec4& second)
{
	return ImVecN<4> {
		first[0] / second.x,
		first[1] / second.y,
		first[2] / second.z,
		first[3] / second.w,
	};
}

#endif // IMGUI_TOOLS_H
