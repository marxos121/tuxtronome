#pragma once

inline constexpr short START_BPM = 90;
inline constexpr short MAX_BPM = 240;
inline constexpr short MIN_BPM = 30;

enum class Notes {
	Whole = 1,
	Half = 2,
	Quarter = 4,
	Eighth = 8,
	Sixteenth = 16
};