#include "Specrtum48kBus.h"

Specrtum48kBus::Specrtum48kBus()
{
	cpu.connectBus(this);
	video.connectBus(this);

	for (auto& i : mem) i = 0x00;

	std::ifstream ifs;
	ifs.open("roms/48.rom", std::ifstream::binary);
	if (ifs.is_open())
	{
		ifs.read((char*)mem, 16 * 1024);
		ifs.close();
	}
}

Specrtum48kBus::~Specrtum48kBus()
{

}

uint8_t Specrtum48kBus::readMemory(uint16_t addr)
{
	return mem[addr];
}

void Specrtum48kBus::writeMemory(uint16_t addr, uint8_t data)
{
	if (addr > 0x3FFF)
		mem[addr] = data;
}

uint8_t Specrtum48kBus::readPeripheral(uint16_t addr)
{
	uint8_t result = 0xFF;

	if ((addr & 0xFF) == 0xFE)
	{
		result = keyboard.getKey(addr >> 8);
		result = (audioIn > 0.3f) ? result | 0x40 : result & ~(0x40);
	}

	return result;
}

void Specrtum48kBus::writePeripheral(uint16_t addr, uint8_t data)
{
	if ((addr & 0xFF) == 0xFE)
	{
		speakerOut = (data & 0x18) ? 1.0f : 0.0f;
		video.borderColor = data & 0x07;
	}
}

void Specrtum48kBus::setSampleFrequency(uint32_t sampleRate)
{
	cpu.setSampleFrequency(sampleRate);
	video.setSampleFrequency(sampleRate);
}

void Specrtum48kBus::reset()
{
	cpu.reset();
}

void Specrtum48kBus::clock()
{
	cpu.updateCpu();
	video.updateVideo();
	mixAudioInputs();
}

void Specrtum48kBus::mixAudioInputs()
{
	audioOut = ((speakerOut * 0.5) + audioIn) / 2;
}
