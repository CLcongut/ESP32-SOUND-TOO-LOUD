#include "myI2S.h"

void myI2S::begin(uint8_t i2s_port, uint8_t BPS)
{
  switch (i2s_port)
  {
  case 0:
    _i2s_port = I2S_NUM_0;
    break;

  case 1:
    _i2s_port = I2S_NUM_1;
    break;

  default:
    Serial.println("error i2s port set");
    break;
  }

  switch (BPS)
  {
  case 8:
    _BPS = I2S_BITS_PER_SAMPLE_8BIT;
    break;

  case 16:
    _BPS = I2S_BITS_PER_SAMPLE_16BIT;
    break;

  case 24:
    _BPS = I2S_BITS_PER_SAMPLE_24BIT;
    break;

  case 32:
    _BPS = I2S_BITS_PER_SAMPLE_32BIT;
    break;

  default:
    Serial.println("error BPS set");
    break;
  }
}

void myI2S::SetSampleRate(uint32_t sampleRate)
{
  _sampleRate = sampleRate;
}

void myI2S::SetChannelFormat(ChannelFormat format)
{
  switch (format)
  {
  case RIGHT_LEFT:
    _format = I2S_CHANNEL_FMT_RIGHT_LEFT;
    break;

  case ALL_RIGHT:
    _format = I2S_CHANNEL_FMT_ALL_RIGHT;
    break;

  case ALL_LEFT:
    _format = I2S_CHANNEL_FMT_ALL_LEFT;
    break;

  case ONLY_RIGHT:
    _format = I2S_CHANNEL_FMT_ONLY_RIGHT;
    break;

  case ONLY_LEFT:
    _format = I2S_CHANNEL_FMT_ONLY_LEFT;
    break;

  default:
    Serial.println("error Channel Format");
    break;
  }
}

void myI2S::SetDMABuffer(int cnt, int len)
{
  _buffer_cnt = cnt;
  _buffer_len = len;
}

void myI2S::SetIntrAllocFlags(uint8_t interrupt)
{
  switch (interrupt)
  {
  case 0:
    _interrupt = 0;
    break;
  case 1:
    _interrupt = ESP_INTR_FLAG_LEVEL1;
    break;
  case 2:
    _interrupt = ESP_INTR_FLAG_LEVEL2;
    break;
  case 3:
    _interrupt = ESP_INTR_FLAG_LEVEL3;
    break;
  case 4:
    _interrupt = ESP_INTR_FLAG_LEVEL4;
    break;
  case 5:
    _interrupt = ESP_INTR_FLAG_LEVEL5;
    break;
  case 6:
    _interrupt = ESP_INTR_FLAG_LEVEL6;
    break;
  case 7:
    _interrupt = ESP_INTR_FLAG_NMI;
    break;

  default:
    Serial.println("error Intr Alloc Flags");
    break;
  }
}

void myI2S::SetTxAutoClear(bool auto_cl_tx)
{
  _auto_cl_tx = auto_cl_tx;
}

bool myI2S::SetInputMode(int bckPin, int wsPin, int dinPin)
{
  if (_mode_set)
  {
    _mode_set = false;
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = _sampleRate,
        .bits_per_sample = _BPS,
        .channel_format = _format,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = _interrupt,
        .dma_buf_count = _buffer_cnt,
        .dma_buf_len = _buffer_len,
        .use_apll = false};

    i2s_pin_config_t pin_config = {
        .bck_io_num = bckPin,
        .ws_io_num = wsPin,
        .data_out_num = I2S_PIN_NO_CHANGE,
        .data_in_num = dinPin};

    if (ESP_OK != i2s_driver_install(_i2s_port, &i2s_config, 0, NULL))
    {
      Serial.println("install i2s driver failed");
      return false;
    }
    if (ESP_OK != i2s_set_pin(_i2s_port, &pin_config))
    {
      Serial.println("i2s set pin failed");
      return false;
    }
    Serial.println("i2s set input mode");
    return true;
  }
  else
  {
    Serial.println("you can only set mode once");
    return false;
  }
}

bool myI2S::SetOutputMode(int bckPin, int wsPin, int douPin)
{
  if (_mode_set)
  {
    _mode_set = false;
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate = _sampleRate,
        .bits_per_sample = _BPS,
        .channel_format = _format,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = _interrupt,
        .dma_buf_count = _buffer_cnt,
        .dma_buf_len = _buffer_len,
        .use_apll = false,
        .tx_desc_auto_clear = _auto_cl_tx};

    i2s_pin_config_t pin_config = {
        .bck_io_num = bckPin,
        .ws_io_num = wsPin,
        .data_out_num = douPin,
        .data_in_num = I2S_PIN_NO_CHANGE};

    if (ESP_OK != i2s_driver_install(_i2s_port, &i2s_config, 0, NULL))
    {
      Serial.println("install i2s driver failed");
      return false;
    }
    if (ESP_OK != i2s_set_pin(_i2s_port, &pin_config))
    {
      Serial.println("i2s set pin failed");
      return false;
    }
    Serial.println("i2s set output mode");
    return true;
  }
  else
  {
    Serial.println("you can only set mode once");
    return false;
  }
}

bool myI2S::SetInOutputMode(int bckPin, int wsPin, int dinPin, int douPin)
{
  if (_mode_set)
  {
    _mode_set = false;
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_TX),
        .sample_rate = _sampleRate,
        .bits_per_sample = _BPS,
        .channel_format = _format,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = _interrupt,
        .dma_buf_count = _buffer_cnt,
        .dma_buf_len = _buffer_len,
        .use_apll = false,
        .tx_desc_auto_clear = _auto_cl_tx};

    i2s_pin_config_t pin_config = {
        .bck_io_num = bckPin,
        .ws_io_num = wsPin,
        .data_out_num = douPin,
        .data_in_num = dinPin};

    if (ESP_OK != i2s_driver_install(_i2s_port, &i2s_config, 0, NULL))
    {
      Serial.println("install i2s driver failed");
      return false;
    }
    if (ESP_OK != i2s_set_pin(_i2s_port, &pin_config))
    {
      Serial.println("i2s set pin failed");
      return false;
    }
    Serial.println("i2s set inoutput mode");
    return true;
  }
  else
  {
    Serial.println("you can only set mode once");
    return false;
  }
}

size_t myI2S::Read(void *data, int numData)
{
  size_t recvSize;
  i2s_read(_i2s_port, data, numData * _BPS / 8, &recvSize, portMAX_DELAY);
  return recvSize;
}

size_t myI2S::Write(void *data, int numData)
{
  size_t sendSize;
  i2s_write(_i2s_port, data, numData * _BPS / 8, &sendSize, portMAX_DELAY);
  return sendSize;
}

void myI2S::End()
{
  i2s_driver_uninstall(_i2s_port);
}