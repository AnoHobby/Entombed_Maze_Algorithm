#include <random>
#include <iostream>
#include <unordered_map>
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
		const auto key = (lasttwo << 3) | ((lastlowpadded >> i) & 0b111) ;
		const std::unordered_map<std::bitset<5>, bool> data = {
			{0b00000, 1 },
			{0b00001, 1 },
			{0b00010, 1 },
			{0b00100, 0 },
			{0b00101, 0 },
			{0b01000 , 1 },
			{0b01001 , 1 },
			{0b01010 , 1 },
			{0b01011 , 1 },
			{0b01101 , 0 },
			{0b01110 , 0 },
			{0b01111 , 0 },
			{0b10000 , 1 },
			{0b10001 , 1 },
			{0b10010 , 1 },
			{0b10100 , 0 },
			{0b10101 , 0 },
			{0b10110 , 0 },
			{0b10111 , 0 },
			{0b11001 , 0 },
			{0b11010 , 1 },
			{0b11101 , 0 },
			{0b11110 , 0 },
			{0b11111 , 0 },
		};
		if (data.contains(key)) {
			newBit = data.at(key);
		}
		else {
			newBit = Random::getInstance().get(0, 1);
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
		lastrows.back() = 0;//back=èåèéÆ
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
			std::cout <<(bit[i]?"Å°":"Å†");
		}
		std::cout << std::endl;
	}
	return EXIT_SUCCESS;
}








