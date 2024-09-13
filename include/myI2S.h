#pragma once
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s.h"

typedef enum
{
  RIGHT_LEFT,
  ALL_RIGHT,
  ALL_LEFT,
  ONLY_RIGHT,
  ONLY_LEFT,
} ChannelFormat;

class myI2S
{
private:
  i2s_port_t _i2s_port;
  i2s_bits_per_sample_t _BPS;
  uint32_t _sampleRate = 44100;
  bool _mode_set = true;
  i2s_channel_fmt_t _format = I2S_CHANNEL_FMT_RIGHT_LEFT;
  int _buffer_cnt = 16;
  int _buffer_len = 64;
  uint8_t _interrupt = 0;
  bool _auto_cl_tx = false;

public:
  void begin(uint8_t i2s_port = 0, uint8_t BPS = 32);
  void SetSampleRate(uint32_t sampleRate);
  void SetChannelFormat(ChannelFormat format);
  void SetDMABuffer(int cnt, int len);
  void SetIntrAllocFlags(uint8_t interrupt);
  void SetTxAutoClear(bool auto_cl_tx);
  bool SetInputMode(int bckPin, int wsPin, int dinPin);
  bool SetOutputMode(int bckPin, int wsPin, int douPin);
  bool SetInOutputMode(int bckPin, int wsPin, int dinPin, int douPin);
  size_t Read(void *data, int numData);
  size_t Write(void *data, int numData);
  void End();
};