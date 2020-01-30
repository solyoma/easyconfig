#pragma once

#ifndef _TRIM_DEFINED
#define _TRIM_DEFINED
#include <algorithm> 
#include <cctype>
#include <locale>

// trim for character arrays
static inline char *LTrim(char *pb)
{
	while (*pb && std::isspace(*pb)) ++pb;
	return pb;
}
static inline char *RTrim(char *buf)
{
	char *pb = buf + strlen(buf) -1;
	while(pb != buf && std::isspace(*pb)) 
		--pb;
	*++pb = 0;
	return buf;
}

static inline char *Trim(char *buf)
{
	return RTrim(LTrim(buf));
}
// --------------- same for Strings

// trim from start (in place)
static inline void ltrim(std::string &s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
		return !std::isspace(ch);
		}));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
		return !std::isspace(ch);
		}).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
	ltrim(s);
	rtrim(s);
}

// trim from start (copying)
static inline std::string ltrim_copy(std::string s) {
	ltrim(s);
	return s;
}

// trim from end (copying)
static inline std::string rtrim_copy(std::string s) {
	rtrim(s);
	return s;
}

// trim from both ends (copying)
static inline std::string trim_copy(std::string s) {
	trim(s);
	return s;
}

#endif
