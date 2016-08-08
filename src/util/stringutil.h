#ifndef UTIL_STRINGUTIL_H
#define UTIL_STRINGUTIL_H

#include <string>
#include <vector>
#include "common_define.h"

namespace cgserver{
    typedef std::vector<std::string> strs_t;

    class StringUtil
    {
    public:
	StringUtil();
	~StringUtil();
    public:
	static void Trim(std::string& str);
	static char* Trim(const char* szStr);

	static std::vector<std::string> Split(const std::string& text, const char* sepStr);
	static std::vector<std::string> Split(const char * text, const char* sepStr);

	static void ToUpperCase(char* str);
	static bool StrToInt8(const char* str, int8_t& value);
	static bool StrToUInt8(const char* str, uint8_t& value);
	static bool StrToInt16(const char* str, int16_t& value);
	static bool StrToUInt16(const char* str, uint16_t& value);
	static bool StrToInt32(const char* str, int32_t& value);
	static bool StrToUInt32(const char* str, uint32_t& value);
	static bool StrToInt64(const char* str, int64_t& value);
	static bool StrToUInt64(const char* str, uint64_t& value);
	static bool StrToFloat(const char* str, float& value);
	static bool StrToDouble(const char* str, double& value);
	static bool HexStrToUint64(const char* str, uint64_t& value);
	static void Uint64ToHexStr(uint64_t value, char* hexStr, int len);

	static uint32_t DeserializeUInt32(const std::string& str);
	static void SerializeUInt32(uint32_t value, std::string& str);

	static uint64_t DeserializeUInt64(const std::string& str);
	static void SerializeUInt64(uint64_t value, std::string& str);

	static int8_t StrToInt8WithDefault(const char* str, int8_t defaultValue);
	static uint8_t StrToUInt8WithDefault(const char* str, uint8_t defaultValue);
	static int16_t StrToInt16WithDefault(const char* str, int16_t defaultValue);
	static uint16_t StrToUInt16WithDefault(const char* str, uint16_t defaultValue);
	static int32_t StrToInt32WithDefault(const char* str, int32_t defaultValue);
	static uint32_t StrToUInt32WithDefault(const char* str, uint32_t defaultValue);
	static int64_t StrToInt64WithDefault(const char* str, int64_t defaultValue);
	static uint64_t StrToUInt64WithDefault(const char* str, uint64_t defaultValue);
	static float StrToFloatWithDefault(const char* str, float defaultValue);
	static double StrToDoubleWithDefault(const char* str, double defaultValue);

	static char* replicate(const char* szStr);
	static char* replicate(const char* szStr, uint32_t nLength);

	static char* mergeString(const char* szStr1, const char *szStr2);

	static void ForWarning();

	static bool equal(const char* szStr1, const char* szStr2);
	static bool safe_equal(const char* szStr1, const char* szStr2);
	static bool equalNoCase(const char* szStr1, const char* szStr2);

	static common::ret_t getParam(char * q, uint32_t & size, const char * name,    char * & out);
	static char * getParam(const char * q, uint32_t size,    const char * name);

	static bool isAscii(const char *str, size_t size);
	static bool isAscii(const char *str);

	static int findChar(const char *src, char dest);
	static void split(const char *src, const char delimiter, strs_t &out);
	static void splitString(const std::string &src, const char delimiter, strs_t &out);

	unsigned char toHex(const unsigned char &x);
	unsigned char fromHex(const unsigned char &x);
	bool URLEncode(const std::string &sIn, std::string &sOut);
	bool URLDecode(const std::string &sIn, std::string &sOut);
	
    private:
    };
    

    
}
#endif
