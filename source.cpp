#include <random>
#include <iostream>
#include <bitset>
#include <algorithm>
#include <numeric>
class Random {
private:
	~Random() = default;
	std::mt19937 engine;
	Random() :engine(std::random_device()()) {}
public:
	Random(const Random&) = delete;
	Random(const Random&&) = delete;
	Random& operator=(const Random&) = delete;
	Random& operator=(const Random&&) = delete;
	static auto& getInstance() {
		static Random instance;
		return instance;
	}
	auto get(const int min, const int max) {
		return std::uniform_int_distribution<>(min, max)(engine);
	}
};
auto generateRow(std::vector<int> &lastrows) {
	const auto lastlowpadded = (((Random::getInstance().get(0, 1) << 8) | lastrows.back()) << 1) | Random::getInstance().get(0, 1);
	auto lasttwo = 0b10;
	auto newrow = 0;
	for (auto i = 7; 0 <= i; --i) {
		bool newBit;
		const std::bitset<5> key = (lasttwo << 3) | ((lastlowpadded >> i) & 0b111) ;
		bool rand = false;
		if (!key[2] && (
			!(key[1] || key[3])
			||
			!(key[4]) && (key[3])
			||
			!(key[0]) && (key[1])
			))newBit = 1;
		else if ((key[2] && ((key >> 1) != 0b0011) && (key << 1) != 0b11000) || key == 0b11001)newBit=0;
		else {
			newBit = Random::getInstance().get(0,1);
		}
		newrow = (newrow << 1) | newBit;
		lasttwo = ((lasttwo << 1) | newBit) & 0b11;
	}
	lastrows.emplace_back(newrow);
	if(11<lastrows.size())lastrows.erase(lastrows.begin(), lastrows.end() - 11);
	std::vector<int> history;
	for (const auto& b : lastrows) {
		history.emplace_back(0xf0 & b);
	}
	if (std::ranges::find(history, 0) == history.end() && std::ranges::all_of(history, [](auto i) {
		return !(i & 0x80);
		})) {
		lastrows.back() = 0;
	}
	history.clear();
	std::for_each_n(lastrows.rbegin(), 7, [&history](int value) {
		history.emplace_back(value & 0xf);
		});
	if (std::ranges::find(history, 0) == history.end()) {
		auto comparator = 0;
		if (9 <= lastrows.size())comparator = lastrows[lastrows.size() - 9 - 1];
		if (std::reduce(history.begin(), history.end(), 0, [](auto acc, int value) {
			return acc + (value & 1);
			}) == (comparator & 1) * 7) {
			lastrows.back() &= 0xf0;
		}
	}
}
int main() {
	std::vector<int> a{ 0 };
	while (true) {
		generateRow(a);
		auto bit = std::bitset<8>(a.back());
		for (auto i = 0; i<bit.size();++i) {
			std::cout <<(bit[i]?"¡":" ");
		}
		std::cout << std::endl;
	}

	return EXIT_SUCCESS;
}
