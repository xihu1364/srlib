#ifndef __CRC32_H__
#define __CRC32_H__
#include <fstream>  

using namespace std;  



class CRC32
{
public:
	unsigned int GetCRC32Num(char* fileName)
	{
		genCRC32table();  

		fstream fileStream;  

		fileStream.open(fileName, ios::in); // Input  

		fileStream.seekg(0L, ios::end);  
		int fileSize = (int)fileStream.tellg();  

		if (fileSize == -1)
		{
			TRACE("fileStream.tellg() error = %d\n",GetLastError());
		}

		char* buffer = new char[fileSize];  
		int len = fileSize;  

		fileStream.seekg(0, ios::beg);  
		fileStream.clear();  

		// =========================================================================  
		unsigned int crc32 = 0xFFFFFFFF;  

		while(!fileStream.eof()) // 是否文件末尾  
		{  
			if (!fileStream.fail()) 
			{ 
				// 是否文件错  
				fileStream.read(buffer, len);  
				for(int i = 0; i < len; i++)  
					crc32 = (crc32 >> 8) ^ crc32table[(crc32 ^ buffer[i]) & 0xFF];  
			}  
			fileStream.setstate(ios::eofbit); // 设置文件状态标记为eof  
		}  
		// =========================================================================  

		fileStream.close();  

		return crc32 ^ ~0U;  
	}
private:
	void genCRC32table()
	{
		int i;  
		int j;  
		unsigned int crc;  
		for(i = 0; i < 256; i++) {  
			crc = i;  
			for(j = 0; j < 8; j++) {  
				if((crc & 1) == 1)  
					crc = (crc >> 1) ^ 0xEDB88320;  
				else  
					crc >>= 1;  
			}  
			crc32table[i] = crc;  
		}  
	}
	unsigned int crc32table[256];
};

#endif