#ifndef WaveFile_h__
#define WaveFile_h__

#include <string>

///\note All meta data is undefined if IsLoaded() == false
class WaveFile
{
public:
	enum AudioFormat
	{
		PCM = 1,
        WAVE_FORMAT_IEEE_FLOAT = 3
	};

	WaveFile();
	WaveFile(const std::string &filename);
	~WaveFile();

	bool Load(const std::string &filename);
	void Unload();

	inline bool IsLoaded() const
	{
		return (data != nullptr && size != 0);
	}

	inline AudioFormat GetAudioFormat() const
	{
		return meta.audioFormat;
	}
	inline unsigned int GetNumChannels() const
	{
		return meta.numChannels;
	}
	inline unsigned int GetSampleRate() const
	{
		return meta.sampleRate;
	}
	inline unsigned int GetBitsPerSample() const
	{
		return meta.bitsPerSample;
	}

	inline const char *GetData() const
	{
		return data;
	}
	inline std::size_t GetDataSize() const
	{
		return size;
	}

private:
	struct Meta
	{
		AudioFormat audioFormat;
		unsigned int numChannels;
		unsigned int sampleRate;
		unsigned int bitsPerSample;
	} meta;
	char *data;
	std::size_t size;
};

#endif // WaveFile_h__
