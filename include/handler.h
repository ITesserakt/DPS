#pragma once

/// Reads a string from [stdin], outputs it back to [stdout] and outputs the same string to [stdout] but latin letters
/// has inverted case
int invert_case();

/// Reads a string from [stdin], outputs it back to [stdout] and outputs it reversed variant to [stdout]
int reverse_str();

/// Reads a string from [stdin], outputs it back to [stdout] and outputs a string to [stdout] with original contents but
/// pairs of letters swapped with each other
int pairwise_swap();

/// Reads a string from [stdin], outputs it back to [stdout] and outputs a string to [stdout] where latin letters
/// converted to russian ones with inverted case
int convert_koi8();